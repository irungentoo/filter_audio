#include <inttypes.h>
#include <math.h>

typedef struct {
	double x[3];
	double y[3];
	double a[3];
	double b[3];
} FilterStateZam;

static inline double sanitize_denormal(double v)
{
	if(!isnormal(v))
		return 0.f;
	return v;
}

void init_highpass_filter_zam(FilterStateZam *hpf, float fc, float fs);
void init_lowpass_filter_zam(FilterStateZam *lpf, float fc, float fs);
int run_filter_zam(FilterStateZam* fil, float* data, int length);
int run_saturator_zam(float *data, int length);
void run_gate(float *gatestate, float *playsignal, float *micsignal, float *outsignal, int frames, float fs);
