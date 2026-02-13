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
	$(LIB)(STcatMax.o) \
	$(LIB)(STcopyMax.o) \
	$(LIB)(STcopyNMax.o) \
	$(LIB)(STdec1double.o) \
	$(LIB)(STdec1float.o) \
	$(LIB)(STdec1int.o) \
	$(LIB)(STdec1long.o) \
	$(LIB)(STdec1ulong.o) \
	$(LIB)(STdecDfrac.o) \
	$(LIB)(STdecIfrac.o) \
	$(LIB)(STdecLlimits.o) \
	$(LIB)(STdecNdouble.o) \
	$(LIB)(STdecNfloat.o) \
	$(LIB)(STdecSrange.o) \
	$(LIB)(STfindToken.o) \
	$(LIB)(STkeyMatch.o) \
	$(LIB)(STkeyXpar.o) \
	$(LIB)(STstrLC.o) \
	$(LIB)(STtrim.o) \
	$(LIB)(STunQuote.o) \
	$(LIB)(nucleus/STdec1val.o) \
	$(LIB)(nucleus/STdecNval.o) \
	$(LIB)(nucleus/STdecPair.o) \
	$(LIB)(nucleus/STdecSpair.o) \
	$(LIB)(nucleus/STstrDots.o) \
	$(LIB)(nucleus/STstrstrNM.o) \
	$(LIB)(nucleus/STtrimIws.o) \
	$(LIB)(nucleus/STtrimNMax.o) \
	$(LIB)(nucleus/STtrimTail.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
