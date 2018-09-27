#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    class AudioProcessNode : public AudioNode {
        void Process(Buffer *in, Buffer *out) {}
    };
}