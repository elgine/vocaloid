#pragma once
#include <vector>
#include <map>
#include <set>
#include <functional>
#include "disposable.h"
#include "audio_node.hpp"
#include "audio_source_node.hpp"
#include "audio_destination_node.hpp"
#include "audio_graph.hpp"
#include "thread_pool.hpp"
using namespace std;
namespace vocaloid{

    class AudioRouter: public IDisposable{
    private:
        AudioGraph *graph_;

        ThreadPool *pool_;
        vector<function<void()>> tasks_;
        int cur_task_index_;

        void Traverse(AudioNode *node){

        }
    public:
        explicit AudioRouter(){
            pool_ = nullptr;
            graph_ = nullptr;
            cur_task_index_ = -1;
        }

        void Setup(AudioGraph *graph,
                    int max_thread_count = 4){
            graph_ = graph;
        }

        void Run(){

        }

        void Stop(){

        }

        void Dispose() override {

        }
    };
}