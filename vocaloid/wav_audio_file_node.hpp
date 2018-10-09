#pragma once
#include <string>
#include "wav.hpp"
#include "audio_context_interface.h"
#include "audio_file_node.hpp"

namespace vocaloid{

    class WAVAudioFileNode: public AudioFileNode{
    public:
        explicit WAVAudioFileNode(IAudioContext *ctx, const char* source = nullptr):AudioFileNode(ctx, source){
            filter_exts_ = {".wav"};
            reader_ = new wav::WAVReader();
        }
    };
}