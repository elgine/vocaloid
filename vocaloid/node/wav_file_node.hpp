#pragma once
#include "vocaloid/io/wav.hpp"
#include "vocaloid/node/audio_file_node.hpp"
namespace vocaloid{
    class WavFileNode: public AudioFileNode{
    public:
        explicit WavFileNode(AudioContext *ctx):AudioFileNode(ctx){
            reader_ = new WAVReader();
        }
    };
}