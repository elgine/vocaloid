#pragma once;
#include <vector>
#include <stdint.h>
#include <algorithm>
#include "disposable.h"
using namespace std;
namespace vocaloid{

    class AudioChannel: public IDisposable{
    private:
        vector<float> channel_data_;
        uint64_t data_len_;
    public:

        explicit AudioChannel(uint64_t len){
            data_len_ = 0;
            channel_data_.resize(len);
        }  

        float& operator[] (int i){
            return channel_data_[i]; 
        } 

        void Set(float *d, uint64_t len, uint64_t dst = 0, uint64_t offset = 0){
            data_len_ = len;
            for(int i = 0;i < len;i++){
                if(i >= data_len_){
                    channel_data_.push_back(d[i + offset]);
                }else
                    channel_data_[i + dst] = d[i + offset];
            }   
        }

        void Set(vector<float> d, uint64_t len = 0, uint64_t dst = 0, uint64_t offset = 0){
            len = len <= 0?d.size():len;
            data_len_ = len;
            for(int i = 0;i < len;i++){
                if(i >= data_len_){
                    channel_data_.push_back(d[i + offset]);
                }else
                    channel_data_[i + dst] = d[i + offset];
            }   
        }

        void Insert(float d, int index){
            channel_data_.insert(channel_data_.begin() + index, d);
            data_len_++;
        }

        void Push(float d){
            channel_data_.push_back(d);
            data_len_++;
        }

        void Push(float *d, uint64_t len, uint64_t offset = 0){
            for(int i = 0;i < len;i++){
                channel_data_.push_back(d[i + offset]);
            }
            data_len_ += len;
        }

        void Push(vector<float> d, uint64_t len, uint64_t offset = 0){
            for(int i = 0;i < len;i++){
                channel_data_.push_back(d[i + offset]);
            }
            data_len_ += len;
        }

        float Pop(){
            float v = channel_data_.front();
            channel_data_.erase(channel_data_.begin());
            data_len_--;
            return v;
        }

        void Remove(float d){
            auto iter = find(channel_data_.begin(), channel_data_.end(), d);
            if(iter != channel_data_.end()){
                channel_data_.erase(iter);
                data_len_--;
            }
        }

        void RemoveAt(int index){
            channel_data_.erase(channel_data_.begin() + index);
            data_len_--;
        }

        uint64_t GetDataLen(){
            return data_len_;
        }

        void Dispose() override {
            data_len_ = 0;
            channel_data_.clear();
        }
    };
}