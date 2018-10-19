#pragma once
#include <stdint.h>
namespace vocaloid{
    class AudioNode;
    class AudioNodeRouter{
    private:
        AudioNode *node_;
        uint16_t channels_;
        vector<AudioNode*> connections_;
    public:
        explicit AudioNodeRouter(AudioNode *n):node_(n){}

        void Connect(){

        }

        void Disconnect(){

        }
    };
}