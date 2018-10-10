#pragma once
#include "disposable.h"
#include "audio_frame.hpp"
#include "audio_port.hpp"
using namespace std;
namespace vocaloid{

    class AudioNode{
    protected:
        static uint64_t ID_COUNT_;
        uint64_t id_;
        AudioPort *input_;
        AudioPort *output_;
    public:
        explicit AudioNode(){
            id_ = ID_COUNT_++;
            input_ = nullptr;
            output_ = nullptr;
        }

        int64_t Run(){
            return 0;
        }

        int64_t Process(){
            return 0;
        }

        uint64_t GetId(){
            return id_;
        }
    };

    uint64_t AudioNode::ID_COUNT_ = 0;
}