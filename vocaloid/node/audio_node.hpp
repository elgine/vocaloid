#pragma once
#include <stdint.h>
namespace vocaloid{
    class AudioContext;
    class AudioNode{
    friend class AudioGraph;
    protected:
        AudioContext *context_;
        uint16_t num_input_nodes_;
        uint16_t num_output_nodes_;
    public:

        explicit AudioNode(AudioContext *ctx):context_(ctx),num_input_nodes_(0),num_output_nodes_(0){

        }

        /**
         * Flush memory buffer left
         */
        virtual void Flush(){}

        uint16_t NumInputNodes(){
            return num_input_nodes_;
        }

        uint16_t NumOutputNodes(){
            return num_output_nodes_;
        }
    };
}