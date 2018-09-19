#pragma once
#include <vector>
#include "interpolator.hpp"
using namespace std;

namespace vocaloid {

	// Simple interpolator, can customize interpolator
	template<typename T>
	class Resampler {

	private:

		// Resample ratio
		float ratio_;

		INTERPOLATOR_TYPE interpolator;

	public:

		Resampler(float ratio, INTERPOLATOR_TYPE interpolateType = INTERPOLATOR_TYPE::LINEAR) {
			ratio_ = ratio;
			interpolator = interpolateType;
		}

		long Resample(vector<T> input, long input_len, vector<T>& output) {
			long output_len = ratio_ * input_len;
			if (ratio_ == 1)output.assign(input.begin(), input.end());
			else {
				if (output.size() != output_len)output.resize(output_len);
				if (ratio_ < 1) {
					for (int i = 0; i < output_len; i++) {
						int index = round(i / ratio_);
						output[i] = input[index >= input_len?input_len - 1:index];
					}
				}
				else {
					Interpolate(interpolator, input, input_len, output, output_len);
				}
			}
			return output_len;
		}
	};
}