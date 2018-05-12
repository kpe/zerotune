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

void capture_read_t(void* p, short* buf, int size) {
    int i;
    double freq = 440/4;
    
    for(i=0;i<size;i++){
        buf[i]  = 0.5* sin(2*3.1415926 *1*freq*((double)gpos/grate)) * 32768;
        buf[i] += 0.30* sin(2*3.1415926*2*freq*((double)gpos/grate)) * 32768;
        buf[i] += 0.35* sin(2*3.1415926*3*freq*((double)gpos/grate)) * 32768;
        buf[i] += 0.35* sin(2*3.1415926*4*freq*((double)gpos/grate)) * 32768;
        //buf[i] += 0.30* sin(2*3.1415926*5*freq*((double)gpos/grate)) * 32768;
        buf[i] += 0.25* sin(2*3.1415926*6*freq*((double)gpos/grate)) * 32768;
        buf[i] += 0.20* sin(2*3.1415926*7*freq*((double)gpos/grate)) * 32768;
        
        gpos += 1;
        //fprintf(stdout,"%d\n",buf[i]);
    }
}

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

        //fprintf(stdout, "buf %d %d %d %d %d %d %d %d \n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

        samps_to_float(buf, FRAME_SIZE, chan, samps);

        //fprintf(stdout, " capt %.3f %.3f %.3f %.3f \n", samps[0], samps[1], samps[2], samps[3]);

        
        wsnac_f0_and_clarity(wsnac, samps, rate, &f0, &clar);

	ct = clock();

        if (f0 > 170 && clar > 1.5) {

            toneA440 = 12*log(f0/440.)/log(2);
	    tone = round(toneA440);

            fprintf(stdout, " %6.1f Hz at %4.2f %-2s%1d % 3.0f cents \n", f0, clar, 
		NOTE_NAMES[(8*12+tone)%12],
		(int)(4+floor((9+tone)/12)),
                (toneA440-tone)*100
		);

	    //fflush(stdout);

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
