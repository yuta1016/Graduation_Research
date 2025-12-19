######################################################################
# Choose your favorite C compiler
#CC = gcc
#CC = g++

######################################################################
# -DNDEBUG prevents the assert() statements from being included in 
# the code.  If you are having problems running the code, you might 
# want to comment this line to see if an assert() statement fires.
FLAG1 = -DNDEBUG

######################################################################
# -DKLT_USE_QSORT forces the code to use the standard qsort() 
# routine.  Otherwise it will use a quicksort routine that takes
# advantage of our specific data structure to greatly reduce the
# running time on some machines.  Uncomment this line if for some
# reason you are unhappy with the special routine.
# FLAG2 = -DKLT_USE_QSORT

######################################################################
# Add your favorite C flags here.
# PK options
#OPT = -O3 -fstrict-aliasing -ffast-math -funroll-loops -march=pentiumpro -fschedule-insns2
#OPT = -m486 -O2 -g
#CFLAGS = $(FLAG1) $(FLAG2) $(OPT)


######################################################################
# There should be no need to modify anything below this line (but
# feel free to if you want).

ARCH = convolve.c error.c pnmio.c pyramid.c selectGoodFeatures.c \
       storeFeatures.c trackFeatures.c klt.c klt_util.c writeFeatures.c
LIB = -L/usr/local/lib -L/usr/lib

.SUFFIXES:  .c .o

#all:  lib $(EXAMPLES:.c=)
all:  lib 

.c.o:
	$(CC) -c $(CFLAGS) $<

lib: $(ARCH:.c=.o)
	\rm -f libklt.a
	ar ruv libklt.a $(ARCH:.c=.o)
	\rm -f *.o
	mv libklt.a ..

depend:
	makedepend $(ARCH) $(EXAMPLES)

clean:
	\rm -f *.o *.a *.tar *.tar.gz ../libklt.a

# DO NOT DELETE

convolve.o: convolve.c convolve.h klt.h klt_util.h
error.o: error.c
pnmio.o: pnmio.c error.h
pyramid.o: pyramid.c convolve.h klt.h klt_util.h pyramid.h
selectGoodFeatures.o: selectGoodFeatures.c base.h 
selectGoodFeatures.o: error.h convolve.h klt.h klt_util.h pyramid.h
storeFeatures.o: storeFeatures.c klt.h
trackFeatures.o: trackFeatures.c base.h error.h convolve.h
trackFeatures.o: klt.h klt_util.h pyramid.h
klt.o: klt.c pyramid.h pnmio.h klt.h klt_util.h base.h error.h
writeFeatures.o: writeFeatures.c pnmio.h klt.h
