#pragma once

#include <climits>
#include <chrono>
#include <random>
#include <stdio.h>
#include <vector>

template<typename SampleType>
inline SampleType LinearToLog(SampleType linValue, SampleType linMin, SampleType linMax, SampleType logMin, SampleType logMax) {
    const SampleType scaledLinFreq = (linValue - linMin) / (linMax - linMin);
    return logMin * pow((logMax / logMin), scaledLinFreq);
}

template<typename SampleType>
inline SampleType LogToLinear(SampleType logValue, SampleType linMin, SampleType linMax, SampleType logMin, SampleType logMax) {
    const SampleType unLog = log2(logValue / logMin) / log2(logMax / logMin);
    return (linMax - linMin) * unLog + linMin;
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// approx of sin(pi*x) where x goes from -1 to 1
template<typename SampleType>
inline SampleType FastSin(SampleType phase) {
    const SampleType phase_abs = abs(phase);
    return 4.0 * phase_abs * (1.0 - phase_abs) * sgn<SampleType>(phase);
}

template<typename SampleType>
class Filter {
public:
    Filter(SampleType _sample_rate);
    ~Filter();

    SampleType processSample(SampleType sample);
    void lowpass(SampleType cutoff, SampleType Q);
    void highpass(SampleType cutoff, SampleType Q);
    void peak(SampleType freq, SampleType Q, SampleType gaindB);

private:
    SampleType sample_rate;

    SampleType a1, a2, b0, b1, b2;
    SampleType w1, w2;
};

template<typename SampleType>
class CircularBuffer {
public:
    CircularBuffer(double _sample_rate);
    ~CircularBuffer();

    void setDelay(const size_t num_delay_samples);
    void clear();

    void pushSample(SampleType sample);
    SampleType popSample();

private:
    double sample_rate;

    size_t read_idx;
    size_t write_idx;
    size_t delay;

    std::vector<SampleType> buffer;
};

class Synth {
public:
    Synth(double _sample_rate);
    ~Synth();
    void processBuffer(unsigned char* buffer, const int buffer_size);
    void playNote(double nextPitch);

    void updateParams(short* knob_vals);

    void startLatencyTimer();
    void stopLatencyTimer();

private:
    double sample_rate;
    double pitch;
    unsigned transient_sample_idx;
    
    double scratchiness;
    unsigned transient_length;

    double feedback;
    double tail;

    double brightness; // lpf cutoff
    double boominess; // hpf cutoff

    const double FilterQ;

    double strike_weight;
    double osc_freq;
    double osc_phase;

    double sharpness;

    double dampening;

    CircularBuffer<double> circular_buffer;

    std::default_random_engine random_engine;
    std::uniform_real_distribution<double> noise;

    Filter<double> lowpassFilter;
    Filter<double> highpassFilter;
    Filter<double> peakFilter;

    std::chrono::time_point<std::chrono::steady_clock> timer;
};

