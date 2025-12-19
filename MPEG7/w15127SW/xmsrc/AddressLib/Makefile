# Makefile for all

###############################################################################
DEBUG = n
IPROF = n
PROF = n
CHECKER = n
EVAL = n
SHARED = y
#select only one system!
LINUX = y
SOLARIS = n
SUNOS = n
DOS = n
MOMUSYSSIMPLE = 1
###############################################################################

CC = gcc
CCC = g++
LINK = gcc

ifeq ($(LINUX),y)
	SOLARIS=n
	SUNOS=n
	DOS=n
	SYSEXT=linux
endif
ifeq ($(SOLARIS),y)
	SUNOS=n
	DOS=n
	SYSEXT=solaris
endif
ifeq ($(SUNOS),y)
	DOS=n
	SYSEXT=sunos
endif
ifeq ($(DOS),y)
	SYSEXT=dos
endif


ADDRLIB = $(HOME)
ADDRLIBINCDIR = $(ADDRLIB)/include
ADDRLIBLIBDIR = $(ADDRLIB)/lib/$(SYSEXT)


## momusys data structure
ifeq ($(MOMUSYSSIMPLE),1)
# distributed with AddressLib
	MOMUSYS =$(ADDRLIB)
	MOMEXT =1
else
# Original MoMuSys Headers
	MOMUSYS =$(HOME)/momusys_vm/mom_baselib
	MOMEXT =0
endif

MOMUSYSINCS = -I$(MOMUSYS)/include -I$(MOMUSYS)/include/proto

## allow only one profiler at a time
ifeq ($(IPROF),y)
	SHARED=n
	PROF=n
	EVAL=n
	DEBUG=n
endif
ifeq ($(PROF),y)
	EVAL=n
	DEBUG=n
endif

ifeq ($(IPROF),y)
	CFLAGS += -a -g -DPROF -DIPRROF
	LFLAGS += -a -g
	ifeq ($(SOARIS),y)
		CFLAGS += -mv8
	endif
	PROFEXT=i
	CC = gcc-iprof
	CCC = g++-iprof
endif

ifeq ($(PROF),y)
	CFLAGS = -pg -g -DPROF
	LFLAGS += -pg -g
	ifeq ($(SOARIS),y)
		CFLAGS += -mv8
	endif
	PROFEXT=p
endif

ifeq ($(EVAL),y)
#	CFLAGS += -g -DEVAL -03
	CFLAGS += -DEVAL
#	LFLAGS += -g
	PROFEXT=eval
endif

ifeq ($(DEBUG),y)
	CFLAGS += -Wall -g -DDEBUG -DADDPROF
	LFLAGS += -WaLL -g
	DBGEXT=d
else
	CFLAGS += -Wall -O3
	ifeq ($(SOARIS),n)
		OPTIM += -mv8
	endif
	LFLAGS += -Wall
endif

ADDRLIBLIB = addrlib$(DBGEXT)$(MOMEXT)$(PROFEXT)

IADDRLIB = -I$(ADDRLIBINCDIR)
LADDRLIB = -L$(ADDRLIBLIBDIR) -l$(ADDRLIBLIB)

#######################################################################
#To be use until here in project Makefiles


ADDRLIBDOC = README License.txt example


ifeq ($(LINUX),y)
	SYSFLAGS = -DLINUX -DLITENDIAN -DBSD
endif

ifeq ($(SOLARIS),y)
	SYSFLAGS = -DSOLARIS -DBIGENDIAN -DSYSV
endif

ifeq ($(SUNOS),y)
	SYSFLAGS = -DSUNOS -DBIGENDIAN -DBSD
endif


ifeq ($(CHECKER),y)
#	CC = gcc
#	CCC = g++
#	LFLAGS += -lefence
#	CFLAGS += -fcheck-memory-usage
#	LINK = purify  -best-effort gcc
#	LINK = purify -suppress-filenames="libX11.so.4" gcc
	CC = insure
	CCC = insure
	LINK = gcc
	LFLAGS += -L$(PARASOFT)/lib.linux
endif

ifeq ($(DOS),n)
	ifeq ($(SHARED),y)
		CFLAGS += -fPIC
		ADDRLIBFILE = lib$(ADDRLIBLIB).so
	else
		ADDRLIBFILE = lib$(ADDRLIBLIB).a
	endif
else
	ADDRLIBFILE = $(ADDRLIBLIB).lib
endif




###############################################################################
ADDRLIBOBJS =  nhood.o vopio.o address.o \
	fifo.o lifo.o \
	intra.o inter.o
ifeq ($(EVAL),y)
	ADDRLIBOBJS += eval.o eval_functions.o
endif

###############################################################################


