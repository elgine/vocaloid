#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    class AudioProcessNode : public AudioNode {
    public:
        explicit AudioProcessNode(IAudioContext *ctx):AudioNode(ctx){}
        virtual void Process(Buffer *in, Buffer *out){}
    };
}