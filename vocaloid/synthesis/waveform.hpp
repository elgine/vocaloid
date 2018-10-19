#pragma once
#include <vector>
#include "vocaloid/maths/fft.hpp"
using namespace std;

// Generate a waveform between [-pi, pi] according
// to Fourier series
namespace vocaloid {

	enum WAVEFORM_TYPE {
		SINE,
		TRIANGLE,
		SAWTOOTH,
		CUSTOM
	};

	template<typename T>
	void GenSineCoefficients(vector<T> &real, vector<T> &imag, int len) {
		real[0] = 0;
		imag[0] = 0;
		for (int i = 1; i < len; i++) {
			real[i] = 0;
			if (i == 1) {
				imag[i] = 1;
			}
			else {
				imag[i] = 0;
			}
		}
	}

	//http://mathworld.wolfram.com/FourierSeriesTriangleWave.html
	template<typename T>
	void GenTriangleCoefficients(vector<T> &real, vector<T> &imag, int len) {
		real[0] = 0;
		imag[0] = 0;
		for (int i = 1; i < len; i++) {
			real[i] = 0;
			if (i & 1) {
				imag[i] = 8 / powf(i * M_PI, 2.0f) * ((((i - 1) >> 1) & 1) ? -1 : 1);
			}else {
				imag[i] = 0;
			}
		}
	}

	template<typename T>
	void GenSawtoothCofficients(vector<T> &real, vector<T> &imag, int len) {
		real[0] = 0;
		imag[0] = 0;
		for (int i = 1; i < len; i++) {
			real[i] = 0;
			imag[i] = powf(-1, i + 1) * 2.0f / (i * (float)M_PI);
		}
	}

	// Generate waveform through predefined waveform-cofficient model.
	void GenWaveform(vector<float> real, vector<float> imag, uint64_t len, vector<float> &output) {
		FFT *frame = new FFT();
		frame->Initialize(len);
		frame->Inverse(real, imag, len, output);
		frame->Dispose();
	}

	// Generate custom waveform through data provided by user.
	void GenWaveform(WAVEFORM_TYPE wave_type, uint64_t len, vector<float> &output) {
		if ((int)wave_type < (int)WAVEFORM_TYPE::SINE || (int)wave_type >(int)WAVEFORM_TYPE::CUSTOM) {
			throw "Should provide wave_type or real, imag array to generate waveform";
		}
		vector<float> real = vector<float>(len), imag = vector<float>(len);
		switch (wave_type) {
		case WAVEFORM_TYPE::SINE:GenSineCoefficients(real, imag, len); break;
		case WAVEFORM_TYPE::TRIANGLE:GenTriangleCoefficients(real, imag, len); break;
		case WAVEFORM_TYPE::SAWTOOTH:GenSawtoothCofficients(real, imag, len); break;
		default:GenSineCoefficients(real, imag, len); break;
		}
		GenWaveform(real, imag, len, output);
	}
}