#pragma once
#include <memory>
#include <stdint.h>
#include "audio_destination_node.hpp"
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

        AudioPlayerNode(const AudioPlayerNode &n) = delete;
        AudioPlayerNode & operator=(const AudioPlayerNode & n) = delete;

        ~AudioPlayerNode(){
            player_ = nullptr;
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            player_->Open(sample_rate_, BITS_PER_SEC, channels_);
        }

        void Close() override {
            player_->Flush();
            player_->Close();
        }

        int64_t PushToDestination() override {
            uint64_t size = summing_buffer_->Size();
            uint64_t byte_len = size * summing_buffer_->Channels() * BITS_PER_SEC / 8;
            auto bytes = new char[byte_len];
            summing_buffer_->ToByteArray(BITS_PER_SEC, bytes, byte_len);
            player_->Push(bytes, byte_len);
            delete[] bytes;
            bytes = nullptr;
            return size;
        }
    };
}