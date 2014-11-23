
#ifndef FILTER_AUDIO
#define FILTER_AUDIO

#include <stdint.h>

#ifndef _FILTER_AUDIO
typedef struct Filter_Audio Filter_Audio;
#endif


Filter_Audio *new_filter_audio(uint32_t fs);

void kill_filter_audio(Filter_Audio *f_a);

int filter_audio(Filter_Audio *f_a, int16_t *data, unsigned int samples);



#endif
