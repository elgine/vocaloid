#pragma once
#include <math.h>
#include <vector>
using namespace std;


namespace vocaloid {

	enum WINDOW_TYPE {
		HANNING,
		HAMMING,
		RECTANGLE,
		BARTLETT,
		BLACKMAN,
		KAISER
	};

	/**
	* Calculates rectangle window coefficients.
	*/
	template <typename T>
	void Rectangle(int N, T amp, vector<T> &win) {
		for (int i = 0; i<(N + 1) / 2; ++i) {
			win[i] = amp;
			win[N - 1 - i] = win[i];
		}
	}

	/**
	* Calculates bartlett window coefficients.
	*/
	template <typename T>
	void Bartlett(int N, T amp, vector<T> &win) {
		for (int i = 0; i<(N + 1) / 2; ++i) {
			win[i] = amp * 2 * i / (N - 1);
			win[N - 1 - i] = win[i];
		}
	}

	/**
	* Calculates hanning window coefficients.
	*/
	template <typename T>
	void Hanning(int N, T amp, vector<T> &win) {
		for (int i = 0; i<(N + 1) / 2; ++i) {
			win[i] = amp * T(0.5 - 0.5*cos(M_PI * 2 * i / (N - 1)));
			win[N - 1 - i] = win[i];
		}
	}

	/**
	* Calculates hamming window coefficients.
	*/
	template <typename T>
	void Hamming(int N, T amp, vector<T> &win) {
		for (int i = 0; i<(N + 1) / 2; ++i) {
			win[i] = amp * T(0.54 - 0.46*cos(M_PI * 2 * i / (N - 1.0)));
			win[N - 1 - i] = win[i];
		}
	}

	/**
	* Calculates hamming window coefficients.
	*/
	template <typename T>
	void Blackman(int N, T amp, vector<T> &win) {
		for (int i = 0; i<(N + 1) / 2; ++i) {
			win[i] = amp * T(0.42 - 0.50*cos(M_PI * 2 * i / (N - 1.0))
				+ 0.08*cos(2 * M_PI * 2 * i / (N - 1.0)));
			win[N - 1 - i] = win[i];
		}
	}

	double BesselI0(double x) {
		double denominator;
		double numerator;
		double z;
		if (x == 0.0) {
			return 1.0;
		}
		else {
			z = x * x;
			numerator = (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z*
				(z* 0.210580722890567e-22 + 0.380715242345326e-19) +
				0.479440257548300e-16) + 0.435125971262668e-13) +
				0.300931127112960e-10) + 0.160224679395361e-7) +
				0.654858370096785e-5) + 0.202591084143397e-2) +
				0.463076284721000e0) + 0.754337328948189e2) +
				0.830792541809429e4) + 0.571661130563785e6) +
				0.216415572361227e8) + 0.356644482244025e9) +
				0.144048298227235e10);
			denominator = (z*(z*(z - 0.307646912682801e4) +
				0.347626332405882e7) - 0.144048298227235e10);
		}
		return -numerator / denominator;
	}

	/**
	* Calculates kasier window coefficients.
	*/
	template <typename T>
	void Kaiser(int N, float beta, vector<T> &win) {
		win.reserve(N + 1);
		for (int i = 0; i <= N; i++) {
			win[i] = BesselI0(beta * sqrt(1.0 - pow((i - N * 0.5) / (N * 0.5), 2.0))) / BesselI0(beta);
		}
	}


	template<typename T>
	void GenerateWin(WINDOW_TYPE type, int length, vector<T> &output, float extra = 1.0) {
		switch (type) {
		case WINDOW_TYPE::HANNING:
			Hanning(length, extra, output);
			break;
		case WINDOW_TYPE::BARTLETT:
			Bartlett(length, extra, output);
			break;
		case WINDOW_TYPE::BLACKMAN:
			Blackman(length, extra, output);
			break;
		case WINDOW_TYPE::KAISER:
			Kaiser(length, extra, output);
			break;
		default:break;
		}
	}
}