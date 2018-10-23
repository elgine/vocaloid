#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "audio_buffer.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext;
    class AudioNode: public enable_shared_from_this<AudioNode>{
    protected:
        AudioContext *context_;
        uint16_t num_input_nodes_;
        uint16_t num_output_nodes_;
        uint16_t channels_;
        vector<AudioNode*> inputs_;
        vector<AudioNode*> outputs_;
        AudioBuffer *input_buffer_;
        AudioBuffer *summing_buffer_;
        uint64_t frame_size_;
        bool enable_;
    public:
        static const uint16_t BITS_PER_SEC = 16;

        explicit AudioNode(AudioContext *ctx):context_(ctx),channels_(2),num_input_nodes_(0),num_output_nodes_(0){
            enable_ = true;
            frame_size_ = 1024;
            input_buffer_ = new AudioBuffer();
            summing_buffer_ = new AudioBuffer();
        }

        virtual void SetChannels(uint16_t c){
            channels_ = c;
        }

        void SetEnable(bool v){
            enable_ = v;
        }

        virtual void Initialize(uint64_t frame_size){
            frame_size_ = frame_size;
            summing_buffer_->Alloc(channels_, frame_size_);
        }

        virtual int64_t Process(AudioBuffer *in){return 0;}

        void SummingInputs(){
            summing_buffer_->Fill(0.0f);
            if(!inputs_.empty()){
                for(const auto &input : inputs_){
                    input->Pull(input_buffer_);
                    summing_buffer_->Mix(input_buffer_);
                }
            }
        }

        virtual void Pull(AudioBuffer *in){
            SummingInputs();
            Process(in);
        }

        bool HasConnectedTo(const AudioNode *n){
            return find(outputs_.begin(), outputs_.end(), n) != outputs_.end();
        }

        bool HasConnectedFrom(const AudioNode *n){
            return find(inputs_.begin(), inputs_.end(), n) != inputs_.end();
        }

        virtual void Connect(AudioNode *n){
            n->ConnectInput(this);
            if(HasConnectedTo(n))return;
            outputs_.emplace_back(n);
            num_output_nodes_++;
        }

        virtual void ConnectInput(AudioNode *n){
            if(HasConnectedFrom(n))return;
            inputs_.emplace_back(n);
            num_input_nodes_++;
        }

        virtual void Disconnect(AudioNode *n){
            auto iter = find(outputs_.begin(), outputs_.end(), n);
            if(iter != outputs_.end()){
                outputs_.erase(iter);
                num_output_nodes_--;
            }
            n->DisconnectInput(this);
        }

        virtual void DisconnectInput(const AudioNode *n){
            auto iter = find(inputs_.begin(), inputs_.end(), n);
            if(iter != inputs_.end()){
                inputs_.erase(iter);
                num_input_nodes_--;
            }
        }

        bool Enable(){
            return enable_;
        }

        uint16_t Channels(){
            return channels_;
        }

        uint16_t NumInputNodes(){
            return num_input_nodes_;
        }

        uint16_t NumOutputNodes(){
            return num_output_nodes_;
        }

        vector<AudioNode*> Inputs(){
            return inputs_;
        }

        vector<AudioNode*> Outputs(){
            return outputs_;
        }
    };
}