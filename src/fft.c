
#include "fft.h"


#ifndef USE_GPU_FFT

#include "kiss_fft.h"

void* fft_alloc(int frame_size, int is_inverse) {
    kiss_fft_cfg cfg = kiss_fft_alloc(frame_size, is_inverse, 0,0);
    return cfg;
}

void fft_fft(void* cfg, const complex_float *in, complex_float *out) {
    kiss_fft((kiss_fft_cfg)cfg, (const kiss_fft_cpx*)in, (kiss_fft_cpx*) out);
}

#else

#include "gpu_fft.h"
#include "mailbox.h"

void* fft_alloc(int frame_size, int is_inverse) {
  int ret;
  int mb;
  struct GPU_FFT *fft;

  if(frame_size != 2048) {
    fprintf(stderr, "unexpected FFT size: %d\n", frame_size);
    exit(1);
  }

  mb = mbox_open();
  if((ret = gpu_fft_prepare(mb, 11, is_inverse, 1, &fft)) < 0) {
    fprintf(stderr, "can't prepare GPU_FFT; error: %d\n", ret);
    exit(1);
  }  
  
  return fft;
}

void fft_fft(void* cfg, const complex_float *in, complex_float *out) {
  int ret;
  struct GPU_FFT *fft = (struct GPU_FFT *) cfg;

  struct GPU_FFT_COMPLEX* base = fft->in + 0*fft->step; // one job
  memcpy(base, in, 2048 * sizeof(struct GPU_FFT_COMPLEX));
  if((ret = gpu_fft_execute(fft)) <0) {
    fprintf(stderr, "can't execute GPU_FFT; error: %d\n", ret);
    exit(1);
  }
  base = fft->out + 0*fft->step; // one job
  memcpy(out, fft->out, 2048 * sizeof(struct GPU_FFT_COMPLEX));  
}

#endif
