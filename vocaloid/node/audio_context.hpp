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
        shared_ptr<AudioDestinationNode> dest_node_;

        atomic<uint64_t> current_time_;
        uint64_t offset_start_;

        void Run(){
            current_time_ = ticker_->GetCurTimestamp() + offset_start_;
        }

    public:
        static const uint16_t BITS_PER_SEC = 16;

        explicit AudioContext(){
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
            dest_node_ = CreateNode(new AudioRecorderNode(this));
            dest_node_->SetSampleRate(sample_rate);
            dest_node_->SetChannels(channels);
        }

        /**
         * Playback audio buffer processed from audio graph
         * @param sample_rate
         * @param bits
         * @param channels
         */
        void SetPlayerMode(uint32_t sample_rate, uint16_t channels){
            dest_node_ = CreateNode(new AudioPlayerNode(this));
            dest_node_->SetSampleRate(sample_rate);
            dest_node_->SetChannels(channels);
        }

        /**
         * Wrap audio node pointer
         * @tparam T
         * @param n
         * @return
         */
        template<class T>
        shared_ptr<T> CreateNode(T *n){
            return make_shared<T>(n);
        }

        /**
         * Setup audio graph, initialize audio thread
         */
        void Setup(){
            dest_node_->Initialize();

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

        shared_ptr<AudioDestinationNode> GetDestination(){
            return dest_node_;
        }

        shared_ptr<mutex> GetMutex(){
            return mutex_;
        }

        Ticker* GetTicker(){
            return ticker_;
        }
    };
}