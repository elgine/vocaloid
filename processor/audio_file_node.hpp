#pragma once
#include <memory>
#include <list>
#include "audio_source_node.hpp"
#include "buffer_queue.hpp"
#include "file.h"
using namespace std;
namespace vocaloid{

    class AudioFileNode: public AudioSourceNode{
    protected:
        IFileReader *reader_;
        string source_;
        list<string> filter_exts_;

        bool FilterSource(const char* str){
            for(auto &ext : filter_exts_){
                if(ext.find_last_of(str) > -1){
                    return true;
                }
            }
            return false;
        }

    public:

        explicit AudioFileNode(AudioContext *ctx, const char* source = nullptr):AudioSourceNode(ctx){
            reader_ = nullptr;
            SetSource(source);
        }

        void SetSource(const char* source){
            if(!source && source_ != source && FilterSource(source)){
                source_ = source;
                reader_->Close();
            }
        }

        int Prepare(){
            return reader_->Open(source_.c_str());
        }

        int Process(Buffer *out) {
            if(reader_->IsEnd())return 0;
            uint64_t len = out->GetBufferSize() * out->GetChannels() * out->GetBits() / 8;
            std::unique_ptr<char*> source_data = std::make_unique<char*>(new char[len]);
            reader_->ReadData(*source_data, len);
            out->FromByteArray(*source_data, len);
            return len;
        }

        void Dispose() override {
            AudioNode::Dispose();
            reader_->Close();
        }
    };
}