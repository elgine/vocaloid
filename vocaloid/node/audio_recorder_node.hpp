#pragma once
#include <string>
#include "audio_context.hpp"
#include "audio_destination_node.hpp"
#include "vocaloid/io/audio_file.h"
using namespace std;
namespace vocaloid{
    class AudioRecorderNode: public AudioDestinationNode{
    private:
        AudioFileWriter *writer_;
        string path_;
    public:
        explicit AudioRecorderNode(AudioContext *ctx):AudioDestinationNode(ctx){
            writer_ = nullptr;
        }

        void Initialize() override {
            writer_->Open(Path(), sample_rate_, AudioContext::BITS_PER_SEC, channels_);
        }

        void Close() override {
            writer_->Close();
        }

        void SetPath(const char* p){
            path_ = p;
        }

        const char* Path(){
            return path_.c_str();
        }

        void Push(AudioBuffer *in) override {
            uint64_t byte_len = in->Channels() * AudioContext::BITS_PER_SEC / 8;
            auto bytes = new char[byte_len];
            in->ToByteArray(AudioContext::BITS_PER_SEC, bytes, byte_len);
            writer_->WriteData(bytes, byte_len);
            delete[] bytes;
            bytes = nullptr;
        }
    };
}