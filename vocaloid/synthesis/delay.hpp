#pragma once
#include <vector>
#include <math.h>
#include <algorithm>
#include "vocaloid/utils/buffer.hpp"
#include "vocaloid/maths/base.hpp"
#include "synthesizer.h"
using namespace std;
namespace vocaloid{
    class Delay: public Synthesizer{
    private:
        uint32_t sample_rate_;
        uint64_t delay_duration_;
        uint64_t delay_size_;
        Buffer<float> *cached_buffer_;
        uint64_t played_;

        void UpdateDelaySize(){
            delay_size_ = uint64_t(delay_duration_ * 0.001 * sample_rate_);
            if(delay_size_ > cached_buffer_->MaxSize()){
                cached_buffer_->Alloc(delay_size_);
            }
        }

        void ClearCached(){
            cached_buffer_->Fill(0);
        }
    public:

        explicit Delay(uint64_t delay_duration = 0,
                        uint32_t sample_rate = 44100):delay_duration_(delay_duration),
                                        sample_rate_(sample_rate){
            played_ = 0;
            delay_size_ = 0;
            cached_buffer_ = new Buffer<float>(1024);
            UpdateDelaySize();
        }

        void SetSampleRate(uint32_t sample_rate){
            if(sample_rate_ == sample_rate)return;
            sample_rate_ = sample_rate;
            UpdateDelaySize();
        }

        void SetDelayDuration(uint64_t dur){
            if(delay_duration_ == dur)return;
            delay_duration_ = dur;
            UpdateDelaySize();
        }

        void Clear(){
            played_ = 0;
            ClearCached();
        }

        uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) override {
            cached_buffer_->Add(in, len);
            int64_t silence_size = delay_size_ - played_;
            silence_size = Clamp<int64_t>(0, len, silence_size);
            if(silence_size > 0){
                fill(out.begin(), out.begin() + silence_size, 0);
            }
            int64_t copy_size = played_ + len - delay_size_;
            copy_size = Clamp<int64_t>(0, len, copy_size);
            if(copy_size > 0){
                for(int i = 0;i < copy_size;i++){
                    out[i] = cached_buffer_->Data()[i];
                }
                cached_buffer_->RemoveLeft(copy_size);
            }
            played_ += len;
            return len;
        }
    };
}