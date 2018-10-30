//
// Created by Elgine on 2018/10/25.
//
#include <math.h>
#include <vocaloid/node/wav_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/gain_node.hpp>
#include <vocaloid/node/delay_node.hpp>
using namespace vocaloid;

int main(){
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto source = new WavFileNode(context);
    source->SetPath("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav");
    auto delay = new DelayNode(context, 0.05f, 0.1f);
    auto osc = new OscillatorNode(context);
    osc->SetWaveform(5.0f, WAVEFORM_TYPE::SINE);
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