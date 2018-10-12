#pragma once
#include <vector>
#include <math.h>
#include "vocaloid/common/clamp.hpp"
#include "process_unit.h"
using namespace std;
namespace vocaloid{
    class Delay: public ProcessUnit{
    private:
        uint32_t sample_rate_;
        uint16_t bits_;
        uint64_t delay_duration_;
        uint64_t delay_size_;
        Buffer<float> *cached_buffer_;
        uint64_t played_;
    public:

        explicit Delay(uint64_t delay_duration = 0,
                        uint32_t sample_rate = 44100,
                        uint16_t bits = 16):delay_duration_(delay_duration),
                                        sample_rate_(sample_rate),
                                        bits_(bits){
            played_ = 0;
            delay_size_ = 0;
            cached_buffer_ = new Buffer<float>(1024);
            UpdateDelaySize();
        }

        void SetFormat(uint32_t sample_rate, uint16_t bits){
            if(sample_rate_ == sample_rate && bits_ == bits)return;
            sample_rate_ = sample_rate;
            bits_ = bits;
            UpdateDelaySize();
        }

        void SetDelayDuration(uint64_t dur){
            if(delay_duration_ == dur)return;
            delay_duration_ = dur;
            UpdateDelaySize();
        }

        void UpdateDelaySize(){
            delay_size_ = uint64_t(delay_duration_ * 0.001 * sample_rate_ /(bits_ / 8.0f));
            if(delay_size_ > cached_buffer_->MaxSize()){
                cached_buffer_->Alloc(delay_size_);
            }
        }

        void ClearCached(){
            cached_buffer_->Fill(0);
        }

        void Clear(){
            played_ = 0;
            ClearCached();
        }

        uint64_t Process(Buffer<float> *in, Buffer<float> *out) override {
            uint64_t size = in->Size();
            cached_buffer_->Add(in, in->Size());
            int64_t silence_size = delay_size_ - played_;
            silence_size = Clamp<int64_t>(0, size, silence_size);
            if(silence_size > 0){
                out->Fill(0, silence_size, 0);
            }
            int64_t copy_size = played_ + size - delay_size_;
            copy_size = Clamp<int64_t>(0, size, copy_size);
            if(copy_size > 0){
                out->Set(cached_buffer_->Data(), copy_size, 0);
                cached_buffer_->RemoveLeft(copy_size);
            }
            played_ += size;
            return size;
        }

        uint64_t Process(std::shared_ptr<Buffer<float>> in, std::shared_ptr<Buffer<float>> out){
            uint64_t size = in->Size();
            cached_buffer_->Add(in, in->Size());
            int64_t silence_size = delay_size_ - played_;
            silence_size = Clamp<int64_t>(0, size, silence_size);
            if(silence_size > 0){
                out->Fill(0, silence_size, 0);
            }
            int64_t copy_size = played_ + size - delay_size_;
            copy_size = Clamp<int64_t>(0, size, copy_size);
            if(copy_size > 0){
                out->Set(cached_buffer_->Data(), copy_size, 0);
                cached_buffer_->RemoveLeft(copy_size);
            }
            played_ += size;
            return size;
        }
    };
}