#pragma once
#include "audio_node.hpp"
namespace vocaloid {
    class AudioDestinationNode : public AudioNode {
        void Process(Buffer *in) {}
    };
}