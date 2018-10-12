#pragma once
#include <vector>
#include <math.h>
using namespace std;
namespace vocaloid {

	template<typename T>
	void Conv(const vector<T> a, int a_len, const vector<T> b, int b_len, vector<T> &output) {
		int output_len = a_len + b_len - 1;
		output.reverse(output_len);
		for (int i = 0; i < output_len; i++) {
			output[i] = 0;
			for (int j = 0; j < i; j++) {
				output[i] += a[j] * b[i - j];
			}
		}
	}

	template<typename T>
	void Gain(const vector<T> input, int count, float scalar, vector<T> &output) {
		for (int i = 0; i < count; i++) {
			output[i] = input[i] * scalar;
			if (output[i] < 0)output[i] = 0;
			else if (output[i] > 1)output[i] = 1;
		}
	}

	template<typename T>
	void Normalize1(const vector<T> input, int count, vector<T> &output) {
		float max = -1.0f, min = 1.0f;
		for (int i = 0; i < count; i++) {
			max = max(max, input[i]);
			min = min(min, input[i]);
		}
		float mid = (max + min) * 0.5f,
			gain = (max - min) * 0.5f;
		for (int i = 0; i < count; i++) {
			output[i] = input[i];
			output[i] -= mid;
			output[i] /= gain;
		}
	}

	template<typename T>
	void Normalize2(const vector<T> input, int n, vector<T> &output) {
		float amp = 0;
		for (int i = 0; i < n; i++)
			amp = max(amp, fabsf(input[i]));
		for (int i = 0; i < n; i++)
			output[i] = input[i] / amp;
	}

	template<typename T>
	void Emphasize(const vector<T> input, int n, vector<T> &output) {
		output[0] = input[0];
		for (int i = 1; i < n; i++) {
			output[i] = input[i] - 0.95 * input[i - 1];
		}
	}

	template<typename T>
	void Smooth(const vector<T> input, int k, int n, vector<T> &output) {
		int sum = 0;
		for (int i = 0; i < n; i++) {
			sum += input[i];
			if (i - k >= 0) sum -= input[i - k];
			output[i] = sum / k;
		}
	}

	template<typename T>
	void Mix(const vector<T> inputs, const vector<vector<T>> rs, int mix_count, int n, vector<T> &output) {
		if (mix_count <= 0 || inputs[0] == nullptr)return;
		for (int i = 0; i < n; i++) {
			float temp = 0;
			for (int j = 0; j < mix_count; j++) {
				temp += inputs[j][i] * rs[j];
			}
			output[i] = temp;
		}
	}

	template<typename T>
	void Mix(const vector<T> input1, float r1, const vector<T> input2, float r2, int n, vector<T> &output) {
		for (int i = 0; i < n; i++) {
			output[i] = input1[i] * r1 + input2[i] * r2;
		}
	}

	template<typename T>
	void Echo(const vector<T> input, int k, int n, vector<T> &output) {
		for (int i = 0; i < n; i++) {
			output[i] = input[i];
			if (i - k >= 0)output[i] += input[i - k] / 2.0f;
		}
		normalize2(output, n, output);
	}

	template<typename T>
	void Superimposed(const vector<T> *inputs, int plus_count, int n, vector<T> &output) {
		for (int j = 0; j < n; j++) {
			float temp = 1.0f;
			for (int i = 0; i < plus_count; i++) {
				temp *= inputs[i][j];
			}
			output[j] = temp;
		}
	}

	template<typename T>
	void Superimposed(const vector<T> input1, const vector<T> input2, int n, vector<T> &output) {
		for (int i = 0; i < n; i++) {
			output[i] = input1[i] * input2[i];
		}
	}
}