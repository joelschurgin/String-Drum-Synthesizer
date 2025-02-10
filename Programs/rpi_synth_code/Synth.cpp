#include "Synth.h"

#include <stdio.h>
#include <math.h>
#include <thread>

template <typename SampleType>
Filter<SampleType>::Filter(SampleType _sample_rate) 
    : sample_rate{_sample_rate}
    , a1{0}
    , a2{0}
    , b0{1}
    , b1{0}
    , b2{0} 
    , w1{0}
    , w2{0} {}

template <typename SampleType>
Filter<SampleType>::~Filter() = default;

template <typename SampleType>
SampleType Filter<SampleType>::processSample(SampleType sample) {
    const SampleType w = sample - a1 * w1 - a2 * w2;
    const SampleType nextSample = b0 * w + b1 * w1 + b2 * w2;

    w2 = w1;
    w1 = w;

    return nextSample;
}

template <typename SampleType>
void Filter<SampleType>::lowpass(SampleType cutoff, SampleType Q) {
    const SampleType K = tan(M_PI * cutoff / sample_rate);
    const SampleType W = K * K;
    const SampleType KQ = K / Q;
    const SampleType invAlpha = 1.0 / (1.0 + KQ + W);

    a1 = 2.0 * (W - 1.0) * invAlpha;
    a2 = (1.0 - KQ + W) * invAlpha;

    b0 = W * invAlpha;
    b1 = 2.0 * b0;
    b2 = b0;
}

template <typename SampleType>
void Filter<SampleType>::highpass(SampleType cutoff, SampleType Q) {
    const SampleType K = tan(M_PI * cutoff / sample_rate);
    const SampleType W = K * K;
    const SampleType KQ = K / Q;
    const SampleType invAlpha = 1.0 / (1.0 + KQ + W);

    a1 = 2.0 * (W - 1.0) * invAlpha;
    a2 = (1.0 - KQ + W) * invAlpha;

    b0 = invAlpha;
    b1 = -2.0 * invAlpha;
    b2 = b0;
}

template <typename SampleType>
void Filter<SampleType>::peak(SampleType freq, SampleType Q, SampleType gaindB) {
    const SampleType gain = pow(10.0, gaindB * 0.05);
    const SampleType phase = (2.0 * M_PI) * freq / sample_rate;

    const SampleType phaseQ2 = 2.0 * phase / Q;
    const SampleType phaseSquared = phase * phase;
    const SampleType betaInv = 1.0 / (phaseQ2 + phaseSquared + 4);

    b0 = (phaseQ2 * gain + phaseSquared + 4.0) * betaInv;
    b1 = (2.0 * phaseSquared - 8.0) * betaInv;
    b2 = (4.0 - phaseQ2 * gain + phaseSquared) * betaInv;
    a1 = b1;
    a2 = (4.0 - phaseQ2 + phaseSquared) * betaInv;
}

template<typename SampleType>
CircularBuffer<SampleType>::CircularBuffer(double _sample_rate) 
    : sample_rate{_sample_rate}
    , read_idx{0}
    , write_idx{0}
    , delay{0}
    , buffer(static_cast<size_t>(sample_rate / 10.0), 0.0) {
    
}

template<typename SampleType>
CircularBuffer<SampleType>::~CircularBuffer() {
    buffer.clear();
}

template<typename SampleType>
void CircularBuffer<SampleType>::setDelay(const size_t num_delay_samples) {
    delay = num_delay_samples;
}

template<typename SampleType>
void CircularBuffer<SampleType>::clear() {
    std::fill(buffer.begin(), buffer.end(), 0); 
}

template<typename SampleType>
void CircularBuffer<SampleType>::pushSample(SampleType sample) {
    buffer[write_idx] = sample;

    const size_t num_samples = buffer.size();
    write_idx = (write_idx + num_samples - 1) % num_samples;
}

template<typename SampleType>
SampleType CircularBuffer<SampleType>::popSample() {
    const size_t num_samples = buffer.size();
    const SampleType sample = buffer[(read_idx + delay) % num_samples];
    read_idx = (read_idx + num_samples - 1) % num_samples;
    return sample;
}

