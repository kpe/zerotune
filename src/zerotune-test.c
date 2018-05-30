#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "capture.h"
#include "wsnac.h"


#define SAMPL_RATE  48000
#define N_CHANNELS  2  

#define FRAME_SIZE  1024


void samps_to_float(const short* buf, int samps, int channels, float* out) {
    int i;
    float samp;
    if(channels == 1) {
        for (i=0; i < samps; i++) {
            samp = (float)buf[i] / 32768.0;
            out[i] = samp;
        }
    } else if (channels == 2) {
        for (i=0; i < (samps<<1); i+=2) {
            samp = (float)((int)buf[i] + (int)buf[i+1]) / 65536.0;
            out[i>>1] = samp;
        }
    } else {
        fprintf(stderr, "illegal number of channels: %d\n", channels);
        exit(1);
    }
}

unsigned long gpos = 0;
unsigned long grate = 0;


const char* NOTE_NAMES[12]={"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};

int main(int argc, char* argv[]) {
    void* capture_handle;
    void* wsnac;

    clock_t t;
    clock_t lt=clock(),ct;

    float f0, clar;

    float toneA440;
    int   tone;
	

    float samps[FRAME_SIZE];

    unsigned int rate = SAMPL_RATE;
    unsigned int chan = N_CHANNELS;
    unsigned long period_size = FRAME_SIZE;
    
    short buf[FRAME_SIZE*N_CHANNELS];

    if (argc <= 1) {
        fprintf(stderr, "no capture device specified; try something like: %s hw:1,0\n", argv[0]);
        exit(1);
    }

    fprintf(stdout, "      opening: %s\n", argv[1]); fflush(stdout);
    capture_handle = capture_open(argv[1], &rate, &chan, &period_size);
    fprintf(stdout, "sampling rate: %d\n", rate);
    fprintf(stdout, "       format: S16_LE\n");
    fprintf(stdout, "     channels: %d\n", chan);
    fprintf(stdout, "   frame_size: %lu\n", period_size);

    grate = rate;

    wsnac = wsnac_open(FRAME_SIZE);
    for(;;){
        capture_read(capture_handle, buf, FRAME_SIZE);

	t = clock();

        samps_to_float(buf, FRAME_SIZE, chan, samps);
        
        wsnac_f0_and_clarity(wsnac, samps, rate, &f0, &clar);

	ct = clock();

        if (f0 > 140 && clar > 0.9) {

		toneA440 = 12*log(f0/440.)/log(2);
	    tone = round(toneA440);

		fprintf(stdout, " %6.1f Hz at %4.2f %-2s%1d % 3.0f cents \n", f0, clar, 
		NOTE_NAMES[(8*12+tone)%12],
		(int)(4+floor((9+tone)/12)),
                (toneA440-tone)*100
		);
	    fflush(stdout);

	    double time_taken = ((double)ct-t)/CLOCKS_PER_SEC; // in seconds
/*
            fprintf(stdout, " ...in %.3f ms; since last: %.3f; cap time:%.3f \n", 
		time_taken*1000., 
		((double)ct-lt)*1000./CLOCKS_PER_SEC,
		((double)lt-t)*1000./CLOCKS_PER_SEC
	    );
*/
        }
	lt = ct;
    }
    wsnac_close(wsnac);
        
    capture_close(capture_handle);
}
