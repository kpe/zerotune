# zeroTune - a pi zero turer

Could run on a [Pi Zero W][1] with a 
[ReSpeaker 2-Mics Pi HAT][2]
to capture 48kHz 2 channel audio and process it
with the [McLeod's WSNAC][3] pitch detection algorithm, 
thus implementing a low latency tuner and intonation
monitor.

*N.B.* it should run on your Linux too.

# How it works
Tha audio samples are captured using ALSA and than
processed with the SNAC algorithm in a tiny C backend.

Using the rawrtc implementation of the WebRTC DataChannel,
the pitch data could then be sent to a web application
for visualization.

The WSNAC implementation uses currently multiple FFTs
of size 2048 for processing an audio frame captured 
every 1024/48kHz seconds. 
Using the [kiss_fft][4] on the Zero Pi needs less than 40% CPU,
but this could be later optimized by using the [GPU_FFT][5] for
the Raspberry Pi.

# How to run it
To build and install (locally with a prefix under `./bulid/prefix`):

    ./make-dependencies.sh
	pushd build
	cmake -DCMAKE_INSTALL_PREFIX=${PWD}/prefix ..
	make install
	popd
	
To check if it works:	

	export LD_LIBRARY_PATH=${PWD}/prefix/lib/:${LD_LIBRARY_PATH}
	./build/prefix/bin/zerotune-test hw:1,0
	
	

# References

[1]: [Raspberry Pi Zero W](https://en.wikipedia.org/wiki/Raspberry_Pi#Pi_Zero)
[2]: [ReSpeaker 2-Mics Pi HAT](http://wiki.seeedstudio.com/ReSpeaker_2_Mics_Pi_HAT/)
[3]: McLeod, Philip - [Fast, Accurate Pitch Detection Tools for Music Analysis], 2008. (http://www.cs.otago.ac.nz/research/publications/oucs-2008-03.pdf)
[4]: [Kiss FFT](https://sourceforge.net/projects/kissfft/)
[5]: [Accelerating Fourier Transforms using the GPU](https://www.raspberrypi.org/blog/accelerating-fourier-transforms-using-the-gpu/), [gpu_fft](https://github.com/raspberrypi/firmware/blob/master/opt/vc/src/hello_pi/hello_fft/gpu_fft.txt)

