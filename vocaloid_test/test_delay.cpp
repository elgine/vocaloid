#include <vocaloid/synthesis/delay.hpp>
#include <vocaloid/synthesis/gain.hpp>
#include <vocaloid/synthesis/robot.hpp>
#include <vocaloid/synthesis/convolution.hpp>
#include <vocaloid/synthesis/pitch_shifter.hpp>
#include <vocaloid/maths/window.hpp>
#include <vocaloid/io/wav.hpp>
#include <pcm_player/pcm_player.h>
using namespace vocaloid;

void FromByteArray(const char *byte_array, uint64_t byte_length, uint16_t bits, uint16_t channels, Buffer<float> **buffers){
    uint16_t depth = bits / 8;
    uint16_t step = depth * channels;
    float max = powf(2.0f, bits - 1);
    for(int i = 0;i < byte_length;i += step){
        for(int j = 0;j < channels;j++){
            long offset = i + j * depth;
            long value = byte_array[offset] & 0xFF;
            for(int k = 1;k < depth;k++){
                value |= (long)(byte_array[offset + k] << (k * 8));
            }
            buffers[j]->Data()[i / step] = value/max;
        }
    }
}

void ToByteArray(Buffer<float> **buffers, uint16_t bits, uint16_t channels, char *byte_array, uint64_t &byte_length){
    uint16_t depth = bits / 8;
    uint16_t step = depth * channels;
    uint64_t buffer_size = buffers[0]->Size();
    byte_length = step * buffer_size;
    float max = powf(2.0f, bits - 1);
    for(int i = 0;i < buffer_size;i++) {
        for (int j = 0; j < channels; j++) {
            float clipped = buffers[j]->Data()[i];
            if(clipped > 0.999f)clipped = 0.999f;
            else if(clipped < -0.999f)clipped = -0.999f;
            auto value = (long)(clipped * max);
            for (int k = 0; k < depth; k++) {
                byte_array[i * step + j * depth + k] = (char)((value >> 8 * k) & 0xFF);
            }
        }
    }
}

Buffer<float>** LoadKernel(){
    auto *reader = new WAVReader();
    int16_t ret = reader->Open("G:\\projects\\cpp\\vocaloid\\samples\\radio.wav");
    if(ret < 0)return nullptr;
    WAV_HEADER header = reader->GetHeader();
    uint64_t byte_length = 8192;
    uint64_t float_length = header.size2/header.block_align;
    auto *bytes = new char[byte_length];
    auto bufs = new Buffer<float>*[header.channels]{
        new Buffer<float>(float_length),
        new Buffer<float>(float_length)
    };
    bufs[0]->SetSize(float_length);
    bufs[1]->SetSize(float_length);
    float max = powf(2.0f, header.bits_per_sec - 1);
    uint16_t depth = header.bits_per_sec/8;
    uint64_t buf_offset = 0;
    while(!reader->IsEnd()){
        uint64_t data_size = reader->ReadData(bytes, byte_length);
        for(int i = 0;i < data_size;i += header.block_align){
            for(int j = 0;j < header.channels;j++){
                long offset = i + j * depth;
                long value = bytes[offset] & 0xFF;
                for(int k = 1;k < depth;k++){
                    value |= (long)(bytes[offset + k] << (k * 8));
                }
                bufs[j]->Data()[i / header.block_align + buf_offset] = value/max;
            }
        }
        buf_offset += data_size / header.block_align;
    }
    reader->Close();
    return bufs;
}

