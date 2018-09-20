#pragma once
#include "stft.hpp"
#include "interpolator.hpp"
#include "resampler.hpp"
namespace vocaloid{

    class PitchShifter: public STFT{
    private:
        float tempo_;
        float pitch_;
        vector<float> prev_in_phase_;
        vector<float> prev_out_phase_;
        INTERPOLATOR_TYPE interpolator_;
    protected:

        // Do pitch shift
        void Processing(){
            for (int i = 0; i < fft_->GetBufferSize(); i++) {
                float omega = (float)(2 * M_PI * hop_size_ * i / fft_->GetBufferSize());
                float magn = FFT::CalculateMagnitude(fft_->real_[i], fft_->imag->[i]);
                float phase = FFT::CalculatePhase(fft_->real_[i], fft_->imag->[i]);
                float diff = phase - prevInPhase[i] - omega;
                float freq_diff = omega + FFT::MapRadianToPi(diff);
                float new_phase = FFT::MapRadianToPi(prev_out_phase_[i] + freq_diff * pitch_/tempo_);
                prev_out_phase_[i] = new_phase;
                prev_in_phase_[i] = phase;
                fft_->real_[i] = (float) Math.cos(new_phase) * magn;
                fft_->imag_[i] = (float) Math.sin(new_phase) * magn;
            }
        }

    public:

        void Initialize(uint32_t fft_size,
                        float sample_rate,
                        float overlap,
                        WINDOW_TYPE win_type,
                        float extra = 1.0f){
            STFT::Initialize(fft_size, sample_rate, overlap, win_type, extra);
            prev_in_phase_.resize(fft_size);
            prev_out_phase_.resize(fft_size);
        }

        uint32_t PopFrame(vector<float> &frame){
            vector<float> temp;
            uint32_t temp_len = STFT::PopFrame(temp);
            uint32_t frame_len = Resample(temp, temp_len, interpolator_, pitch_/tempo_, frame);
            return frame_len;
        }

        void SetInterpolator(INTERPOLATOR_TYPE type){
            interpolator_ = type;
        }

        INTERPOLATOR_TYPE GetInterpolator(){
            return interpolator_;
        }

        void SetPitch(float v){
            pitch_ = v;
        }

        float GetPitch(){
            return pitch_;
        }

        void SetTempo(float v){
            tempo_ = v;
        }

        float GetTempo(){
            return tempo_;
        }

        void Dispose(){
            STFT::Dispose();
            prev_in_phase_.clear();
            prev_out_phase_.clear();
        }
    };
}