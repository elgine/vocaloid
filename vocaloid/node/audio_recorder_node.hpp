#pragma once
#include <string>
#include "audio_destination_node.hpp"
#include "vocaloid/io/audio_file.h"
#include "vocaloid/io/wav.hpp"
using namespace std;
namespace vocaloid{
    class AudioRecorderNode: public AudioDestinationNode{
    private:
        AudioFileWriter *writer_;
        string path_;
    public:
        explicit AudioRecorderNode(AudioContext *ctx):AudioDestinationNode(ctx){
            writer_ = new WAVWriter();
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            writer_->Open(Path(), sample_rate_, BITS_PER_SEC, channels_);
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

        int64_t PushToDestination() override {
            uint64_t size = summing_buffer_->Size();
            uint64_t byte_len = size * summing_buffer_->Channels() * BITS_PER_SEC / 8;
            auto bytes = new char[byte_len];
            summing_buffer_->ToByteArray(BITS_PER_SEC, bytes, byte_len);
            writer_->WriteData(bytes, byte_len);
            delete[] bytes;
            bytes = nullptr;
            return size;
        }
    };
}