#include <iostream>
#include <stdio.h>
#include <gtest/gtest.h>
#include <waveform.hpp>
#include <tone.hpp>
#include <process.hpp>
#include <window.hpp>
#include <emitter.hpp>
#include <interpolator.hpp>
#include <resampler.hpp>
#include <resampler.hpp>
#include <fstream>
#include <thread>
#include <buffer.hpp>
#include <wav.hpp>
#include <pcm_player.h>

using namespace std;
using namespace vocaloid;

TEST(TestFequencyToNote, Normal){
    string note;
    note = FreqToNote(27.500);
    ASSERT_STREQ("A0", note.c_str());
    note = FreqToNote(440.f);
    ASSERT_STREQ("A4", note.c_str());
    note = FreqToNote(466.164f);
    ASSERT_STREQ("A#4", note.c_str());
    note = FreqToNote(7902.133);
    ASSERT_STREQ("B8", note.c_str());
}

TEST(TestNoteToFequency, Normal){
    float frequency;
    frequency = NoteToFreq("A0");
    ASSERT_FLOAT_EQ(frequency, 27.500);
    frequency = NoteToFreq("A4");
    ASSERT_FLOAT_EQ(frequency, 440.f);
    frequency = NoteToFreq("A#4");
    ASSERT_FLOAT_EQ(frequency, 466.164);
    frequency = NoteToFreq("B8");
    ASSERT_FLOAT_EQ(frequency, 7902.133);
}

//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    int result = RUN_ALL_TESTS();
//    getchar();
//    return result;
//}

// Read pcm data from wav file
int PlayWavFile(){
    auto *reader = new wav::Reader();
    uint16_t ret = reader->Open("output.wav");
    if(ret < 0)return -1;
    uint64_t byte_length = 4096;
    auto *bytes = new uint8_t[byte_length];

    auto *pcm_player = new PCMPlayer();
    wav::WAV_HEADER header = reader->GetHeader();
    pcm_player->Open(header.samples_per_sec, header.bits_per_sec, header.channels);
    while(!reader->IsEnd()){
        uint64_t data_size = reader->ReadPCMData(bytes, byte_length);
        pcm_player->Push((const char*)bytes, data_size);
    }
    pcm_player->Flush();
    pcm_player->Close();
    reader->Close();
    return 0;
}

// Write pcm data to wav file
uint64_t FloatToBytes(vector<float> input, uint64_t n, uint16_t bits, uint8_t *output) {
    int depth = bits / 8;
    int output_len = (int)floorf(n * depth);
    float range = powf(2, bits - 1);
    for (int i = 0; i < n; i++) {
        short v = input[i] * range;
        for (int j = 0; j < depth; j++) {
            output[i * depth + j] = v >> (8 * j) & 0xFF;
        }
    }
    return output_len;
}

int GenerateWavWithWAVWriter(){
    uint32_t sample_rate = 44100, waveform_sample_rate = 440;
    uint16_t bits = 16, channels = 1;
    uint64_t waveform_size = 4096;
    vector<float> waveform(waveform_size);
    vector<float> resampled_waveform;
    GenWaveform(WAVEFORM_TYPE::SINE, waveform_sample_rate, waveform_size, waveform);
    // Resammple waveform
    auto *resampler = new Resampler<float>((float)sample_rate/float(waveform_sample_rate * waveform_size), INTERPOLATOR_TYPE::CUBIC);
    uint64_t resampled_waveform_len = resampler->Resample(waveform, waveform_size, resampled_waveform);

    // Transfer to byte array
    uint8_t *bytes = new uint8_t[bits/8 * channels * resampled_waveform_len];
    uint64_t byte_length = FloatToBytes(resampled_waveform, resampled_waveform_len, bits, bytes);

    auto *writer = new wav::Writer(sample_rate, bits, channels);
    writer->Open("output.wav");

    double seconds = 5.0f;
    uint64_t N = (uint64_t)ceil((double)sample_rate * seconds * bits/8),
            times = N / byte_length;
    for(int i = 0;i < times;i++){
        writer->WritePCMData(bytes, byte_length);
    }
    writer->Close();
    return 0;
}

//int main(){
//    GenerateWavWithWAVWriter();
//    PlayWavFile();
//    return 0;
//}