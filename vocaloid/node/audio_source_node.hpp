#pragma once
#include "audio_node.hpp"
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
    public:
        explicit AudioSourceNode(AudioContext *ctx):AudioNode(ctx){
            can_be_connected_ = false;
        }

        void Pull(AudioBuffer *in) final {
            in->Alloc(channels_, frame_size_);
            Process(in);
        }
    };
}