#pragma once
#include <stdint.h>
#include <vector>
#include "audio_node.hpp"
#include "audio_buffer.hpp"
#include "vocaloid/io/audio_file.h"
using namespace std;
namespace vocaloid{

    enum AudioOutputType{
        PLAYER,
        RECORDER
    };

    class AudioOutputNode: public AudioNode{
    protected:
        AudioOutputType type_;
    public:
        explicit AudioOutputNode(AudioContext *ctx):AudioNode(ctx),type_(AudioOutputType::PLAYER){}
        virtual uint64_t Process(AudioBuffer *in, uint64_t len) = 0;
        virtual uint32_t SampleRate() = 0;

        AudioOutputType Type(){
            return type_;
        }
    };
}