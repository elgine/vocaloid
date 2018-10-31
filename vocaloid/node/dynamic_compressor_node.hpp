#pragma once
#include <math.h>
#include <float.h>
#include "vocaloid/utils/buffer.hpp"
#include "audio_node.hpp"
#include "audio_context.hpp"
#include "audio_param.hpp"
#include "vocaloid/maths/base.hpp"
// From Chromium
// https://github.com/nwjs/chromium.src/blob/df7f8c8582b9a78c806a7fa1e9d3f3ba51f7a698/third_party/WebKit/Source/platform/audio/DynamicsCompressorKernel.cpp
namespace vocaloid{
    #define DEFAULT_PREDELAY_FRAMES 256
    #define MAX_PREDELAY_FRAMES 1024
    #define MAX_PREDELAY_FRAMES_MASK 1
    class DynamicCompressorNode: public AudioNode{
    private:
        float m_attack_;
        float m_knee_;
        float m_ratio_;
        float m_release_;
        float m_threshold_;
        float m_linear_threshold_;
        float m_db_knee_;
        float m_db_threshold_;
        float m_slope_;
        float m_knee_threshold_db_;
        float m_knee_threshold_;
        float m_y_knee_threshold_db_;
        float m_last_predelay_frames_;
        float m_predelay_read_index_;
        float m_predelay_write_index_;
        float m_detector_average_;
        float m_compressor_gain_;
        float m_max_attack_compression_diff_db_;
        float m_metering_gain_;
        float m_metering_release_k_;
        AudioBuffer *m_predelay_buffers_;

        float Slope(float x, float k){
            if(x < m_linear_threshold_){
                return 1;
            }
            float x2 = x * 1.001;
            float x_db = Linear2Db(x);
            float x2_db = Linear2Db(x2);
            float y_db = Linear2Db(KneeCurve(x, k));
            float y2_db = Linear2Db(KneeCureve(x2, k));

            float m = (y2_db - y_db) / (x2_db - x_db);
            return m;
        }

        void SetPredelayTime(float predelay_time){
            auto predelay_frames = predelay_time * context_->GetSampleRate();
            if(predelay_frames > MAX_PREDELAY_FRAMES - 1)
                predelay_frames = MAX_PREDELAY_FRAMES - 1;
            if(m_last_predelay_frames_ != predelay_frames){
                m_last_predelay_frames_ = predelay_frames;
                m_predelay_buffers_->Fill(0);
                m_predelay_read_index_ = 0;
                m_predelay_write_index_ = predelay_frames;
            }
        }

        float KneeCurve(float x, float k){
            if(x < m_linear_threshold_)return x;
            return m_linear_threshold_ + (1 - expf(-k * m_linear_threshold_)) / k;
        }

        float Saturate(float x, float k){
            float y;
            if(x < m_knee_threshold_){
                y = KneeCurve(x, k);
            }else{
                float x_db = Linear2Db(x);
                float y_db = m_y_knee_threshold_db_ + m_slope * (x_db - m_knee_threshold_);
                y = Db2Linear(y_db);
            }
            return y;
        }

        float KneeSlope(float desired_slope){
            float x_db = m_db_threshold_ + m_db_knee_;
            float x = Db2Linear(x_db);

            float min_k = 0.1;
            float max_k = 10000;
            float k = 5;
            for(auto i = 0;i < 15;i++){
                float slope = Slope(x, k);
                if(slope < desired_slope){
                    max_k = k;
                }else{
                    min_k = k;
                }
                k = sqrtf(min_k * max_k);
            }
            return k;
        }

