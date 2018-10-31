//
// Created by Elgine on 2018/10/31.
//
#include <vocaloid/dsp/waveform.hpp>
#include <vocaloid/node/wav_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/biquad_node.hpp>
#include <vocaloid/node/gain_node.hpp>
#include <vocaloid/node/delay_node.hpp>
using namespace vocaloid;

int main(){
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto source = new WavFileNode(context);
    source->SetPath("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav");
    auto delay = new DelayNode(context, 0.01);
    auto osc1 = new OscillatorNode(context);
    osc1->SetWaveform(50, WAVEFORM_TYPE::SAWTOOTH);
    auto osc2 = new OscillatorNode(context);
    osc2->SetWaveform(1000, WAVEFORM_TYPE::SAWTOOTH);
    auto osc3 = new OscillatorNode(context);
    osc3->SetFrequency(50);
    auto gain = new GainNode(context, 0.004);
    osc1->Connect(gain);
    osc2->Connect(gain);
    osc3->Connect(gain);
    gain->Connect(delay->delay_time_);
    source->Connect(delay);
    delay->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}