#pragma once
#include "audio_node.hpp"
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
    public:
        explicit AudioSourceNode(IAudioContext *ctx):AudioNode(ctx){}
        void Process(Buffer *out){}
        void Flush(){}
    };
}