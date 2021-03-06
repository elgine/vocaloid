#pragma once
#include <stdint.h>
#include <vector>
#include <memory>
namespace vocaloid{
    class Processor{
        virtual uint64_t Process(std::vector<float> in, uint64_t len, std::vector<float> &out) = 0;
    };
}