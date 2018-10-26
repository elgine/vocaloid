//
// Created by Elgine on 2018/10/24.
//
#include <math.h>
#include <vocaloid/io/wav.hpp>
#include <vocaloid/node/audio_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/gain_node.hpp>
#include <vocaloid/node/delay_node.hpp>
using namespace vocaloid;

int main() {
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto oscillator = new OscillatorNode(context);
    oscillator->GenWaveformData(440.0f, WAVEFORM_TYPE::SINE, 4096);
    auto gain = new GainNode(context);
    auto lfo = new OscillatorNode(context);
    lfo->GenWaveformData(2.0f, WAVEFORM_TYPE::SINE, 4096);
    lfo->Connect(gain->gain_);
    oscillator->Connect(gain);
    gain->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}
