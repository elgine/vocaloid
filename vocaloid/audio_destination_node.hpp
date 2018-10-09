#pragma once
#include "audio_context_interface.h"
#include "audio_node.hpp"
namespace vocaloid {
    class AudioDestinationNode : public AudioNode {
    public:
        explicit AudioDestinationNode(IAudioContext *ctx):AudioNode(ctx){}
        int16_t Process(Buffer *in) {return 0;}
    };
}