int PlayWavFile(){
    auto kernels = LoadKernel();
    auto *reader = new WAVReader();
    int16_t ret = reader->Open("G:\\projects\\cpp\\vocaloid\\samples\\speech.wav");
    if(ret < 0)return -1;
    uint64_t byte_length = kernels[0]->Size() * 4;
    auto *bytes = new char[byte_length];
    auto header = reader->GetHeader();
    auto float_arr_len = byte_length/header.block_align;
    vector<Delay*> delays = {
        new Delay(5000, header.samples_per_sec, header.bits_per_sec),
        new Delay(5000, header.samples_per_sec, header.bits_per_sec)
    };
    vector<Robot*> robots = {
        new Robot(500, header.samples_per_sec),
        new Robot(500, header.samples_per_sec)
    };
    vector<Gain*> gains = {
        new Gain(0.1, header.samples_per_sec, header.bits_per_sec),
        new Gain(0.1, header.samples_per_sec, header.bits_per_sec)
    };
    gains[0]->LinearRampToValueAtTime(0.1, 0);
    gains[1]->LinearRampToValueAtTime(0.1, 0);
    gains[0]->LinearRampToValueAtTime(0.3, 3000);
    gains[1]->LinearRampToValueAtTime(0.3, 3000);
    gains[0]->LinearRampToValueAtTime(0.4, 5000);
    gains[1]->LinearRampToValueAtTime(0.4, 5000);
    gains[0]->LinearRampToValueAtTime(1.0, 7000);
    gains[1]->LinearRampToValueAtTime(1.0, 7000);
    gains[0]->LinearRampToValueAtTime(0.2, 20000);
    gains[1]->LinearRampToValueAtTime(0.2, 20000);
    gains[0]->LinearRampToValueAtTime(0.0, 22000);
    gains[1]->LinearRampToValueAtTime(0.0, 22000);

    auto *inputs = new Buffer<float>*[header.channels];
    auto *outputs = new Buffer<float>*[header.channels];
    auto *output_bytes = new char[float_arr_len * header.block_align];
    vector<Convolution*> convolutions = {
        new Convolution(float_arr_len),
        new Convolution(float_arr_len)
    };
    vector<PitchShifter*> pitch_shifters = {
            new PitchShifter(),
            new PitchShifter()
    };
    for(auto i = 0;i < header.channels;i++){
        inputs[i] = new Buffer<float>(float_arr_len);
        outputs[i] = new Buffer<float>(float_arr_len);
        inputs[i]->SetSize(float_arr_len);
        outputs[i]->SetSize(float_arr_len);
//        pitch_shifters[i]->Initialize(float_arr_len, 0.5f);
//        pitch_shifters[i]->SetPitch(1.2f);
//        pitch_shifters[i]->SetTempo(1.25f);
        convolutions[i]->Initialize(kernels[i]->Data(), kernels[i]->Size());
    }

    auto *writer = new WAVWriter(header.samples_per_sec, header.bits_per_sec, header.channels);
    writer->Open("convolution.wav");

    auto *pcm_player = new PCMPlayer();
    pcm_player->Open(header.samples_per_sec, header.bits_per_sec, header.channels);

    while(!reader->IsEnd()){
        uint64_t data_size = reader->ReadData(bytes, byte_length);
        FromByteArray(bytes, data_size, header.bits_per_sec, header.channels, inputs);
        for(auto i = 0;i < header.channels;i++){
//            delays[i]->Process(inputs[i], outputs[i]);
//            gains[i]->Process(inputs[i], float_arr_len, outputs[i]);
//            robots[i]->Process(inputs[i], outputs[i]);
//            pitch_shifters[i]->Process(inputs[i]->Data(), float_arr_len, outputs[i]->Data());
            convolutions[i]->Process(inputs[i]->Data(), float_arr_len, outputs[i]->Data());
        }
        ToByteArray(outputs, header.bits_per_sec, header.channels, output_bytes, data_size);
        pcm_player->Push(output_bytes, data_size);
//        writer->WriteData(output_bytes, data_size);
    }
//    writer->Close();
    pcm_player->Flush();
    pcm_player->Close();
    reader->Close();
    return 0;
}

int main(){
    PlayWavFile();
    return 0;
}
//#include <vocaloid/process/delay.hpp>
//#include <vector>
//#include "example.hpp"
//using namespace std;
//using namespace vocaloid;
//
//class DelayExample: public Example{
//private:
//    vector<Delay*> delay_nodes;
//public:
//    void Initialize(WAV_HEADER header) override {
//        delay_nodes.reserve(header.channels);
//        for(int i = 0;i < header.channels;i++){
//            delay_nodes.emplace_back(new Delay(4000, header.samples_per_sec, header.bits_per_sec));
//        }
//    }
//
//    uint64_t Process(vector<float>* inputs, uint16_t channel_size, uint64_t data_size_per_channel, vector<float>* outputs) override {
//        for(int i = 0;i < channel_size;i++){
//            delay_nodes[i]->Process(inputs[i], data_size_per_channel, outputs[i]);
//        }
//        return data_size_per_channel;
//    }
//};
//
//int main(){
//    auto example = new DelayExample();
//    example->Run("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav");
//    return 0;
//}
