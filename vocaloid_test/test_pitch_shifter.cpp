//
// Created by Elgine on 2018/9/27.
//
#include <vector>
#include <wav.hpp>
#include <buffer.hpp>
#include <buffer_queue.hpp>
#include <pitch_shfiter.hpp>
#include <pcm_player.h>
using namespace std;
using namespace vocaloid;
// Read pcm data from wav file
int PlayWavFile(){
    auto *reader = new wav::WAVReader();
    int16_t ret = reader->Open("output1.wav");
    if(ret < 0)return -1;
    uint64_t byte_length = 8192;
    auto *bytes = new char[byte_length];
    auto *output_bytes = new char[byte_length];

    auto header = reader->GetHeader();
    vector<PitchShifter*> pitch_shifters = {
            new PitchShifter(),
            new PitchShifter()
    };

    auto *data = new vector<float>[header.channels];

    uint32_t float_length = byte_length / header.block_align;
    for(int i = 0;i < header.channels;i++){
        pitch_shifters[i]->Initialize(float_length,
                                      reader->GetHeader().samples_per_sec, 0.75f);
        pitch_shifters[i]->SetPitch(0.8f);
        pitch_shifters[i]->SetTempo(1.25f);
        data[i].resize(float_length);
    }

    auto *buffer = new Buffer();
    buffer->Initialize(header.samples_per_sec, header.bits_per_sec, header.channels);

    auto *buffer_queue = new BufferQueue(header.channels);

    auto *pcm_player = new PCMPlayer();
    pcm_player->Open(header.samples_per_sec, header.bits_per_sec, header.channels);

//    auto *writer = new wav::Writer(header.samples_per_sec, header.bits_per_sec, header.channels);
//    writer->Open("pitch.wav");

    while(!reader->IsEnd()){
        uint64_t data_size = reader->ReadData(bytes, byte_length);
        buffer->FromByteArray(bytes, data_size);
        for(int i = 0;i < buffer->GetChannels();i++){
            pitch_shifters[i]->Process(buffer->GetChannelAt(i), float_length);
            while(!pitch_shifters[i]->IsOutputQueueEmpty(float_length)){
                uint32_t length = pitch_shifters[i]->PopFrame(data[i], float_length);
                buffer_queue->Push(i, data[i], length);
            }
        }
        while(buffer_queue->IsCountAvalidated(float_length)){
            data_size = buffer_queue->Pop(buffer, float_length);
            buffer->ToByteArray(output_bytes, data_size);
            pcm_player->Push(output_bytes, data_size);
//            writer->WritePCMData(output_bytes, data_size);
        }
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