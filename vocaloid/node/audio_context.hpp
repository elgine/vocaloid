#pragma once
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <memory>
#include <mutex>
#include <atomic>
#include "audio_destination_node.hpp"
#include "audio_player_node.hpp"
#include "audio_recorder_node.hpp"
#include "vocaloid/utils/ticker.hpp"
using namespace std;
namespace vocaloid{

    enum AudioContextState{
        STOPPED,
        PLAYING
    };

    class AudioContext{
    private:
        Ticker *ticker_;
        atomic<AudioContextState> state_;
        unique_ptr<thread> audio_thread_;
        shared_ptr<mutex> mutex_;
        AudioDestinationNode *dest_node_;

        atomic<uint64_t> current_time_;
        uint64_t offset_start_;

        atomic<uint64_t> frame_size_;

        void Run(){
            while(state_ == AudioContextState::PLAYING){
                current_time_ = ticker_->GetCurTimestamp() + offset_start_;
                dest_node_->Pull();
            }
        }

    public:
        explicit AudioContext(){
            frame_size_ = 1024;
            current_time_ = offset_start_ = 0;
            state_ = AudioContextState::STOPPED;
            ticker_ = new Ticker();
        }

        /**
         * Record audio buffer processed from audio graph
         * @param path
         * @param sample_rate
         * @param channels
         */
        void SetRecorderMode(const char* path, uint32_t sample_rate, uint16_t channels){
            auto recorder = new AudioRecorderNode(this);
            recorder->SetPath(path);
            recorder->SetSampleRate(sample_rate);
            recorder->SetChannels(channels);
            dest_node_ = recorder;
        }

        /**
         * Playback audio buffer processed from audio graph
         * @param sample_rate
         * @param bits
         * @param channels
         */
        void SetPlayerMode(uint32_t sample_rate, uint16_t channels){
            dest_node_ = new AudioPlayerNode(this);
            dest_node_->SetSampleRate(sample_rate);
            dest_node_->SetChannels(channels);
        }

        /**
         * Setup audio graph, initialize audio thread
         */
        void Setup(){
            SetupRecursively(dest_node_);
        }

        void SetupRecursively(ConnectUnit *n){
            n->Initialize(frame_size_);
            for(auto input : n->Inputs()){
                SetupRecursively(input);
            }
        }

        /**
         * Start processing
         * @param offset start-time
         */
        void Start(uint64_t offset = 0){
            offset_start_ = offset;
            state_ = AudioContextState::PLAYING;
            ticker_->Start();
            audio_thread_ = make_unique<thread>(thread(&AudioContext::Run, this));
        }

        /**
         * Stop processing
         */
        void Stop(){
            state_ = AudioContextState::STOPPED;
            if(audio_thread_->joinable())
                audio_thread_->join();
            dest_node_->Close();
        }

        /**
         * Dispose all
         */
        void Dispose(){
            Stop();
            current_time_ = offset_start_ = 0;
        }

        AudioDestinationNode* GetDestination(){
            return dest_node_;
        }

        uint32_t GetSampleRate(){
            return dest_node_->SampleRate();
        }

        shared_ptr<mutex> GetMutex(){
            return mutex_;
        }

        Ticker* GetTicker(){
            return ticker_;
        }
    };
}