#pragma once
#include <stdint.h>
#include "audio_node.hpp"
namespace vocaloid{

    enum AudioGraphOutputMode{
        PLAYER,
        ENCODER
    };

    class AudioGraph{
    private:
        AudioGraphOutputMode output_mode_;

        void DetectAudioDeviceInfo(){

        }
    public:
        explicit AudioGraph(){
            SetPlayerMode();
        }

        void SetPlayerMode(){
            output_mode_ = AudioGraphOutputMode::PLAYER;
        }

        void SetEncoderMode(uint32_t sample_rate, uint16_t bits, uint16_t channels){

            output_mode_ = AudioGraphOutputMode::ENCODER;
        }

        void Connect(AudioNode *from, AudioNode *to){

        }

        void Disconnect(AudioNode *from, AudioNode *to){

        }

        void Dispose(){

        }
    };
}