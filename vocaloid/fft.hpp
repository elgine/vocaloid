#pragma once
#include <stdint.h>
#include <vector>
#include <math.h>
using namespace std;

namespace vocaloid {

	// Fast Fourier Transformation
	class FFT{
	private:
        vector<int> reverse_table_;
        vector<float> sin_table_;
        vector<float> cos_table_;

	protected:
        uint32_t buffer_size_;
		float sample_rate_;
		float band_width_;
	public:

        vector<float> real_;
        vector<float> imag_;

        static float CalculateMagnitude(float real, float imag){
            return sqrtf(powf(real, 2) + powf(imag, 2));
        }

        static float CalculatePhase(float real, float imag){
            return atan2f(imag, real);
        }

        static float MapRadianToPi(float rad) {
            while (rad > M_PI)
                rad -= 2 * M_PI;
            while (rad < -M_PI)
                rad += 360;
            return rad;
        }

        void CalculateSpectrum(vector<float> &spectrum, float &peak_band, float &peak) {
            spectrum.resize(buffer_size_ / 2, 0);
            float b_si = 2 / buffer_size_, rval, ival, mag;
            for (int i = 0, N = buffer_size_ / 2; i < N; i++) {
                rval = real_[i];
                ival = imag_[i];
                mag = (float)(b_si * sqrt(powf(rval, 2) + powf(ival, 2)));
                if (mag > peak) {
                    peak_band = i;
                    peak = mag;
                }
                spectrum[i] = mag;
            }
        }

		void Initialize(int buffer_size, float sample_rate) {
			buffer_size_ = buffer_size;
			sample_rate_ = sample_rate;
			real_ = vector<float>(buffer_size_, 0);
			imag_ = vector<float>(buffer_size_, 0);
			band_width_ = 2.0f / buffer_size_ * sample_rate_ / 2.0f;
			reverse_table_ = vector<int>(buffer_size_);
			int limit = 1;
			int bit = buffer_size_ >> 1;
			int i;
			while (limit < buffer_size_) {
				for (i = 0; i < limit; i++) {
					reverse_table_[i + limit] = reverse_table_[i] + bit;
				}
				limit = limit << 1;
				bit = bit >> 1;
			}
			sin_table_ = vector<float>(buffer_size_);
			cos_table_ = vector<float>(buffer_size_);
			for (i = 0; i < buffer_size_; i++) {
				sin_table_[i] = sinf(-M_PI / (float)i);
				cos_table_[i] = cosf(-M_PI / (float)i);
			}
		}

		uint32_t GetBufferSize() {
			return buffer_size_;
		}

		float GetSampleRate() {
			return sample_rate_;
		}

		float GetBandWidth() {
			return band_width_;
		}

		// Do FFT
		void Forward(const vector<float> buffer, int buffer_len) {
			float k = floorf(logf(buffer_size_) / 0.693f);
			if (pow(2, k) != buffer_size_) {
				throw "Invalid buffer size, must be a power of 2.";
			}
			if (buffer_size_ != buffer_len) {
				throw "Supplied buffer is not the same size as defined FFT. ";
			}

			float phase_shift_step_real, phase_shift_step_imag,
				cur_phase_shift_real, cur_phase_shift_imag,
				tr, ti, tmp_real;
			int i, half_size = 1, off;
			for (i = 0; i < buffer_size_; i++) {
				real_[i] = buffer[reverse_table_[i]];
				imag_[i] = 0;
			}

			while (half_size < buffer_size_) {
				phase_shift_step_real = cos_table_[half_size];
				phase_shift_step_imag = sin_table_[half_size];

				cur_phase_shift_real = 1;
				cur_phase_shift_imag = 0;

				for (int fft_step = 0; fft_step < half_size; fft_step++) {
					i = fft_step;
					while (i < buffer_size_) {
						off = i + half_size;
						tr = (cur_phase_shift_real * real_[off]) - (cur_phase_shift_imag * imag_[off]);
						ti = (cur_phase_shift_real * imag_[off]) + (cur_phase_shift_imag * real_[off]);

						real_[off] = real_[i] - tr;
						imag_[off] = imag_[i] - ti;
						real_[i] += tr;
						imag_[i] += ti;

						i += half_size << 1;
					}

					tmp_real = cur_phase_shift_real;
					cur_phase_shift_real = (tmp_real * phase_shift_step_real) - (cur_phase_shift_imag * phase_shift_step_imag);
					cur_phase_shift_imag = (tmp_real * phase_shift_step_imag) + (cur_phase_shift_imag * phase_shift_step_real);
				}
				half_size = half_size << 1;
			}
		}

		void Inverse(vector<float> &output) {
			Inverse(real_, imag_, buffer_size_, output);
		}

		// Do IFFT
		void Inverse(vector<float> real, vector<float> imag, int len, vector<float> &output) {
			int half_size = 1, off, i;
			float phase_shift_step_real, phase_shift_step_imag,
				cur_phase_shift_real, cur_phase_shift_imag,
				tr, ti, tmp_real;
			for (i = 0; i < buffer_size_; i++) {
				imag[i] *= -1;
			}

			vector<float>rev_real = vector<float>(buffer_size_),
				rev_imag = vector<float>(buffer_size_);

			for (i = 0; i < len; i++) {
				rev_real[i] = real[reverse_table_[i]];
				rev_imag[i] = imag[reverse_table_[i]];
			}
			real = rev_real;
			imag = rev_imag;

			while (half_size < buffer_size_) {
				phase_shift_step_real = cos_table_[half_size];
				phase_shift_step_imag = sin_table_[half_size];
				cur_phase_shift_real = 1;
				cur_phase_shift_imag = 0;

				for (int fft_step = 0; fft_step < half_size; fft_step++) {
					i = fft_step;
					while (i < buffer_size_) {
						off = i + half_size;
						tr = (cur_phase_shift_real * real[off]) - (cur_phase_shift_imag * imag[off]);
						ti = (cur_phase_shift_real * imag[off]) + (cur_phase_shift_imag * real[off]);
						real[off] = real[i] - tr;
						imag[off] = imag[i] - ti;
						real[i] += tr;
						imag[i] += ti;
						i += half_size << 1;
					}
					tmp_real = cur_phase_shift_real;
					cur_phase_shift_real = (tmp_real * phase_shift_step_real) - (cur_phase_shift_imag * phase_shift_step_imag);
					cur_phase_shift_imag = (tmp_real * phase_shift_step_imag) + (cur_phase_shift_imag * phase_shift_step_real);
				}
				half_size = half_size << 1;
			}

			for (i = 0; i < buffer_size_; i++) {
				output[i] = real[i];
			}
		}

		void Dispose() {
			real_.clear();
			imag_.clear();
			reverse_table_.clear();
			sin_table_.clear();
			cos_table_.clear();
			band_width_ = 0;
		}
	};
}