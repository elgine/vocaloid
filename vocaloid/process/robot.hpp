#pragma once
#include <math.h>
#include "process_unit.h"
namespace vocaloid{

    class Robot: public ProcessUnit{
    private:
        uint32_t sample_rate_;
        uint32_t sample_rate_diff_;
    public:

        explicit Robot(uint32_t sample_rate_diff, uint32_t sample_rate = 44100):sample_rate_(sample_rate),
                                                                                sample_rate_diff_(sample_rate_diff){}

        void SetSampleRateDiff(uint32_t v){
            if(v == sample_rate_diff_)return;
            sample_rate_diff_ = v;
        }

        uint64_t Process(Buffer<float> *in, Buffer<float> *out) override {
            size_t size = in->Size();
            for(int i = 0;i < size;i++){
                out->Data()[i] = in->Data()[i] * sin(3.1415926f * 2.0f * sample_rate_diff_ * i / sample_rate_);
            }
            return size;
        }

        uint64_t Process(std::shared_ptr<Buffer<float>> in, std::shared_ptr<Buffer<float>> out) override {
            size_t size = in->Size();
            for(int i = 0;i < size;i++){
                out->Data()[i] = in->Data()[i] * sin(3.1415926f * 2.0f * sample_rate_diff_ * i / sample_rate_);
            }
            return size;
        }
    };
}