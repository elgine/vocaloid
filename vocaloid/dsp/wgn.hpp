#pragma once
#include <stdint.h>
#include <math.h>
#include <random>
using namespace std;
namespace vocaloid{

    void Randn(uint64_t len, vector<float> &out){
        random_device rd;
        mt19937 gen(rd());
        for(int i = 0;i < len;i++){
            normal_distribution<float> normal(0, 1);
            out[i] = normal(gen);
        }
    }

    float CalculateSignalPower(vector<float> signal, uint64_t len){
        float sum = 0;
        for(int i = 0;i < len;i++){
            sum += powf(abs(signal[i]), 2);
        }
        return sum / len;
    }

    void WGN(uint64_t len, float p, vector<float> &out){
        float noise_power_sqrt = sqrtf(p);
        Randn(len, out);
        for(int i = 0;i < len;i++){
            out[i] *= noise_power_sqrt;
        }
    }

    void AWGN(vector<float> signal, uint64_t len, float snr, vector<float> &output){
        float signal_power = CalculateSignalPower(signal, len);
        float noise_power = signal_power / snr;
        vector<float> noise(len);
        WGN(len, noise_power, noise);
        for(int i = 0;i < len;i++){
            output[i] = signal[i] + noise[i];
        }
    }
}