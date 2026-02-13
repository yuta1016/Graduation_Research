#
#	Makefile for ResampAudio
#

PROG = ResampAudio
OBJECTS = \
	ResampAudio.o \
	RSresamp.o \
	nucleus/RSexpTime.o \
	nucleus/RSfiltSpec.o \
	nucleus/RSincTime.o \
	nucleus/RSinterp.o \
	nucleus/RSintFilt.o \
	nucleus/RSKaiserLPF.o \
	nucleus/RSoptions.o \
	nucleus/RSratio.o \
	nucleus/RSrefresh.o \
	nucleus/RSuKaiser.o \
	nucleus/RSwriteCof.o \
	#

HEADERS = ResampAudio.h

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
