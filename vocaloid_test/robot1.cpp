//
// Created by Elgine on 2018/10/30.
//
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
    auto osc = new OscillatorNode(context);
    osc->SetFrequency(700);
    auto osc_gain = new GainNode(context, 0.004);
    auto biquad = new BiquadNode(context);
    biquad->type_ = BIQUAD_TYPE::HIGH_PASS;
    biquad->frequency_->value_ = 695;

    osc->Connect(osc_gain);
    osc_gain->Connect(delay->delay_time_);

    source->Connect(delay);
    delay->Connect(biquad);
    biquad->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}