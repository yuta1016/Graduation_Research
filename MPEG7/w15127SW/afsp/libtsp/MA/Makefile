#
# Makefile for libtsplite/XX
#

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
ARFLAGS = rUv

LIB = $(dist_libdir)/libtsplite.a

.PHONY: mostlyclean clean

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(MAdAllocMat.o) \
	$(LIB)(MAdFreeMat.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
