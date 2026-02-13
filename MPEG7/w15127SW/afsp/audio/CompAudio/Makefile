#
#	Makefile for CompAudio
#

PROG = CompAudio
OBJECTS = \
	CompAudio.o \
	CAcomp.o \
	CAcorr.o \
	CAstats.o \
	nucleus/CAoptions.o \
	nucleus/CAprcorr.o \
	nucleus/CAprstat.o \
	nucleus/CASNR.o \
	#

HEADERS = CompAudio.h

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_bindir = $(dist_dir)/bin
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
LDFLAGS = -s -lm

LLIBS = $(dist_libdir)/libAO.a $(dist_libdir)/libtsplite.a

.PHONY: clean

BIN = $(dist_bindir)/$(PROG)
$(BIN): $(OBJECTS) $(LLIBS)
	$(CC) $(OBJECTS) $(LLIBS) $(LDFLAGS) -o $(BIN)

$(OBJECTS): $(HEADERS)

mostlyclean:
	rm -f *.o */*.o

clean:	mostlyclean
	rm -f $(BIN)
