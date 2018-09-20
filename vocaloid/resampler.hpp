#pragma once
#include <vector>
#include <stdint.h>
#include "interpolator.hpp"
using namespace std;

namespace vocaloid {

	template<typename T>
	uint64_t Resample(vector<T> input,  uint64_t input_len, INTERPOLATOR_TYPE interpolator, float ratio,vector<T>& output) {
		uint64_t output_len = ratio * input_len;
		if (ratio == 1)output.assign(input.begin(), input.end());
		else {
			if (output.size() != output_len)
				output.resize(output_len);
			if (ratio < 1) {
				for (int i = 0; i < output_len; i++) {
					uint64_t index = round(i / ratio);
					output[i] = input[index >= input_len?input_len - 1:index];
				}
			}
			else {
				Interpolate(interpolator, input, input_len, output, output_len);
			}
		}
		return output_len;
	}
}