#pragma once
#include <vector>
#include <math.h>
#include <stdint.h>
#include <memory>
#include <queue>
#include "disposable.h"
#include "audio_data.hpp"
#include "audio_node_interface.h"
using namespace std;
namespace vocaloid{

    class AudioNode;
    class AudioChannel: public IDisposable{
    friend class AudioFlow;
    private:
        AudioNode* owner_;
        uint16_t channel_index_;
        queue<shared_ptr<AudioData>> buffer_;
    public:
        explicit AudioChannel(AudioNode* owner, uint16_t channel_index):owner_(owner),channel_index_(channel_index){}

        void PushFrame(shared_ptr<AudioData> data){
            buffer_.emplace(data);
        }

        shared_ptr<AudioData> PullFrame(){
            auto d = buffer_.front();
            buffer_.pop();
            return d;
        }

        void Dispose() override {
            while(!buffer_.empty())
                buffer_.pop();
        }

        size_t GetDataSize(){
            return buffer_.size();
        }
    };

    class AudioChannelGroup{
    private:
        vector<AudioChannel*> channels_;
    public:
        void AddChannel(AudioChannel *channel){
            channels_.emplace_back(channel);
        }

        void RemoveChannel(AudioChannel *channel){
            auto iter = find(channels_.begin(), channels_.end(), channel);
            if(iter != channels_.end()){
                channels_.erase(iter);
            }
        }

        void PushFrame(const shared_ptr<AudioData> &data){
            for(auto channel : channels_){
                channel->PushFrame(data);
            }
        }

        void PullFrame(AudioData &data){
            data.Silence(data.size_);
            for(auto channel : channels_){
                auto f = channel->PullFrame();
                data.Plus(*f, data.size_);
            }
        }

        uint64_t FramesCanBeProcessed(){
            uint64_t size = ULONG_MAX;
            for(auto channel : channels_){
                size = min(size, channel->GetDataSize());
            }
            return size;
        }
    };

    class AudioFlow{
    private:
        IAudioNode *owner_;
        vector<AudioChannelGroup*> inputs_;
        vector<AudioChannelGroup*> outputs_;
        uint16_t num_input_channel_;
        uint16_t num_output_channel_;

        void DownMixing(vector<shared_ptr<AudioData>> &frames){
            if(num_input_channel_ == 2){
                (*frames[1]).Scale(0.5).PlusScale(*frames[0], 0.5);
                frames.erase(frames.begin() + 1);
            }else if(num_input_channel_ == 4){
                if(num_output_channel_ == 1){
                    (*frames[3]).Plus(*frames[0]).Plus(*frames[1]).Plus(*frames[2]).Scale(0.25);
                    frames.erase(frames.begin(), frames.begin() + 3);
                }else{
                    (*frames[2]).Plus(*frames[0]).Scale(0.5);
                    (*frames[3]).Plus(*frames[1]).Scale(0.5);
                    frames.erase(frames.begin(), frames.begin() + 2);
                }
            }else{
                if(num_output_channel_ == 1){
                    (*frames[0]).Plus(*frames[1]).Scale(0.7071);
                    (*frames[2]).Plus(*frames[3]).Scale(0.5);
                    (*frames[5]).Silence().Plus(*frames[4]).Plus(*frames[0]).Plus(*frames[2]);
                    frames.erase(frames.begin(), frames.begin() + 5);
                }else if(num_output_channel_ == 2){
                    (*frames[2]).Plus(*frames[4]).Scale(0.7071);
                    (*frames[3]).Plus(*frames[4]).Scale(0.7071);
                    (*frames[0]).Plus(*frames[2]);
                    (*frames[1]).Plus(*frames[3]);
                    frames.erase(frames.begin() + 2, frames.begin() + 6);
                }else{
                    (*frames[4]).Scale(0.7071);
                    (*frames[0]).Plus(*frames[4]);
                    (*frames[1]).Plus(*frames[4]);
                    frames.erase(frames.begin() + 4, frames.begin() + 6);
                }
            }
        }

