#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include "ticker.hpp"
#include "audio_context_state.h"
#include "emitter.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext: public Emitter{
    protected:
        Ticker *ticker_;
        AudioContextState state_;
        // All nodes' sample-rate is the same
        // as it's audio-context
        uint32_t sample_rate_;
    public:

        int max_thread_count_ = 4;

        explicit AudioContext(uint32_t sample_rate):sample_rate_(sample_rate){
            state_ = AudioContextState::FREE;
            ticker_ = new Ticker();
        }

        void Setup() {
            state_ = AudioContextState::PREPARE;
        }

        void Run() {
            state_ = AudioContextState::RUN;
        }

        void Stop() {
            state_ = AudioContextState::STOP;
        }

        uint32_t GetSampleRate(){
            return sample_rate_;
        }

        AudioGraph* GetGraph() {
            return graph_;
        }

        Ticker* GetTicker() {
            return ticker_;
        }

        AudioContextState GetState() {
            return state_;
        }

        void Dispose() override{
            Emitter::Dispose();
        }
    };
}