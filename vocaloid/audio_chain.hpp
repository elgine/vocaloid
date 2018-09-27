#pragma once
#include "disposable.h"
#include "audio_context_interface.h"
#include "audio_node.hpp"
#include "buffer.hpp"
#include "buffer_queue.hpp"
#include "audio_context_state.h"
namespace vocaloid{

    class AudioChain: public IDisposable{
    private:
        IAudioContext *ctx_;
        vector<AudioNode*> node_list_;
        BufferQueue *input_queue_;
        BufferQueue *output_queue_;
        uint32_t frame_size_processed;
    public:
        explicit AudioChain(IAudioContext *ctx,
                            vector<AudioNode*> nodes,
                            uint16_t in_channels,
                            uint16_t out_channels,
                            uint32_t frame_size){
            ctx_ = ctx;
            node_list_.assign(nodes.begin(), nodes.end());
            input_queue_ = new BufferQueue(in_channels);
            output_queue_ = new BufferQueue(out_channels);
            frame_size_processed = frame_size;
        }

        void Process(Buffer *input, Buffer *output){
            input_queue_->Push(input);
            auto *process_in = new Buffer(frame_size_processed), *process_out = new Buffer(frame_size_processed);
            if(!input_queue_->IsCountAvalidated(frame_size_processed)){
                input_queue_->Pop(process_in);
                for(auto *node : node_list_){
                    node->Process(process_in, process_out);
                    process_in->Copy(process_out);
                }
            }
        }
    };
}