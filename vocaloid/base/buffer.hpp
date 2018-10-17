#pragma once
#include <vector>
#include <stdint.h>
#include <memory>
using namespace std;
namespace vocaloid{
    template<typename T>
    class Buffer{
    protected:
        vector<T> data_;
        uint64_t max_size_;
        uint64_t size_;
    public:
        explicit Buffer(uint64_t max_size){
            max_size_ = 0;
            size_ = 0;
            Alloc(max_size);
        }

        explicit Buffer(vector<T> data, uint64_t len, uint64_t offset){
            max_size_ = size_ = len;
            Alloc(len);
            Set(data, len, offset);
        }

        explicit Buffer(T* data, uint64_t len, uint64_t offset){
            max_size_ = size_ = len;
            Alloc(len);
            Set(data, len, offset);
        }

        void Fill(T v){
            for(auto i = 0;i < size_;i++){
                data_[i] = v;
            }
        }

        void Fill(T v, uint64_t length, uint64_t offset){
            if(max_size_ < length){
                Alloc(length);
            }
            if(length > size_)size_ = length;
            for(auto i = 0;i < length;i++){
                data_[i + offset] = v;
            }
        }

        void RemoveLeft(uint64_t step){
            step = step % size_;
            for(auto i = 0;i < size_ - step;i++){
                data_[i] = data_[i + step];
            }
            size_ -= step;
        }

        void Add(const vector<T> data, uint64_t len, uint64_t offset = 0){
            uint64_t data_offset = size_;
            if(max_size_ < size_ + len){
                Alloc(size_ + len);
            }
            for(auto i = 0; i < len;i++){
                data_[i + data_offset] = data[i + offset];
            }
            size_ += len;
        }

        void Add(T *data, uint64_t len, uint64_t offset = 0){
            uint64_t data_offset = size_;
            if(max_size_ < size_ + len){
                Alloc(size_ + len);
            }
            for(auto i = 0; i < len;i++){
                data_[i + data_offset] = data[i + offset];
            }
            size_ += len;
        }

        void Add(Buffer<T> *data, uint64_t len, uint64_t offset = 0){
            uint64_t data_offset = size_;
            if(max_size_ < size_ + len){
                Alloc(size_ + len);
            }
            auto source_data = data->Data();
            for(auto i = 0; i < len;i++){
                data_[i + data_offset] = source_data[i + offset];
            }
            size_ += len;
        }

        void Add(shared_ptr<Buffer<T>> data, uint64_t len, uint64_t offset = 0){
            uint64_t data_offset = size_;
            if(max_size_ < size_ + len){
                Alloc(size_ + len);
            }
            auto source_data = data->Data();
            for(auto i = 0; i < len;i++){
                data_[i + data_offset] = source_data[i + offset];
            }
            size_ += len;
        }

        void Set(vector<T> data, uint64_t len, uint64_t offset = 0){
            Alloc(len);
            for(auto i = 0;i < len;i++){
                data_[i] = data[i + offset];
            }
            size_ = len;
        }

        void Set(const T* data, uint64_t len, uint64_t offset = 0){
            Alloc(len);
            for(auto i = 0;i < len;i++){
                data_[i] = data[i + offset];
            }
            size_ = len;
        }

        void Alloc(uint64_t size){
            if(max_size_ < size){
                data_.reserve(size);
                for(auto i = max_size_;i < size;i++){
                    data_.emplace_back(0);
                }
                max_size_ = size;
            }
        }

        void SetSize(uint64_t size){
            if(size_ == size)return;
            if(size_ < size)Alloc(size);
            size_ = size;
        }

        uint64_t Size(){
            return size_;
        }

        vector<T>& Data(){
            return data_;
        }

        uint64_t MaxSize(){
            return max_size_;
        }
    };
}