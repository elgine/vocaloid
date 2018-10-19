//
// Created by Elgine on 2018/10/19.
//
#include <stdint.h>
#include <vocaloid/synthesis/wgn.hpp>
#include <vocaloid/base/conversion.hpp>
#include <pcm_player/pcm_player.h>
#include <vocaloid/io/wav.hpp>
#include <vocaloid/synthesis/gain.hpp>
#include <vocaloid/synthesis/biquad.hpp>
using namespace vocaloid;
// Generate gunshot follow https://webaudio.prototyping.bbc.co.uk/gunfire/
int main(){
    uint16_t envelop_size = 4;
    uint32_t sample_rate = 44100;
    uint16_t bits_per_sec = 16;
    uint16_t channels = 1;
    uint64_t duration = 1;
    uint64_t frame_size = sample_rate * duration;
    uint64_t byte_len = frame_size * bits_per_sec / 8 * channels;
    // Initialize writer
    auto writer = new WAVWriter(sample_rate, bits_per_sec, channels);
    writer->Open("gunshot.wav");
    // Initialize player
//    auto player = new PCMPlayer();
//    player->Open(sample_rate, bits_per_sec, channels);
    // Generate noise
    vector<float> noise(frame_size);
    WGN(frame_size, 0.0965, noise);
    // Generate envelops
    vector<Gain*> envelops(envelop_size);
    for(int i = 0;i < envelop_size;i++){
        envelops[i] = new Gain();
        envelops[i]->LinearRampToValueAtTime(0, 0);
        envelops[i]->LinearRampToValueAtTime(1, 1);
        envelops[i]->LinearRampToValueAtTime(0.3, 101);
        envelops[i]->LinearRampToValueAtTime(0, 1000);
    }
    // Generate biquad filter
    auto filter = new Biquad(BIQUAD_TYPE::LOW_PASS, sample_rate, 800, 1);
    // Initialize buffers
    auto buf = new vector<float>[envelop_size];
    vector<float> output(frame_size);
    auto output_bytes = new char[byte_len];
    for(int i = 0;i < envelop_size;i++){
        buf[i].resize(frame_size);
        envelops[i]->Process(noise, frame_size, buf[i]);
        filter->Process(buf[i], frame_size, buf[i]);
        for(int j = 0;j < frame_size;j++){
            output[j] += buf[i][j];
        }
    }
    for(int i = 0;i < frame_size;i++){
        output[i] *= 5;
    }
    FloatVectorToByteArray(new vector<float>[1]{output}, frame_size, bits_per_sec, channels, output_bytes, byte_len);
    writer->WriteData(output_bytes, byte_len);
    writer->Close();
//    player->Push(output_bytes, byte_len);
//    player->Flush();
//    player->Close();
    return 0;
}