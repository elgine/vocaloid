#pragma once
#include <vector>
#include <algorithm>
#include <math.h>
#include "interpolate.hpp"
#include "audio_process_node.hpp"
namespace vocaloid{

    struct GainValue{
        uint64_t timestamp;
        float value;
        INTERPOLATOR_TYPE interpolator;
    };

    class Gain{
    private:
        vector<GainValue> value_list;
        int FindIndex(uint64_t timestamp, bool &accurate){
            int start = 0, last = value_list.size() - 1, middle = 0;
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

    public:
        float value;

        explicit Gain(float v = 1.0f){
            value = v;
        }

        void SetValueAtTime(float v, uint64_t start_time){
            bool accurate = false;
            auto index = FindIndex(start_time, accurate);
            value_list.insert(value_list.begin() + index, {start_time, v, INTERPOLATOR_TYPE::NONE});
        }

        void LinearRampToValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, INTERPOLATOR_TYPE::LINEAR});
        }

        float GetValueAtTime(uint64_t time){
            bool accurate = false;
            int index = FindIndex(time, accurate);
            if(index - 1 < 0)return value;
            if(index >= value_list.size()){
                return value_list[value_list.size() - 1].value;
            }
            float v = value;
            if(accurate){
                return value_list[index].value;
            }else{
                int pre = index - 1;
                auto preV = value_list[pre].value;
                auto curV = value_list[index].value;
                auto duration = value_list[index].timestamp - value_list[pre].timestamp;
                auto interpolator = value_list[index].interpolator;
                if(interpolator == INTERPOLATOR_TYPE::NONE)return preV;
                else if(interpolator == INTERPOLATOR_TYPE::LINEAR)
                    return preV + (curV - preV) * (time - value_list[pre].timestamp)/duration;
            }
            return v;
        }
    };

    class AudioGainNode: public AudioProcessNode{
    public:

        Gain *gain_;

        explicit AudioGainNode(AudioContext *ctx, float value = 1.0f):AudioProcessNode(ctx){
            gain_ = new Gain(value);
        }

        int Process(Buffer *in, Buffer *out) {
            auto time = ctx_->GetTicker()->GetCurTimestamp();
            float value = gain_->GetValueAtTime(time);
            int16_t size = in->GetBufferSize();
            for(int i = 0;i < in->GetChannels();i++){
                auto in_channel_data = in->GetChannelAt(i);
                auto out_channel_data = out->GetChannelAt(i);
                for(int j = 0;j < size;j++){
                    out_channel_data[j] = in_channel_data[j] * value;
                }
            }
            return size;
        }
    };
}