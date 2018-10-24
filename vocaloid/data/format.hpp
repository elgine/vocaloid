#pragma once
#include <stdint.h>
#include <math.h>
namespace vocaloid{

    #define CHANNEL_MONO 1
    #define CHANNEL_STEREO 2
    #define CHANNEL_QUAD 4
    #define CHANNEL_5_1 6

    enum Channel{
        LEFT,
        RIGHT,
        SURROUND_LEFT,
        SURROUND_RIGHT,
        CENTER,
        LFE
    };

    struct AudioFormat{
        uint32_t sample_rate;
        uint16_t bits;
        uint16_t channels;
        uint16_t block_align;
    };

    // Linear gain unit to decibel
    float GainUnitToDecibel(float v){
        if(v == 0)return -1000.f;
        return 20.0f * log10f(v);
    }

    // Decibel to linear gain unit
    float DecibelToGainUnit(float v){
        return powf(10.f, v * 0.05f);
    }
}