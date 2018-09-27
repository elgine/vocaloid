#pragma once
#include <vector>
#include <queue>
#include <algorithm>
#include <stdio.h>
#include "disposable.h"
#include "fft.hpp"
#include "window.hpp"
using namespace std;
namespace vocaloid{

    class STFT: public IDisposable{
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

        virtual void Processing() = 0;

    public:

        STFT(){
            fft_ = new FFT();
        }

        void SetOverlap(float o){
            if(overlap_ == o)return;
            uint32_t fft_size = fft_->GetBufferSize();
            overlap_ = o;
            overlap_size_ = (int32_t)(fft_size * overlap_);
            hop_size_a_ = hop_size_ = fft_size - overlap_size_;
        }

        void Initialize(uint32_t fft_size, float sample_rate, float overlap, WINDOW_TYPE win_type, float extra = 1.0f){
            fft_->Initialize(fft_size, sample_rate);
            SetOverlap(overlap);
            win_.resize(fft_size);
            temp_.resize(fft_size);
            GenerateWin(win_type, fft_size, win_, extra);
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

        uint32_t PopFrame(vector<float> &frame, uint32_t len){
            uint32_t frame_len = len;
            frame_len = min(output_queue_.size(), frame_len);
            float overlap_scaling = (float) fft_->GetBufferSize() / ((float) hop_size_a_ * 2.0f);
            for(int i = 0;i < frame_len;i++){
                frame[i] = output_queue_[i]/overlap_scaling;
            }
            output_queue_.erase(output_queue_.begin(), output_queue_.begin() + frame_len);
            return frame_len;
        }

        void Dispose() override {
            fft_->Dispose();
            win_.clear();
            temp_.clear();
            input_queue_.clear();
            output_queue_.clear();
        }
    };
}