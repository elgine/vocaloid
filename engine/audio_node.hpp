#pragma once
#include <map>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <queue>
#include "channel.hpp"
#include "disposable.h"
#include "audio_data.hpp"
#include "audio_flow.hpp"
#include "audio_node_interface.h"
using namespace std;
namespace vocaloid{

    class AudioNode: public IAudioNode{
    protected:
        static uint64_t ID_COUNT_;
        // Unique id
        uint64_t id_;
        // Input port array
        AudioChannel** input_channels_;
        // Output port array
        AudioChannel** output_channels_;
        uint16_t num_input_channel_;
        uint16_t num_output_channel_;
        AudioFlow *flow_;

        void Process(std::shared_ptr<AudioData> in, std::shared_ptr<AudioData> out) override {

        }

        void ProcessFrame(){

        }

    public:
        // Node name set by user
        string name_;

        explicit AudioNode(uint16_t num_input_channel,
                            uint16_t num_output_channel):num_input_channel_(num_input_channel),
                                                        num_output_channel_(num_output_channel){
            id_ = ID_COUNT_++;
            flow_ = new AudioFlow(this, num_input_channel_, num_output_channel_);
            input_channels_ = new AudioChannel*[num_input_channel_];
            output_channels_ = new AudioChannel*[num_output_channel_];
        }

        AudioNode* Connect(AudioNode* dst, uint16_t out_channel = 0, uint16_t in_channel = 0){
            if(Connected(dst, out_channel, in_channel))return dst;

            return dst;
        }

        bool Connected(AudioNode* dst, uint16_t dst_channel = 0, uint16_t src_channel = 0){
            bool ret = false;

            return ret;
        }

        void Disconnect(AudioNode* dst){

        }

        void Disconnect(AudioNode* dst, uint16_t out_channel, uint16_t in_channel){

        }

        void Run(){

        }

        uint64_t GetId(){
            return id_;
        }
    };

    uint64_t AudioNode::ID_COUNT_ = 0;
}