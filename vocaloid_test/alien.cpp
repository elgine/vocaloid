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



int main(){
    auto context = new AudioContext();
//    context->SetRecorderMode("alien.wav", 44100, 2);
    context->SetPlayerMode(44100, 2);


    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}