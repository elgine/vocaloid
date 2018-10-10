//
// Created by Elgine on 2018/9/27.
//
#include <stdint.h>
#include <math.h>
#include <vector>
#include <resample.hpp>
#include <waveform.hpp>
#include <interpolate.hpp>
#include <wav.hpp>
using namespace std;
using namespace vocaloid;
// Write pcm data to wav file
uint64_t FloatToBytes(vector<float> input, uint64_t n, uint16_t bits, char *output) {
    int depth = bits / 8;
    int output_len = (int)floorf(n * depth);
    float range = powf(2, bits - 1);
    for (int i = 0; i < n; i++) {
        short v = input[i] * range;
        for (int j = 0; j < depth; j++) {
            output[i * depth + j] = v >> (8 * j) & 0xFF;
        }
    }
    return output_len;
}

int GenerateWavWithWAVWriter(){
    uint32_t sample_rate = 44100, waveform_sample_rate = 440;
    uint16_t bits = 16, channels = 1;
    uint64_t waveform_size = 4096;
    vector<float> waveform(waveform_size);
    vector<float> resampled_waveform;
    GenWaveform(WAVEFORM_TYPE::SINE, waveform_sample_rate, waveform_size, waveform);
    // Resammple waveform
    uint64_t resampled_waveform_len = Resample(waveform, waveform_size,
                                               INTERPOLATOR_TYPE::CUBIC,
                                               (float)sample_rate/float(waveform_sample_rate * waveform_size),
                                               resampled_waveform);

    // Transfer to byte array
    char *bytes = new char[bits/8 * channels * resampled_waveform_len];
    uint64_t byte_length = FloatToBytes(resampled_waveform, resampled_waveform_len, bits, bytes);

    auto *writer = new wav::Writer(sample_rate, bits, channels);
    writer->Open("output.wav");

    double seconds = 5.0f;
    uint64_t N = (uint64_t)ceil((double)sample_rate * seconds * bits/8),
            times = N / byte_length;
    for(int i = 0;i < times;i++){
        writer->WritePCMData(bytes, byte_length);
    }
    writer->Close();
    return 0;
}

int main(){
    GenerateWavWithWAVWriter();
    return 0;
}