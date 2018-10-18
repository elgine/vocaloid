#pragma once
#include <math.h>
#include <vector>
#include "synthesizer.h"
using namespace std;
namespace vocaloid{

    class Robot: public Synthesizer{
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

        uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) override {
            for(int i = 0;i < len;i++){
                out[i] = in[i] * sin(3.1415926f * 2.0f * sample_rate_diff_ * i / sample_rate_);
            }
            return len;
        }
    };
}