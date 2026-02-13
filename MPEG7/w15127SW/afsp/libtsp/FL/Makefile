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
	$(LIB)(FLbackup.o) \
	$(LIB)(FLbaseName.o) \
	$(LIB)(FLdefName.o) \
	$(LIB)(FLdirName.o) \
	$(LIB)(FLextName.o) \
	$(LIB)(FLfileDate.o) \
	$(LIB)(FLfullName.o) \
	$(LIB)(FLgetLine.o) \
	$(LIB)(FLpathList.o) \
	$(LIB)(nucleus/FLdReadTF.o) \
	$(LIB)(nucleus/FLexist.o) \
	$(LIB)(nucleus/FLfReadTF.o) \
	$(LIB)(nucleus/FLfileSize.o) \
	$(LIB)(nucleus/FLjoinNames.o) \
	$(LIB)(nucleus/FLseekable.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o */*.o

clean: mostlyclean
