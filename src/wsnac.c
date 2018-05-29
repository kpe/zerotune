#include "wsnac.h"
#include "fft.h"

#include <math.h>

#define PI 3.1415926535897932384626433832795028842


typedef struct {
    int   frame_size;
    void* fft;
    void* ifft;

    complex_float* fft_in;
    complex_float* fft_in_a;
    complex_float* fft_in_b;

    complex_float* fft_out;
    complex_float* fft_out_a;
    complex_float* fft_out_b;

    complex_float* fft_tmp;
    
    complex_float* wsnac_tmp;

    float* window;
}wsnac_handle;


void* wsnac_open(int frame_size) {
    int i;
    
    wsnac_handle* wsnac = (wsnac_handle*) calloc(1, sizeof(wsnac_handle));
    wsnac->frame_size = frame_size;

    wsnac->fft  = fft_alloc(frame_size<<1, 0);
    wsnac->ifft = fft_alloc(frame_size<<1, 1);
    
    wsnac->fft_in    = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));
    wsnac->fft_in_a  = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));
    wsnac->fft_in_b  = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));

    wsnac->fft_out   = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));
    wsnac->fft_out_a = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));
    wsnac->fft_out_b = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));

    wsnac->fft_tmp   = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));
    wsnac->wsnac_tmp = (complex_float*)calloc(frame_size<<1, sizeof(complex_float));

    wsnac->window    = (float*)calloc(frame_size, sizeof(float));

    for(i=0;i<frame_size;i++) {
        wsnac->window[i] = sin(PI * i/(wsnac->frame_size-1));
    }
    
    return wsnac;
}

void wsnac_close(void *wsnac_h) {
    wsnac_handle* wsnac = (wsnac_handle*)wsnac_h;
    free(wsnac->fft_in);
    free(wsnac->fft_in_a);
    free(wsnac->fft_in_b);

    free(wsnac->fft_out);
    free(wsnac->fft_out_a);
    free(wsnac->fft_out_b);

    free(wsnac->fft_tmp);
    free(wsnac->wsnac_tmp);

    free(wsnac->window);

    free(wsnac->ifft);
    free(wsnac->fft);
}

// calcs the autocorrelation of fft_in into fft_out
void wsnac_autocorr(const wsnac_handle *wsnac) {
    int i;
    // pad with zeros
    memset(wsnac->fft_in + wsnac->frame_size, 0, wsnac->frame_size*sizeof(complex_float));
    memcpy(wsnac->fft_out, wsnac->fft_in,        (wsnac->frame_size<<1)*sizeof(complex_float));
    
    // fft
    fft_fft(wsnac->fft, wsnac->fft_in, wsnac->fft_out);

    // to magnitude
    for(i=0; i< (wsnac->frame_size<<1); i++) {
        wsnac->fft_tmp[i].r =
            (wsnac->fft_out[i].r * wsnac->fft_out[i].r) +
            (wsnac->fft_out[i].i * wsnac->fft_out[i].i);
        wsnac->fft_tmp[i].i = 0;
    }

    // ifft
    fft_fft(wsnac->ifft, wsnac->fft_tmp, wsnac->fft_out);

    // to real only - clean imag
    for(i=0; i< wsnac->frame_size; i++) {
        wsnac->fft_out[i].i = 0;
//        wsnac->fft_out[i].r /= wsnac->frame_size<<1;
    }
}

// calcs the crosscorrelation of fft_in_a and fft_in_b into fft_out
//           destroys fft_out_a and fft_out_b
void wsnac_crosscorr(const wsnac_handle *wsnac) {
    int i;

    // pad
    memset(wsnac->fft_in_a + wsnac->frame_size, 0, wsnac->frame_size*sizeof(complex_float));
    memset(wsnac->fft_in_b + wsnac->frame_size, 0, wsnac->frame_size*sizeof(complex_float));

    // fft
    fft_fft(wsnac->fft, wsnac->fft_in_a, wsnac->fft_out_a);
    fft_fft(wsnac->fft, wsnac->fft_in_b, wsnac->fft_out_b);

    // multiply
    for(i=0; i< (wsnac->frame_size<<1); i++) {
        wsnac->fft_tmp[i].r =
            (wsnac->fft_out_a[i].r * wsnac->fft_out_b[i].r) +
            (wsnac->fft_out_a[i].i * wsnac->fft_out_b[i].i);
        wsnac->fft_tmp[i].i = 
            (- wsnac->fft_out_a[i].r * wsnac->fft_out_b[i].i) +
            (  wsnac->fft_out_a[i].i * wsnac->fft_out_b[i].r);
    }

    // ifft
    fft_fft(wsnac->ifft, wsnac->fft_tmp, wsnac->fft_out);
    
    // to real only - clean imag
    for(i=0; i< wsnac->frame_size; i++) {
        wsnac->fft_out[i].i = 0;
//        wsnac->fft_out[i].r /= wsnac->frame_size<<1;
    }
}

#define FFT_SWAP(A,B,TMP) {TMP=A;A=B;B=TMP;}

