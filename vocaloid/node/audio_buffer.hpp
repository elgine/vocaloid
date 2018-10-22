#pragma once
#include <stdint.h>
#include <math.h>
#include "vocaloid/utils/buffer.hpp"
#include "vocaloid/data/channel.h"
namespace vocaloid{

    class AudioBuffer{
    private:
        uint16_t channels_;
        Buffer<float> **data_;
    public:

        explicit AudioBuffer(uint16_t channels = 2, uint64_t max_size = 1024):channels_(channels){
            data_ = new Buffer<float>*[6];
            for(auto i = 0;i < channels;i++){
                data_[channels_++] = new Buffer<float>(max_size);
            }
        }

        ~AudioBuffer(){
            Dispose();
        }

        void Copy(AudioBuffer *b){
            if(channels_ != b->Channels())AllocChannels(b->Channels());
            if(Size() != b->Size())Alloc(b->Size());
            for(auto i = 0;i < b->Channels();i++){
                for(auto j = 0;j < b->Size();j++){
                    data_[i]->Data()[j] = b->Data()[i]->Data()[j];
                }
            }
        }

        void FromByteArray(const char* byte_array, uint64_t byte_length, uint16_t bits, uint16_t channels){
            uint16_t depth = bits / 8;
            uint16_t step = depth * channels;
            AllocChannels(channels);
            uint64_t len = byte_length / step;
            Alloc(len);
            float max = powf(2.0f, bits - 1);
            for(int i = 0;i < byte_length;i += step){
                for(int j = 0;j < channels;j++){
                    long offset = i + j * depth;
                    long value = byte_array[offset] & 0xFF;
                    for(int k = 1;k < depth;k++){
                        value |= (long)(byte_array[offset + k] << (k * 8));
                    }
                    data_[j]->Data()[i / step] = value/max;
                }
            }
        }

        void ToByteArray(uint16_t bits, char *byte_array, uint64_t &byte_length){
            uint16_t depth = bits / 8;
            uint16_t step = depth * channels_;
            uint64_t buffer_size = Size();
            byte_length = step * buffer_size;
            float max = powf(2.0f, bits - 1);
            for(int i = 0;i < buffer_size;i++) {
                for (int j = 0; j < channels_; j++) {
                    float clipped = data_[j]->Data()[i];
                    if(clipped > 0.999f)clipped = 0.999f;
                    else if(clipped < -0.999f)clipped = -0.999f;
                    auto value = (long)(clipped * max);
                    for (int k = 0; k < depth; k++) {
                        byte_array[i * step + j * depth + k] = (char)((value >> 8 * k) & 0xFF);
                    }
                }
            }
        }

        void Fill(float v){
            for(auto i = 0;i < channels_;i++){
                for(auto j = 0;j < Size();j++){
                    data_[i]->Data()[j] = v;
                }
            }
        }

