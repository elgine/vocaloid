#pragma once
#include <stdint.h>
#include <math.h>
#include "synthesizer.h"
namespace vocaloid {

	//            b0 + b1'*z^-1 + b2'*z^-2
	//    H(z) = -------------------------- ,
	//            a0 + a1'*z^-1 + a2'*z^-2

	//            b0     (b1'/a0)*z^-1 + (b2'/a0)*z^-2
	//    H(z) = ---- * -------------------------------
	//            a0     (a1'/b0)*z^-1 + (a2'/b0)*z^-2

	//                a1*z^-1 + a2*z^-2
	//    H(z) = A * -------------------
	//                b1*z^-1 + b2*z^-2
	//
	//    where A = gain = b1'/a0
	//
	//    a1 = a1'/b0,  a2 = a2'/b0,  b1 = b1'/a0,  b2 = b2'/a0
	//
	enum BIQUAD_TYPE {
		LOW_PASS,
		HIGH_PASS,
		BAND_PASS,
		LOW_SHELF,
		HIGH_SHELF,
		PEAKING,
		ALL_PASS,
		NOTCH
	};

	class Biquad: public Synthesizer {
	private:
		BIQUAD_TYPE type_;
		float freq_;
		float b0_;
		float b1_;
		float b2_;
		float a1_;
		float a2_;
		float Q_;
		float gain_;
		float x1_;
		float y1_;
		float x2_;
		float y2_;

		void SetNormalizeCoefficients(float b0, float b1, float b2, float a0, float a1, float a2) {
			float a_i = 1 / a0;
			b0_ = b0 * a_i;
			b1_ = b1 * a_i;
			b2_ = b2 * a_i;
			a1_ = a1 * a_i;
			a2_ = a2 * a_i;
		}

