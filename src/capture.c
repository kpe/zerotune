#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>


void* capture_open(const char* dev, unsigned int* rate, unsigned int* channs, unsigned long *period_size) {
    int err;
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;

    if ((err = snd_pcm_open (&capture_handle, dev, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf (stderr, "cannot open audio device %s (%s)\n", 
				 dev,
				 snd_strerror (err));
        exit (1);
    }
		   
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
				 
    if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
	
    if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
	
    if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
	
    if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
	
    if ((err = snd_pcm_hw_params_set_channels_near (capture_handle, hw_params, channs)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_hw_params_set_period_size (capture_handle, hw_params, *period_size,  0)) < 0) {
        fprintf (stderr, "cannot set channel period size (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
	
    if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
	

    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
				 snd_strerror (err));
        exit (1);
    }

    
    // getting actual format
    snd_pcm_format_t format = -1;
    if ((err = snd_pcm_hw_params_get_format (hw_params, &format)) < 0) {
        fprintf (stderr, "cannot get audio interface sample format (%s)\n",
				 snd_strerror (err));
        exit (1);

    }
    if(format != SND_PCM_FORMAT_S16_LE) {
        fprintf(stderr, "using unexpected sample format: %x\n",  format);
        exit(1);
    }

    if ((err = snd_pcm_hw_params_get_rate (hw_params, rate, 0)) < 0) {
        fprintf (stderr, "cannot get audio interface sampl rate (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
    if ((err = snd_pcm_hw_params_get_period_size (hw_params, period_size, 0)) < 0) {
        fprintf (stderr, "cannot get audio interface period size (%s)\n",
				 snd_strerror (err));
        exit (1);
    }
    
    
    snd_pcm_hw_params_free (hw_params);
    
    return capture_handle;
}

void capture_close(void* capture_handle) {
    snd_pcm_close((snd_pcm_t*)capture_handle);
}


void capture_read(void* capture_handle, short* buf, int size) {
    int err;
    if ((err = snd_pcm_readi ((snd_pcm_t*)capture_handle, buf, size)) != size) {
        fprintf (stderr, "read from audio interface failed (%s)\n",
                 snd_strerror (err));
        exit (1);
    }
}
