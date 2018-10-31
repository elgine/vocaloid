#pragma once
#include <vector>
#include "vocaloid/maths/fft.hpp"
using namespace std;
// Generate a waveform between [0, 2pi]
namespace vocaloid {

	enum WAVEFORM_TYPE {
		SINE,
		TRIANGLE,
		SAWTOOTH,
		SQUARE
	};

	template<typename T>
	void GenSine(vector<T> &wave, uint64_t n){
		for(auto i = 0;i < n;i++){
			wave[i] = sin(2 * M_PI * i / n);
		}
	}

	template<typename T>
	void GenSquare(vector<T> &wave, uint64_t n){
		auto half = n / 2;
		for(auto i = 0;i < half;i++)wave[i] = 1;
		for(auto i = half;i < n;i++)wave[i] = -1;
	}

	template<typename T>
	void GenTriangle(vector<T> &wave, uint64_t n){
		auto half = n / 2;
		auto step = half / 2;
		for(auto i = 0;i < step;i++){
			wave[i] = float(i) / step;
		}
		for(auto i = step;i < step + half;i++){
			wave[i] = 2 - float(i) / step;
		}
		for(auto i = step + half;i < n;i++){
			wave[i] = float(i - step - half)/step - 1;
		}
	}

	template<typename T>
	void GenSawTooth(vector<T> &wave, uint64_t n){
		auto half = n / 2;
		for(auto i = 0;i < half;i++){
			wave[i] = float(i) / half;
		}
		for(auto i = half;i < n;i++){
			wave[i] = float(i - half)/half - 1;
		}
	}

	template<typename T>
	void GenWaveform(WAVEFORM_TYPE type, uint64_t n, vector<T> &wave){
		switch(type){
			case WAVEFORM_TYPE::SAWTOOTH:
				GenSawTooth(wave, n);
				break;
			case WAVEFORM_TYPE::TRIANGLE:
				GenTriangle(wave, n);
				break;
			case WAVEFORM_TYPE::SQUARE:
				GenSquare(wave, n);
				break;
			default:
				GenSine(wave, n);
				break;
		}
	}

//	template<typename T>
//	void GenSineCoefficients(vector<T> &real, vector<T> &imag, uint64_t len) {
//		real[0] = 0;
//		imag[0] = 0;
//		for (int i = 1; i < len; i++) {
//			real[i] = 0;
//			if (i == 1) {
//				imag[i] = 1;
//			}
//			else {
//				imag[i] = 0;
//			}
//		}
//	}
//
//	//http://mathworld.wolfram.com/FourierSeriesTriangleWave.html
//	template<typename T>
//	void GenTriangleCoefficients(vector<T> &real, vector<T> &imag, uint64_t len) {
//		real[0] = 0;
//		imag[0] = 0;
//		for (int i = 1; i < len; i++) {
//			real[i] = 0;
//			if (i & 1) {
//				imag[i] = 8 / powf(i * (float)M_PI, 2.0f) * ((((i - 1) >> 1) & 1) ? -1 : 1);
//			}else {
//				imag[i] = 0;
//			}
//		}
//	}
//
//	template<typename T>
//	void GenSawtoothCofficients(vector<T> &real, vector<T> &imag, uint64_t len) {
//		real[0] = 0;
//		imag[0] = 0;
//		for (int i = 1; i < len; i++) {
//			real[i] = 0;
//			imag[i] = powf(-1, i + 1) * 2.0f / (i * (float)M_PI);
//		}
//	}
//
//	// Generate waveform through predefined waveform-cofficient model.
//	void GenWaveform(const vector<float> &real, const vector<float> &imag, uint64_t len, vector<float> &output) {
//		FFT *frame = new FFT();
//		frame->Initialize(len);
//		frame->Inverse(real, imag, len, output);
//		frame->Dispose();
//	}
//
//	// Generate custom waveform through data provided by user.
//	void GenWaveform(WAVEFORM_TYPE wave_type, uint64_t len, vector<float> &output) {
//		if ((int)wave_type < (int)WAVEFORM_TYPE::SINE || (int)wave_type >(int)WAVEFORM_TYPE::CUSTOM) {
//			throw "Should provide wave_type or real, imag array to generate waveform";
//		}
//		vector<float> real = vector<float>(len), imag = vector<float>(len);
//		switch (wave_type) {
//		case WAVEFORM_TYPE::SINE:GenSineCoefficients(real, imag, len); break;
//		case WAVEFORM_TYPE::TRIANGLE:GenTriangleCoefficients(real, imag, len); break;
//		case WAVEFORM_TYPE::SAWTOOTH:GenSawtoothCofficients(real, imag, len); break;
//		default:GenSineCoefficients(real, imag, len); break;
//		}
//		GenWaveform(real, imag, len, output);
//	}
}