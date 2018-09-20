#pragma once
#include <vector>
#include <queue>
#include <algorithm>
#include "disposable.h"
#include "fft.hpp"
#include "window.hpp"
using namespace std;
namespace vocaloid{

    class STFT: public IDisposable{
    protected:

        FFT *fft_;
        float overlap_;
        uint32_t overlap_size_;
        uint32_t hop_size_;
        vector<float> win_;
        // Input queue buffer
        queue<float> input_queue_;
        // Output queue buffer
        queue<float> output_queue_;
        // Record last frame after processing
        vector<float> temp_;

        void PushToQueue(vector<float> frame){
            for(auto data : frame)
                input_queue_.push(data);
        }

        void Processing(){

        }

    public:

        STFT(){}

        void Initialize(uint32_t fft_size, float sample_rate, float overlap, WINDOW_TYPE win_type, float extra = 1.0f){
            fft_->Initialize(fft_size, sample_rate);
            overlap_ = overlap;
            overlap_size_ = fft_size * overlap_;
            hop_size_ = fft_size - overlap_size_;
            win_.resize(fft_size);
            GenerateWin(win_type, fft_size, win_, extra);
        }

        void Process(const vector<float> buffer, uint32_t len){
            uint32_t fft_size = fft_->GetBufferSize();
            vector<float> frame = vector<float>(fft_size);
            frame.assign(buffer.begin(), buffer.end());
            PushToQueue(buffer);
            while(input_queue_.size() >= fft_size){
                for(int i = 0;i < fft_size;i++){
                    float value = input_queue_.front();
                    frame[i] = value * win_[i];
                }
                fft_->Forward(frame, fft_size);
                Processing();
                fft_->Inverse(frame);
                for(int i = 0;i < fft_size;i++){
                    frame[i] *= win_[i];
                }
                // Overlap add
                if(temp_.empty()){
                    temp_.resize(fft_size);
                    temp_.assign(frame.begin(), frame.end());
                }else{
                    for(int i = 0;i < fft_size;i++){
                        if(i < overlap_size_)
                            temp_[i] += frame[i];
                        else
                            temp_.push_back(frame[i]);
                    }
                }
                for(int i = 0;i < hop_size_;i++){
                    output_queue_.push(temp_[0]);
                    temp_.erase(temp_.begin());
                    input_queue_.pop();
                }
            }
        }

        bool IsOutputQueueEmpty(){
            return output_queue_.empty();
        }

        uint32_t PopFrame(vector<float> &frame){
            uint32_t frame_len = fft_->GetBufferSize();
            min(output_queue_.size(), frame_len);
            for(int i = 0;i < frame_len;i++){
                frame[i] = output_queue_.front();
                output_queue_.pop();
            }
            return frame_len;
        }

        void Dispose() override {
            fft_->Dispose();
            win_.clear();
            temp_.clear();
            while (!input_queue_.empty()) input_queue_.pop();
            while (!output_queue_.empty()) output_queue_.pop();
        }
    };
}