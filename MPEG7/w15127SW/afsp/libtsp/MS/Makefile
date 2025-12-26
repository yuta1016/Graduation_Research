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
	$(LIB)(MSdNint.o) \
	$(LIB)(MSfGaussRand.o) \
	$(LIB)(MSfUnifRand.o) \
	$(LIB)(MSiCeil.o) \
	$(LIB)(MSratio.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
