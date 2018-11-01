#pragma once
#include "audio_node.hpp"
#include "vocaloid/dsp/convolution.hpp"
namespace vocaloid{

    class ConvolutionNode: public AudioNode{
    private:
        Convolution **convolutions_;
    public:
        vector<float> kernel_;

        explicit ConvolutionNode(AudioContext *ctx):AudioNode(ctx){
            convolutions_ = new Convolution*[8];
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            for(auto i = 0;i < channels_;i++){
                convolutions_[i] = new Convolution(frame_size);
                convolutions_[i]->Initialize(kernel_, kernel_.size());
            }
        }

        int64_t Process(AudioBuffer *in) override {
            for(auto i = 0;i < channels_;i++){
                convolutions_[i]->Process(summing_buffer_->Channel(i)->Data(), summing_buffer_->Size(), in->Channel(i)->Data())
            }
            return frame_size_;
        }
    };
}