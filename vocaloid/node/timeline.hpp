#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>
#include "vocaloid/maths/interpolate.hpp"
using namespace std;
namespace vocaloid{
    enum TimelineInterpolator{
        NONE,
        LINEAR,
        EXPONENTIAL
    };

    struct TimelineValue{
        uint64_t timestamp;
        float value;
        TimelineInterpolator interpolator;
    };

    class Timeline{
    protected:
        vector<TimelineValue> value_list;
    public:
        float value_;

        explicit Timeline(float value = 0.0f){
            value_ = value;
        }

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

        void SetValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, TimelineInterpolator::NONE});
        }

        void ExponentialRampToValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, TimelineInterpolator::EXPONENTIAL});
        }

        void LinearRampToValueAtTime(float v, uint64_t end_time){
            bool accurate = false;
            auto index = FindIndex(end_time, accurate);
            value_list.insert(value_list.begin() + index, {end_time, v, TimelineInterpolator::LINEAR});
        }

        float GetValueAtTime(uint64_t time){
            bool accurate = false;
            int64_t index = FindIndex(time, accurate);
            if(index - 1 < 0)return value_;
            if(index >= value_list.size()){
                return value_list[value_list.size() - 1].value;
            }
            float v = value_;
            if(accurate){
                return value_list[index].value;
            }else{
                int64_t pre = index - 1;
                auto preV = value_list[pre].value;
                auto curV = value_list[index].value;
                auto duration = value_list[index].timestamp - value_list[pre].timestamp;
                auto interpolator = value_list[index].interpolator;
                if(interpolator == TimelineInterpolator::LINEAR)
                    v = LinearInterpolateStep(preV, curV, float(time - value_list[pre].timestamp)/duration);
                else if(interpolator == TimelineInterpolator::EXPONENTIAL)
                    v = ExponentialInterpolateStep(preV, curV, float(time - value_list[pre].timestamp)/duration);
            }
            return v;
        }

        bool TimelineEmpty(){
            return value_list.empty();
        }
    };
}