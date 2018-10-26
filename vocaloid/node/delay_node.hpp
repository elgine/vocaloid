#pragma once
#include <map>
#include "vocaloid/maths/base.hpp"
#include "audio_node.hpp"
#include "audio_param.hpp"
#include "audio_buffer.hpp"
#include "audio_context.hpp"
using namespace std;
namespace vocaloid{
    class DelayNode: public AudioNode{
    private:
        vector<map<uint64_t, float>> delay_lines_;
        float max_delay_time_;
        uint64_t played_;
    public:
        AudioParam *delay_time_;

        explicit DelayNode(AudioContext *ctx, float delay_time = 0, float max_delay_time = 1.0f):AudioNode(ctx),max_delay_time_(max_delay_time){
            delay_time_ = new AudioParam();
            delay_time_->value_ = delay_time;
            played_ = 0;
        }

        void Initialize(uint64_t frame_size) override {
            AudioNode::Initialize(frame_size);
            delay_lines_.resize(channels_);
        }

        int64_t Process(AudioBuffer *in) override {
            delay_time_->ComputingValues();
            in->Fill(0.0f);
            auto sample_rate = context_->GetSampleRate();
            for(auto i = 0;i < frame_size_;i++){
                auto delay_time = Clamp(0.0f, max_delay_time_, delay_time_->ResultBuffer()->Data()[i]);
                auto delay_offset = uint64_t(delay_time * sample_rate);
                for(auto j = 0;j < channels_;j++){
                    delay_lines_[j][played_ + i + delay_offset] = summing_buffer_->Channel(j)->Data()[i];
                }
            }
            for(auto i = 0;i < frame_size_;i++){
                for(auto j = 0;j < channels_;j++){
                    auto value =  delay_lines_[j][played_ + i];
                    in->Channel(j)->Data()[i] = value;
                }
            }
            played_ += frame_size_;
            for(auto j = 0;j < channels_;j++){
                auto iter = delay_lines_[j].begin();
                while(iter != delay_lines_[j].end() && iter->first < played_){
                    iter++;
                }
                if(iter != delay_lines_[j].end()){
                    delay_lines_[j].erase(delay_lines_[j].begin(), iter);
                }
            }
            return frame_size_;
        }
    };
}