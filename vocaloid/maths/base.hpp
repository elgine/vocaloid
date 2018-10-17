#pragma once

template<typename T>
T Clamp(T min, T max, T v){
    if(v < min)return min;
    else if(v > max)return max;
    return v;
}

template<typename T>
T NextPow2(T v){
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}