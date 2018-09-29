#pragma once
#include <stdin.h>
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "disposable.h"
using namespace std;

class ThreadPool: public IDisposable{
private:
    vector<thread> threads_;
    queue<function<void()>> tasks_;
    uint32_t task_count_;
    uint32_t thread_count_;
    atomic<uint32_t> busy_task_count_;
    atomic<bool> is_busy_;
public:
    explicit ThreadPool(uint32_t thread_size, uint32_t task_size){
        threads_.resize(thread_size);
        thread_count_.resize(task_size);
        task_count_ = 0;
        thread_count_ = 0;
        busy_task_count_ = 0;
        is_busy_ = false;
    }

    void AddTask(function<void()> fun){
        task_count_++;
    }

    void Dispose() override {

    }
};