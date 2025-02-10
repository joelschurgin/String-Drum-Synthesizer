#pragma once

#include <alsa/asoundlib.h>
#include <cstdlib>
#include <pthread.h>
#include <stdio.h>
#include <vector>
#include "Synth.h"

class AudioDevice {
public:
    AudioDevice();
    ~AudioDevice();

    int initiallize();
    void play();
    void stop();

    void processBuffer();

    Synth& getSynth() { return synth; }

private:
    void startAudioThread();

private:
    snd_pcm_t *pcm_handle;
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
    snd_pcm_hw_params_t *hwparams;

    const unsigned int target_sample_rate = 48000; /* Sample rate */
    unsigned int exact_sample_rate;   /* Sample rate returned by */
                      /* snd_pcm_hw_params_set_rate_near */ 
    int dir;          /* exact_rate == rate --> dir = 0 */
                      /* exact_rate < rate  --> dir = -1 */
                      /* exact_rate > rate  --> dir = 1 */
    const int periods = 2;       /* Number of periods */
    snd_pcm_uframes_t period_size = 256; /* period_size (bytes) */
    const int buffer_size = period_size >> 2; // divide by 4 because 2 bytes per sample x 2 channels

    bool isPlaying;

    std::vector<unsigned char> data;
    unsigned char* dataPtr;

    Synth synth;
};