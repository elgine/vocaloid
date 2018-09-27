#pragma once
#include "audio_source_node.hpp"
#include "buffer_queue.hpp"
#include "reader.h"
namespace vocaloid{

    class AudioFileNode: public AudioSourceNode{
    private:
        IReader reader_;
        string source_;
        BufferQueue *buffer_queue_;
    public:

        AudioFileNode(IAudioContext *ctx, const char* source = nullptr):AudioSourceNode(ctx){
            SetSource(source);
        }

        void SetSource(const char* source){
            if(!source && source_ != source){
                source_ = source;
                reader_->Close();
                reader_->Open(source_.c_str());
            }
        }

        uint32_t Load(){

        }

        void Process(Buffer *out){

        }
    };
}