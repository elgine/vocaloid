//
// Created by Elgine on 2018/10/19.
//
#include <vector>
#include <vocaloid/synthesis/waveform.hpp>
using namespace std;
using namespace vocaloid;
int main(){
    uint64_t frame_size = 1024;
    vector<float> primary(frame_size);
    vector<float> modulating(frame_size);
    GenWaveform(WAVEFORM_TYPE::SINE, );
    return 0;
}