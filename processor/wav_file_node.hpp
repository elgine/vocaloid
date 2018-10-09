#pragma once
#include <string>
#include "wav.hpp"
#include "audio_file_node.hpp"
namespace vocaloid{

    class WAVFileNode: public AudioFileNode{
    public:
        explicit WAVFileNode(AudioContext *ctx, const char* source = nullptr):AudioFileNode(ctx, source){
            filter_exts_ = {".wav"};
            reader_ = new wav::WAVReader();
        }
    };
}