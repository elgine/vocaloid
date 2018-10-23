#pragma once
#include "audio_node.hpp"
#include "vocaloid/synthesis/gain.hpp"
#include "audio_context.hpp"
namespace vocaloid{

    class GainNode: public AudioNode{
    private:
        Gain *gain_;
    public:
        explicit GainNode(AudioContext *ctx, float init_value = 1.0f):AudioNode(ctx){
            gain_ = new Gain();
            gain_->value = init_value;
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            gain_->Initialize(context_->GetSampleRate(), BITS_PER_SEC);
        }

        int64_t Process(AudioBuffer *in) override {
            for(auto i = 0;i < channels_;i++){
                gain_->Process(summing_buffer_->Channel(i)->Data(), frame_size_, in->Channel(i)->Data());
            }
            gain_->Offset(frame_size_);
            return frame_size_;
        }

        Gain* GetGain(){
            return gain_;
        }
    };
}