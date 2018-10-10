#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>
using namespace std;
// The channel input should be [L, R, SL, SR, LFE, C]
namespace vocaloid{

    #define MAX_CHANNEL_COUNT 6

    void ChannelMonoToStereo(vector<float> *input, int data_len, vector<float> *output){
        output[0].assign(input[0].begin(), input[1].begin() + data_len);
        output[1].assign(input[0].begin(), input[1].begin() + data_len);
    }

    void ChannelMonoToQuad(vector<float> *input, int data_len, vector<float> *output){
        fill(output[2].begin(), output[2].begin() + data_len, 0);
        fill(output[3].begin(), output[3].begin() + data_len, 0);
        output[0].assign(input[0].begin(), input[0].begin() + data_len);
        output[1].assign(input[0].begin(), input[0].begin() + data_len);
    }

    void ChannelMonoTo5_1(vector<float> *input, int data_len, vector<float> *output){
        fill(output[0].begin(), output[0].begin() + data_len, 0);
        fill(output[1].begin(), output[1].begin() + data_len, 0);
        fill(output[2].begin(), output[2].begin() + data_len, 0);
        fill(output[3].begin(), output[3].begin() + data_len, 0);
        fill(output[4].begin(), output[4].begin() + data_len, 0);
        output[5].assign(input[0].begin(), input[0].begin() + data_len);
    }

    void ChannelStereoToQuad(vector<float> *input, int data_len, vector<float> *quad){
        fill(quad[2].begin(), quad[2].begin() + data_len, 0);
        fill(quad[3].begin(), quad[3].begin() + data_len, 0);
        quad[0].assign(input[0].begin(), input[0].begin() + data_len);
        quad[1].assign(input[1].begin(), input[1].begin() + data_len);
    }

    void ChannelStereoTo5_1(vector<float> *input, int data_len, vector<float> *output){
        fill(output[2].begin(), output[2].begin() + data_len, 0);
        fill(output[3].begin(), output[3].begin() + data_len, 0);
        fill(output[4].begin(), output[4].begin() + data_len, 0);
        fill(output[5].begin(), output[5].begin() + data_len, 0);
        output[0].assign(input[0].begin(), input[0].begin() + data_len);
        output[1].assign(input[1].begin(), input[1].begin() + data_len);
    }

    void ChannelQuadTo5_1(vector<float> *input, int data_len, vector<float> *output){
        fill(output[4].begin(), output[4].begin() + data_len, 0);
        fill(output[5].begin(), output[5].begin() + data_len, 0);
        output[0].assign(input[0].begin(), input[0].begin() + data_len);
        output[1].assign(input[1].begin(), input[1].begin() + data_len);
        output[2].assign(input[2].begin(), input[2].begin() + data_len);
        output[3].assign(input[3].begin(), input[3].begin() + data_len);
    }

    void ChannelStereoToMono(vector<float> *input, int data_len, vector<float> *output){
        for(int i = 0;i < data_len;i++){
            output[0][i] = input[0][i] * 0.5f + input[1][i] * 0.5f;
        }
    }

    void ChannelQuadToMono(vector<float> *input, int data_len, vector<float> *output){
        for(int i = 0;i < data_len;i++){
            output[0][i] = 0.25f * (input[0][i] + input[1][i] + input[2][i] + input[3][i]);
        }
    }

    void Channel5_1ToMono(vector<float> *input, int data_len, vector<float> *output){
        for(int i = 0;i < data_len;i++){
            output[0][i] = sqrtf(0.5f) * (input[0][i] + input[1][i]) + input[5][i] + 0.5f * (input[2][i] + input[3][i]);
        }
    }

    void ChannelQuadToStereo(vector<float> *input, int data_len, vector<float> *output){
        for(int i = 0;i < data_len;i++){
            output[0][i] = 0.5f * (input[0][i] + input[2][i]);
            output[1][i] = 0.5f * (input[1][i] + input[3][i]);
        }
    }

    void Channel5_1ToStereo(vector<float> *input, int data_len, vector<float> *output){
        for(int i = 0;i < data_len;i++){
            output[0][i] = input[0][i] + sqrtf(0.5) * (input[5][i] + input[2][i]);
            output[1][i] = input[1][i] + sqrtf(0.5) * (input[5][i] + input[3][i]);
        }
    }

    void Channel5_1ToQuad(vector<float> *input, int data_len, vector<float> *output){
        for(int i = 0;i < data_len;i++){
            output[0][i] = input[0][i] + sqrtf(0.5) * input[5][i];
            output[1][i] = input[1][i] + sqrtf(0.5) * input[5][i];
            output[2][i] = input[2][i];
            output[3][i] = input[3][i];
        }
    }

    void MixChannel(uint16_t in_, uint16_t out_, vector<float> *input, int data_len, vector<float> *output){
        if(in_ == out_){
            for(int i = 0;i < in_;i++){
                output[i].assign(input[i].begin(), input[i].begin() + data_len);
            }
            return;
        }else if(in_ > out_){
            if(in_ == 2 && out_ == 1)
                ChannelStereoToMono(input, data_len, output);
            else if(in_ == 4 && out_ == 1)
                ChannelQuadToMono(input, data_len, output);
            else if(in_ == 6 && out_ == 1)
                Channel5_1ToMono(input, data_len, output);
            else if(in_ == 4 && out_ == 2)
                ChannelQuadToStereo(input, data_len, output);
            else if(in_ == 6 && out_ == 2)
                Channel5_1ToStereo(input, data_len, output);
            else if(in_ == 6 && out_ == 4)
                Channel5_1ToQuad(input, data_len, output);
        }else{
            if(in_ == 1 && out_ == 2){
                ChannelMonoToStereo(input, data_len, output);
            }else if(in_ == 1 && out_ == 4){
                ChannelMonoToQuad(input, data_len, output);
            }else if(in_ == 1 && out_ == 6){
                ChannelMonoTo5_1(input, data_len, output);
            }else if(in_ == 2 && out_ == 4){
                ChannelStereoToQuad(input, data_len, output);
            }else if(in_ == 2 && out_ == 6){
                ChannelStereoTo5_1(input, data_len, output);
            }else if(in_ == 4 && out_ == 6)
                ChannelQuadTo5_1(input, data_len, output);
        }
    }
}