#pragma once
#include "stft.hpp"
#include "interpolator.hpp"
#include "resample.hpp"
namespace vocaloid{

    class PitchShifter: public STFT{
    private:
        float tempo_ = 1.0f;
        float pitch_ = 1.0f;
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
                float new_phase = FFT::MapRadianToPi(prev_out_phase_[i] + freq_diff * pitch_ * tempo_);
                prev_out_phase_[i] = new_phase;
                prev_in_phase_[i] = phase;
                fft_->real_[i] = cosf(new_phase) * magn;
                fft_->imag_[i] = sinf(new_phase) * magn;
            }
        }

        void UpdateHopSize(){
            hop_size_a_ = uint32_t(hop_size_ * pitch_ * tempo_);
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
        }

//        uint64_t PopFrame(vector<float> &frame, uint32_t len){
//            vector<float> temp(len);
//            uint32_t temp_len = STFT::PopFrame(temp, len);
//            uint64_t frame_len = vocaloid::Resample(temp, temp_len, interpolator_, pitch_, frame);
//            return frame_len;
//        }

        void SetInterpolator(INTERPOLATOR_TYPE type){
            interpolator_ = type;
        }

        INTERPOLATOR_TYPE GetInterpolator(){
            return interpolator_;
        }

        void SetPitch(float v){
            if(pitch_ == v)return;
            pitch_ = v;
            UpdateHopSize();
        }

        float GetPitch(){
            return pitch_;
        }

        void SetTempo(float v){
            if(tempo_ == v)return;
            tempo_ = v;
            UpdateHopSize();
        }

        float GetTempo(){
            return tempo_;
        }

        void Dispose() override {
            STFT::Dispose();
            prev_in_phase_.clear();
            prev_out_phase_.clear();
        }
    };
}