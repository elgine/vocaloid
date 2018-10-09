#pragma once
#include <string>
#include <map>
#include <memory>
#include "disposable.h"
#include "audio_context_interface.h"
#include "audio_node.hpp"
#include "buffer.hpp"
#include "buffer_queue.hpp"
#include "audio_context_state.h"
#include "mutex"
namespace vocaloid{

    class AudioProcessChain: public IDisposable{
    private:
        IAudioContext *ctx_;
        vector<AudioNode*> node_list_;
        mutex buffer_mutex_;
        map<string, BufferQueue*> buffer_group_;
        BufferQueue *input_queue_;
        BufferQueue *output_queue_;
        uint32_t frame_size_;

        void MixBufferGroupIfNeed(Buffer *buf){
            unique_lock<mutex> lock(buffer_mutex_);
            int group_size = buffer_group_.size();
            if(group_size <= 0)return;
            for(auto item : buffer_group_){
                if(!item.second->IsCountAvalidated(frame_size_))
                    return;
            }
            int i = 0;
            unique_ptr<Buffer*> temp = make_unique(new Buffer(buf->GetChannels()));
            for(auto item : buffer_group_){
                item.second->Pop(temp, frame_size_);
                if(i == 0){
                    buf->Copy(temp);
                }else{
                    for(int j = 0;j < buf->GetChannels();j++){
                        for(int k = 0;k < buf->GetBufferSize();k++){
                            vector<float> d = buf->GetChannelAt(j);
                            d[k] += temp->GetChannelAt(j)[k];
                        }
                    }
                }
                i++;
            }
        }

    public:
        explicit AudioProcessChain(IAudioContext *ctx,
                            vector<AudioNode*> nodes,
                            uint16_t in_channels,
                            uint16_t out_channels,
                            uint32_t frame_size){
            ctx_ = ctx;
            node_list_.assign(nodes.begin(), nodes.end());
            input_queue_ = new BufferQueue(in_channels);
            output_queue_ = new BufferQueue(out_channels);
            frame_size_ = frame_size;
        }

        void PushBuffer(string id, Buffer *input){
            unique_lock<mutex> lock(buffer_mutex_);
            BufferQueue *b = nullptr;
            if(buffer_group_.find(id) != buffer_group_.end()){
                b = buffer_group_[id];
            }else{
                b = new BufferQueue(input->GetChannels());
                buffer_group_[id] = b;
            }
            b->Push(input);
        }

        int16_t Process(Buffer *output){
            unique_lock<mutex> lock(buffer_mutex_);
            auto *process_in = new Buffer(frame_size_), *process_out = new Buffer(frame_size_);
            MixBufferGroupIfNeed(process_in);
            if(!input_queue_->IsCountAvalidated(frame_size_)){
                input_queue_->Pop(process_in);
                for(auto *node : node_list_){
                    node->Process(process_in, process_out);
                    process_in->Copy(process_out);
                }
            }
            return 0;
        }
    };
}