#pragma once
#include "vocaloid/maths/interpolate.hpp"
#include "audio_buffer.hpp"
namespace vocaloid{
    enum AudioParamInterpolator{
        LINEAR,
        EXPONENTIAL
    };

    struct AudioParamValue{
        uint64_t timestamp;
        float value;
        AudioParamInterpolator interpolator;
    };

    class AudioNode;
    class AudioParam{
    protected:
        vector<AudioNode*> inputs_;
        uint16_t num_input_nodes_;
        AudioBuffer *input_buffer_;
        AudioBuffer *summing_buffer_;
        uint64_t frame_size_;
        bool enable_;
        uint64_t offset_;
        uint32_t sample_rate_;
        vector<AudioParamValue> value_list;

        int64_t FindIndex(uint64_t timestamp, bool &accurate){
            int64_t start = 0, last = value_list.size() - 1, middle = 0;
            int64_t delta = 0;
            accurate = false;
            if(last < 0)return 0;
            while(start <= last){
                middle = (last + start) / 2;
                delta =  timestamp - value_list[middle].timestamp;
                if(abs(delta) < 0.000001){
                    accurate = true;
                    break;
                }else if(delta > 0){
                    start = middle + 1;
                }else{
                    last = middle - 1;
                }
            }
            if(delta > 0){
                middle++;
            }
            return middle;
        }

        uint64_t CalculatePlayedTime(uint64_t offset){
            return uint64_t((float)(offset) / float(sample_rate_) * 1000.0f);
        }

    public:
        float value;

        explicit AudioParam(){
            sample_rate_ = 44100;
            frame_size_ = 1024;
            value = 1.0f;
            input_buffer_ = new AudioBuffer(2, frame_size_);
            summing_buffer_ = new AudioBuffer(1, frame_size_);
        }

        void SummingInputs(){
            summing_buffer_->Fill(0.0f);
            if(!inputs_.empty()){
                for(const auto &input : inputs_){
                    input->Pull(input_buffer_);
                    summing_buffer_->Mix(input_buffer_);
                }
            }
        }

        virtual void ComputingValues(Buffer<float> *v){
            SummingInputs();
            for(auto i = 0;i < frame_size_;i++){
                v->Data()[i] = summing_buffer_->Channel(0)->Data()[i] + GetValueAtTime(offset_ + i);
            }
        }

        virtual void DisconnectInput(const AudioNode *n){
            auto iter = find(inputs_.begin(), inputs_.end(), n);
            if(iter != inputs_.end()){
                inputs_.erase(iter);
                num_input_nodes_--;
            }
        }

        bool HasConnectedFrom(const AudioNode *n){
            return find(inputs_.begin(), inputs_.end(), n) != inputs_.end();
        }

        virtual void ConnectInput(AudioNode *n){
            if(HasConnectedFrom(n))return;
            inputs_.emplace_back(n);
            num_input_nodes_++;
        }

        void SetValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, AudioParamInterpolator::EXPONENTIAL});
        }

        void ExponentialRampToValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, AudioParamInterpolator::EXPONENTIAL});
        }

        void LinearRampToValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, AudioParamInterpolator::LINEAR});
        }

        float GetValueAtTime(uint64_t time){
            bool accurate = false;
            int64_t index = FindIndex(time, accurate);
            if(index - 1 < 0)return value;
            if(index >= value_list.size()){
                return value_list[value_list.size() - 1].value;
            }
            float v = value;
            if(accurate){
                return value_list[index].value;
            }else{
                int64_t pre = index - 1;
                auto preV = value_list[pre].value;
                auto curV = value_list[index].value;
                auto duration = value_list[index].timestamp - value_list[pre].timestamp;
                auto interpolator = value_list[index].interpolator;
                if(interpolator == AudioParamInterpolator::NONE)return preV;
                else if(interpolator == AudioParamInterpolator::LINEAR)
                    v = LinearInterpolateStep(preV, curV, float(time - value_list[pre].timestamp)/duration);
                else if(interpolator == AudioParamInterpolator::EXPONENTIAL)
                    v = ExponentialInterpolateStep(preV, curV, float(time - value_list[pre].timestamp)/duration);
            }
            return v;
        }

        void SetOffset(uint64_t offset){
            offset_ = offset;
        }

        void Offset(uint64_t len){
            offset_ += len;
        }

        bool Enable(){
            return enable_;
        }

        uint16_t NumInputNodes(){
            return num_input_nodes_;
        }

        uint64_t GetOffset(){
            return offset_;
        }
    };
}