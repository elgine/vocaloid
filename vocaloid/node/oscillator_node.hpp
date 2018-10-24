#pragma once
#include "audio_source_node.hpp"
#include "vocaloid/synthesis/waveform.hpp"
#include "vocaloid/utils/buffer.hpp"
#include "vocaloid/maths/resample.hpp"
#include "audio_context.hpp"
namespace vocaloid{

    class OscillatorNode: public AudioSourceNode{
    private:
        float frequency_;
        WAVEFORM_TYPE type_;

        Buffer<float> *waveform_buffer_;
        Buffer<float> *waveform_buffer_after_resampled_;
        bool waveform_dirty_;
        uint64_t offset_ = 0;
    public:
        explicit OscillatorNode(AudioContext *ctx):AudioSourceNode(ctx){
            channels_ = 1;
            frequency_ = 440;
            waveform_dirty_ = true;
            type_ = WAVEFORM_TYPE::SINE;
            waveform_buffer_ = new Buffer<float>();
            waveform_buffer_after_resampled_ = new Buffer<float>();
        }

        int64_t Process(AudioBuffer *in) override {
            // Resample
            if(waveform_dirty_){
                auto ori_size = waveform_buffer_->Size();
                if(ori_size > 0){
                    float ratio = (float)context_->GetSampleRate()/(frequency_ * ori_size);
                    auto size = uint64_t(ori_size * ratio);
                    waveform_buffer_after_resampled_->Alloc(size);
                    waveform_buffer_after_resampled_->SetSize(size);
                    Resample(waveform_buffer_->Data(), ori_size,
                             INTERPOLATOR_TYPE::LINEAR,
                             ratio,
                             waveform_buffer_after_resampled_->Data());
                }
            }
            in->Alloc(channels_, frame_size_);
            in->SetSize(frame_size_);
            // Fill buffer
            uint64_t size = 0;
            uint64_t fill_size = 0;
            uint64_t buffer_size = waveform_buffer_after_resampled_->Size();
            while(size < frame_size_){
                fill_size = min(frame_size_ - size, buffer_size - offset_);
                in->Data()[0]->Set(waveform_buffer_after_resampled_->Data(), fill_size, offset_, size);
                size += fill_size;
                offset_  = (offset_ + fill_size) % buffer_size;
            }
            return frame_size_;
        }

        void GenWaveformData(float frequency, WAVEFORM_TYPE wave_type, uint64_t buffer_size) {
            frequency_ = frequency;
            waveform_buffer_->Alloc(buffer_size);
            waveform_buffer_->SetSize(buffer_size);
            GenWaveform(wave_type, buffer_size, waveform_buffer_->Data());
            waveform_dirty_ = true;
        }

        void GenWaveformData(float frequency, const vector<float> &real, const vector<float> &imag, uint64_t buffer_size) {
            frequency_ = frequency;
            waveform_buffer_->Alloc(buffer_size);
            waveform_buffer_->SetSize(buffer_size);
            GenWaveform(real, imag, buffer_size, waveform_buffer_->Data());
            waveform_dirty_ = true;
        }

        WAVEFORM_TYPE WaveformType(){
            return type_;
        }

        float Frequency(){
            return frequency_;
        }
    };
}