#pragma once
#include "audio_node.hpp"
namespace vocaloid{

    class AudioOutputNode: public AudioNode{
    public:
        explicit AudioOutputNode():AudioNode(){}
        virtual uint64_t Process(vector<float> in, uint64_t len) = 0;
    };
}