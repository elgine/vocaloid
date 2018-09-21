#pragma once
#include <queue>
#include <stdint.h>
#include "disposable.h"
using namespace std;
namespace vocaloid{

    template<typename T>
    class BufferQueue: public IDisposable{
    private:
        uint16_t channels_;
        queue<T> *buffers_;
    public:

        BufferQueue(uint16_t channels){
            channels_ = channels;
            buffers_ = new queue<T>[channels_];
        }

        void Push(uint16_t index, T *data, uint64_t length){
            for(int i = 0;i < length;i++){
                buffers_[index].push(data[i]);
            }
        }

        void Push(uint16_t index, vector<T> data, uint64_t length){
            for(int i = 0;i < length;i++){
                buffers_[index].push(data[i]);
            }
        }

        void Push(T *data){
            for(int i = 0;i < channels_;i++){
                buffers_[i].push(data[i]);
            }
        }

        void Push(vector<T> data){
            for(int i = 0;i < channels_;i++){
                buffers_[i].push(data[i]);
            }
        }

        void Pop(vector<T> &output){
            for(int i = 0;i < channels_;i++){
                output[i] = buffers_[i].front();
                buffers_[i].pop();
            }
        }

        void Pop(T* output){
            for(int i = 0;i < channels_;i++){
                output[i] = buffers_[i].front();
                buffers_[i].pop();
            }
        }

        uint64_t Pop(T **output, uint64_t length){
            if(!IsCountAvalidated(length))return 0;
            for(int i = 0;i < length;i++){
                for(int j = 0;j < channels_;j++){
                    output[j][i] = buffers_[j].front();
                    buffers_[j].pop();
                }
            }
            return length;
        }

        uint64_t Pop(vector<T> *output, uint64_t length){
            if(!IsCountAvalidated(length))return 0;
            for(int i = 0;i < length;i++){
                for(int j = 0;j < channels_;j++){
                    output[j][i] = buffers_[j].front();
                    buffers_[j].pop();
                }
            }
            return length;
        }

        bool IsCountAvalidated(uint64_t length){
            for(int i = 0;i < channels_;i++){
                if(buffers_[i].size() < length){
                    return false;
                }
            }
            return true;
        }

        void Dispose() override {
            for(int i = 0;i < channels_;i++){
                buffers_[i] = queue<float>();
            }
        }
    };
}