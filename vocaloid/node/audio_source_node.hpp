#pragma once
#include "audio_node.hpp"
namespace vocaloid{
    class AudioSourceNode: public AudioNode{
    public:
        explicit AudioSourceNode(AudioContext *ctx):AudioNode(ctx){}

        void SetChannels(uint16_t c) final {}
        void ConnectInput(AudioNode *n) final {}
        void DisconnectInput(const AudioNode *n) final {}

        void Pull(AudioBuffer *in) final {
            Process(in);
        }
    };
}