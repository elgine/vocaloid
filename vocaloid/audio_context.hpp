#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include "audio_context_interface.h"
#include "emitter.hpp"
#include "audio_node.hpp"
#include "audio_source_node.hpp"
#include "audio_destination_node.hpp"
#include "thread_pool.hpp"
#include "ticker.hpp"
using namespace std;
namespace vocaloid{

    class AudioContext: public Emitter, public IAudioContext{
    protected:
        ThreadPool *thread_pool_;
        Ticker *ticker_;
        AudioContextState state_;
        uint32_t sample_rate_;
        vector<AudioNode*> nodes_;
        vector<AudioSourceNode*> source_nodes_;
        vector<AudioDestinationNode*> dest_nodes_;
        map<AudioNode*, vector<AudioNode*>> node_chains_;

        void AddNode(AudioNode* node) override {
            auto iter = find(nodes_.begin(), nodes_.end(), node);
            if(iter == nodes_.end()){
                nodes_.push_back(node);
                node_chains_.insert(pair<AudioNode*, vector<AudioNode*>>(node, {}));
                if(typeid(node) == typeid(AudioSourceNode)){
                    auto iter1 = find(source_nodes_.begin(), source_nodes_.end(), node);
                    if(iter1 == source_nodes_.end()){
                        source_nodes_.push_back((AudioSourceNode*)node);
                    }
                }else if(typeid(node) == typeid(AudioDestinationNode)){
                    auto iter1 = find(dest_nodes_.begin(), dest_nodes_.end(), node);
                    if(iter1 == dest_nodes_.end()){
                        dest_nodes_.push_back((AudioDestinationNode*)node);
                    }
                }
            }
        }

        void RemoveNode(AudioNode* node) override {
            auto iter = find(nodes_.begin(), nodes_.end(), node);
            if(iter != nodes_.end()){
                nodes_.erase(iter);
                node_chains_.erase(node);
                if(typeid(node) == typeid(AudioSourceNode)){
                    auto iter1 = find(source_nodes_.begin(), source_nodes_.end(), node);
                    if(iter1 != source_nodes_.end()){
                        source_nodes_.erase(iter1);
                    }
                }else if(typeid(node) == typeid(AudioDestinationNode)){
                    auto iter1 = find(dest_nodes_.begin(), dest_nodes_.end(), node);
                    if(iter1 != dest_nodes_.end()){
                        dest_nodes_.erase(iter1);
                    }
                }
            }
        }

        void Connect(AudioNode *from, AudioNode *to) override {
            if(node_chains_.find(from) == node_chains_.end()){
                node_chains_.insert(pair<AudioNode*, vector<AudioNode*>>(from, {}));
            }
            auto iter =  find(node_chains_[from].begin(), node_chains_[from].end(), to);
            if(iter == nodes_.end()){
                node_chains_[from].push_back(to);
            }
        }

        void Disconnect(AudioNode *from, AudioNode *to) override {
            if(node_chains_.find(from) != node_chains_.end()){
                auto iter =  find(node_chains_[from].begin(), node_chains_[from].end(), to);
                if(iter != nodes_.end()){
                    node_chains_[from].erase(iter);
                }
            }
        }

    public:

        uint32_t max_thread_count_;

        explicit AudioContext(uint32_t sample_rate = 44100):sample_rate_(sample_rate){
            state_ = AudioContextState::FREE;
            thread_pool_ = nullptr;
            ticker_ = new Ticker();
            max_thread_count_ = 5;
        }

        void Setup() override {
            // Initialize thread pool
            thread_pool_ = new ThreadPool(max_thread_count_);
            thread_pool_->Run();
            // Initialize all source nodes
            for(auto source_node : source_nodes_){
                source_node->Prepare();
            }
            // TODO: Generate workers...
            state_ = AudioContextState::PREPARE;
        }

        void Run() override {
            state_ = AudioContextState::RUN;
        }

        void Stop() override {
            state_ = AudioContextState::STOP;
            thread_pool_->Stop();
        }

        Ticker* GetTicker() override {
            return ticker_;
        }

        AudioContextState GetState() override {
            return state_;
        }

        uint32_t GetSampleRate() override {
            return sample_rate_;
        }

        void Dispose() override{
            Emitter::Dispose();
        }
    };
}