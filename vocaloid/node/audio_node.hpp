#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <memory>
#include "audio_buffer.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext;
    class AudioNode;
    typedef shared_ptr<AudioNode> AudioNodeRef;
    class AudioNode: public enable_shared_from_this<AudioNode>{
    protected:
        AudioContext *context_;
        uint16_t num_input_nodes_;
        uint16_t num_output_nodes_;
        uint16_t channels_;
        vector<AudioNodeRef> inputs_;
        vector<AudioNodeRef> outputs_;
        AudioBuffer *input_buffer_;
        AudioBuffer *summing_buffer_;
    public:

        explicit AudioNode(AudioContext *ctx):context_(ctx),channels_(2),num_input_nodes_(0),num_output_nodes_(0){
            input_buffer_ = new AudioBuffer();
            summing_buffer_ = new AudioBuffer();
        }

        void SetChannels(uint16_t v){
            channels_ = v;
        }

        virtual int64_t Process(AudioBuffer *in){return 0;}

        void Pull(AudioBuffer *in){
            summing_buffer_->Fill(0.0f);
            if(!inputs_.empty()){
                for(const auto &input : inputs_){
                    input->Pull(input_buffer_);
                    summing_buffer_->Mix(input_buffer_);
                }
            }
            Process(in);
        }

        bool HasConnectedTo(const AudioNodeRef &n){
            return find(outputs_.begin(), outputs_.end(), n) != outputs_.end();
        }

        bool HasConnectedFrom(const AudioNodeRef &n){
            return find(inputs_.begin(), inputs_.end(), n) != inputs_.end();
        }

        virtual void Connect(const AudioNodeRef &n){
            n->ConnectInput(shared_from_this());
            if(HasConnectedTo(n))return;
            outputs_.emplace_back(n);
        }

        void ConnectInput(const AudioNodeRef &n){
            if(HasConnectedFrom(n))return;
            inputs_.emplace_back(n);
        }

        virtual void Disconnect(const AudioNodeRef &n){
            auto iter = find(outputs_.begin(), outputs_.end(), n);
            if(iter != outputs_.end()){
                outputs_.erase(iter);
            }
            n->DisconnectInput(shared_from_this());
        }

        void DisconnectInput(const AudioNodeRef &n){
            auto iter = find(inputs_.begin(), inputs_.end(), n);
            if(iter != inputs_.end()){
                inputs_.erase(iter);
            }
        }

        virtual AudioNodeRef operator>>(const AudioNodeRef &n){
            Connect(n);
            return shared_from_this();
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