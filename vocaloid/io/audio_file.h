#pragma once
#include <stdint.h>
#include "vocaloid/data/format.hpp"
namespace vocaloid{
    class AudioFileReader{
    public:
        virtual uint64_t ReadData(char* data, uint64_t length) = 0;
        virtual int16_t Open(const char* output_path) = 0;
        virtual void Close() = 0;
        virtual bool IsEnd() = 0;
        virtual uint64_t Seek(uint64_t pos) = 0;
        virtual AudioFormat Format() = 0;
    };

    class AudioFileWriter{
    public:
        virtual int16_t Open(const char* output_path, uint32_t sample_rate, uint16_t bits, uint16_t channels) = 0;
        virtual uint64_t WriteData(const char* bytes, uint64_t byte_length) = 0;
        virtual void Close() = 0;
    };
}