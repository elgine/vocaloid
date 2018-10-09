#pragma once
#include <stdint.h>
#include "disposable.h"
#include "audio_context_state.h"
#include "ticker.hpp"
namespace vocaloid{
    class AudioNode;
    class IAudioContext: public IDisposable{
    friend class AudioNode;
    protected:
    virtual void AddNode(AudioNode* node) = 0;
    virtual void RemoveNode(AudioNode* node) = 0;
    virtual void Connect(AudioNode* from, AudioNode* to) = 0;
    virtual void Disconnect(AudioNode* from, AudioNode *to) = 0;
    public:
        virtual void Setup() = 0;
        virtual void Run() = 0;
        virtual void Stop() = 0;
        virtual Ticker* GetTicker() = 0;
        virtual AudioContextState GetState() = 0;
        virtual uint32_t GetSampleRate() = 0;
    };
}