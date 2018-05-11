
#include "fft.h"
#include "kiss_fft.h"


void* fft_alloc(int frame_size, int is_inverse) {
    kiss_fft_cfg cfg = kiss_fft_alloc(frame_size, is_inverse, 0,0);
    return cfg;
}

void fft_fft(void* cfg, const complex_float *in, complex_float *out) {
    kiss_fft((kiss_fft_cfg)cfg, (const kiss_fft_cpx*)in, (kiss_fft_cpx*) out);
}

