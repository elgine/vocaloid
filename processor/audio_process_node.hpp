#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    class AudioProcessNode : public AudioNode {
    public:
        explicit AudioProcessNode(AudioContext *ctx):AudioNode(ctx){}
        int Process(Buffer *in, Buffer *out){return 0;}
    };
}