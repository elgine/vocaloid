#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    class AudioProcessNode : public AudioNode {
    public:
        explicit AudioProcessNode(IAudioContext *ctx):AudioNode(ctx){}
        int16_t Process(Buffer *in, Buffer *out){return 0;}
    };
}