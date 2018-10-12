#pragma once
namespace vocaloid{
    enum ChannelLayout{
        CHANNEL_MONO,
        CHANNEL_STEREO,
        CHANNEL_QUAD,
        CHANNEL_5_1
    };

    enum Channel{
        LEFT,
        RIGHT,
        SURROUND_LEFT,
        SURROUND_RIGHT,
        CENTER,
        LFE
    };
}
