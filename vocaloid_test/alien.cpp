//
// Created by Elgine on 2018/10/25.
//
#include <math.h>
#include <vocaloid/io/wav.hpp>
#include <vocaloid/node/audio_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/gain_node.hpp>
#include <vocaloid/node/delay_node.hpp>
using namespace vocaloid;

class WAVFileReaderNode: public AudioFileNode{
public:
    explicit WAVFileReaderNode(AudioContext *ctx):AudioFileNode(ctx){
        reader_ = new WAVReader();
    }
};

int main(){
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto source = new WAVFileReaderNode(context);
    source->SetPath("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav");
    auto delay = new DelayNode(context, 0.05f, 1.0f);
    auto osc = new OscillatorNode(context);
    osc->GenWaveformData(5.0f, WAVEFORM_TYPE::SINE, 4096);
    auto osc_gain = new GainNode(context, 0.05f);
    osc->Connect(osc_gain);
    osc_gain->Connect(delay->delay_time_);
    source->Connect(delay);
    delay->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}