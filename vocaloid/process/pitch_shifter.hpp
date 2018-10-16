#pragma once
#include <vector>
#include <queue>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include "vocaloid/common/disposable.h"
#include "process_unit.h"
#include "vocaloid/utils/fft.hpp"
#include "vocaloid/utils/window.hpp"
#include "vocaloid/utils/interpolate.hpp"
#include "vocaloid/utils/resample.hpp"
using namespace std;
namespace vocaloid{

    class PitchShifter: public ProcessUnit, IDisposable{
    protected:
        FFT *fft_;
        float overlap_;
        int32_t overlap_size_;
        int32_t hop_size_;
        int32_t hop_size_a_;
        vector<float> win_;
        // Input queue buffer
        vector<float> input_queue_;
        // Output queue buffer
        vector<float> output_queue_;
        // Record last frame after processing
        vector<float> temp_;
        // Sum stretch
        float stretch_ = 1.0f;
        // Hop size mul pitch ratio
        int32_t hop_size_pitch_;
        // Hop size mul tempo ratio
        int32_t hop_size_tempo_;
        vector<float> omega_;
        vector<float> prev_in_phase_;
        vector<float> prev_out_phase_;
        INTERPOLATOR_TYPE interpolator_;

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

        void CalculateHopSizeTempo(float tempo){
            hop_size_tempo_ = (int32_t)roundf(hop_size_ * tempo);
        }

        void CalculateHopSizePitch(float pitch){
            hop_size_pitch_ = (int32_t)roundf(hop_size_tempo_ * pitch);
            stretch_ = (float)hop_size_pitch_/(float)hop_size_;
            hop_size_a_ = (int32_t)roundf(stretch_ * hop_size_);
        }

        void SetOverlap(float o){
            if(overlap_ == o)return;
            uint32_t fft_size = fft_->GetBufferSize();
            overlap_ = o;
            overlap_size_ = (int32_t)(fft_size * overlap_);
            hop_size_a_ = hop_size_ = fft_size - overlap_size_;
        }

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

    public:

        PitchShifter(){
            fft_ = new FFT();
        }

        void Initialize(uint32_t fft_size,
                        float overlap,
                        WINDOW_TYPE win_type = WINDOW_TYPE::HANNING,
                        float extra = 1.0f) {
            fft_->Initialize(fft_size);
            SetOverlap(overlap);
            win_.resize(fft_size);
            temp_.resize(fft_size);
            GenerateWin(win_type, fft_size, win_, extra);
            omega_.resize(fft_size);
            for(int i = 0;i < fft_size;i++){
                omega_[i] = (float)(M_PI * 2.0f * hop_size_ * i / fft_size);
            }
            prev_in_phase_.resize(fft_size);
            prev_out_phase_.resize(fft_size);
            hop_size_tempo_ = hop_size_pitch_ = hop_size_;
        }

        void Process(vector<float> buffer, uint64_t len){
            // Add to input data queue
            for(int i = 0;i < len;i++){
                input_queue_.push_back(buffer[i]);
            }
            uint32_t fft_size = fft_->GetBufferSize();
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
                    temp_[i] += frame[i] * win_[i];
                    if (i < hop_size_a_) {
                        // Push to output data queue
                        output_queue_.push_back(temp_[i]);
                    }
                }
                // Move items left
                for (int i = 0; i < fft_size; i++) {
                    if (i + hop_size_a_ >= fft_size) {
                        temp_[i] = 0.0f;
                    } else
                        temp_[i] = temp_[i + hop_size_a_];
                }
                // Delete
                input_queue_.erase(input_queue_.begin(), input_queue_.begin() + hop_size_);
            }
        }

        bool IsOutputQueueEmpty(uint64_t length){
            return output_queue_.size() < length;
        }

        uint64_t PopFrame(vector<float> &frame, uint64_t len){
            vector<float> temp(len);
            uint64_t frame_len = len;
            frame_len = min(output_queue_.size(), frame_len);
            float overlap_scaling = (float) fft_->GetBufferSize() / ((float) hop_size_a_ * 2.0f);
            for(int i = 0;i < frame_len;i++){
                temp[i] = output_queue_[i]/overlap_scaling;
            }
            output_queue_.erase(output_queue_.begin(), output_queue_.begin() + frame_len);
            frame_len = Resample(temp, frame_len, interpolator_, 1.000000f/GetPitch(), frame);
            return frame_len;
        }

        uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) override {
            Process(in, len);
            return PopFrame(out, len);
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
            fft_->Dispose();
            win_.clear();
            temp_.clear();
            input_queue_.clear();
            output_queue_.clear();
            prev_in_phase_.clear();
            prev_out_phase_.clear();
        }
    };
}