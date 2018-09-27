#pragma once
#include <stdint.h>
namespace vocaloid{
    class IReader{
    public:
        virtual uint64_t ReadData(char* data, uint64_t length) = 0;
        virtual int16_t Open(const char* output_path) = 0;
        virtual void Close() = 0;
        virtual bool IsEnd() = 0;
        virtual uint64_t Seek(uint64_t pos) = 0;
    };
}