        void Mix(AudioBuffer *in){
            uint16_t out_channels = channels_, in_channels = in->Channels();
            uint64_t size = in->Size();
            if(out_channels == in_channels){
                for(auto i = 0;i < channels_;i++){
                    for(auto j = 0;j < size;j++){
                        data_[i]->Data()[j] += in->Data()[i]->Data()[j];
                    }
                }
                return;
            }
            // Up mixing
            if(out_channels > in_channels){
                if(out_channels == CHANNEL_STEREO){
                    for(auto j = 0;j < size;j++){
                        data_[Channel::LEFT]->Data()[j] += data_[Channel::RIGHT]->Data()[j] = in->Data()[Channel::LEFT]->Data()[j];
                    }
                }else if(out_channels == CHANNEL_QUAD){
                    if(in_channels == CHANNEL_STEREO){
                        for(auto j = 0;j < size;j++){
                            data_[Channel::LEFT]->Data()[j] += in->Data()[Channel::LEFT]->Data()[j];
                            data_[Channel::RIGHT]->Data()[j] += in->Data()[Channel::RIGHT]->Data()[j];
                        }
                    }else{
                        for(auto j = 0;j < size;j++){
                            data_[Channel::LEFT]->Data()[j] += data_[Channel::RIGHT]->Data()[j] = in->Data()[Channel::LEFT]->Data()[j];
                        }
                    }
                }else if(out_channels == CHANNEL_5_1){
                    if(in_channels == CHANNEL_QUAD){
                        for(auto j = 0;j < size;j++){
                            data_[Channel::LEFT]->Data()[j] += in->Data()[Channel::LEFT]->Data()[j];
                            data_[Channel::RIGHT]->Data()[j] += in->Data()[Channel::RIGHT]->Data()[j];
                            data_[Channel::SURROUND_LEFT]->Data()[j] += in->Data()[Channel::SURROUND_LEFT]->Data()[j];
                            data_[Channel::SURROUND_RIGHT]->Data()[j] += in->Data()[Channel::SURROUND_RIGHT]->Data()[j];
                        }
                    }else if(in_channels == CHANNEL_STEREO){
                        for(auto j = 0;j < size;j++){
                            data_[Channel::LEFT]->Data()[j] += in->Data()[Channel::LEFT]->Data()[j];
                            data_[Channel::RIGHT]->Data()[j] += in->Data()[Channel::RIGHT]->Data()[j];
                        }
                    }else{
                        for(auto j = 0;j < size;j++){
                            data_[Channel::CENTER]->Data()[j] += in->Data()[Channel::LEFT]->Data()[j];
                        }
                    }
                }
            }
            // Down mixing
            else{
                if(in_channels == CHANNEL_STEREO){
                    for(auto j = 0;j < size;j++){
                        data_[Channel::LEFT]->Data()[j] += 0.5f * (in->Data()[Channel::LEFT]->Data()[j] + in->Data()[Channel::RIGHT]->Data()[j]);
                    }
                }else if(in_channels == CHANNEL_QUAD){
                    if(out_channels == CHANNEL_MONO){
                        for(auto j = 0;j < size;j++){
                            data_[Channel::LEFT]->Data()[j] += 0.25f * (in->Data()[Channel::LEFT]->Data()[j] +
                                    in->Data()[Channel::RIGHT]->Data()[j] +
                                    in->Data()[Channel::SURROUND_LEFT]->Data()[j] +
                                    in->Data()[Channel::SURROUND_RIGHT]->Data()[j]);
                        }
                    }else{
                        for(auto j = 0;j < size;j++){
                            data_[Channel::LEFT]->Data()[j] += 0.5f * (in->Data()[Channel::LEFT]->Data()[j] + in->Data()[Channel::SURROUND_LEFT]->Data()[j]);
                            data_[Channel::RIGHT]->Data()[j] += 0.5f * (in->Data()[Channel::RIGHT]->Data()[j] + in->Data()[Channel::SURROUND_RIGHT]->Data()[j]);
                        }
                    }
                }else{
                    if(out_channels == CHANNEL_MONO){
                        for(auto j = 0;j < size;j++) {
                            data_[Channel::LEFT]->Data()[j] +=
                                        0.7071 * (in->Data()[Channel::LEFT]->Data()[j] + in->Data()[Channel::LEFT]->Data()[j]) +
                                        in->Data()[Channel::CENTER]->Data()[j] +
                                        0.5 * (in->Data()[Channel::SURROUND_LEFT]->Data()[j] + in->Data()[Channel::SURROUND_RIGHT]->Data()[j]);
                        }
                    }else if(out_channels == CHANNEL_STEREO){
                        for(auto j = 0;j < size;j++) {
                            data_[Channel::LEFT]->Data()[j] += in->Data()[Channel::LEFT]->Data()[j] + 0.7071 * (in->Data()[Channel::CENTER]->Data()[j] + in->Data()[Channel::SURROUND_LEFT]->Data()[j]);
                            data_[Channel::RIGHT]->Data()[j] += in->Data()[Channel::RIGHT]->Data()[j] + 0.7071 * (in->Data()[Channel::CENTER]->Data()[j] + in->Data()[Channel::SURROUND_RIGHT]->Data()[j]);
                        }
                    }else{
                        for(auto j = 0;j < size;j++) {
                            data_[Channel::LEFT]->Data()[j] += in->Data()[Channel::LEFT]->Data()[j] + 0.7071 * (in->Data()[Channel::CENTER]->Data()[j]);
                            data_[Channel::RIGHT]->Data()[j] += in->Data()[Channel::RIGHT]->Data()[j] + 0.7071 * (in->Data()[Channel::CENTER]->Data()[j]);
                            data_[Channel::SURROUND_LEFT]->Data()[j] += in->Data()[Channel::SURROUND_LEFT]->Data()[j];
                            data_[Channel::SURROUND_RIGHT]->Data()[j] += in->Data()[Channel::SURROUND_RIGHT]->Data()[j];
                        }
                    }
                }
            }
        }

        void AllocChannels(uint16_t channels){
            if(channels_ < channels){
                for(auto i = channels_;i < channels;i++){
                    data_[i] = new Buffer<float>(Size());
                }
            }
            channels_ = channels;
        }

        void Alloc(uint64_t size){
            for(auto i = 0;i < channels_;i++){
                data_[i]->Alloc(size);
            }
        }

        void Dispose(){
            for(auto i = 0;i < channels_;i++){
                data_[i]->Dispose();
            }
            delete[] data_;
        }

        uint16_t Channels(){
            return channels_;
        }

        uint64_t Size(){
            if(channels_ > 0)
                return data_[0]->Size();
            return 0;
        }

        Buffer<float>* Channel(uint16_t index){
            return data_[index];
        }

        Buffer<float>** Data(){
            return data_;
        }
    };
}