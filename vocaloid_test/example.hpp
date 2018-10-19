#pragma once
#include <vector>
#include <stdint.h>
#include <vocaloid/synthesis/delay.hpp>
#include <vocaloid/synthesis/gain.hpp>
#include <vocaloid/synthesis/robot.hpp>
#include <vocaloid/synthesis/convolution.hpp>
#include <vocaloid/synthesis/pitch_shifter.hpp>
#include <vocaloid/maths/window.hpp>
#include <vocaloid/base/conversion.hpp>
#include <vocaloid/io/wav.hpp>
#include <pcm_player/pcm_player.h>
using namespace std;
using namespace vocaloid;

class Example{
public:
    uint64_t frame_size_ = 8192;

    virtual uint64_t Process(vector<float>* inputs, uint16_t channel_size, uint64_t data_size_per_channel, vector<float>* outputs) = 0;

    virtual void Initialize(WAV_HEADER header) = 0;

    void Run(const char* path, const char* output_path){
        auto *reader = new WAVReader();
        int16_t ret = reader->Open(path);
        if(ret < 0)return;
        auto header = reader->GetHeader();

        Initialize(header);

        uint64_t byte_length = frame_size_ * header.block_align;
        auto *bytes = new char[byte_length];
        auto *output_bytes = new char[byte_length];

        auto inputs = new vector<float>[header.channels];
        auto outputs = new vector<float>[header.channels];
        for(int i = 0;i < header.channels;i++){
            inputs[i].resize(frame_size_);
            outputs[i].resize(frame_size_);
        }

        auto *pcm_player = new PCMPlayer();
        pcm_player->Open(header.samples_per_sec, header.bits_per_sec, header.channels);

        auto writer = new WAVWriter(header.samples_per_sec, header.bits_per_sec, header.channels);
        writer->Open(output_path);
        while(!reader->IsEnd()){
            uint64_t data_size = reader->ReadData(bytes, byte_length);
            ByteArrayToFloatVector(bytes, data_size, header.bits_per_sec, header.channels, inputs);
            uint64_t buffer_size = Process(inputs, header.channels, frame_size_, outputs);
            FloatVectorToByteArray(outputs, buffer_size, header.bits_per_sec, header.channels, output_bytes, data_size);
            pcm_player->Push(output_bytes, data_size);
            writer->WriteData(output_bytes, data_size);
        }
        writer->Close();
        pcm_player->Flush();
        pcm_player->Close();
        reader->Close();
    }
};