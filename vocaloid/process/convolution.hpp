#pragma once
#include <algorithm>
#include "vocaloid/common/buffer.hpp"
#include "vocaloid/utils/fft.hpp"
#include "vocaloid/utils/window.hpp"
#include "vocaloid/common/nextpow2.hpp"
#include "process_unit.h"
#include "pitch_shifter.hpp"
using namespace std;
namespace vocaloid{

    class Convolution: public ProcessUnit{
    private:
        uint64_t kernel_size_;
        uint64_t input_size_;
        uint64_t fft_size_;
        uint64_t n_;
        vector<float> input_;
        vector<float> buffer_;
        FFT *kernel_;
        FFT *main_;

    protected:
        void Processing(){
            for(int i = 0;i < fft_size_;i++){
                float a = main_->real_[i];
                float b = main_->imag_[i];
                float c = kernel_->real_[i];
                float d = kernel_->imag_[i];
                main_->real_[i] = a * c - b * d;
                main_->imag_[i] = b * c + a * d;
            }
        }
    public:

        explicit Convolution(uint64_t input_size):input_size_(input_size),kernel_size_(0),fft_size_(input_size),n_(input_size){
            kernel_ = new FFT();
            main_ = new FFT();
        }

        void Initialize(vector<float> k, uint64_t kernel_len){
            kernel_size_ = kernel_len;
            fft_size_ = n_ = kernel_size_ + input_size_ - 1;
            if((fft_size_ & (fft_size_ - 1)) != 0){
                fft_size_ = NextPow2(fft_size_);
            }
            input_.reserve(fft_size_);
            for(auto i = input_.size();i < fft_size_;i++){
                input_.emplace_back(0);
            }
            buffer_.reserve(fft_size_);
            for(auto i = buffer_.size();i < fft_size_;i++){
                buffer_.emplace_back(0);
            }

            vector<float> kernel(fft_size_, 0);
            for(auto i = 0;i < kernel_len;i++){
                kernel[i] = k[i];
            }

            kernel_->Dispose();
            kernel_->Initialize(fft_size_);
            kernel_->Forward(kernel, fft_size_);
            main_->Dispose();
            main_->Initialize(fft_size_);
        }

        uint64_t Process(vector<float> in, uint64_t len, vector<float> &out) override {
            for(int i = 0;i < fft_size_;i++){
               if(i >= input_size_){
                    input_[i] = 0;
               }else
                   input_[i] = in[i];
            }
            // Forward fft
            main_->Forward(input_, fft_size_);
            // Do processing
            Processing();
            // Do inverse fft
            main_->Inverse(input_);

            for (int i = 0; i < fft_size_; i++) {
                buffer_[i] += input_[i];
                if (i < input_size_) {
                    out[i] = buffer_[i];
                }
            }
            // Move items left
            for (int i = 0; i < fft_size_;i++) {
                if (i + input_size_ >= fft_size_) {
                    buffer_[i] = 0.0f;
                } else
                    buffer_[i] = buffer_[i + input_size_];
            }
            return input_size_;
        }

        uint64_t FFTSize(){
            return fft_size_;
        }

        uint64_t KernelSize(){
            return kernel_size_;
        }
    };
}