#pragma once
#include "audio_node.hpp"
#include "audio_buffer.hpp"
namespace vocaloid{

    class AudioDestinationNode: public AudioNode{
    protected:
        uint32_t sample_rate_ = 44100;
    public:
        explicit AudioDestinationNode(AudioContext *ctx):AudioNode(ctx){
        }

        // Override and make those function final, destination
        // shouldn't have output nodes
        void Connect(const AudioNodeRef &n) final {}
        void Disconnect(const AudioNodeRef &n) final {}
        AudioNodeRef operator>>(const AudioNodeRef &n) final {return n;}

        virtual void Initialize(){}

        virtual void Close(){}

        /**
         * Push buffer to destination
         * @param in
         */
        virtual void Push(AudioBuffer *in) = 0;

        virtual void SetSampleRate(uint32_t sample_rate){
            sample_rate_ = sample_rate;
        }

        uint32_t SampleRate(){
            return sample_rate_;
        }
    };
}