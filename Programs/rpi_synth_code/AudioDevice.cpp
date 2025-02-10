#include "AudioDevice.h"

#define AUDIO_HARDWARE_NAME "plughw:CARD=KTUSBAUDIO"

AudioDevice::AudioDevice() 
    : isPlaying{false}
    , data(period_size << 1, 0) 
    , synth(target_sample_rate) {
    dataPtr = &data.data()[0];
}


AudioDevice::~AudioDevice() {
    stop();
    // TODO: more cleanup needed
}

int AudioDevice::initiallize() {
    snd_pcm_hw_params_alloca(&hwparams);

    if (snd_pcm_open(&pcm_handle, AUDIO_HARDWARE_NAME, stream, 0) < 0) {
        fprintf(stderr, "Error opening PCM device %s\n", AUDIO_HARDWARE_NAME);
        return -1;
    }

    /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
        fprintf(stderr, "Can not configure this PCM device.\n");
        return -1;
    }

    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
        fprintf(stderr, "Error setting access.\n");
        return(-1);
    }
  
    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
        fprintf(stderr, "Error setting format.\n");
        return(-1);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */ 
    exact_sample_rate = target_sample_rate;
    if (snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &exact_sample_rate, 0) < 0) {
        fprintf(stderr, "Error setting rate.\n");
        return(-1);
    }
    if (target_sample_rate != exact_sample_rate) {
        fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n ==> Using %d Hz instead.\n", target_sample_rate, exact_sample_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, 2) < 0) {
        fprintf(stderr, "Error setting channels.\n");
        return(-1);
    }

    /* Set number of periods. Periods used to be called fragments. */ 
    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
        fprintf(stderr, "Error setting periods.\n");
        return(-1);
    }

    /* Set buffer size (in frames). The resulting latency is given by */
    /* latency = period_size * periods / (rate * bytes_per_frame)     */
    if (snd_pcm_hw_params_set_buffer_size(pcm_handle, hwparams, (period_size * periods)>>2) < 0) {
        fprintf(stderr, "Error setting buffersize.\n");
        return(-1);
    }

    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
        fprintf(stderr, "Error setting HW params.\n");
        return(-1);
    }

    return 0;
}

void AudioDevice::play() {
    isPlaying = true;
    startAudioThread(); // no real thread created right now
}

void AudioDevice::stop() {
    isPlaying = false;
    snd_pcm_drain(pcm_handle);
}

void AudioDevice::processBuffer() {
    synth.processBuffer(dataPtr, buffer_size);
}

void AudioDevice::startAudioThread() {
    int pcmreturn;
    bool half_cycle = false;
    dataPtr = &data.data()[0];

    while (isPlaying) {
        processBuffer();
        while ((pcmreturn = snd_pcm_writei(pcm_handle, dataPtr, buffer_size)) < 0) {
            snd_pcm_prepare(pcm_handle);
            fprintf(stderr, "<<<<<<<<<<<<<<< Buffer Underrun >>>>>>>>>>>>>>>\n");
        }
        dataPtr = (half_cycle) ? &data.data()[0] : &data.data()[period_size];
        half_cycle = !half_cycle;
    }
}
