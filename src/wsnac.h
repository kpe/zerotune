
void* wsnac_open(int frame_size);
void  wsnac_close(void* wsnac);

void wsnac_f0_and_clarity(void *wsnac,
                          const float *in,   int rate, float *f0, float *clarity);

    
