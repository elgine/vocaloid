#pragma once
template<typename T>
T Clamp(T min, T max, T v){
    if(v < min)return min;
    else if(v > max)return max;
    return v;
}