#pragma once
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
}