        void UpMixing(vector<shared_ptr<AudioData>> &frames){
            if(num_input_channel_ == 1){
                if(num_output_channel_ == 2){
                    shared_ptr<AudioData> r(new AudioData(frames[0]->size_));
                    r->Copy(*frames[0]);
                    frames.emplace_back(r);
                }else if(num_output_channel_ == 4){
                    shared_ptr<AudioData> r(new AudioData(frames[0]->size_)),
                                        sl(new AudioData(frames[0]->size_)),
                                        sr(new AudioData(frames[0]->size_));
                    r->Copy(*frames[0]);
                    frames.emplace_back(r);
                    frames.emplace_back(sl);
                    frames.emplace_back(sr);
                }else{
                    shared_ptr<AudioData>
                            r(new AudioData(frames[0]->size_)),
                            sl(new AudioData(frames[0]->size_)),
                            sr(new AudioData(frames[0]->size_)),
                            c(new AudioData(frames[0]->size_)),
                            lfe(new AudioData(frames[0]->size_));
                    c->Copy(*frames[0]);
                    frames.emplace_back(r);
                    frames.emplace_back(sl);
                    frames.emplace_back(sr);
                    frames.emplace_back(c);
                    frames.emplace_back(lfe);
                }
            }else if(num_input_channel_ == 2){
                if(num_output_channel_ == 4){
                    shared_ptr<AudioData> sl(new AudioData(frames[0]->size_)),
                                        sr(new AudioData(frames[0]->size_));
                    frames.emplace_back(sl);
                    frames.emplace_back(sr);
                }else{
                    shared_ptr<AudioData> sl(new AudioData(frames[0]->size_)),
                            sr(new AudioData(frames[0]->size_)),
                            c(new AudioData(frames[0]->size_)),
                            lfe(new AudioData(frames[0]->size_));
                    frames.emplace_back(sl);
                    frames.emplace_back(sr);
                    frames.emplace_back(c);
                    frames.emplace_back(lfe);
                }
            }else{
                shared_ptr<AudioData>
                        c(new AudioData(frames[0]->size_)),
                        lfe(new AudioData(frames[0]->size_));
                frames.emplace_back(c);
                frames.emplace_back(lfe);
            }
        }

        void ProcessNext(vector<shared_ptr<AudioData>> frames){
            auto ret = num_input_channel_ - num_output_channel_;
            if(ret > 0){
                DownMixing(frames);
            }else if(ret < 0){
                UpMixing(frames);
            }
            for(int i = 0;i < num_output_channel_;i++){
                outputs_[i]->PushFrame(frames[i]);
            }
        }

    public:
        explicit AudioFlow(IAudioNode* owner, uint16_t num_input_channel, uint16_t num_output_channel){
            owner_ = owner;
            UpdateChannels(num_input_channel, num_output_channel);
        }

        void Process(){
            if(!CanBeProcessed())return;
            shared_ptr<AudioData> frame(new AudioData(2048));
            vector<shared_ptr<AudioData>> frames;
            frames.reserve(max(num_input_channel_, num_output_channel_));
            for(auto group : inputs_){
                shared_ptr<AudioData> f(new AudioData(2048));
                group->PullFrame(*frame);
                owner_->Process(frame, f);
                frames.emplace_back(f);
            }
            ProcessNext(frames);
            frames.clear();
        }

        bool CanBeProcessed(){
            auto ret = true;
            for(auto group : inputs_){
                auto size = group->FramesCanBeProcessed();
                if(size <= 0){
                    ret = false;
                    break;
                }
            }
            return ret;
        }

        void UpdateChannels(uint16_t num_input_channel, uint16_t num_output_channel){
            if(num_input_channel_ != num_input_channel){
                num_input_channel_ = num_input_channel;
                if(inputs_.size() != num_input_channel_){
                    inputs_.reserve(num_input_channel_);
                    for(auto i = inputs_.size();i < num_input_channel_;i++){
                        inputs_.emplace_back(new AudioChannelGroup());
                    }
                }
            }
            if(num_output_channel_ != num_output_channel){
                num_output_channel_ = num_output_channel;
                if(outputs_.size() != num_output_channel_){
                    outputs_.reserve(num_output_channel_);
                    for(auto i = outputs_.size();i < num_output_channel_;i++){
                        outputs_.emplace_back(new AudioChannelGroup());
                    }
                }
            }
        }

        void AddInput(uint16_t index, AudioChannel* input_channel){
            inputs_[index]->AddChannel(input_channel);
        }

        void AddOutput(uint16_t index, AudioChannel* output_channel){
            outputs_[index]->AddChannel(output_channel);
        }

        void RemoveInput(uint16_t index, AudioChannel* input_channel){
            inputs_[index]->RemoveChannel(input_channel);
        }

        void RemoveOutput(uint16_t index, AudioChannel* output_channel){
            outputs_[index]->RemoveChannel(output_channel);
        }
    };
}