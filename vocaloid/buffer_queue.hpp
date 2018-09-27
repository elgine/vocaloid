#pragma once
#include <queue>
#include <stdint.h>
#include "disposable.h"
#include "buffer.hpp"
using namespace std;
namespace vocaloid{

    class BufferQueue: public IDisposable{
    private:
        uint16_t channels_;
        queue<float> *buffers_;
    public:

        explicit BufferQueue(uint16_t channels){
            channels_ = channels;
            buffers_ = new queue<float>[channels_];
        }

        void Push(Buffer *buf, uint64_t length = 0){
            if(buf->GetChannels() != channels_)
                throw "The buffer has the difference channels with buffer queue";
            length = length <= 0?buf->GetBufferSize():length;
            for(int i = 0;i < channels_;i++){
                for(int j = 0;j < length;j++)
                    buffers_[i].push(buf->GetChannelAt(i)[j]);
            }
        }

        void Push(uint16_t index, float *data, uint64_t length){
            for(int i = 0;i < length;i++){
                buffers_[index].push(data[i]);
            }
        }

        void Push(uint16_t index, vector<float> data, uint64_t length){
            for(int i = 0;i < length;i++){
                buffers_[index].push(data[i]);
            }
        }

        void Push(float *data){
            for(int i = 0;i < channels_;i++){
                buffers_[i].push(data[i]);
            }
        }

        void Push(vector<float> data){
            for(int i = 0;i < channels_;i++){
                buffers_[i].push(data[i]);
            }
        }

        void Pop(vector<float> &output){
            for(int i = 0;i < channels_;i++){
                output[i] = buffers_[i].front();
                buffers_[i].pop();
            }
        }

        void Pop(float* output){
            for(int i = 0;i < channels_;i++){
                output[i] = buffers_[i].front();
                buffers_[i].pop();
            }
        }

        uint64_t Pop(float **output, uint64_t length){
            if(!IsCountAvalidated(length))return 0;
            for(int i = 0;i < length;i++){
                for(int j = 0;j < channels_;j++){
                    output[j][i] = buffers_[j].front();
                    buffers_[j].pop();
                }
            }
            return length;
        }

        uint64_t Pop(Buffer *buf, uint64_t length = 0){
            length = length <= 0?buf->GetBufferSize():length;
            if(!IsCountAvalidated(length))return 0;
            for(int i = 0;i < length;i++){
                for(int j = 0;j < channels_;j++){
                    buf->data_[j][i] = buffers_[j].front();
                    buffers_[j].pop();
                }
            }
            buf->buffer_size_ = length;
            return length;
        }

        uint64_t Pop(vector<float> *output, uint64_t length){
            if(!IsCountAvalidated(length))return 0;
            for(int i = 0;i < length;i++){
                for(int j = 0;j < channels_;j++){
                    output[j][i] = buffers_[j].front();
                    buffers_[j].pop();
                }
            }
            return length;
        }

        uint32_t Least(){
            uint32_t size = 0;
            for(int i = 0;i < channels_;i++){
                if(i == 0 || buffers_[i].size() < size){
                    size = buffers_[i].size();
                }
            }
            return size;
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