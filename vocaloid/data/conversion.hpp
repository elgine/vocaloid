#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>
using namespace std;
namespace vocaloid{

    void ByteArrayToFloatVector(const char* byte_array, uint64_t byte_length, uint16_t bits, uint16_t channels, vector<float> *buffers){
        uint16_t depth = bits / 8;
        uint16_t step = depth * channels;
        float max = powf(2.0f, bits - 1);
        for(int i = 0;i < byte_length;i += step){
            for(int j = 0;j < channels;j++){
                long offset = i + j * depth;
                long value = byte_array[offset] & 0xFF;
                for(int k = 1;k < depth;k++){
                    value |= (long)(byte_array[offset + k] << (k * 8));
                }
                buffers[j][i / step] = value/max;
            }
        }
    }

    void FloatVectorToByteArray(vector<float> *buffers, uint64_t buffer_size,
                                uint16_t bits, uint16_t channels, char *byte_array, uint64_t &byte_length){
        uint16_t depth = bits / 8;
        uint16_t step = depth * channels;
        byte_length = step * buffer_size;
        float max = powf(2.0f, bits - 1);
        for(int i = 0;i < buffer_size;i++) {
            for (int j = 0; j < channels; j++) {
                float clipped = buffers[j][i];
                if(clipped > 0.999f)clipped = 0.999f;
                else if(clipped < -0.999f)clipped = -0.999f;
                auto value = (long)(clipped * max);
                for (int k = 0; k < depth; k++) {
                    byte_array[i * step + j * depth + k] = (char)((value >> 8 * k) & 0xFF);
                }
            }
        }
    }
}