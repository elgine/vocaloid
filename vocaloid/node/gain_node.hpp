#pragma once
#include "audio_node.hpp"
#include "audio_param.hpp"
namespace vocaloid{

    class GainNode: public AudioNode{
    public:
        AudioParam *gain_;

        explicit GainNode(AudioContext *ctx, float v = 1.0f):AudioNode(ctx){
            gain_ = new AudioParam();
            gain_->value_ = v;
        }

        int64_t Process(AudioBuffer *in) override {
            gain_->ComputingValues();
            for(auto i = 0;i < frame_size_;i++){
                auto value = gain_->ResultBuffer()->Data()[i];
                if(value < 0)value = 0;
                for(auto j = 0;j < channels_;j++){
                    in->Channel(j)->Data()[i] = value * summing_buffer_->Channel(j)->Data()[i];
                }
            }
            return frame_size_;
        }
    };
}