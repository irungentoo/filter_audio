#include <math.h>
#include <stdio.h>
#include "filters.h"

static inline float from_dB(float gdb) {
	return (exp(gdb/20.f*log(10.f)));
}

static void pushsample(Gate *gate, float sample) {
	gate->pos++;
	if (gate->pos >= MAX_GATE)
		gate->pos = 0;
	gate->samples[gate->pos] = sample;
}

static float averageabs(Gate *gate) {
	int i;
	float average = 0.f;

	for (i = 0; i < MAX_GATE; i++) {
		average += fabsf(gate->samples[i]);
	}
	average /= (float) MAX_GATE;
	return average;
}

void run_gate(Gate *gate, float *playsignal, float *micsignal, float *outsignal, int frames, float fs) {
	float absample;
	float attack;
	float release;
	int i;
	float gain;
	gain = gate->gain;

	attack = 1000.f / (0.05 * fs);
	release = 1000.f / (0.4 * fs);
	
	for(i = 0; i < frames; i++) {
		gain = sanitize_denormal(gain);
		pushsample(gate, playsignal[i]/32768.f);
		absample = averageabs(gate);
		if (absample > from_dB(-60.0)) {
			gain -= attack;
			if (gain < 0.f)
				gain = 0.f;
		} else {
			gain += release;
			if (gain > 1.f)
				gain = 1.f;
		}
		outsignal[i] = gain*micsignal[i];
		gate->gain = gain;
	}
}
