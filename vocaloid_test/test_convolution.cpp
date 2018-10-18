#include <vector>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <vocaloid/synthesis/convolution.hpp>
#include <vocaloid/base/data_format.hpp>
#include "example.hpp"
using namespace std;
using namespace vocaloid;

class ConvolutionExample: public Example{
private:
    vector<Convolution*> convolutions_;
    vector<float> *kernel_;
public:

    string kernel_file_path_;

    uint64_t LoadKernel(const char* kernel_path){
        auto *reader = new WAVReader();
        int16_t ret = reader->Open(kernel_path);
        if(ret < 0)return 0;
        auto header = reader->GetHeader();
        uint64_t float_length = header.size2/header.block_align;
        auto bytes = new char[header.size2];
        kernel_ = new vector<float>[header.channels];
        for(int i = 0;i < header.channels;i++){
            kernel_[i].resize(float_length);
        }
        uint64_t data_size = reader->ReadData(bytes, header.size2);
        ByteArrayToFloatVector(bytes, data_size, header.bits_per_sec, header.channels, kernel_);
        reader->Close();
        return data_size / header.block_align;
    }

    void Initialize(WAV_HEADER header) override {
        uint64_t frame_size = LoadKernel(kernel_file_path_.c_str());
        frame_size_ = frame_size;
        convolutions_.resize(header.channels);
        for(int i = 0;i < header.channels;i++){
            convolutions_[i] = new Convolution(frame_size_);
            convolutions_[i]->Initialize(kernel_[i], frame_size_);
        }
    }

    uint64_t Process(vector<float>* inputs, uint16_t channel_size,
                    uint64_t data_size_per_channel, vector<float>* outputs) override {
        auto start = clock();
        for(int i = 0;i < channel_size;i++){
            convolutions_[i]->Process(inputs[i], data_size_per_channel, outputs[i]);
        }
        auto finish = clock();
//        cout << (double)(finish - start)/CLOCKS_PER_SEC << endl;
        return data_size_per_channel;
    }
};

int main(){
    auto convolutionExample = new ConvolutionExample();
    convolutionExample->kernel_file_path_ = "G:\\Projects\\cpp\\vocaloid\\samples\\radio.wav";
    convolutionExample->Run("G:\\Projects\\cpp\\vocaloid\\samples\\speech.wav", "convolution.wav");
    return 0;
}