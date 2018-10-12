#pragma once
#include <memory>
#include "process_unit.h"
#include "vocaloid/common/buffer.hpp"
using namespace std;
namespace vocaloid{

    class Convolution: public ProcessUnit{
    private:
        Buffer<float> *kernel_;
        uint64_t frame_size_to_process_;
        uint64_t output_frame_count_;
        void UpdateBufferSize(){
            output_frame_count_ = frame_size_to_process_ + kernel_->Size() - 1;
        }
    public:
        explicit Convolution(uint64_t frame_size_to_process){
            frame_size_to_process_ = frame_size_to_process;
            kernel_ = new Buffer<float>(1024);
        }

        void SetFrameSizeToProcess(uint64_t len){
            if(frame_size_to_process_ == len)return;
            frame_size_to_process_ = len;
            UpdateBufferSize();
        }

        void SetKernel(float* v, uint64_t len){
            kernel_->Set(v, len);
            UpdateBufferSize();
        }

        void SetKernel(vector<float> v, uint64_t len){
            kernel_->Set(v, len);
            UpdateBufferSize();
        }

        static void DoConvolution(vector<float> a, uint64_t a_len,
                                vector<float> b, uint64_t b_len,
                                vector<float> &output){
            auto output_len = a_len + b_len - 1;
            for (int i = 0; i < output_len; i++) {
                output[i] = 0;
                auto j_min = (i >= b_len - 1) ? i - (b_len - 1) : 0;
                auto j_max = (i < a_len - 1) ? i : a_len - 1;
                for (auto j = j_min; j <= j_max; j++) {
                    output[i] += a[j] * b[i - j];
                }
            }
        }

        uint64_t Process(Buffer<float> *in, Buffer<float> *out) override {
            DoConvolution(in->Data(), in->Size(), kernel_->Data(), kernel_->Size(), out->Data());
            return output_frame_count_;
        }

        uint64_t Process(std::shared_ptr<Buffer<float>> in, std::shared_ptr<Buffer<float>> out) override {
            DoConvolution(in->Data(), in->Size(), kernel_->Data(), kernel_->Size(), out->Data());
            return output_frame_count_;
        }

        uint64_t OutputFrameSize(){
            return output_frame_count_;
        }
    };
}