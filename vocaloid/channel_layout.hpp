#pragma once
namespace vocaloid{

    enum CHANNEL_LAYOUT{
        MONO,
        STEREO,
        QUAD,
        FIVE_ONE
    };

    CHANNEL_LAYOUT GetChannelLayout(int channel_count){
        if(channel_count == 1)return CHANNEL_LAYOUT::MONO;
        else if(channel_count == 2)return CHANNEL_LAYOUT::STEREO;
        else if(channel_count == 4)return CHANNEL_LAYOUT::QUAD;
        return CHANNEL_LAYOUT::FIVE_ONE;
    }

    void MergeChannels(vector<vector<float>> input_channel_data, int input_channel_count,
                        vector<vector<float>> &output_channel_data, int output_channel_count){

    }

    void SplitChannels(){

    }
}