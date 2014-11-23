
#include <stdint.h>
#include <stdlib.h>

#include "agc/include/gain_control.h"
#include "ns/include/noise_suppression_x.h"
#include "other/signal_processing_library.h"

typedef struct {
    NsxHandle *noise_sup_x;
    void *gain_control;
    uint32_t fs;

    WebRtcSpl_State48khzTo16khz state_in;
    WebRtcSpl_State16khzTo48khz state_out;
    int32_t tmp_mem[496];
} Filter_Audio;

#define _FILTER_AUDIO
#include "filter_audio.h"

void kill_filter_audio(Filter_Audio *f_a)
{
    WebRtcNsx_Free(f_a->noise_sup_x);
    WebRtcAgc_Free(f_a->gain_control);
    free(f_a);
}

Filter_Audio *new_filter_audio(uint32_t fs)
{
    if (fs != 8000 && fs != 16000 && fs != 48000) {
        return NULL;
    }

    Filter_Audio *f_a = calloc(sizeof(Filter_Audio), 1);

    if (!f_a) {
        return NULL;
    }

    f_a->fs = fs;

    if (fs == 48000) {
        fs = 16000;
    }

    if (WebRtcAgc_Create(&f_a->gain_control) == -1) {
        free(f_a);
        return NULL;
    }

    if (WebRtcNsx_Create(&f_a->noise_sup_x) == -1) {
        WebRtcAgc_Free(f_a->gain_control);
        free(f_a);
        return NULL;
    }

    WebRtcAgc_config_t gain_config;

    gain_config.targetLevelDbfs = 1;
    gain_config.compressionGaindB = 50;
    gain_config.limiterEnable = kAgcFalse;
 
    if (WebRtcAgc_Init(f_a->gain_control, 0, 255, kAgcModeAdaptiveDigital, fs) == -1 || WebRtcAgc_set_config(f_a->gain_control, gain_config) == -1) {
        kill_filter_audio(f_a);
        return NULL;
    }


    if (WebRtcNsx_Init(f_a->noise_sup_x, fs) == -1 || WebRtcNsx_set_policy(f_a->noise_sup_x, 2) == -1) {
        kill_filter_audio(f_a);
        return NULL;
    }

    return f_a;
}

static void downsample_audio(Filter_Audio *f_a, int16_t *out, int16_t *in)
{
    WebRtcSpl_Resample48khzTo16khz(in, out, &f_a->state_in, f_a->tmp_mem);
}

static void upsample_audio(Filter_Audio *f_a, int16_t *out, int16_t *in)
{
    WebRtcSpl_Resample16khzTo48khz(in, out, &f_a->state_out, f_a->tmp_mem);
}


int filter_audio(Filter_Audio *f_a, int16_t *data, unsigned int samples)
{
    unsigned int nsx_samples = f_a->fs / 100;
    if (!samples || (samples % nsx_samples) != 0) {
        return -1;
    }

    _Bool resample = 0;
    unsigned int resampled_samples = 0;
    if (f_a->fs == 48000) {
        samples = (samples / nsx_samples) * 160;
        nsx_samples = 160;
        resample = 1;
    }

    unsigned int temp_samples = samples;

    while (temp_samples) {
        int16_t d[nsx_samples];
        if (resample) {
            downsample_audio(f_a, d, data + resampled_samples);
        } else {
            memcpy(d, data + (samples - temp_samples), sizeof(d));
        }

        if (WebRtcNsx_Process(f_a->noise_sup_x, d, 0, d, 0) == -1) {
            return -1;
        }

        //TODO echo canceller here

        int32_t inMicLevel = 1, outMicLevel;
        uint8_t saturationWarning;

        if (WebRtcAgc_Process(f_a->gain_control, d, 0, nsx_samples, d, 0, inMicLevel, &outMicLevel, 0, &saturationWarning) == -1) {
            return -1;
        }

        if (resample) {
            upsample_audio(f_a, data + resampled_samples, d);
            resampled_samples += 480;
        } else {
            memcpy(data + (samples - temp_samples), d, sizeof(d));
        }

        temp_samples -= nsx_samples;
    }

    return 0;
}

