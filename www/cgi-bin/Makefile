CFLAGS=-g -Wall
CC=gcc
AR=ar
RANLIB=ranlib
LIBS=-L./ -lcgic

OBJECTS = pwm_on.cgi

all: libcgic.a $(OBJECTS)

libcgic.a: cgic.o cgic.h
		$(AR) rc libcgic.a cgic.o
			$(RANLIB) libcgic.a

%.cgi: %.o libcgic.a
		$(CC) $< -o $@ ${LIBS}

clean:
		del *.o *.a *.cgi

