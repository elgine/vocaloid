#pragma once
#include <stdint.h>
#include <vector>
#include "audio_node.hpp"
#include "audio_buffer.hpp"
using namespace std;
namespace vocaloid{
    class AudioProcessNode: public AudioNode{
    public:
        explicit AudioProcessNode(){}
        virtual uint64_t Process(AudioBuffer *in, uint64_t len, AudioBuffer *out) = 0;
    };
}