Synth::Synth(double _sample_rate) 
    : sample_rate{_sample_rate} 
    , pitch{0.0}
    , transient_sample_idx{UINT32_MAX}
    , transient_length{0}
    , scratchiness{0.5}
    , tail{1.0}
    , FilterQ{1.0/sqrt(2.0)}
    , strike_weight{0.5}
    , osc_phase{0.0}
    , sharpness{0.5}
    , dampening{1.0}
    , brightness{20000.0}
    , circular_buffer{sample_rate}
    , noise(-1.0, 1.0)
    , lowpassFilter(sample_rate) 
    , highpassFilter(sample_rate) 
    , peakFilter(sample_rate) {
}
Synth::~Synth() {}

void Synth::processBuffer(unsigned char* buffer, const int buffer_size) {
    if (pitch == 0) return;

    double scaledScratchiness = 4 * scratchiness * scratchiness;
    transient_length = static_cast<unsigned>(scaledScratchiness * sample_rate / pitch);

    feedback = pow(tail, 0.25);

    const double osc_phase_inc = 2.0 * M_PI * osc_freq / sample_rate;

    lowpassFilter.lowpass(brightness, FilterQ);
    highpassFilter.highpass(boominess, FilterQ);
    peakFilter.peak(3600, dampening, -6.0);

    double sample;
    for(int sampleIdx = 0; sampleIdx < buffer_size; sampleIdx++) {
        sample = circular_buffer.popSample();
        if (transient_sample_idx <= transient_length) {
            if (transient_sample_idx == 0) {
                stopLatencyTimer();
            }

            const double osc_sample = (-2.0 * (abs((double)transient_sample_idx - (double)transient_length * 0.5) + 1.0) / (double)transient_length) * sin(osc_phase);
            sample = (1.0 - sharpness) * osc_sample + sharpness * noise(random_engine);
            
            osc_phase += osc_phase_inc;
            if (osc_phase >= M_PI) {
                osc_phase -= M_PI * 2.0;
            }

            transient_sample_idx++;
        }
        
        sample = lowpassFilter.processSample(sample);
        sample = highpassFilter.processSample(sample);
        sample = peakFilter.processSample(sample);
        circular_buffer.pushSample(feedback * sample);

        const short outSample = SHRT_MAX * sample;
        buffer[4*sampleIdx+0] = (unsigned char)outSample;
        buffer[4*sampleIdx+1] = outSample >> 8;
        buffer[4*sampleIdx+2] = (unsigned char)outSample;
        buffer[4*sampleIdx+3] = outSample >> 8;
    }
}

void Synth::playNote(double nextPitch) {
    pitch = nextPitch;
    transient_sample_idx = 0;
    circular_buffer.setDelay(static_cast<size_t>(sample_rate / pitch));
    circular_buffer.clear();
    osc_phase = 0.0;
}

void Synth::updateParams(short *knob_vals) {
    sharpness = static_cast<double>(knob_vals[0]) / 255.0;

    strike_weight = static_cast<double>(knob_vals[1]) / 255.0;
    osc_freq = LinearToLog<double>(1.0 - strike_weight, 0.0, 1.0, 20.0, 20000.0);

    dampening = LinearToLog<double>(static_cast<double>(255 - knob_vals[4]), 0.0, 255.0, 0.1, 40.0);

    brightness = LinearToLog<double>(static_cast<double>(knob_vals[5]), 0.0, 255.0, 20.0, 20000.0);
    boominess = LinearToLog<double>(static_cast<double>(255 - knob_vals[5]), 0.0, 255.0, 20.0, 20000.0);

    scratchiness = static_cast<double>(knob_vals[7]) / 255.0;
}

void Synth::startLatencyTimer() {
    timer = std::chrono::steady_clock::now();
}

void Synth::stopLatencyTimer() {
    const auto currTime = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currTime - timer);
    printf("\033[A\033[ALatency: %f ms\n\n", (static_cast<double>(duration.count()) * 1.0E-9) + (1000.0 / sample_rate));
}