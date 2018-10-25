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

class LFO: public AudioNode{
private:
    OscillatorNode *node_;
    AudioBuffer *in_;
public:
    explicit LFO(AudioContext *ctx):AudioNode(ctx){
        in_ = new AudioBuffer(1, 1024);
        node_ = new OscillatorNode(ctx);
        node_->GenWaveformData(2.0f, WAVEFORM_TYPE::SINE, 4096);
    }

    void Initialize(uint64_t frame_size) override {
        AudioNode::Initialize(frame_size);
        node_->Initialize(frame_size);
        in_->Alloc(1, frame_size);
    }

    int64_t Process(AudioBuffer *in) override {
        node_->Process(in_);
        for(auto j = 0;j < frame_size_;j++){
            for(auto i = 0;i < channels_;i++){
                in->Channel(i)->Data()[j] = summing_buffer_->Channel(i)->Data()[j] * (in_->Channel(0)->Data()[j] + 0.5f);
            }
        }
        return frame_size_;
    }
};

class WAVFileReaderNode: public AudioFileNode{
public:
    explicit WAVFileReaderNode(AudioContext *ctx):AudioFileNode(ctx){
        reader_ = new WAVReader();
    }
};

int main() {
    auto context = new AudioContext();
//    context->SetRecorderMode("alien.wav", 44100, 2);
    context->SetPlayerMode(44100, 2);
    auto oscillator = new OscillatorNode(context);
    oscillator->GenWaveformData(440.0f, WAVEFORM_TYPE::SINE, 4096);
    auto lfo = new LFO(context);
    oscillator->Connect(lfo);
    lfo->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}
