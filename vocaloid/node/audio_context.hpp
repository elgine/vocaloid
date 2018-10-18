#pragma once
#include <stdint.h>
#include "audio_node.hpp"
#include "audio_output_node.hpp"
namespace vocaloid{

    enum AudioContextOutputMode{
        PLAYER,
        ENCODER
    };

    class AudioContext{
    private:
        AudioContextOutputMode output_mode_;
        AudioOutputNode output_;

        void DetectAudioDeviceInfo(){

        }
    public:
        explicit AudioContext(){
            SetPlayerMode();
        }

        void SetPlayerMode(){
            output_mode_ = AudioGraphOutputMode::PLAYER;
        }

        void SetEncoderMode(uint32_t sample_rate, uint16_t bits, uint16_t channels){

            output_mode_ = AudioGraphOutputMode::ENCODER;
        }

        void Dispose(){

        }

        AudioOutputNode& OutputNode(){
            return output_;
        }
    };
}