#pragma once
#include <math.h>
#include <vector>
#include <stdint.h>
using namespace std;
namespace vocaloid{

    class Buffer{

    private:

        vector<vector<float>> data_;

        int channels_;
        int bits_;
        float sample_rate_;

        int buffer_size_;

    public:

        Buffer(float sample_rate, int bits, int channels){
            sample_rate_ = sample_rate;
            bits_ = bits;
            channels_ = channels;
            buffer_size_ = 0;
            data_ = vector<vector<float>>(8);
        }

        void Initialize(float sample_rate, int bits, int channels){
            sample_rate_ = sample_rate;
            bits_ = bits;
            channels_ = channels;
        }

        vector<float> GetChannel(int index){
            return data_[index];
        }

        float GetSampleRate(){
            return sample_rate_;
        }

        int GetBits(){
            return bits_;
        }

        double GetDuration(){
            return buffer_size_ * GetChannelCount() / sample_rate_ / GetBlockAlign();
        }

        int GetBlockAlign(){
            return bits_/8 * channels_;
        }

        int GetChannelCount(){
            return channels_;
        }

        int GetBufferSize(){
            return buffer_size_;
        }

        void CopyFrom(Buffer *b){
            Initialize(b->GetSampleRate(), b->GetBits(), b->GetChannelCount());
        }

        int FromPCMData(uint32_t sample_rate, uint16_t bits, uint16_t channels, const uint8_t* bytes, uint64_t length){
            Initialize(sample_rate, bits, channels);
            return FromPCMData(bytes, length);
        }

        int FromPCMData(const uint8_t *bytes, int length){
            int depth = bits_ / 8;
            int output_index = 0;
            int output_len = (int)floorf(float(length - length%depth) / depth);
            float range = powf(2, bits_ - 1);
            int step = depth * channels_;
            for(auto i = 0;i < length;i += step){
                for(auto j = 0;j < channels_;j++){
                    data_[j].reserve(output_len);
                    int offset = i + j * depth;
                    long v = bytes[offset];
                    for(auto k = 1;k < depth;k++){
                        v |= bytes[offset + k] << (k * 8);
                    }
                    data_[j][output_index] = v/range;
                }
                output_index++;
            }
            buffer_size_ = output_len;
            return output_len;
        }

        uint64_t ToPCMData(uint8_t *bytes, bool planar = false){
            int depth = bits_ / 8;
            int step = depth * channels_;
            float range = powf(2, bits_ - 1);
            int index = 0;
            if(!planar){
                for(int i = 0;i < buffer_size_ ;i++){
                    for(int j = 0;j < channels_;j++){
                        long value = data_[j][i] * range;
                        for(int k = 0;k < depth;k++){
                            bytes[ i * step + j * depth + k] = value >> (8 * k) & 0xFF;
                        }
                    }
                }
            }else{
                for (int j = 0; j < channels_; j++) {
                    for(int i = 0;i < buffer_size_ ;i++) {
                        long value = data_[j][i] * range;
                        for(int k = 0;k < depth;k++){
                            bytes[index++] = value >> (8 * k) & 0xFF;
                        }
                    }
                }
            }
            return buffer_size_ * step;
        }
    };
}