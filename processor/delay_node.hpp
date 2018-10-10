#pragma once
#include "audio_process_node.hpp"
namespace vocaloid{

    class DelayNode: public AudioProcessNode{
    private:
        // Delay duration, in million seconds
        uint64_t delay_;
        // The data offset
        uint64_t offset_;
        // Temporary buffer for input buffer
        Buffer *echo_buffer_;
        // The offset position of echo_buffer
        uint64_t offset_buffer_;
    public:
        explicit DelayNode(AudioContext *ctx, uint64_t delay = 0):AudioProcessNode(ctx), delay_(delay){}

        int Prepare(){
            offset_ = delay_ * 0.001 * ctx_->GetSampleRate();
        }

        int Process(Buffer *in, Buffer *out){

        }

        void Dispose() override {

        }
    };
}