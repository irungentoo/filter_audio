#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#define MAX_GATE 400

typedef struct {
	double x[3];
	double y[3];
	double a[3];
	double b[3];
} FilterStateZam;

typedef struct {
	float samples[MAX_GATE];
	float gain;
	int pos;
	float *playbuf;
} Gate;

void init_highpass_filter_zam(FilterStateZam *hpf, float fc, float fs);
void init_lowpass_filter_zam(FilterStateZam *lpf, float fc, float fs);
int run_filter_zam(FilterStateZam* fil, float* data, int length);
int run_saturator_zam(float *data, int length);
void run_gate(Gate *gate, float *playsignal, float *micsignal, float *outsignal, int frames, float fs);
void FloatToFloatS16(const float* src, size_t size, float* dest);
void FloatS16ToFloat(const float* src, size_t size, float* dest);
inline double sanitize_denormal(double v);