ifeq ($(SHARED),y)
	AR =gcc -shared -o
else
	AR = ar -r
endif
CFLAGS += -DANSI $(SYSFLAGS) $(MOMUSYSINCS) $(IADDRLIB)
CCFLAGS = $(CFLAGS)
#LFLAGS +=
#OUTPUT_OPTION=
#CPPFLAGS=
INSTALL = /bin/cp
RM = /bin/rm
CHMOD = chmod
MKDIR = mkdir -p
UMASK= 644

ifeq ($(LINUX),y)
	TAR = tar
else
	TAR = gtar
endif

###############################################################################
##default rules
#OUTPUT_OPTION=
#CPPFLAGS=
#COMPILE.c=$(CC) $(CFLAGS) $(CPPFLAGS) -c
#COMPILE.cc=$(CCC) $(CCFLAGS) $(CPPFLAGS)  -c
#.c.o:
#	 $(COMPILE.c) $(OUTPUT_OPTION) $<
#
#.cc.o:
#	 $(COMPILE.cc) $(OUTPUT_OPTION) $<
#
#.cpp.o:
#	 $(COMPILE.cc) $(OUTPUT_OPTION) $<
#
#

###############################################################################
###############################################################################
all : varlist $(ADDRLIBFILE)

debug : 
	make incinstall
	make clean
	make DEBUG=y SHARED=y install
	make clean
	make DEBUG=y SHARED=n install

nodebug:
	make incinstall
	make clean
	make DEBUG=n SHARED=y install
	make clean
	make DEBUG=n SHARED=n install

$(ADDRLIBFILE) : $(ADDRLIBOBJS)
	$(AR) $(ADDRLIBFILE) $(ADDRLIBOBJS)

varlist:
	@echo '*******************'
	@echo 'CC            : $(CC)'
	@echo 'DEBUG         : $(DEBUG)'
	@echo 'PURIFY        : $(PURIFY)'
	@echo 'PURE          : $(PURE)'
	@echo 'IPROF         : $(IPROF)'
	@echo 'CFLAGS        : $(CFLAGS)'
	@echo 'CCFLAGS       : $(CCFLAGS)'
	@echo 'INSTALL       : $(INSTALL)'
	@echo 'RM            : $(RM)'
	@echo 'CHMOD         : $(CHMOD)'
	@echo 'MKDIR         : $(MKDIR)'
	@echo 'UMASK         : $(UMASK)'
	@echo 'EVAL          : $(EVAL)'
	@echo 'SYSEXT        : $(SYSEXT)'
	@echo 'ADDRLIBLIBDIR : $(ADDRLIBLIBDIR)'
	@echo '*******************'
	@echo

install : $(ADDRLIBFILE)
	$(MKDIR) $(ADDRLIBLIBDIR)
	$(MKDIR) $(ADDRLIBINCDIR)
	$(INSTALL) $(ADDRLIBFILE) $(ADDRLIBLIBDIR)
	$(INSTALL) vopio.o $(ADDRLIBLIBDIR)
	$(INSTALL) *.h $(ADDRLIBINCDIR)
	$(CHMOD) $(UMASK) $(ADDRLIBINCDIR)/*.h

incinstall :
	$(MKDIR) $(ADDRLIBINCDIR)
	$(INSTALL) *.h $(ADDRLIBINCDIR)
	$(CHMOD) $(UMASK) $(ADDRLIBINCDIR)/*.h

clean:
	$(RM) -f *.a *.so $(ADDRLIBOBJS)


###############################################################################
sourcepkg:
	$(TAR) -czvf addrlibsrc.tgz *.c *.h Makefile $(ADDRLIBDOC)

libpkg: $(ADDRLIBFILE)
	$(TAR) -czvf addrliblib.tgz $(ADDRLIBFILE) vopio.o *.h

import:
	$(TAR) -czvf addrlibsrc.tgz.old *.c *.h Makefile
	$(TAR) -xzvf addrlibsrc.tgz

###############################################################################
## C-Sourcen
address.o : address.c address.h
vopio.o : vopio.c vopio.h \
	address.h

nhood.o : nhood.c nhood.h \
	address.h \
	vopio.h
inter.o : inter.c inter.h \
	nhood.h \
	address.h \
	vopio.h
intra.o : intra.c intra.h \
	nhood.h \
	address.h \
	vopio.h
lifo.o : lifo.c lifo.h \
	nhood.h \
	address.h \
	vopio.h
fifo.o : fifo.c fifo.h \
	nhood.h \
	address.h \
	vopio.h
eval.o : eval.c eval.h \
	eval_glbldefs.h

eval_functions.o : eval_functions.c eval_functions.h \
	eval.c eval.h
