#pragma once
#include <map>
#include <vector>
#include <algorithm>
#include "disposable.h"
#include "audio_source_node.hpp"
#include "audio_destination_node.hpp"
using namespace std;
namespace vocaloid{

    class AudioGraph: public IDisposable{
    private:
        vector<AudioNode*> nodes_;
        vector<AudioSourceNode*> source_nodes_;
        vector<AudioDestinationNode*> dest_nodes_;
        map<AudioNode*, vector<AudioNode*>> connections_;
    public:
        const vector<AudioNode*> GetEdges(AudioNode* n){
            return connections_[n];
        }

        void AddNode(AudioNode* node) {
            auto iter = find(nodes_.begin(), nodes_.end(), node);
            if(iter == nodes_.end()){
                nodes_.push_back(node);
                connections_.insert(pair<AudioNode*, vector<AudioNode*>>(node, {}));
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

        void RemoveNode(AudioNode* node) {
            auto iter = find(nodes_.begin(), nodes_.end(), node);
            if(iter != nodes_.end()){
                nodes_.erase(iter);
                connections_.erase(node);
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

        void Connect(AudioNode *from, AudioNode *to) {
            if(connections_.find(from) == connections_.end()){
                connections_.insert(pair<AudioNode*, vector<AudioNode*>>(from, {}));
            }
            auto iter =  find(connections_[from].begin(), connections_[from].end(), to);
            if(iter == nodes_.end()){
                from->output_channel_count_++;
                to->input_channel_count_++;
                connections_[from].push_back(to);
            }
        }

        void Disconnect(AudioNode *from, AudioNode *to) {
            if(connections_.find(from) != connections_.end()){
                auto iter =  find(connections_[from].begin(), connections_[from].end(), to);
                if(iter != nodes_.end()){
                    from->output_channel_count_--;
                    to->input_channel_count_--;
                    connections_[from].erase(iter);
                }
            }
        }

        void Dispose() override {
            nodes_.clear();
            source_nodes_.clear();
            dest_nodes_.clear();
            connections_.clear();
        }

        vector<AudioNode*> GetNodes(){
            return nodes_;
        }

        vector<AudioSourceNode*> GetSourceNodes(){
            return source_nodes_;
        }

        vector<AudioDestinationNode*> GetDestinationNodes(){
            return dest_nodes_;
        }

        map<AudioNode*, vector<AudioNode*>> GetConnections(){
            return connections_;
        }
    };
}