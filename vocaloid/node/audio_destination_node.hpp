#pragma once
#include "audio_node.hpp"
#include "audio_buffer.hpp"
namespace vocaloid{

    class AudioDestinationNode: public AudioNode{
    protected:
        uint32_t sample_rate_ = 44100;
    public:
        explicit AudioDestinationNode(AudioContext *ctx):AudioNode(ctx){
            can_connect_ = false;
        }

        virtual int64_t PushToDestination(){
            return 0;
        }

        int64_t Pull(){
            PullInputs();
            return PushToDestination();
        }

        virtual void Close(){}

        virtual void SetSampleRate(uint32_t sample_rate){
            sample_rate_ = sample_rate;
        }

        uint32_t SampleRate(){
            return sample_rate_;
        }
    };
}