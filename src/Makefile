.PHONY: clean

CFLAGS  = -Wall -O0 -g
CFLAGS  = -Wall -O3
LDFLAGS = -lasound -lm

OBJ = calsa.o capture.o wsnac.c  fft.o  kiss_fft.o
HDR = $(OBJ:%.o=%.h)

all: calsa

.c.o:
	gcc $(CFLAGS) -c $<

calsa: $(OBJ)
	gcc $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o