        void Process(AudioBuffer *in, float db_threshold, float db_knee, float ratio,
                    float attack, float release, float predelay, float db_post_gain,
                    float effect_blend, float release_zone1, float release_zone2,
                    float release_zone3, float release_zone4){
            auto sample_rate = context_->GetSampleRate();
            float dry_mix = 1 - effect_blend;
            float wet_mix = effect_blend;
            float k = UpdateStaticCurve(db_threshold, db_knee, ratio);

            float full_range_gain = Saturate(1, k);
            float full_range_makeup_gain = 1 / full_range_gain;

            full_range_makeup_gain = powf(full_range_makeup_gain, 0.6f);
            float master_linear_gain = Db2Linear(db_post_gain) * full_range_makeup_gain;

            attack = max(0.001, attack);
            float attack_frames = attack * sample_rate;

            float release_frames = sample_rate * release;

            float sat_release = 0.0025;
            float sat_release_frames = sat_release * sample_rate;

            float y1 = release_frames * release_zone1;
            float y2 = release_frames * release_zone2;
            float y3 = release_frames * release_zone3;
            float y4 = release_frames * release_zone4;

            float a = 0.9999999999999998f * y1 + 1.8432219684323923e-16f * y2 -
                      1.9373394351676423e-16f * y3 + 8.824516011816245e-18f * y4;
            float b = -1.5788320352845888f * y1 + 2.3305837032074286f * y2 -
                      0.9141194204840429f * y3 + 0.1623677525612032f * y4;
            float c = 0.5334142869106424f * y1 - 1.272736789213631f * y2 +
                      0.9258856042207512f * y3 - 0.18656310191776226f * y4;
            float d = 0.08783463138207234f * y1 - 0.1694162967925622f * y2 +
                      0.08588057951595272f * y3 - 0.00429891410546283f * y4;
            float e = -0.042416883008123074f * y1 + 0.1115693827987602f * y2 -
                      0.09764676325265872f * y3 + 0.028494263462021576f * y4;

            SetPredelayTime(predelay);
            const int n_division_frames = 32;
            const int n_divisions = frame_size_ / n_division_frames;
            uint16_t frame_index = 0;
            for(auto i = 0; i < n_divisions;i++){
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Calculate desired gain
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                if(isnan(m_detector_average_))m_detector_average_ = 1;
                if(isinf(m_detector_average_))m_detector_average_ = 1;
                float desired_gain = m_detector_average_;
                float scaled_desired_gain = asinf(desired_gain) / M_PI_2;

                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Deal with envelopes
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // envelopeRate is the rate we slew from current compressor level to the
                // desired level.  The exact rate depends on if we're attacking or
                // releasing and by how much.
                float envelope_rate;
                bool is_releasing = scaled_desired_gain > m_compressor_gain_;
                float compression_diff_db = Linear2Db(m_compressor_gain_ / scaled_desired_gain);
                if(is_releasing){
                    m_max_attack_compression_diff_db_ = -1;
                    if(isnan(compression_diff_db)){
                        compression_diff_db = -1;
                    }
                    if(isinf(compression_diff_db)){
                        compression_diff_db = -1;
                    }

                    float x = compression_diff_db;
                    x = Clamp(-12, 0, x);
                    x = 0.25 * (x + 12);

                    float x2 = x * x;
                    float x3 = x2 * x;
                    float x4 = x2 * x2;
                    float release_frames = a + b * x + c * x2 + d * x3 + e * x4;

#define SPACING_DB 5
                    float db_per_frame = SPACING_DB / release_frames;

                    envelope_rate = Db2Linear(db_per_frame);
                }else{
                    if(isnan(compression_diff_db))
                        compression_diff_db = 1;
                    if(isinf(compression_diff_db))
                        compression_diff_db = 1;
                    if(m_max_attack_compression_diff_db_ == -1 ||
                        m_max_attack_compression_diff_db_ < compression_diff_db)
                        m_max_attack_compression_diff_db_ = compression_diff_db;
                    float eff_atten_diff_db = max(0.5, m_max_attack_compression_diff_db_);
                    float x = 0.25 / eff_atten_diff_db;
                    envelope_rate = 1 - powf(x, 1 / attack_frames);
                }

                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // Inner loop - calculate shaped power average - apply compression.
                // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                {
                    auto predelay_read_index = m_predelay_read_index_;
                    auto predelay_write_index = m_predelay_write_index_;
                    float detector_average = m_detector_average_;
                    float compressor_gain = m_compressor_gain_;

                    int loop_frames = n_division_frames;
                    while(loop_frames--){
                        float compressor_input = 0;
                        for(auto i = 0;i < channels_;i++){
                            float undelayed_source = summing_buffer_->Channel(i)->Data()[frame_index];
                            m_predelay_buffers_->Channel(i)->Data()[predelay_write_index] = undelayed_source;
                            float abs_undelayed_source = undelayed_source > 0?undelayed_source:-undelayed_source;
                            if(compressor_input < abs_undelayed_source)
                                compressor_input = abs_undelayed_source;
                        }

                        float scaled_input = compressor_input;
                        float abs_input = scaled_input > 0?scaled_input:-scaled_input;

                        float shaped_input = Saturate(abs_input, k);
                        float attenuation = abs_input <= 0.0001?1:shaped_input/abs_input;

                        float attenuation_db = -Linear2Db(attenuation);
                        attenuation_db = max(2.0f, abs_undelayed_source);

                        float db_per_frame = attenuation_db / sat_release_frames;
                        float sat_release_rate = Db2Linear(db_per_frame) - 1;

                        bool is_release = (attenuation > detector_average);
                        float rate = is_release?sat_release_rate:1;

                        detector_average += (attenuation - detector_average) * rate;
                        detector_average = min(1.0f, detector_average);

                        if(isnan(detector_average))
                            detector_average = 1;
                        if(isinf(detector_average))
                            detector_average = 1;
                        if(envelope_rate < 1)
                            compressor_gain += (scaled_desired_gain - compressor_gain) * envelope_rate;
                        else{
                            compressor_gain *= envelope_rate;
                            compressor_gain = min(1.0f, compressor_gain);
                        }

                        float post_warp_compressor_gain = sinf(M_PI_2 * compressor_gain);
                        float total_gain = dry_mix + wet_mix * master_linear_gain * post_warp_compressor_gain;

                        float db_real_gain = 20 * log10f(post_warp_compressor_gain);
                        if(db_real_gain < m_metering_gain_){
                            m_metering_gain_ = db_real_gain;
                        }else{
                            m_metering_gain_ += (db_real_gain - m_metering_gain_) * m_metering_release_k_;
                        }

                        for(auto i = 0;i < channels_;i++){
                            in->Channel(i)[frame_index] = m_predelay_buffers_->Channel(i)->Data()[predelay_read_index] * total_gain;
                        }
                        frame_index++;
                        predelay_read_index = (predelay_read_index + 1) & MAX_PREDELAY_FRAMES_MASK;
                        predelay_write_index = (predelay_write_index + 1) & MAX_PREDELAY_FRAMES_MASK;
                    }

                    // Locals back to member variables.
                    m_predelay_read_index_ = predelay_read_index;
                    m_predelay_write_index_ = predelay_write_index;
                    m_detector_average_ = (fabs(detector_average) < FLT_MIN) ? 0.0f : detector_average;
                    m_compressor_gain_ = (fabs(compressor_gain) < FLT_MIN) ? 0.0f : compressor_gain;
                }
            }
        }
    public:
        AudioParam *attack_;
        AudioParam *knee_;
        AudioParam *ratio_;
        AudioParam *release_;
        AudioParam *threshold_;

