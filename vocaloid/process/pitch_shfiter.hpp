#pragma once
#include "vocaloid/utils/stft.hpp"
#include "vocaloid/utils/interpolate.hpp"
#include "vocaloid/utils/resample.hpp"
#include "process_unit.h"
namespace vocaloid{

    class PitchShifter: public STFT{
    private:
        float stretch_ = 1.0f;
        int32_t hop_size_pitch_;
        int32_t hop_size_tempo_;
        vector<float> omega_;
        vector<float> prev_in_phase_;
        vector<float> prev_out_phase_;
        INTERPOLATOR_TYPE interpolator_;
    protected:

        // Do pitch shift
        void Processing() override {
            for (int i = 0; i < fft_->GetBufferSize(); i++) {
                float magn = FFT::CalculateMagnitude(fft_->real_[i], fft_->imag_[i]);
                float phase = FFT::CalculatePhase(fft_->real_[i], fft_->imag_[i]);
                float diff = phase - prev_in_phase_[i] - omega_[i];
                float freq_diff = omega_[i] + FFT::MapRadianToPi(diff);
                float new_phase = FFT::MapRadianToPi(prev_out_phase_[i] + freq_diff * stretch_);
                prev_out_phase_[i] = new_phase;
                prev_in_phase_[i] = phase;
                fft_->real_[i] = cosf(new_phase) * magn;
                fft_->imag_[i] = sinf(new_phase) * magn;
            }
        }

        void CalculateHopSizeTempo(float tempo){
            hop_size_tempo_ = (int32_t)roundf(hop_size_ * tempo);
        }

        void CalculateHopSizePitch(float pitch){
            hop_size_pitch_ = (int32_t)roundf(hop_size_tempo_ * pitch);
            stretch_ = (float)hop_size_pitch_/(float)hop_size_;
            hop_size_a_ = (int32_t)roundf(stretch_ * hop_size_);
        }

    public:

        PitchShifter():STFT(){}

        void Initialize(uint32_t fft_size,
                        float sample_rate,
                        float overlap,
                        WINDOW_TYPE win_type = WINDOW_TYPE::HANNING,
                        float extra = 1.0f) {
            STFT::Initialize(fft_size, sample_rate, overlap, win_type, extra);
            omega_.resize(fft_size);
            for(int i = 0;i < fft_size;i++){
                omega_[i] = (float)(M_PI * 2.0f * hop_size_ * i / fft_size);
            }
            prev_in_phase_.resize(fft_size);
            prev_out_phase_.resize(fft_size);
            hop_size_tempo_ = hop_size_pitch_ = hop_size_;
        }

        uint64_t PopFrame(vector<float> &frame, uint32_t len){
            vector<float> temp(len);
            uint32_t temp_len = STFT::PopFrame(temp, len);
            uint64_t frame_len = vocaloid::Resample(temp, temp_len, interpolator_, 1.000000f/GetPitch(), frame);
            return frame_len;
        }

        void ResetPhase(){
            fill(prev_in_phase_.begin(), prev_in_phase_.end(), 0.0f);
            fill(prev_out_phase_.begin(), prev_out_phase_.end(), 0.0f);
        }

        void SetInterpolator(INTERPOLATOR_TYPE type){
            interpolator_ = type;
        }

        INTERPOLATOR_TYPE GetInterpolator(){
            return interpolator_;
        }

        void SetPitch(float v){
            CalculateHopSizePitch(v);
            ResetPhase();
        }

        float GetPitch(){
            return (float)hop_size_pitch_/(float)hop_size_tempo_;
        }

        void SetTempo(float v){
            float pitch = GetPitch();
            CalculateHopSizeTempo(v);
            CalculateHopSizePitch(pitch);
            ResetPhase();
        }

        float GetTempo(){
            return (float)hop_size_tempo_/(float)hop_size_;
        }

        void Dispose() override {
            STFT::Dispose();
            prev_in_phase_.clear();
            prev_out_phase_.clear();
        }
    };
}