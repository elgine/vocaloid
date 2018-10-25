#pragma once
#include "audio_node.hpp"
#include "audio_param.hpp"
namespace vocaloid{

    class GainNode2: public AudioNode{
    private:
        AudioParam *gain_;
    public:



        AudioParam* Gain(){
            return gain_;
        }
    };
}