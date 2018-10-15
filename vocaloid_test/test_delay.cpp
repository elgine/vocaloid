#include <vocaloid/process/delay.hpp>
#include <vocaloid/process/gain.hpp>
#include <vocaloid/process/robot.hpp>
#include <vocaloid/process/convolution.hpp>
#include <vocaloid/utils/window.hpp>
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
            float clipped = fmaxf(-1.0f, buffers[j]->Data()[i]);
            clipped = fminf(1.0f, clipped);
            long value = long(clipped * max);
            for (int k = 0; k < depth; k++) {
                byte_array[i * step + j * depth + k] = (char)((value >> 8 * k) & 0xFF);
            }
        }
    }
}

void AddByteArray(Buffer<float> **b, uint16_t bits, uint16_t channels, char* bytes, uint64_t byte_length){
    uint16_t depth = bits / 8;
    uint16_t step = depth * channels;
    float max = powf(2.0f, bits - 1);
    for(int i = 0;i < byte_length;i += step){
        for(int j = 0;j < channels;j++){
            long offset = i + j * depth;
            long value = bytes[offset] & 0xFF;
            for(int k = 1;k < depth;k++){
                value |= (long)(bytes[offset + k] << (k * 8));
            }
            b[j]->Data()[b[j]->Size()] = value/max;
            b[j]->SetSize(b[j]->Size() + 1);
        }
    }
}

Buffer<float>** LoadTele(){
    auto *reader = new WAVReader();
    int16_t ret = reader->Open("spring.wav");
    if(ret < 0)return nullptr;
    WAV_HEADER header = reader->GetHeader();
    uint64_t byte_length = 8192;
    uint64_t float_length = (header.size2 - 4)/(header.bits_per_sec/8/header.channels);
    auto *bytes = new char[byte_length];
    auto bufs = new Buffer<float>*[header.channels]{
        new Buffer<float>(float_length),
        new Buffer<float>(float_length)
    };
    while(!reader->IsEnd()){
        uint64_t data_size = reader->ReadData(bytes, byte_length);
        AddByteArray(bufs, header.bits_per_sec, header.channels, bytes, data_size);
    }
    reader->Close();
    return bufs;
}

int PlayWavFile(){
    auto kernels = LoadTele();
    auto *reader = new WAVReader();
    int16_t ret = reader->Open("speech.wav");
    if(ret < 0)return -1;
    uint64_t byte_length = 8192;
    auto *bytes = new char[byte_length];
    auto header = reader->GetHeader();
    auto float_arr_len = byte_length/header.channels/(header.bits_per_sec/8);
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
    for(auto i = 0;i < header.channels;i++){
        inputs[i] = new Buffer<float>(float_arr_len);
        outputs[i] = new Buffer<float>(float_arr_len);
        inputs[i]->SetSize(float_arr_len);
        outputs[i]->SetSize(float_arr_len);

        convolutions[i]->Initialize(header.samples_per_sec, kernels[i]->Data(), kernels[i]->Size());
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
            convolutions[i]->Process(inputs[i]->Data(), float_arr_len, outputs[i]->Data());
        }
        ToByteArray(outputs, header.bits_per_sec, header.channels, output_bytes, data_size);
        pcm_player->Push(output_bytes, data_size);
        writer->WriteData(output_bytes, data_size);
    }
    writer->Close();
    pcm_player->Flush();
    pcm_player->Close();
    reader->Close();
    return 0;
}

int main(){
    PlayWavFile();
    return 0;
}
