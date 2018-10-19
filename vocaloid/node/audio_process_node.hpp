#pragma once
#include <stdint.h>
#include <vector>
#include "audio_node.hpp"
using namespace std;
namespace vocaloid{
    class AudioProcessNode: public AudioNode{
    public:
        explicit AudioProcessNode(){}
        virtual uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) = 0;
    };
}