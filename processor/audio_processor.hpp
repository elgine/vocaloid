#pragma once
#include <vector>
#include <map>
#include <set>
#include "disposable.h"
#include "audio_node.hpp"
#include "audio_source_node.hpp"
#include "audio_destination_node.hpp"
#include "audio_graph.hpp"
using namespace std;
namespace vocaloid{

    class AudioProcessor: public IDisposable{
    public:
        void Setup(AudioGraph *graph,
                    int max_thread_count = 4){
        }

        void Run(){
            
        }

        void Stop(){

        }

        void Dispose() override {

        }
    };
}