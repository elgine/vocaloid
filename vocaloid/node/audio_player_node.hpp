#pragma once
#include <memory>
#include <stdint.h>
#include "audio_destination_node.hpp"
#include "audio_context.hpp"
#include <pcm_player/pcm_player.h>
using namespace std;
namespace vocaloid{

    class AudioPlayerNode: public AudioDestinationNode{
    private:
        PCMPlayer *player_ = nullptr;
    public:
        explicit AudioPlayerNode(AudioContext *ctx):AudioDestinationNode(ctx){
            player_ = new PCMPlayer();
        }

        void Initialize() override {
            player_->Open(sample_rate_, AudioContext::BITS_PER_SEC, channels_);
        }

        void Close() override {
            player_->Flush();
            player_->Close();
        }

        void Push(AudioBuffer *in) override {
            uint64_t byte_len = in->Channels() * AudioContext::BITS_PER_SEC / 8;
            auto bytes = new char[byte_len];
            in->ToByteArray(AudioContext::BITS_PER_SEC, bytes, byte_len);
            player_->Push(bytes, byte_len);
            delete[] bytes;
            bytes = nullptr;
        }
    };
}