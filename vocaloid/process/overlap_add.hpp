#pragma once
#include "process_unit.h"
#include "vocaloid/common/buffer.hpp"
namespace vocaloid{

    class OverlapAdd: public ProcessUnit{
    protected:
        Buffer<float> buffer_;
        uint64_t overlap_size_;
        uint64_t frame_size_;
    public:
        explicit OverlapAdd(){
            buffer_ = new Buffer<float>();
        }

        void Initialize(uint64_t frame_size, uint64_t overlap_size){
            frame_size_ = frame_size;
            overlap_size_ = overlap_size;
        }
    };
}