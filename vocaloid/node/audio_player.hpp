#pragma once
#include <pcm_player/pcm_player.h>
#include <memory>
#include <stdint.h>
#include "audio_output_node.hpp"
#include "vocaloid/base/conversion.hpp"
using namespace std;
namespace vocaloid{

    class AudioPlayer: public AudioOutputNode{
    private:
        PCMPlayer *inner_player_;
        uint32_t sample_rate_;
        uint16_t bits_;
        uint16_t channels_;
    public:
        explicit AudioPlayer(AudioContext *ctx,
                            uint32_t sample_rate,
                            uint16_t bits,
                            uint16_t channels):AudioOutputNode(ctx),
                                                sample_rate_(sample_rate),
                                                bits_(bits),
                                                channels_(channels){
            inner_player_ = new PCMPlayer();
            inner_player_->Open(sample_rate, bits, channels);
        }

        uint64_t Process(vector<float> in, uint64_t len) override {

        }
    };
}