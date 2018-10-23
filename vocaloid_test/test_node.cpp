//
// Created by Elgine on 2018/10/19.
//
#include <vocaloid/io/wav.hpp>
#include <vocaloid/node/audio_file_node.hpp>
#include <vocaloid/node/audio_context.hpp>
#include <vocaloid/node/oscillator_node.hpp>
#include <vocaloid/node/gain_node.hpp>
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
    auto oscillator = new OscillatorNode(context);
    oscillator->GenWaveformData(880.f, WAVEFORM_TYPE::TRIANGLE, 4096);
    auto gain1 = new GainNode(context, 0.5f);
    oscillator->Connect(gain1);
    auto file = new WAVFileNode(context);
    auto master = new GainNode(context);
    file->SetPath("output1.wav");
    file->Connect(master);
    gain1->Connect(master);
    master->Connect(context->GetDestination());
    master->GetGain()->LinearRampToValueAtTime(0.1, 0);
    master->GetGain()->LinearRampToValueAtTime(0.3, 3000);
    master->GetGain()->LinearRampToValueAtTime(0.4, 5000);
    master->GetGain()->LinearRampToValueAtTime(1.0, 7000);
    master->GetGain()->LinearRampToValueAtTime(0.2, 35000);
    master->GetGain()->LinearRampToValueAtTime(0.0, 52000);

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}