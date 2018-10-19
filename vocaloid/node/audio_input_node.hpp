#pragma once
#include <stdint.h>
#include <vector>
#include "audio_node.hpp"
using namespace std;
namespace vocaloid{
    class AudioInputNode: public AudioNode{
    public:
        explicit AudioInputNode(AudioContext *ctx):AudioNode(ctx){}
        virtual uint64_t Process(vector<float> &out) = 0;
    };
}