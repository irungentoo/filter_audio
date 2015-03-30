#include <math.h>
#include <stdio.h>
#include "filters.h"

void run_gate(float *gatestate, float *playsignal, float *micsignal, float *outsignal, int frames, float fs) {
	float absample;
	float attack;
	float release;
	int i;
	float gain;
	gain = *gatestate;

	attack = 1000.f / (0.001 * fs);
	release = 1000.f / (0.8 * fs);
	
	for(i = 0; i < frames; i++) {
		gain = sanitize_denormal(gain);
		absample = fabsf(playsignal[i]);
		if (absample*absample/32768/32768 > 0.65) {
			gain -= release;
			if (gain < 0.f)
				gain = 0.f;
		} else {
			gain += attack;
			if (gain > 1.f)
				gain = 1.f;
		}
		outsignal[i] = gain*micsignal[i];
		*gatestate = gain;
		//printf("gain=%f absample^2=%f\n",gain, absample*absample/32768/32768);
	}
}
