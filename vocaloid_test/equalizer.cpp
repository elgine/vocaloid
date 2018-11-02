//
// Created by Elgine on 2018/11/2.
//
#include <vocaloid/node/wav_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/biquad_node.hpp>
#include <vocaloid/node/gain_node.hpp>
#include <vocaloid/node/delay_node.hpp>

int main(){
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto source = new WavFileNode(context);
    source->SetPath("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav");


    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}