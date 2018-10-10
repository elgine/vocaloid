#pragma once
#include <stdint.h>
#include <vector>
#include "channel.hpp"
using namespace std;
namespace vocaloid{
    class AudioFrame{
    private:
        vector<float> data_[MAX_CHANNEL_COUNT];
        uint64_t max_size_;
    public:
        uint16_t channel_count_;
        uint64_t sample_count_;

        static void Mix(AudioFrame *output, vector<AudioFrame*> frames){
            uint16_t channel_count = output->channel_count_;
            uint64_t sample_count = output->sample_count_;
            AudioFrame temp_frame(sample_count, channel_count);
            output->Fill(0.0);
            for(auto frame : frames){
                MixChannel(frame->channel_count_, temp_frame.channel_count_, frame->GetData(), sample_count, temp_frame.GetData());
                for(int i = 0;i < channel_count;i++){
                    for(int j = 0;j < sample_count;j++){
                        (*output)[i][j] += temp_frame[i][j];
                    }
                }
            }
        }

        explicit AudioFrame(uint64_t size, uint16_t channel_count = 2):max_size_(0),channel_count_(channel_count), sample_count_(size){
            Alloc(size);
        }

        void Alloc(uint64_t size){
            if(max_size_ > size)return;
            for(int i = 0;i < channel_count_;i++){
                data_[i].reserve(size);
                for(uint64_t j = max_size_;j < size;j++)
                    data_[i].push_back(0);
            }
            max_size_ = size;
        }

        void Fill(float data){
            for(int i = 0;i < channel_count_;i++){
                fill(data_[i].begin(), data_[i].begin() + sample_count_, data);
            }
        }

        void Fill(int index, float data){
            fill(data_[index].begin(), data_[index].begin() + sample_count_, data);
        }

        AudioFrame& Copy(AudioFrame frame){
            uint16_t channel_count = frame.channel_count_;
            uint64_t size = frame.sample_count_;
            if(channel_count > channel_count_){
                channel_count_ = channel_count;
                Alloc(size);
            }
            for(int i = 0;i < channel_count;i++){
                data_[i].assign(frame[i].begin(), frame[i].begin() + frame.sample_count_);
            }
            return *this;
        }

        void FromByteArray(const char *byte_array, uint16_t bits, uint64_t byte_length){
            uint16_t depth = bits / 8;
            uint16_t step = depth * channel_count_;
            uint64_t new_buffer_size = byte_length / step;
            float max = powf(2.0f, bits - 1);
            if(max_size_ < new_buffer_size){
                Alloc(new_buffer_size);
            }
            for(int i = 0;i < byte_length;i += step){
                for(int j = 0;j < channel_count_;j++){
                    long offset = i + j * depth;
                    long value = byte_array[offset] & 0xFF;
                    for(int k = 1;k < depth;k++){
                        value |= (long)(byte_array[offset + k] << (k * 8));
                    }
                    data_[j][i / step] = value/max;
                }
            }
        }

        void ToByteArray(char *byte_array, uint16_t bits, uint64_t &byte_length){
            uint16_t depth = bits / 8;
            uint16_t step = depth * channel_count_;
            byte_length = step * sample_count_;
            float max = powf(2.0f, bits - 1);
            for(int i = 0;i < sample_count_;i++) {
                for (int j = 0; j < channel_count_; j++) {
                    float clipped = fmaxf(-1.0f, data_[j][i]);
                    clipped = fminf(1.0f, clipped);
                    long value = long(clipped * max);
                    for (int k = 0; k < depth; k++) {
                        byte_array[i * step + j * depth + k] = (char)((value >> 8 * k) & 0xFF);
                    }
                }
            }
        }

        uint64_t GetMaxSize(){
            return max_size_;
        }

        vector<float>* GetData(){
            return data_;
        }

        AudioFrame& operator= (AudioFrame frame){
            return Copy(frame);
        }

        vector<float>& operator[] (int index){
            return data_[index];
        }
    };
}