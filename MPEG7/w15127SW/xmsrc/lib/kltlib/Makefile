# Makefile for klt


# select one of the following CC CFLAGS settings

#
# GNU gcc
#
CC = g++
CFLAGS = -O2

all: libklt

libklt:
	cd klt; make 'CC=$(CC)' \
	'CFLAGS=$(CFLAGS) $(USE_DISP) $(USE_SHMEM) $(INCLUDEDIR)' \
	'LIBS=$(LIBS)' 'LIBRARYDIR=$(LIBRARYDIR)' 

pc:
	cd src/klt; make pc 'CC=$(CC)' 'CFLAGS=$(CFLAGS)'

clean:
	cd klt; make clean
