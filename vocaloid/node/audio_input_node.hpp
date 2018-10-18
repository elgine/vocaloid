#pragma once
#include "audio_node.hpp"
namespace vocaloid{

    class AudioInputNode: public AudioNode{
    public:
        explicit AudioInputNode():AudioNode(){}
        virtual uint64_t Process(vector<float> &out) = 0;
    };
}