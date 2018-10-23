//
// Created by Elgine on 2018/10/19.
//
#include <vocaloid/io/wav.hpp>
#include <vocaloid/node/audio_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
using namespace vocaloid;

class WAVFileNode: public AudioFileNode{
public:
explicit WAVFileNode(AudioContext *ctx):AudioFileNode(ctx){
    reader_ = new WAVReader();
}
};

int main(){
    auto context = new AudioContext();
    context->SetPlayerMode(44100, 2);
    auto file = new WAVFileNode(context);
    file->SetPath("output1.wav");
    file->Connect(context->GetDestination());
//    auto oscillator = new OscillatorNode(context);
//    oscillator->GenWaveformData(440.f, WAVEFORM_TYPE::SINE, 4096);
//    oscillator->Connect(context->GetDestination());
    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}