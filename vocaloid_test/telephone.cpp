//
// Created by Elgine on 2018/10/31.
//
#include <vocaloid/node/wav_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/biquad_node.hpp>
#include <vocaloid/node/gain_node.hpp>
#include <vocaloid/node/delay_node.hpp>
#include <vocaloid/node/dynamic_compressor_node.hpp>
using namespace vocaloid;

int main(){
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto source = new WavFileNode(context);
    source->SetPath("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav");

    auto dynamicCompressor = new DynamicsCompressorNode(context);

    auto biquad1 = new BiquadNode(context);
    biquad1->frequency_->value_ = 2000;
    biquad1->type_ = BIQUAD_TYPE::LOW_PASS;

    auto biquad2 = new BiquadNode(context);
    biquad2->frequency_->value_ = 2000;
    biquad2->type_ = BIQUAD_TYPE::LOW_PASS;

    auto biquad3 = new BiquadNode(context);
    biquad3->frequency_->value_ = 500;
    biquad3->type_ = BIQUAD_TYPE::HIGH_PASS;

    auto biquad4 = new BiquadNode(context);
    biquad4->frequency_->value_ = 500;
    biquad4->type_ = BIQUAD_TYPE::HIGH_PASS;

    source->Connect(biquad1);
    biquad1->Connect(biquad2);
    biquad2->Connect(biquad3);
    biquad3->Connect(biquad4);
    biquad4->Connect(dynamicCompressor);
    dynamicCompressor->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}
