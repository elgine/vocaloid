#pragma once
#include "audio_node.hpp"
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
    public:
        explicit AudioSourceNode(IAudioContext *ctx):AudioNode(ctx){}
        int16_t Process(Buffer *out){
            return 0;
        }
        void Flush(){}
    };
}