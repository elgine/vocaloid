#pragma once
#include <mutex>
#include <map>
#include <queue>
#include "disposable.h"
#include "audio_frame.hpp"
using namespace std;
namespace vocaloid{

    class AudioPort: public IDisposable{
    private:
        map<uint64_t, queue<AudioFrame*>> buffer_group_;
    public:

        void AddBuffer(uint64_t id){
            buffer_group_.insert(pair<uint64_t, queue<AudioFrame*>>(id, {}));
        }

        void PushFrame(uint64_t id, AudioFrame *frame){
            if(buffer_group_.find(id) != buffer_group_.end()){
                buffer_group_[id].push(frame);
            }
        }

        void GetFrame(AudioFrame *frame){
            vector<AudioFrame*> frames;
            frames.reserve(buffer_group_.size());
            for(auto iter = buffer_group_.begin(); iter != buffer_group_.end();iter++){
                auto f = iter->second.front();
                iter->second.pop();
                frames.push_back(f);
            }
            AudioFrame::Mix(frame, frames);
        }

        size_t GetNumOfInput(){
            return buffer_group_.size();
        }

        void Dispose() override {

        }
    };
}