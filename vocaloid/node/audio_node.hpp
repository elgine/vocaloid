#pragma once
#include <stdint.h>
#include "connect_unit.hpp"
using namespace std;
namespace vocaloid{
    class AudioContext;
    class AudioNode: public ConnectUnit{
    protected:
        AudioContext *context_;
    public:
        static const uint16_t BITS_PER_SEC = 16;
        explicit AudioNode(AudioContext *ctx):ConnectUnit(), context_(ctx){}
    };
}