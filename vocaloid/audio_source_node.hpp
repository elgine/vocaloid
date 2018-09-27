#pragma once
#include "audio_node.hpp"
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
        void Process(Buffer *out){}
    };
}