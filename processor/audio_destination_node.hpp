#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    class AudioDestinationNode : public AudioNode {
    public:
        explicit AudioDestinationNode(AudioContext *ctx):AudioNode(ctx){}
        int Process(Buffer *in) {return 0;}
        void Flush(){}
    };
}