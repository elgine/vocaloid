#pragma once
#include <stdint.h>
#include "audio_node.hpp"
#include "audio_output_node.hpp"
#include "audio_player.hpp"
namespace vocaloid{

    enum AudioContextOutputMode{
        PLAYER,
        ENCODER
    };

    class AudioContext{
    private:
        AudioContextOutputMode output_mode_;
        AudioOutputNode *output_;

        void DetectAudioDeviceInfo(){

        }
    public:

        void SetPlayerMode(uint32_t sample_rate, uint16_t bits, uint16_t channels){
            output_mode_ = AudioContextOutputMode::PLAYER;
            output_ = new AudioPlayer(this, sample_rate, bits, channels);
        }

        void SetEncoderMode(uint32_t sample_rate, uint16_t bits, uint16_t channels){
            output_mode_ = AudioContextOutputMode::ENCODER;
        }

        void Dispose(){

        }

        AudioOutputNode* OutputNode(){
            return output_;
        }
    };
}