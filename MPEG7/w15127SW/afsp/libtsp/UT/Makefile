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
	$(LIB)(UTdate.o) \
	$(LIB)(UTerror.o) \
	$(LIB)(UTfree.o) \
	$(LIB)(UTgetOption.o) \
	$(LIB)(UThalt.o) \
	$(LIB)(UTmalloc.o) \
	$(LIB)(UTrealloc.o) \
	$(LIB)(UTsetProg.o) \
	$(LIB)(UTsysMsg.o) \
	$(LIB)(UTwarn.o) \
	$(LIB)(nucleus/UTbyteCode.o) \
	$(LIB)(nucleus/UTbyteOrder.o) \
	$(LIB)(nucleus/UTcheckIEEE.o) \
	$(LIB)(nucleus/UTctime.o) \
	$(LIB)(nucleus/UTdIEEE80.o) \
	$(LIB)(nucleus/UTdecOption.o) \
	$(LIB)(nucleus/UTeIEEE80.o) \
	$(LIB)(nucleus/UTswapCode.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
