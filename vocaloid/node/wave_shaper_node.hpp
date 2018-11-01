#pragma once
#include <math.h>
#include "audio_node.hpp"
#include "audio_context.hpp"
namespace vocaloid{

    class WaveShaperNode: public AudioNode{
    private:
        vector<float> curve_;
        uint64_t curve_length_;
    public:
        explicit WaveShaperNode(AudioContext *ctx):AudioNode(ctx){
            curve_length_ = 0;
        }

        void SetCurve(vector<float> &c){
            curve_ = c;
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            curve_length_ = curve_.size();
        }

        int64_t Process(AudioBuffer *in) override {
            for(uint16_t i = 0;i < channels_;i++){
                for(auto j = 0;j < frame_size_;j++){
                    auto input = summing_buffer_->Channel(i)->Data()[j];
                    // Calculate an index based on input -1 -> +1 with 0 being at the center of the curve data.
                    auto index = uint64_t((curve_length_ * (input + 1)) / 2);
                    // Clip index to the input range of the curve.
                    // This takes care of input outside of nominal range -1 -> +1
                    index = max(index, uint64_t(0));
                    index = min(index, curve_length_ - 1);
                    in->Channel(i)->Data()[j] = curve_[index];
                }
            }
            return frame_size_;
        }

        uint64_t CurveLength(){
            return curve_length_;
        }
    };
}