#pragma once
#include "vocaloid/common/buffer.hpp"
#include "vocaloid/utils/fft.hpp"
#include "vocaloid/utils/window.hpp"
#include "vocaloid/common/nextpow2.hpp"
#include "process_unit.h"
#include "stft.hpp"
namespace vocaloid{

    class Convolution: public STFT, ProcessUnit{
    private:
        uint64_t kernel_size_;
        uint64_t input_size_;
        uint64_t fft_size_;
        vector<float> input_;
        FFT *kernel_;

        void UpdateOutputSize(){
            fft_size_ = kernel_size_ + input_size_ - 1;
            if((fft_size_ & (fft_size_ - 1)) != 0){
                fft_size_ = NextPow2(fft_size_);
            }
        }

    protected:
        void Processing() override {
            for(int i = 0;i < fft_size_;i++){
                fft_->real_[i] = fft_->real_[i] * kernel_->real_[i];
                fft_->imag_[i] = fft_->imag_[i] * kernel_->imag_[i];
            }
        }
    public:

        explicit Convolution(uint64_t input_size):input_size_(input_size),kernel_size_(0),fft_size_(input_size){
            kernel_ = new FFT();
        }

        void Initialize(uint32_t sample_rate, vector<float> k, uint64_t kernel_len, WINDOW_TYPE win_type = WINDOW_TYPE::HAMMING, float extra = 1.0f){
            kernel_size_ = kernel_len;
            UpdateOutputSize();
            auto overlap = 1.0f - (float)input_size_ / fft_size_;
            STFT::Initialize(fft_size_, sample_rate, overlap, win_type, extra);
            input_.reserve(fft_size_);
            for(auto i = input_.size(); i < fft_size_;i++){
                input_.emplace_back(0);
            }
            // Calculate kernel in frequency domain
            vector<float> kernel(fft_size_);
            for(int i = 0;i < kernel_len;i++){
                kernel[i] = k[i];
            }
            kernel_->Dispose();
            kernel_->Initialize(fft_size_, sample_rate);
            kernel_->Forward(kernel, fft_size_);
        }

        uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) override {
            for(int i = 0;i < len;i++){
                input_[i] = in[i];
            }
            STFT::Process(input_, fft_size_);
            return PopFrame(out, input_size_);
        }

        uint64_t FFTSize(){
            return fft_size_;
        }

        uint64_t KernelSize(){
            return kernel_size_;
        }
    };
}