        static Db2Linear(float v){
            if(v == 0)return -1000;
            return 20 * log10f(v);
        }

        static Linear2Db(float v){
            return powf(10, v * 0.05f);
        }

        explicit DynamicCompressorNode(AudioContext *ctx):AudioNode(ctx){
            delay_buf_ = new Buffer<float>();
            attack_ = new AudioParam(AudioParamType::K_RATE);
            attack_->value_ = 0.003;
            knee_ = new AudioParam(AudioParamType::K_RATE);
            knee_->value_ = 30;
            ratio_ = new AudioParam(AudioParamType::K_RATE);
            ratio_->value_ = 12;
            release_ = new AudioParam(AudioParamType::K_RATE);
            release_->value_ = 0.25;
            threshold_ = new AudioParam(AudioParamType::K_RATE);
            threshold_->value_ = -24;
        }

        float UpdateStaticCurve(float db_threshold, float db_knee, float ratio){
            if(db_threshold != m_db_threshold_ || db_knee != m_db_knee_ || ratio != m_ratio_){
                m_db_threshold_ = db_threshold;
                m_linear_threshold_ = Db2Linear(db_threshold);
                m_db_knee_ = db_knee;
                m_ratio_ = ratio;
                m_slope_ = 1 / m_ratio_;
                float k = KneeSlope(1 / m_ratio_);
                m_knee_threshold_db_ = db_threshold + db_knee;
                m_knee_threshold_ = Db2Linear(m_knee_threshold_db_);
                m_y_knee_threshold_db_ = Linear2Db(KneeCurve(m_knee_threshold_, k));
                m_knee_ = k;
            }
            return m_knee_;
        }

        void Initialize(uint64_t frame_size){
            AudioNode::Initialize(frame_size);
            auto sample_rate = context_->GetSampleRate();
            attack_->Initialize(sample_rate, frame_size);
            knee_->Initialize(sample_rate, frame_size);
            ratio_->Initialize(sample_rate, frame_size);
            release_->Initialize(sample_rate, frame_size);
            threshold_->Initialize(sample_rate, frame_size);
            m_predelay_buffers_->Alloc(channels_, frame_size_);
        }

        void Reset(){
            m_detector_average_ = 0;
            m_compressor_gain_ = 1;
            m_metering_gain_ = 1;
            m_predelay_buffers_->Fill(0);
            m_predelay_read_index_ = 0;
            m_predelay_write_index_ = DEFAULT_PREDELAY_FRAMES;
            m_max_attack_compression_diff_db_ = -1;
        }

        int64_t Process(AudioBuffer *in){

            return frame_size_;
        }
    };
}