#pragma once
#include <stdint.h>
#include <vector>
#include "audio_node.hpp"
using namespace std;
namespace vocaloid{
    class AudioOutputNode: public AudioNode{
    public:
        explicit AudioOutputNode(AudioContext *ctx):AudioNode(ctx){}
        virtual uint64_t Process(vector<float> in, uint64_t len) = 0;
    };
}