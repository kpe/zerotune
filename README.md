# zeroTune - a pi zero turer

Could run on a [Pi Zero W] with a 
[ReSpeaker 2-Mics Pi HAT]
to capture 48kHz 2 channel audio and process it
with the [McLeod's WSNAC] pitch detection algorithm, 
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
Using the [kiss_fft] on the Zero Pi needs less than 40% CPU,
and with [GPU_FFT] it comes down to 18% CPU.

# How to run it
First make sure you have CMake 3.2+ and the ALSA dev libs:

    sudo apt-get install cmake libasound2-dev

To build and install (locally with a prefix under `./bulid/prefix`):

	./make-dependencies.sh
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=${PWD}/prefix ..
	make install

*N.B.* You might want to

    sudo apt-get install libssl-dev
	
before doing `./make-dependencies.sh` on the Zero Pi, as compliling openssl from sources would take quite some time.

To check if it works:	

	export LD_LIBRARY_PATH=${PWD}/prefix/lib/:${LD_LIBRARY_PATH}
	./build/prefix/bin/zerotune-test hw:1,0

*N.B.* When using [GPU_FFT] you might have to run it with sudo.
	

# References
[Pi Zero W]: https://en.wikipedia.org/wiki/Raspberry_Pi#Pi_Zero
[ReSpeaker 2-Mics Pi HAT]: http://wiki.seeedstudio.com/ReSpeaker_2_Mics_Pi_HAT/
[McLeod's WSNAC]: http://www.cs.otago.ac.nz/research/publications/oucs-2008-03.pdf 
[kiss_fft]: https://sourceforge.net/projects/kissfft/
[GPU_FFT]: http://www.aholme.co.uk/GPU_FFT/Main.htm

[Raspberry Pi Zero W](https://en.wikipedia.org/wiki/Raspberry_Pi#Pi_Zero)

[ReSpeaker 2-Mics Pi HAT](http://wiki.seeedstudio.com/ReSpeaker_2_Mics_Pi_HAT/)

[McLeod, Philip - Fast, Accurate Pitch Detection Tools for Music Analysis, 2008.](http://www.cs.otago.ac.nz/research/publications/oucs-2008-03.pdf)

[Kiss FFT](https://sourceforge.net/projects/kissfft/)

[Accelerating Fourier Transforms using the GPU](https://www.raspberrypi.org/blog/accelerating-fourier-transforms-using-the-gpu/)

[hello_fft at RaspberryPi Firmware](https://github.com/raspberrypi/firmware/blob/master/opt/vc/src/hello_pi/hello_fft/gpu_fft.txt).


