
#include <alsa/asoundlib.h>


void* capture_open(const char *dev, unsigned int* rate, unsigned int *channs, unsigned long *period_size);
void  capture_close(void *capture_handle);
int   capture_read(void *capture_handle, short *buf, int size);
