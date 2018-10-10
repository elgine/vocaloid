#pragma once
#include "disposable.h"
#include "audio_node.hpp"
namespace vocaloid{

    class AudioNodeLink: public IDisposable{
    private:
        AudioNode *src_;
        AudioNode *dst_;
    public:
        void Dispose() override {

        }
    };
}