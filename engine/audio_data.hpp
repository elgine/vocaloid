#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
using namespace std;
namespace vocaloid{
    class AudioData{
    private:
        vector<float> data_;
        uint64_t max_size_;
    public:
        // The validated data size
        uint64_t size_;

        explicit AudioData(uint64_t max_size){
            max_size_ = 0;
            size_ = 0;
            Alloc(max_size);
        }

        explicit AudioData(AudioData *d){
            Copy(*d);
        }

        explicit AudioData(vector<float> data, uint64_t len, uint64_t offset){
            max_size_ = size_ = len;
            Alloc(len);
            Set(data, len, offset);
        }

        explicit AudioData(float* data, uint64_t len, uint64_t offset){
            max_size_ = size_ = len;
            Alloc(len);
            Set(data, len, offset);
        }

        AudioData& Copy(AudioData b){
            Alloc(b.maxSize());
            Set(b.data(), b.size_, 0);
            return *this;
        }

        AudioData& Silence(uint64_t len, uint64_t offset = 0){
            fill(data_.begin() + offset, data_.begin() + len, 0.0f);
            return *this;
        }

        AudioData& Silence(){
            fill(data_.begin(), data_.end(), 0.0f);
            return *this;
        }

        AudioData& Plus(AudioData d, uint64_t len, uint64_t offset = 0){
            auto data = d.data();
            for(auto i = offset;i < len;i++){
                data_[i] += data[i];
            }
            return *this;
        }

        AudioData& Plus(AudioData d){
            auto data = d.data();
            auto size = d.size_;
            for(auto i = 0;i < size;i++){
                data_[i] += data[i];
            }
            return *this;
        }

        AudioData& Scale(float s, uint64_t len, uint64_t offset = 0){
            for(auto i = offset;i < len;i++){
                data_[i] *= s;
            }
            return *this;
        }

        AudioData& Scale(float s){
            for(auto float_data : data_){
                float_data *= s;
            }
            return *this;
        }

        AudioData& PlusScale(AudioData d, float s, uint64_t len, uint64_t offset = 0){
            auto data = d.data();
            for(auto i = offset;i < len;i++){
                data_[i] += data[i] * s;
            }
            return *this;
        }

        AudioData& PlusScale(AudioData d, float s){
            auto data = d.data();
            auto size = d.size_;
            for(auto i = 0;i < size;i++){
                data_[i] += data[i] * s;
            }
            return *this;
        }

        void Set(vector<float> data, uint64_t len, uint64_t offset){
            Alloc(len);
            for(auto i = 0;i < len;i++){
                data_[i] = data[i + offset];
            }
        }

        void Set(const float* data, uint64_t len, uint64_t offset){
            Alloc(len);
            for(auto i = 0;i < len;i++){
                data_[i] = data[i + offset];
            }
        }

        void Alloc(uint64_t size){
            if(max_size_ < size){
                data_.reserve(size);
                for(int i = max_size_;i < size_;i++){
                    data_.emplace_back(0.0f);
                }
                max_size_ = size;
            }
        }

        vector<float> data(){
            return data_;
        }

        uint64_t maxSize(){
            return max_size_;
        }
    };
}