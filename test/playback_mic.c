#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <math.h>

#include <AL/al.h>
#include <AL/alc.h> 

#include "../filter_audio.h"


static void sourceplaybuffer(ALuint source, int16_t *data, int samples, _Bool channels, unsigned int sample_rate)
{
    if(!channels || channels > 2) {
        return;
    }

    ALuint bufid;
    ALint processed = 0, queued = 16;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    if(processed) {
        ALuint bufids[processed];
        alSourceUnqueueBuffers(source, processed, bufids);
        alDeleteBuffers(processed - 1, bufids + 1);
        bufid = bufids[0];
    } else if(queued < 16) {
        alGenBuffers(1, &bufid);
    } else {
        printf("dropped audio frame\n");
        return;
    }

    alBufferData(bufid, (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, samples * 2 * channels, sample_rate);
    alSourceQueueBuffers(source, 1, &bufid);

    ALint state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING) {
        alSourcePlay(source);
        printf("Starting source\n");
    }
}

int main()
{
    unsigned int sample_rate = 48000;
    unsigned int samples_perframe = sample_rate/50;
    _Bool filter = 1;

    const char *in_device_list = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

    const char *temp_d = in_device_list;
    while (*temp_d) {
        printf("%s\n", temp_d);
        temp_d += strlen(temp_d) + 1;
    }

    ALCdevice *device_in = alcCaptureOpenDevice(in_device_list, sample_rate, AL_FORMAT_MONO16, samples_perframe);
    if (!device_in) {
        printf("open in dev failed\n");
        return 0;
    }

    const char *out_device_list = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
    ALCdevice *device_out = alcOpenDevice(out_device_list);

    ALCcontext *context = alcCreateContext(device_out, NULL);
    if(!alcMakeContextCurrent(context)) {
        printf("alcMakeContextCurrent() failed\n");
        alcCloseDevice(device_out);
        return 0;
    }

    Filter_Audio *f_a = new_filter_audio(sample_rate);

    ALuint source;
    alGenSources(1, &source);
    alcCaptureStart(device_in);

    printf("Starting\n");
    int16_t last = 0;
    while (1) {
        ALint samples;
        alcGetIntegerv(device_in, ALC_CAPTURE_SAMPLES, sizeof(samples), &samples);
        //printf("%u\n", samples);
        if(samples >= samples_perframe) {
            int16_t buf[samples_perframe];
            alcCaptureSamples(device_in, buf, samples_perframe);
            pass_audio_output(f_a, buf, samples_perframe);
	    if (filter && filter_audio(f_a, buf, samples_perframe) == -1) {
                printf("filter_audio fail\n");
                return 0;
            }

            buf[0] = (((unsigned long)buf[1] + (unsigned long)last) / (unsigned long)2);
            sourceplaybuffer(source, buf, samples_perframe, 1, sample_rate);
            last = buf[samples_perframe - 1];
        }

        usleep(1000);
    }

    return 0;
}

