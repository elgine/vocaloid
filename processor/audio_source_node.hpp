#pragma once
#include "audio_node.hpp"
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
    public:
        explicit AudioSourceNode(AudioContext *ctx):AudioNode(ctx){}
        int Process(Buffer *out){return 0;}
        void Flush(){}
    };
}