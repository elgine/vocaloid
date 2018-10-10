#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    // Audio-process-node is the unit of audio-processing,
    // so it have serveral input channels and output channels
    class AudioProcessNode : public AudioNode {
    public:
        explicit AudioProcessNode(AudioContext *ctx):AudioNode(ctx){}

        // Process input buffer to output buffer
        int Process(Buffer *in, Buffer *out){return 0;}
    };
}