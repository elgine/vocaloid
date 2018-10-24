#pragma once
#include "synthesizer.h"
#include "vocaloid/maths/base.hpp"
namespace vocaloid{
    class DynamicCompressor: public Synthesizer{
    private:
        float knee_;
        float attack_;
        float release_;
        float ratio_;
        float reduction_;

        float detector_average_;
        float compressor_gain_;
        uint32_t sample_rate_;
    public:
        void Initialize(){
            detector_average_ = 0.0f;
            compressor_gain_ = 1.0f;
        }

        void DetectReduction(vector<float> in, uint64_t len){
            auto attack = attack_ * sample_rate_;
            auto release = release_ * sample_rate_;
            float attenuation = 0.0f, shaped_input;
            float envelop_rate = 0.0;
            float detector_rate = 0.0;
            float gain_increment = 0.0;
            float reduction_gain = 0.0;
            bool releasing = false;
            for(auto i = 0;i < len;i++){
                // 1. If the absolute value of input is less than 0.0001, let attenuation be 1.0.
                // Else, let shaped input be the value of applying the compression curve to the absolute value of input.
                // Let attenuation be shaped input divided by the absolute value of input.
                if(abs(in[i]) < 0.0001f){
                    attenuation = 1.0f;
                }else{
                    shaped_input = CompressionCurve(abs(in[i]));
                }
                attenuation = shaped_input / abs(in[i]);
                // 2. Let releasing be true if attenuation is greater than compressor gain, false otherwise.
                releasing = attenuation > compressor_gain_;
                // 3. Let detector rate be the result of applying the detector curve to attenuation.
                detector_rate = DetectorCurve(attenuation);
                // 4. Substract detector average to attenuation, and multiply the result by detector rate.
                // Add this new result to detector average.
                detector_average_ = (detector_average_ - attenuation) * detector_rate;
                // 5. Clamp detector average to a maximum of 1.0.
                Clamp(0.0, 1.0, detector_average_);
                // 6. Let envelope rate be the result of computing the envelope rate based on values of attack and release.
                envelop_rate = ComputeEnvelopeRate(attack, release);
                // 7. If releasing is true, set compressor gain to the multiplication of compressor gain by envelope rate, clamped to a maximum of 1.0.
                if(releasing){
                    compressor_gain_ *= envelop_rate;
                    Clamp(0.0, 1.0, compressor_gain_);
                }
                // 8. Else, if releasing is false, let gain increment to be detector average minus compressor gain.
                // Multiply gain increment by envelope rate, and add the result to compressor gain.
                else{
                    gain_increment = (detector_average_ - compressor_gain_) * envelop_rate;
                    compressor_gain_ += gain_increment;
                }
                // 9. Compute reduction gain to be compressor gain multiplied by the return value of computing the makeup gain.
                reduction_gain = compressor_gain_ *
            }
        }

        float ComputeEnvelopeRate(float attack, float release){

        }

        float DetectorCurve(float attenuation){}

        float CompressionCurve(float v){

        }

        uint64_t Process(std::vector<float> in, uint64_t len, std::vector<float> &out){

        }
    };
}