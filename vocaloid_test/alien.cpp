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
    uint64_t offset_;
    uint64_t N_;
    uint32_t sample_rate_;
public:
    explicit LFO(AudioContext *ctx):AudioNode(ctx){
        offset_ = 0;
        N_ = 0;
        sample_rate_ = 2;
    }

    void Initialize(uint64_t frame_size) override {
        AudioNode::Initialize(frame_size);
        N_ = uint64_t(1.0f / sample_rate_ * context_->GetSampleRate());
    }

    int64_t Process(AudioBuffer *in) override {
        for(auto j = 0;j < frame_size_;j++){
            auto ratio = sinf(float(M_PI) * offset_/N_);
            for(auto i = 0;i < channels_;i++){
                in->Channel(i)->Data()[j] = summing_buffer_->Channel(i)->Data()[j] * ratio;
            }
            offset_ = (offset_ + 1) % N_;
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
    context->SetRecorderMode("alien.wav", 44100, 2);
    auto oscillator = new OscillatorNode(context);
    oscillator->GenWaveformData(440.0f, WAVEFORM_TYPE::SINE, 4096);
    auto oscillatorGain = new GainNode(context, 1.0f);

//    auto delay = new DelayNode(context);
//    delay->SetDelayDuration(50);
//    auto file = new WAVFileReaderNode(context);
//    file->SetPath("output1.wav");

//    file->Connect(delay);
//    oscillator->Connect(oscillatorGain);
//    oscillatorGain->Connect(delay);
//    delay->Connect(context->GetDestination());
    auto lfo = new LFO(context);
    oscillator->Connect(lfo);
    lfo->Connect(context->GetDestination());

    context->Setup();
    context->Start();
    getchar();
    context->Stop();
    return 0;
}