// calcs wsnac from fft_in into fft_out
//    destroys   in_a, in_b
void wsnac_wsnac(wsnac_handle *wsnac) {
    int i;
    float x;
    complex_float *tmp;

    // in = x*w; in_a = in; in_b = x*x*w
    for(i=0; i< (wsnac->frame_size<<1); i++){
        if (wsnac->frame_size <= i) {
            wsnac->fft_in_a[i].r = 0;
            wsnac->fft_in_a[i].i = 0;
            wsnac->fft_in_b[i].r = 0;
            wsnac->fft_in_b[i].i = 0;
            wsnac->fft_in[i].r = 0;
            wsnac->fft_in[i].i = 0;
            continue;
        }
        x = wsnac->fft_in[i].r;
        wsnac->fft_in_a[i].r = wsnac->window[i];
        wsnac->fft_in_a[i].i = 0;
        wsnac->fft_in_b[i].r = x * x * wsnac->window[i];
        wsnac->fft_in_b[i].i = 0;

        wsnac->fft_in[i].r = x * wsnac->window[i];
        wsnac->fft_in[i].i = 0;
    }

    wsnac_crosscorr(wsnac);                           // (in_a, in_b) -> out
	memcpy(wsnac->wsnac_tmp, wsnac->fft_out, wsnac->frame_size*sizeof(complex_float));
        
    FFT_SWAP(wsnac->fft_in_b, wsnac->fft_in_a, tmp);  // in_a <-> in_b
    wsnac_crosscorr(wsnac);                           // (in_a, in_b) -> out
    for(i=0;i<wsnac->frame_size;i++) {
		wsnac->wsnac_tmp[i].r += wsnac->fft_out[i].r;
	}

    wsnac_autocorr(wsnac);                            // in -> out

    for(i=0;i<wsnac->frame_size;i++){
        x = wsnac->wsnac_tmp[i].r;
        if (x>0 && x < 1e-12) {
            x = 1e-12;
        } else if(x<0 && (-x) < 1e-12)  {
            x = -1e-12;
        }
        wsnac->fft_out[i].r = 2*wsnac->fft_out[i].r/x;
    }
}

// calcs fft_in -> f0 and clarity
void wsnac_f0_and_clarity(void *wsnac_h, const float *in, int rate, float *f0, float *clarity) {
    //const float thresh = 0.3;
    const float slope_thresh_pos = 0.8;
    const float slope_thresh_neg = -0.7;
    
    int i;
    double x0,x1,x2, y0,y1,y2, a,b,c ,xx;
    int above;
    int pos,lpos,amax,xmax;
    float x, vmax;
    wsnac_handle *wsnac = (wsnac_handle *)wsnac_h;

    for(i = 0; i< wsnac->frame_size; i++){
        wsnac->fft_in[i].r = in[i];
        wsnac->fft_in[i].i = 0;
    }
    
    wsnac_wsnac(wsnac); // in -> out

    above = 0;
    pos = 1;   // positive
    amax = 0;
    vmax = 0;
    xmax = 0;
    for(i=0; i<wsnac->frame_size;i++){
        lpos = pos;
        x = wsnac->fft_out[i].r;

        // hystheresis
        if(pos) {
            if (x < slope_thresh_neg) {
                pos = 0;
            }
        } else if ( x >= slope_thresh_pos ) {
            pos = 1;
        }
        
        if(i<3) continue; // ignore first few samples

        x = wsnac->fft_out[i].r;
        if(above) {
            if(x>vmax) {
                amax = i;
                vmax = x;
            }
            if(lpos && !pos) {       // negative slope
                above = 0;
                xmax = amax;
                break;
                //if (vmax > thresh)
                //  break; // returning xmax as maximum
            }
        } else if( pos && !lpos ){  // positive slope
            above = 1;
            amax = i;
            vmax = x;
        }
        
    }

    
    // no peak found
    if ((xmax < 4) || (xmax > (wsnac->frame_size*1/3))) {
        *f0 = 0;
        *clarity = 0;
        return;
    }

    //fprintf(stdout,"max %d %.3f\n", xmax, vmax);

    // xmax -> freq
    
    x0 = xmax-1;
    x1 = xmax;
    x2 = xmax+1;
    y0 = wsnac->fft_out[xmax-1].r;
    y1 = wsnac->fft_out[xmax].r;
    y2 = wsnac->fft_out[xmax+1].r;
    
    // fit parabola a*x*x + b*x +c
    /*
      y0-y1 = a*(x0*x0 -x1*x1) + b*(x0-x1) = a*(x0*x0 -x1*x1) -b
      y2-y1 = a*(x2*x2 -x1*x1) + b*(x2-x1) = a*(x2*x2 -x1*x1) +b

      y0+y2 -2*y1 = a*(x0*x0 + x2*x2 -2*x1*x1)
     */

    a = ((y0+y2) - 2*y1)/((x0*x0 + x2*x2) -2*x1*x1);
    b = y2-y1 - a*(x2*x2-x1*x1);
    c = y1 - a*x1*x1 -b*x1;

    // max is at
    xx =  -b/(a*2);
    *f0  = (rate/xx);
    *clarity = (a*xx*xx + b*xx + c);
}
