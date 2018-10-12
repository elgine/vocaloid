#pragma once
#include <stdint.h>
#include <memory>
#include "vocaloid/common/buffer.hpp"
namespace vocaloid{
    class ProcessUnit{
        virtual uint64_t Process(Buffer<float> *in, Buffer<float> *out) = 0;
        virtual uint64_t Process(std::shared_ptr<Buffer<float>> in, std::shared_ptr<Buffer<float>> out) = 0;
    };
}