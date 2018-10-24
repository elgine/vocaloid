#pragma once
#include "audio_node.hpp"
#include "audio_context.hpp"
#include "vocaloid/synthesis/delay.hpp"
namespace vocaloid{
    class DelayNode: public AudioNode{
    private:
        Delay** delays_;
        uint64_t delay_duration_;
    public:
        explicit DelayNode(AudioContext *ctx):AudioNode(ctx){
            delay_duration_ = 0;
            delays_ = new Delay*[8]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
        }

        void SetDelayDuration(uint64_t v){
            delay_duration_ = v;
            for(auto i = 0;i < channels_;i++){
                if(delays_[i] != nullptr){
                    delays_[i]->SetDelayDuration(delay_duration_);
                }
            }
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            for(auto i = 0;i < channels_;i++){
                if(delays_[i] == nullptr){
                    delays_[i] = new Delay(delay_duration_, context_->GetSampleRate());
                }
                delays_[i]->Clear();
            }
        }

        int64_t Process(AudioBuffer *in) override {
            for(auto i = 0;i < channels_;i++){
                delays_[i]->Process(summing_buffer_->Data()[i]->Data(), frame_size_, in->Data()[i]->Data());
            }
            return frame_size_;
        }

        uint64_t DelayDuration(){
            return delay_duration_;
        }
    };
}