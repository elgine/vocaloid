//
// Created by Elgine on 2018/10/17.
//
#include <vector>
#include <vocaloid/process/pitch_shifter.hpp>
#include "example.hpp"
using namespace std;
using namespace vocaloid;
class PitchShifterExample: public Example{
private:
    vector<PitchShifter*> pitch_shifters_;
public:

    void Initialize(WAV_HEADER header) override {
        pitch_shifters_.clear();
        for(int i = 0;i < header.channels;i++){
            pitch_shifters_.emplace_back(new PitchShifter());
            pitch_shifters_[i]->Initialize(frame_size_, 0.75f);
            pitch_shifters_[i]->SetPitch(1.2f);
        }
    }

    uint64_t Process(vector<float>* inputs, uint16_t channel_size, uint64_t data_size_per_channel, vector<float>* outputs) override {
        for(int i = 0;i < channel_size;i++){
            data_size_per_channel = pitch_shifters_[i]->Process(inputs[i], data_size_per_channel, outputs[i]);
        }
        return data_size_per_channel;
    }
};

int main(){
    auto pitch_shifter_example = new PitchShifterExample();
    pitch_shifter_example->Run("G:\\Projects\\cpp\\vocaloid\\samples\\output1.wav");
    return 0;
}
