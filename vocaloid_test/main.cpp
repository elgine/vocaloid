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

//TEST(TestGain, Normal){
//	vector<float> samples(512);
//	GenWaveform(WAVEFORM_TYPE::SINE, 440.0f, 512, samples);
//    vector<float> output(512);
//    Gain(samples, 2.0, 512, output);
//    ASSERT_FLOAT_EQ(samples[1] * 2.0f, output[1]);
//}

//TEST(TestAudioGraph, FindNodeByName) {
//	auto ctx = new AudioContext();
//	auto node = new AudioNode(ctx);
//	auto node1 = new AudioNode(ctx);
//	auto node2 = new AudioNode(ctx);
//	node1->name = "node1";
//	node2->name = "node2";
//	node->name = "root-node";
//	ctx->AddNode(node);
//	ctx->AddNode(node1);
//	ctx->AddNode(node2);
//	ctx->Connect(node, node1);
//	ctx->Connect(node, node2);
//	ctx->Connect(node1, node2);
//	ctx->DumpInfo();
//	/*vector<AudioNode*> sources = ctx->GetRoots();
//	auto result = ctx->FindNodeByName("node1");
//	ASSERT_TRUE(result == node);*/
//}

//TEST(Interpolator, Linear) {
//	Interpolator<float> *interpolator = new CubicInterpolator<float>();
//	int input_len = 16, output_len = 32;
//	vector<float> input(input_len);
//	for (int i = 0; i < input_len; i++) {
//		input[i] = i;
//	}
//	vector<float> output(output_len);
//	interpolator->Interpolate(input, input_len, output, output_len);
//	for (auto o : output) {
//		cout << o << " ";
//	}
//	cout << endl;
//}

//int main(int argc, char** argv) {
//    ::testing::InitGoogleTest(&argc, argv);
//    int result = RUN_ALL_TESTS();
//    getchar();
//    return result;
//}

#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

namespace little_endian_io
{
    template <typename Word>
    std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
    {
        for (; size; --size, value >>= 8)
            outs.put( static_cast <char> (value & 0xFF) );
        return outs;
    }
}
using namespace little_endian_io;


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

int GenerateWav()
{
    ofstream f( "example.wav", ios::binary );

    // Write the file headers
    f << "RIFF----WAVEfmt ";     // (chunk size to be filled in later)
    write_word( f,     16, 4 );  // no extension data
    write_word( f,      1, 2 );  // PCM - integer samples
    write_word( f,      1, 2 );  // two channels (stereo file)
    write_word( f,  44100, 4 );  // samples per second (Hz)
    write_word( f, 176400, 4 );  // (Sample Rate * BitsPerSample * Channels) / 8
    write_word( f,      4, 2 );  // data block size (size of two integer samples, one for each channel, in bytes)
    write_word( f,     16, 2 );  // number of bits per sample (use a multiple of 8)

    // Write the data chunk header
    size_t data_chunk_pos = f.tellp();
    f << "data----";  // (chunk size to be filled in later)

    // Write the audio samples
    // (We'll generate a single C4 note with a sine wave, fading from left to right)
    constexpr double two_pi = 6.283185307179586476925286766559;
    constexpr double max_amplitude = 32760;  // "volume"

    double hz        = 44100;    // samples per second
    double frequency = 261.626;  // middle C
    double seconds   = 2.5;      // time

    double sample_rate = 440.f;
    long samples = 4096;
    vector<float> waveform(samples);
    vector<float> output;
    GenWaveform(WAVEFORM_TYPE::SINE, sample_rate, samples, waveform);

    // Resample
    auto *sampler = new Resampler<float>(hz/(sample_rate * samples), INTERPOLATOR_TYPE::CUBIC);
    uint64_t out_size = sampler->Resample(waveform, samples, output);

    uint8_t* bytes = new uint8_t[2 * out_size];
    uint64_t byte_length = FloatToBytes(output, out_size, 16, bytes);

    int N = hz * 2;  // total number of samples
    for (int n = 0; n < N; n ++) {
        uint16_t value = output[n % out_size] * max_amplitude;
        write_word(f, value, 2);
    }

    // (We'll need the final file size to fix the chunk sizes above)
    size_t file_length = f.tellp();

    // Fix the data chunk header to contain the data size
    f.seekp( data_chunk_pos + 4 );
    write_word( f, file_length - data_chunk_pos + 8 );

    // Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
    f.seekp( 0 + 4 );
    write_word( f, file_length - 8, 4 );
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

    double seconds = 2.0f;
    uint64_t N = (uint64_t)ceil((double)sample_rate * seconds * bits/8),
            times = N / byte_length;
    for(int i = 0;i < times;i++){
        writer->WritePCMData(bytes, byte_length);
    }
    writer->Close();
    return 0;
}

int main(){
    GenerateWav();
    GenerateWavWithWAVWriter();
    return 0;
}