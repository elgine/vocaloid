#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include "audio_source_node.hpp"
#include "audio_output_node.hpp"
#include "vocaloid/utils/ticker.hpp"
using namespace std;
namespace vocaloid{

    class AudioContext{
    private:
        Ticker *ticker_;

        AudioOutputNode *output_;
        vector<AudioSourceNode*> sources_;
    public:

        explicit AudioContext(){
            ticker_ = new Ticker();
            output_ = nullptr;
        }

        void AddSource(AudioSourceNode *s){
            auto iter = find(sources_.begin(), sources_.end(), s);
            if(iter == sources_.end()){
                sources_.emplace_back(s);
            }
        }

        void RemoveSource(AudioSourceNode *s){
            auto iter = find(sources_.begin(), sources_.end(), s);
            if(iter == sources_.end())return;
            sources_.erase(iter);
        }

        void Dispose(){

        }

        void Start(uint64_t offset = 0){

        }

        void Stop(){

        }

        uint32_t SampleRate(){
            return output_->SampleRate();
        }

        AudioOutputNode* Output(){
            return output_;
        }
    };
}