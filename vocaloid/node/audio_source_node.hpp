#pragma once
#include <stdint.h>
#include <vector>
#include "audio_buffer.hpp"
#include "audio_node.hpp"
using namespace std;
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
    public:
        explicit AudioSourceNode(AudioContext *ctx):AudioNode(ctx){}
        virtual uint64_t Process(AudioBuffer *out) = 0;
    };
}