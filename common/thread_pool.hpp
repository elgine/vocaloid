#pragma once
#include <stdint.h>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
using namespace std;

class ThreadPool{
private:
    using Task = function<void()>;
    vector<thread> threads_;
    queue<Task> tasks_;
    mutex task_mutex_;
    condition_variable task_cv_;
    atomic<bool> running_;
    atomic<int> thread_count_;

    void AddTaskToQueue(Task& t){
        {
            unique_lock<mutex> lock(task_mutex_);
            tasks_.emplace(move(t));
        }
    }

    void Process(){
        while(true){
            Task task;
            {
                unique_lock<mutex> lock(task_mutex_);
                task_cv_.wait(lock, [this]{
                    return !this->running_ || !tasks_.empty();
                });
                if(!running_ && tasks_.empty())break;
                task = move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

public:

    explicit ThreadPool(uint32_t thread_count){
        thread_count_ = thread_count < 1?1:thread_count;
        running_ = false;
    }

    void AddTask(function<void()> f){
        if(!running_)return;
        AddTaskToQueue(f);
    }

    template<typename F, typename... Args>
    void AddTask(const F& f, Args... args){
        if(!running_)return;
        Task task = [&f, args...]{return f(args...);};
        AddTaskToQueue(task);
    }

    template<typename F, typename Self, typename... Args>
    void AddTask(const F& f, Self* self, Args... args){
        if(!running_)return;
        Task task = [&f, &self, args...]{return (*self.*f)(args...);};
        AddTaskToQueue(task);
    }

    void Run(){
        running_ = true;
        for(int i = 0;i < thread_count_;i++){
            threads_.emplace_back(thread(&ThreadPool::Process, this));
        }
    }

    void Stop(){
        running_ = false;
        task_cv_.notify_all();
        for(thread& t : threads_){
            if(t.joinable())
                t.join();
        }
        threads_.clear();
    }
};