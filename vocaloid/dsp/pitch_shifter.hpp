#pragma once
#include "vocaloid/maths/fft.hpp"
#include "vocaloid/maths/resample.hpp"
#include "vocaloid/maths/window.hpp"
#include "processor.h"

namespace vocaloid{

    class PitchShifter: public Processor{
    private:
        FFT *fft_;
        vector<float> win_;
        vector<float> omega_;
        vector<float> input_queue_;
        vector<float> output_queue_;
        vector<float> buffer_;
        int64_t overlap_size_;
        int64_t hop_size_;
        int64_t hop_size_s_;
        float stretch_;
        float pitch_;
        float tempo_;
        vector<float> prev_in_phase_;
        vector<float> prev_out_phase_;

        void Processing(){
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

        void ShiftWindow(vector<float> &data, uint64_t len){
            uint64_t halfLen = len/2;
            if (len % 2 == 0) {
                for (int i = 0; i < halfLen; i++) {
                    float tmp = data[i];
                    data[i] = data[i + halfLen];
                    data[i + halfLen] = tmp;
                }
            } else {
                uint64_t shiftAmt = halfLen,
                        remaining = len,
                        curr = 0;
                float save = data[curr];
                while (remaining >= 0) {
                    float next = data[(curr + shiftAmt) % len];
                    data[(curr + shiftAmt) % len] = save;
                    save = next;
                    curr = (curr + shiftAmt) % len;
                    remaining--;
                }
            }
        }

        void UpdateHopSize(){
            stretch_ = pitch_ * tempo_;
            hop_size_s_ = (uint64_t)roundf(hop_size_ * stretch_);
            stretch_ = float(hop_size_s_) / hop_size_;
        }

    public:

        explicit PitchShifter():hop_size_(0),
                                hop_size_s_(0),
                                overlap_size_(0),
                                stretch_(1.0f),
                                pitch_(1.0f),
                                tempo_(1.0f){
            fft_ = new FFT();
        }

        void Initialize(uint64_t fft_size, float overlap, WINDOW_TYPE win = WINDOW_TYPE::HAMMING, float extra = 1.0f){
            fft_->Initialize(fft_size);
            win_.resize(fft_size);
            GenerateWin(win, fft_size, win_, extra);
            buffer_.resize(fft_size);
            prev_in_phase_.resize(fft_size);
            prev_out_phase_.resize(fft_size);
            overlap_size_ = (int64_t)(fft_size * overlap);
            hop_size_s_ = hop_size_ = fft_size - overlap_size_;
            omega_.resize(fft_size);
            for(int i = 0;i < fft_size;i++){
                omega_[i] = (float)(M_PI * 2.0f * hop_size_ * i / fft_size);
            }
        }

        uint64_t Process(std::vector<float> in, uint64_t len, std::vector<float> &out) override {
            // Add to input data queue
            for(int i = 0;i < len;i++){
                input_queue_.push_back(in[i]);
            }
            uint64_t fft_size = fft_->GetBufferSize();
            vector<float> frame = vector<float>(fft_size);
            while(input_queue_.size() >= fft_size) {
                // Windowing
                for (int i = 0; i < fft_size; i++) {
                    frame[i] = input_queue_[i] * win_[i];
                }
                ShiftWindow(frame, fft_size);
                // Forward fft
                fft_->Forward(frame, fft_size);
                // Do processing
                Processing();
                // Do inverse fft
                fft_->Inverse(frame);
                ShiftWindow(frame, fft_size);
                // Overlap add
                for (int i = 0; i < fft_size; i++) {
                    buffer_[i] += frame[i] * win_[i];
                    if (i < hop_size_s_) {
                        // Push to output data queue
                        output_queue_.push_back(buffer_[i]);
                    }
                }
                // Move items left
                for (int i = 0; i < fft_size; i++) {
                    if (i + hop_size_s_ >= fft_size) {
                        buffer_[i] = 0.0f;
                    } else
                        buffer_[i] = buffer_[i + hop_size_s_];
                }
                // Delete
                input_queue_.erase(input_queue_.begin(), input_queue_.begin() + hop_size_);
            }
            return PopFrame(out, len);
        }

        uint64_t PopFrame(vector<float> &frame, uint64_t len){
            auto frame_len = min((uint64_t)output_queue_.size(), len);
            vector<float> temp;
            temp.assign(output_queue_.begin(), output_queue_.begin() + frame_len);
            output_queue_.erase(output_queue_.begin(), output_queue_.begin() + frame_len);
            return Resample(temp, frame_len, INTERPOLATOR_TYPE::LINEAR, 1.0f / pitch_, frame);
        }

        void SetPitch(float v){
            if(pitch_ == v)return;
            pitch_ = v;
            UpdateHopSize();
        }

        void SetTempo(float v){
            if(tempo_ == v)return;
            tempo_ = v;
            UpdateHopSize();
        }

        float GetPitch(){
            return pitch_;
        }

        float GetTempo(){
            return tempo_;
        }

        void Dispose() {
            fft_->Dispose();
            win_.clear();
            buffer_.clear();
            input_queue_.clear();
            output_queue_.clear();
        }
    };
}