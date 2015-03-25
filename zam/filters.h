#include <inttypes.h>
#include <math.h>

typedef struct {
	float x[3];
	float y[3];
	float a[3];
	float b[3];
} FilterStateZam;

void init_highpass_filter_zam(FilterStateZam *hpf, float fc, uint32_t fs);
void init_lowpass_filter_zam(FilterStateZam *lpf, float fc, uint32_t fs);
int run_filter_zam(FilterStateZam* fil, float* data, int length);
int run_saturator_zam(float *data, int length);
