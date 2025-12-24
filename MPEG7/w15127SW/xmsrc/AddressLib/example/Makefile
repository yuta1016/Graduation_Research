# Makefile for all

###############################################################################
DISPLAY = y
DEBUG = y
CHECKER = n
IPROF = n
PROF = n
EVAL = n
#select only one system!
LINUX = y
SOLARIS = n
SUNOS = n
###############################################################################


ifeq ($(LINUX),y)
	SOLARIS=n
	SUNOS=n
	SYSEXT=linux
endif
ifeq ($(SOLARIS),y)
	SUNOS=n
	SYSEXT=solaris
endif
ifeq ($(SUNOS),y)
	SYSEXT=sunos
endif


## ADDRESSLIBRARY #############################################################

ADDRLIB = $(HOME)
ADDRLIBINCDIR = $(ADDRLIB)/include
ADDRLIBLIBDIR = $(ADDRLIB)/lib.$(SYSEXT)
#ADDRLIBLIBDIR = $(HOME)/lib/$(SYSEXT)


## momusys data structure
MOMUSYSSIMPLE = 1
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

ifeq ($(EVAL),y)
	ADDRLIBLIB = addrlibeval$(MOMEXT)
else	
	ifeq ($(DEBUG),y)
		ADDRLIBLIB = addrlibd$(MOMEXT)
	else
		ADDRLIBLIB = addrlib$(MOMEXT)
	endif
endif

#WIN = /usr/openwin
#IWIN = -I$(WIN)/include

ifeq ($(EVAL),y)
	CFLAGS += -DEVAL
endif

IADDRLIB = -I$(ADDRLIBINCDIR)
LADDRLIB = -L$(ADDRLIBLIBDIR) -l$(ADDRLIBLIB)

## SEPECIFIC DEFINES #########################################################
ifeq ($(LINUX),y)
	BINDIR = $(HOME)/bin/linux
	SYSFLAGS = -DLINUX -DLITENDIAN -DBSD
endif

ifeq ($(SOLARIS),y)
	BINDIR = $(HOME)/bin/solaris
	SYSFLAGS = -DSOLARIS -DBIGENDIAN -DSYSV
endif

ifeq ($(SUNOS),y)
	BINDIR = $(HOME)/bin/sunos
	SYSFLAGS = -DSUNOS -DBIGENDIAN -DBSD
endif


## SWITCH SPECIFIC DEFINES ###################################################
ifeq ($(DISPLAY),y)
	DISPFLAG =
else
	DISPFLAG = -DNODISP
endif

ifeq ($(PROF),y)
	PROFOPT = -DPROF -pg
else
	PROFOPT =
endif

ifeq ($(IPROF),y)
	IPROFOPT = -a -DPROF
	DEBUGOPT = -g
	OPTIM = -O3
	ifeq ($(SOARIS),y)
		OPTIM += -mv8
	endif
else
	IPROFOPT =
	ifeq ($(DEBUG),y)
		DEBUGOPT = -g -Wall -DDEBUG
		OPTIM = 
	else
		DEBUGOPT =
		OPTIM = -O3
		ifeq ($(SOARIS),n)
			OPTIM += -mv8
		endif
	endif
endif

ifeq ($(CHECKER),y)
#	LFLAGS += -lefence
#	CFLAGS += -fcheck-memory-usage
	CHECKCMD = purify  -best-effort
#	CHECKCMD = purify -suppress-filenames="libX11.so.4"
else
	CHECKCMD =
endif

###############################################################################

EXAMOBJS = example.o

###############################################################################

#CC = gcc-2.7.2
CC = gcc
#CC = g++ -DCPP
CCC = g++ -DCPP
LINK = $(CHECKCMD) gcc
AR = ar -r
CFLAGS += $(IPROFOPT) -DANSI $(SYSFLAGS) $(DEBUGOPT) $(PROFOPT) $(OPTIM)\
	$(MOMUSYSINCS) $(IADDRLIB)
CCFLAGS = $(CFLAGS)
LFLAGS += $(DEBUGOPT) $(PROFOPT)
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
all : varlist example

example : $(EXAMOBJS)
	$(LINK) $(LFLAGS) -o example $(EXAMOBJS) \
	$(LADDRLIB)


clean :
	$(RM) -f example  $(EXAMOBJS)

varlist:
	@echo '*******************'
	@echo 'DISPLAY   : $(DISPLAY)'
	@echo 'PROF      : $(PROF)'
	@echo 'DEBUG     : $(DEBUG)'
	@echo 'PURIFY    : $(PURIFY)'
	@echo 'PURE      : $(PURE)'
	@echo 'IPROF     : $(IPROF)'
	@echo 'CC        : $(CC)'
	@echo 'CFLAGS    : $(CFLAGS)'
	@echo 'CCFLAGS   : $(CCFLAGS)'
	@echo 'LFLAGS    : $(LFLAGS)'
	@echo 'EVAL      : $(EVAL)'
	@echo '*******************'
	@echo

###############################################################################
## C-Sourcen

example.o : example.c example.h
