#pragma once
#include <vector>
#include <stdint.h>
#include "interpolate.hpp"
using namespace std;
template<typename T>
uint64_t Resample(vector<T> input,  uint64_t input_len, INTERPOLATOR_TYPE interpolator, float ratio, vector<T>& output) {
	auto output_len = uint64_t(input_len * ratio);
	if (ratio == 1.0f)output.assign(input.begin(), input.begin() + input_len);
	else {
		if (output.size() != output_len)
			output.resize(output_len);
		if (ratio < 1) {
			for (int i = 0; i < output_len; i++) {
				auto index = round(i / ratio);
				output[i] = input[index >= input_len?input_len - 1:index];
			}
		}
		else {
			Interpolate(interpolator, input, input_len, output, output_len);
		}
	}
	return output_len;
}