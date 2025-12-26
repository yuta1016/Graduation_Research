#
# Makefile for libAO
#

SHELL = /bin/sh

dist_dir = ..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
ARFLAGS = rUv

LIB = $(dist_libdir)/libAO.a

.PHONY: mostlyclean clean

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AOdecFI.o) \
	$(LIB)(AOdecFO.o) \
	$(LIB)(AOdecHelp.o) \
	$(LIB)(AOdecOpt.o) \
	$(LIB)(AOnFrame.o) \
	$(LIB)(AOsetDFormat.o) \
	$(LIB)(AOsetFIopt.o) \
	$(LIB)(AOsetFOopt.o) \
	$(LIB)(AOstdin.o) \
	$(LIB)(nucleus/AOsetFtypeW.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
