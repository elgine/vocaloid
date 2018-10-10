#pragma once
#include <stdint.h>
#include <vector>
#include <map>
#include "ticker.hpp"
#include "audio_context_state.h"
#include "emitter.hpp"
#include "audio_graph.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext: public Emitter{
    protected:
        Ticker *ticker_;
        AudioContextState state_;
        AudioGraph *graph_;
    public:

        int max_thread_count_ = 4;

        explicit AudioContext() = default{
            state_ = AudioContextState::FREE;
            ticker_ = new Ticker();
            graph_ = new AudioGraph();
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