#pragma once
#include "disposable.h"
#include "buffer.hpp"
using namespace std;
namespace vocaloid{
    class AudioRouter;
    class AudioContext;
    class AudioNode: public IDisposable{
    friend class AudioRouter;
    friend class AudioGraph;
    protected:
        AudioContext *ctx_;

        // Input channels
        uint32_t input_channel_count_;

        // Output channels
        uint32_t output_channel_count_;
    public:
        explicit AudioNode(AudioContext *ctx):ctx_(ctx),input_channel_count_(0), output_channel_count_(0){}

        // Initialize audio node before start to run
        int Prepare(){ return 0; }

        // Process audio data
        template<typename... Args>
        int Process(Args... args){return 0;}

        // Dispose audio node
        void Dispose() override{}
    };
}