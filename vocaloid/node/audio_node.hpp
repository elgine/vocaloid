#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "audio_buffer.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext;
    class AudioNode{
    protected:
        AudioContext *context_;
        uint16_t num_input_nodes_;
        uint16_t num_output_nodes_;
        uint16_t channels_;
        vector<shared_ptr<AudioNode>> inputs_;
        vector<shared_ptr<AudioNode>> outputs_;
        AudioBuffer *input_buffer_;
        AudioBuffer *inner_buffer_;
    public:

        explicit AudioNode(AudioContext *ctx):context_(ctx),channels_(2),num_input_nodes_(0),num_output_nodes_(0){
            input_buffer_ = new AudioBuffer();
            inner_buffer_ = new AudioBuffer();
        }

        virtual int64_t Process(AudioBuffer *in, AudioBuffer *out) = 0;

        virtual int64_t PullBuffer(AudioBuffer *in){
            inner_buffer_->Fill(0.0f);
            for(auto input : inputs_){
                input_buffer_->Fill(0.0f);
                input->PullBuffer(input_buffer_);
                inner_buffer_->Mix(input_buffer_);
            }
            return Process(inner_buffer_, in);
        }

        bool HasConnectedTo(AudioNode *n){
            return find(outputs_.begin(), outputs_.end(), n) != outputs_.end();
        }

        bool HasConnectedFrom(AudioNode *n){
            return find(inputs_.begin(), inputs_.end(), n) != inputs_.end();
        }

        void Connect(AudioNode *n){
            n->ConnectInput(this);
            if(HasConnectedTo(n))return;
            outputs_.push_back(n);
        }

        void ConnectInput(AudioNode *n){
            if(HasConnectedFrom(n))return;
            inputs_.push_back(n);
        }

        void Disconnect(AudioNode *n){
            auto iter = find(outputs_.begin(), outputs_.end(), n);
            if(iter != outputs_.end()){
                outputs_.erase(iter);
            }
            n->DisconnectInput(this);
        }

        void DisconnectInput(AudioNode *n){
            auto iter = find(inputs_.begin(), inputs_.end(), n);
            if(iter != inputs_.end()){
                inputs_.erase(iter);
            }
        }

        AudioNode& operator>>(AudioNode &n){

            return n;
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
    };
}