		void UpdateParams() {
			switch (type_) {
			case BIQUAD_TYPE::HIGH_PASS:
			{
				freq_ = fmaxf(0.0, fminf(freq_, 1.0f));
				if (freq_ == 1) {
					SetNormalizeCoefficients(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
				else if (freq_ > 0) {
					Q_ = fmaxf(0.0f, Q_);
					float g = powf(10.0, 0.05f * Q_),
						d = sqrtf((4 - sqrtf(16 - 16 / powf(g, 2))) * 0.5f),
						theta = (float)M_PI * freq_,
						sn = 0.5f * d * sinf(theta),
						beta = 0.5f * (1 - sn) / (1 + sn),
						gamma = (0.5f + beta) * cosf(theta),
						alpha = 0.25f * (0.5f + beta + gamma);
					SetNormalizeCoefficients(2 * alpha, -4 * alpha, 2 * alpha,
						1, 2 * -gamma, 2 * beta);
				}
				else {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			case BIQUAD_TYPE::LOW_SHELF:
			{
				freq_ = fmaxf(0.0f, fminf(freq_, 1.0f));
				float A = powf(10.0, gain_ * 0.025f);
				if (freq_ == 1) {
					SetNormalizeCoefficients(powf(A, 2), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
				else if (freq_ > 0) {
					float w0 = (float)M_PI *freq_,
						S = 1.0f,
						alpha = 0.5f * sinf(w0) * sqrtf((A + 1 / A) * (1 / S - 1) + 2),
						k = cosf(w0),
						k2 = 2 * sqrtf(A) * alpha,
						a_p_one = A + 1,
						a_m_one = A - 1;
					SetNormalizeCoefficients(A * (a_p_one - a_m_one * k + k2), 2 * A * (a_m_one - a_p_one * k),
						A * (a_p_one - a_m_one * k - k2), a_p_one + a_m_one * k + k2,
						-2 * (a_m_one + a_p_one * k), a_p_one + a_m_one * k - k2);
				}
				else {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			case BIQUAD_TYPE::HIGH_SHELF:
			{
				freq_ = fmaxf(0.0f, fminf(freq_, 1.0f));
				auto A = powf(1.0f, gain_ * 0.025f);
				if (freq_ == 1) {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
				else if (freq_ > 0) {
					float w0 = (float)M_PI * freq_,
						S = 1.0f,
						alpha = 0.5f * sinf(w0) * sqrtf((A + 1 / A) * (1 / S - 1) + 2),
						k = cosf(w0),
						k2 = 2 * sqrtf(A) * alpha,
						a_p_one = A + 1,
						a_m_one = A - 1;
					SetNormalizeCoefficients(A * (a_p_one + a_m_one * k + k2), -2 * A * (a_m_one + a_p_one * k),
						A * (a_p_one + a_m_one * k - k2), a_p_one - a_m_one * k + k2,
						2 * (a_m_one - a_p_one * k), a_p_one - a_m_one * k - k2);
				}
				else {
					SetNormalizeCoefficients(powf(A, 2), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			case BIQUAD_TYPE::PEAKING:
			{
				freq_ = fmaxf(0.0f, fminf(freq_, 1.0f));
				Q_ = fmaxf(0.0f, Q_);
				auto A = powf(10.0f, gain_ * 0.025f);
				if (freq_ > 0 && freq_ < 1) {
					if (Q_ > 0) {
						float w0 = (float)M_PI * freq_,
							alpha = sinf(w0) / (2 * Q_),
							k = cosf(w0);
						SetNormalizeCoefficients(1 + alpha * A, -2 * k, 1 - alpha * A,
							1 + alpha / A, -2 * k, 1 - alpha / A);
					}
					else {
						SetNormalizeCoefficients(A * A, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
					}
				}
				else {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			case BIQUAD_TYPE::ALL_PASS:
			{
				freq_ = fmaxf(0.0f, fminf(freq_, 1.0f));
				Q_ = fmaxf(0.0f, Q_);
				if (freq_ > 0 && freq_ < 1) {
					if (Q_ > 0) {
						float w0 = (float)M_PI * freq_,
							alpha = sinf(w0) / (2 * Q_),
							k = cosf(w0);
						SetNormalizeCoefficients(1 - alpha, -2 * k, 1 + alpha,
							1 + alpha, -2 * k, 1 - alpha);
					}
					else {
						SetNormalizeCoefficients(-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
					}
				}
				else {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			case BIQUAD_TYPE::NOTCH:
			{
				freq_ = fmaxf(0.0f, fminf(freq_, 1.0f));
				Q_ = fmaxf(0.0f, Q_);
				if (freq_ > 0 && freq_ < 1) {
					if (Q_ > 0) {
						float w0 = (float)M_PI * freq_,
							alpha = sinf(w0) / (2 * Q_),
							k = cosf(w0);
						SetNormalizeCoefficients(1, -2 * k, 1, 1 + alpha, -2 * k, 1 - alpha);
					}
					else {
						SetNormalizeCoefficients(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
					}
				}
				else {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			case BIQUAD_TYPE::BAND_PASS:
			{
				freq_ = fmaxf(0.0f, freq_);
				Q_ = fmaxf(0.0f, Q_);
				if (freq_ > 0 && freq_ < 1) {
					float w0 = (float)M_PI * freq_;
					if (Q_ > 0) {
						float alpha = sinf(w0) / (2 * Q_),
							k = cosf(w0);
						SetNormalizeCoefficients(alpha, 0, -alpha, 1 + alpha, -2 * k, 1 - alpha);
					}
					else {
						SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
					}
				}
				else {
					SetNormalizeCoefficients(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			default:
			{
				freq_ = fmaxf(0.0, fminf(freq_, 1.0f));
				if (freq_ == 1) {
					SetNormalizeCoefficients(1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
				else if (freq_ > 0) {
					Q_ = fmaxf(0.0f, Q_);
					float g = powf(10.0, 0.05f * Q_),
						d = sqrtf((4 - sqrtf(16 - 16 / powf(g, 2.0f))) * 0.5f),
						theta = (float)M_PI * freq_,
						sn = 0.5f * d * sinf(theta),
						beta = 0.5f * (1 - sn) / (1 + sn),
						gamma = (0.5f + beta) * cosf(theta),
						alpha = 0.25f * (0.5f + beta - gamma);
					SetNormalizeCoefficients(2 * alpha, 4 * alpha, 2 * alpha, 1, 2 * -gamma, 2 * beta);
				}
				else {
					SetNormalizeCoefficients(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				}
			}
			break;
			}
		}
	public:
		Biquad(BIQUAD_TYPE type, float sample_rate, float frequency, float Q, float gain, float detune = 0) {
			type_ = type;
			gain_ = gain;
			Q_ = Q;
			float nyquist = 0.5f * sample_rate;
			freq_ = frequency / nyquist;
			if (detune > 0) {
				freq_ *= powf(2, detune / 1200);
			}
			UpdateParams();
		}

		uint64_t Process(vector<float> input, uint64_t input_len, vector<float> &output) override {
			for (int i = 0; i < input_len; i++) {
				float x = input[i],
					y = b0_ * x + b1_ * x1_ + b2_ * x2_ - a1_ * y1_ - a2_ * y2_;
				output[i] = y;
				x2_ = x1_;
				x1_ = x;
				y2_ = y1_;
				y1_ = y;
			}
		}
	};
}