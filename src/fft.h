
#include "kiss_fft.h"

typedef struct {
    float r;
    float i;
}complex_float;


void* fft_alloc(int frame_size, int is_inverse);
void  fft_fft(void* cfg, const complex_float *in, complex_float *out);
