#pragma once
#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "disposable.h"
using namespace std;
namespace vocaloid{

    class Buffer: IDisposable{
    friend class BufferQueue;
    private:
        static const uint16_t max_channels = 6;
        vector<float> *data_;
        uint64_t buffer_size_;

        uint16_t channels_ = 2;
        uint16_t bits_ = 16;
        uint32_t sample_rate_ = 44100;
    public:

        explicit Buffer(uint32_t buffer_size = 1024):buffer_size_(buffer_size){
            data_ = new vector<float>[max_channels];
            for(int i = 0;i < max_channels;i++){
                data_[i].resize(buffer_size_);
            }
        }

        void Initialize(uint32_t sample_rate, uint16_t bits, uint16_t channels){
            channels_ = channels;
            bits_ = bits;
            sample_rate_ = sample_rate;
        }

        void Copy(Buffer *another){
            channels_ = another->GetChannels();
            buffer_size_ = another->GetBufferSize();
            for(int i = 0;i < channels_;i++){
                auto channel_data = another->GetChannelAt(i);
                data_[i].assign(channel_data.begin(), channel_data.end());
            }
        }

        void Set(vector<float> *data, uint32_t buffer_size){
            buffer_size_ = buffer_size;
            for(int i = 0;i < channels_;i++){
                if(data_[i].size() != buffer_size_)
                    data_[i].resize(buffer_size_);
                data_[i].assign(data[i].begin(), data[i].end());
            }
        }

        void FromByteArray(const char *byte_array, uint64_t byte_length){
            uint16_t depth = bits_ / 8;
            uint16_t step = depth * channels_;
            uint64_t new_buffer_size = byte_length / step;
            float max = powf(2.0f, bits_ - 1);
            if(buffer_size_ < new_buffer_size){
                for(int i = 0;i < channels_;i++){
                    data_[i].resize(new_buffer_size);
                }
            }
            buffer_size_ = new_buffer_size;
            for(int i = 0;i < byte_length;i += step){
                for(int j = 0;j < channels_;j++){
                    long offset = i + j * depth;
                    long value = byte_array[offset] & 0xFF;
                    for(int k = 1;k < depth;k++){
                        value |= (long)(byte_array[offset + k] << (k * 8));
                    }
                    data_[j][i / step] = value/max;
                }
            }
        }

        void ToByteArray(char *byte_array, uint64_t &byte_length){
            uint16_t depth = bits_ / 8;
            uint16_t step = depth * channels_;
            byte_length = step * buffer_size_;
            float max = powf(2.0f, bits_ - 1);
            for(int i = 0;i < buffer_size_;i++) {
                for (int j = 0; j < channels_; j++) {
                    float clipped = fmaxf(-1.0f, data_[j][i]);
                    clipped = fminf(1.0f, clipped);
                    long value = long(clipped * max);
                    for (int k = 0; k < depth; k++) {
                        byte_array[i * step + j * depth + k] = (char)((value >> 8 * k) & 0xFF);
                    }
                }
            }
        }

        void PushFloatArray(vector<float> *float_array, uint64_t array_length, uint64_t offset = 0){
            for(int i = 0;i < channels_;i++){
                for(int j = 0;j < array_length;j++){
                    data_[i].push_back(float_array[i][offset + j]);
                }
            }
        }

        void PushByteArray(const char *byte_array, uint64_t array_length, uint64_t offset = 0){
            uint16_t depth = bits_ / 8;
            uint16_t step = depth * channels_;
            float max = powf(2.0f, bits_ - 1);
            for(int i = 0;i < array_length;i += step){
                for(int j = 0;j < channels_;j++){
                    uint64_t index = i + offset + j * depth;
                    long value = byte_array[index] & 0xFF;
                    for(int k = 1;k < depth;k++){
                        value |= long(byte_array[index + k] << (k * 8));
                    }
                    data_[j].push_back(value/max);
                }
            }
            buffer_size_ += array_length / step;
        }

        vector<float>* GetData(){
            return data_;
        }

        vector<float> GetChannelAt(int channel){
            return data_[channel];
        }

        uint64_t GetBufferSize(){
            return buffer_size_;
        }

        uint16_t GetChannels(){
            return channels_;
        }

        uint16_t GetBits(){
            return bits_;
        }

        uint32_t GetSampleRate(){
            return sample_rate_;
        }

        void Dispose() override{
            for(int i = 0;i < max_channels;i++){
                data_[i].clear();
            }
        }
    };
}