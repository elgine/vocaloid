#pragma once
#include <vector>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "disposable.h"
using namespace std;
namespace vocaloid{

    class Buffer: IDisposable{
    private:
        static const uint16_t max_channels = 6;
        vector<float> data_[max_channels];
        uint32_t buffer_size_;

        uint16_t channels_ = 2;
        uint16_t bits_ = 16;
        uint32_t sample_rate_ = 44100;
    public:

        Buffer(uint32_t buffer_size = 1024):buffer_size_(buffer_size){
            for(int i = 0;i < max_channels;i++){
                data_[i] = vector<float>(buffer_size_);
            }
        }

        void Initialize(uint32_t sample_rate, uint16_t bits, uint16_t channels){
            channels_ = channels;
            bits_ = bits;
            sample_rate_ = sample_rate;
        }

        void FromByteArray(uint8_t *byte_array, uint64_t byte_length){
            uint16_t depth = bits_ / 8;
            uint16_t step = depth * channels_;
            uint64_t data_index = 0;
            buffer_size_ = byte_length / step;
            float max = powf(2.0f, bits_ - 1);
            if(data_[0].size() < buffer_size_){
                for(int i = 0;i < channels_;i++){
                    data_[i].resize(buffer_size_);
                }
            }
            for(int i = 0;i < byte_length;i += step){
                for(int j = 0;j < channels_;j++){
                    uint64_t offset = i + j * depth;
                    long value = byte_array[offset];
                    for(int k = 1;k < depth;k++){
                        value |= byte_array[offset + k] << (k * 8);
                    }
                    data_[j][data_index] = (value - max)/max;
                }
                data_index++;
            }
        }

        void ToByteArray(uint8_t *byte_array, uint64_t &byte_length, bool planar = false){
            uint16_t depth = bits_ / 8;
            uint16_t step = depth * channels_;
            byte_length = step * buffer_size_;
            float max = powf(2.0f, bits_ - 1);
            if(!planar){
                for(int i = 0;i < buffer_size_;i++){
                    for(int j = 0;j < channels_;j++){
                        long value = data_[j][i] * max + max;
                        for(int k = 0;k < depth;k++){
                            byte_array[i * step + j * depth + k] = (value >> (8 * k)) & 0xFF;
                        }
                    }
                }
            }else{
                for(int j = 0;j < channels_;j++){
                    for(int i = 0;i < buffer_size_;i++){
                        long value = data_[j][i] * max + max;
                        uint64_t offset = j * buffer_size_;
                        for(int k = 0;k < depth;k++){
                            byte_array[offset + i * depth + k] = (value >> (8 * k)) & 0xFF;
                        }
                    }
                }
            }
        }

        void AddFloatArray(vector<float> *float_array, uint64_t array_length, uint64_t offset = 0){
            for(int i = 0;i < channels_;i++){
                for(int j = 0;j < array_length;j++){
                    data_[i].push_back(float_array[i][offset + j]);
                }
            }
        }

        void AddByteArray(uint8_t *byte_array, uint64_t array_length, uint64_t offset = 0){
            uint16_t depth = bits_ / 8;
            uint16_t step = depth * channels_;
            float max = powf(2.0f, bits_ - 1);
            for(int i = 0;i < array_length;i += step){
                for(int j = 0;j < channels_;j++){
                    uint64_t index = i + offset + j * depth;
                    long value = byte_array[index];
                    for(int k = 1;k < depth;k++){
                        value |= byte_array[index + k] << (k * 8);
                    }
                    data_[j].push_back((value - max)/max);
                }
            }
            buffer_size_ += array_length / step;
        }

        vector<float>* GetData(){
            return data_;
        }

        vector<float> GetChannelAt(uint16_t channel){
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