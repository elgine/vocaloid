#pragma once
namespace vocaloid{
    class AudioGraph;
    class AudioNode{
    friend class AudioGraph;
    protected:
        AudioGraph *graph_;
        uint16_t num_input_nodes_;
        uint16_t num_output_nodes_;
    public:

        explicit AudioNode(AudioGraph *graph):graph_(graph),num_input_nodes_(0),num_output_nodes_(0){

        }

        template<typename... Args>
        virtual uint64_t Process(Args... args) = 0;

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