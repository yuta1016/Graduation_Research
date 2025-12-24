SHELL=/bin/sh
###############################################################################
##
## This software module was originally developed by
##
## Munich University of Technology, Stephan Herrmann
## (contributing organizations names)
##
## and edited by
## Karol Wnukowicz (Visual Atoms)
## Stavros Paschalakis (Visual Atoms)
##
## in the course of develpment of the MPEG-7 Experimentation Model.
##
## This software module is an implementation of a part of one or more MPEG-7
## Experimentation Model tools as specified by the MPEG-7 Requirements.
##
## ISO#IEC gives users of MPEG-7 free license to this software module or
## modifications thereof for use in hardware or software products claiming
## conformance to MPEG-7.
##
## Those intending to use this software module in hardware or software products
## are advised that its use may infringe existing patents. The original
## developer of this software module and his#her company, the subsequent
## editors and their companies, and ISO#IEC have no liability for use of this
## software module or modifications thereof in an implementation.
##
## Copyright is not released for non MPEG-7 conforming products. The
## organizations named above retain full right to use the code for their own
## purpose, assign or donate the code to a third party and inhibit
## third parties the from using the code for non MPEG-7 conforming products.
##
## Copyright (c) 1998-2015.
##
## This notice must be included in all copies or derivative works.
##
## Makefile
###############################################################################
# Section one
DISPLAY=n
DEBUG=n
CHECKER=n
#CHECKER=PURIFY
#CHECKER=INSURE
#CHECKER=EFENCE
#CHECKER can be n=no; PURIFY=purify; INSURE=insure, EFENCE=electric fence
STATIC=n
XMLIBSHARED=y

# PK note: iprof can only be used with C programs (not C++ programs !)
IPROF=n
M2ENCIPROF=n

PROF=n
EVAL=n
USEAFSP=n
USEMAGICK=y
MAGICKCONFIG=y
USEXML=y
USEXML3=y
USEOPENCV=y
USEQHULL=y
USEMPEG2LIB=y

DISABLEVIDIO=n
DISABLEAUDIO=y
DISABLEMDS=n

#set language
#deutsch
#LANG=49
#francaise
#LANG=33
#espanol
#LANG=34
#default (engish)
LANG=

INSTALLDIR=/usr/local
#INSTALLDIR=/usr/local/XM
###############################################################################

# version no.
M7XMVERSION=$(shell cat Version)

# default system
#SYSTEM=SOLARIS
SYSTEM=LINUX

## MACHINE DEFINES ###########################################################
###  LINUX  ###
ifeq ($(SYSTEM),LINUX)
#	include machines/LINUX_options
	SYSEXT=linux
	ifeq ($(XMLIBSHARED),y)
		LIBEXT=.so
	else
		LIBEXT=.a
	endif
	CDEFS +=-DUNIX -DLINUX
endif

###  SUNOS  ###

ifeq ($(SYSTEM),SUNOS)
#	include machines/SOLARIS1_options
	SYSEXT=sunos
	ifeq ($(XMLIBSHARED),y)
		LIBEXT=.so
	else
		LIBEXT=.a
	endif
	CDEFS+=-DUNIX
endif

###  SOLARIS ###
ifeq ($(SYSTEM),SOLARIS)
#	include machines/SOLARIS2_options
	SYSEXT=solaris
	ifeq ($(XMLIBSHARED),y)
		LIBEXT=.so
	else
		LIBEXT=.a
	endif
	CDEFS +=-DUNIX
endif

####  HP   ###
#ifeq ($(SYSTEM),HP)
#   include machines/HP_options
#endif
#
####  SGI  ###
#ifeq ($(SYSTEM),SGI)
#   include machines/SGI_options
#endif
#
####  CONVEX  ###
#ifeq ($(SYSTEM),CONVEX)
#   include machines/CONVEX_options
#endif
#
####  DEC ALPHA  ###
#ifeq ($(SYSTEM),ALPHA)
#   include machines/ALPHA_options
#endif

## SWITCH SPECIFIC DEFINES ###################################################
ifeq ($(PROF),y)
	IPROF=n
	DEBUG=n
	CHECKER=n
	DISPLAY=n
	CDEFS +=-DPROF
	DBGEXT=_prof
	CFLAGS +=-pg
	SHLFLAGS +=-pg
endif

ifeq ($(IPROF),y)
	DEBUG=n
	CHECKER=n
	DISPLAY=n
	CFLAGS +=-ggdb -a
endif

ifeq ($(CHECKER),PURIFY)
	DEBUG=y
	LFLAGS +=-static
#	CFLAGS +=-fcheck-memory-usage
endif
ifeq ($(CHECKER),INSURE)
	DEBUG=y
endif
ifeq ($(CHECKER),EFENCE)
	DEBUG=y
	LFLAGS +=-lefence
endif

ifeq ($(DEBUG),y)
	CDEFS +=-DDEBUG -DADDPROF
	CFLAGS +=-ggdb -Wall -Wno-deprecated
	DBGEXT=_dbg
else
	CFLAGS +=-O3 -Wno-deprecated
	ifeq ($(SYSTEM),SOLARIS)
		CFLAGS +=-mv8
	endif
endif
ifeq ($(STATIC),y)
	LFLAGS +=-static
else
	SHAREDFLAGS=-fPIC
#	CFLAGS +=-D_DL_FLOAT_TEST
	LFLAGS +=-fPIC
endif

#Section two
# external packages
### X11 WINDOWS LIBS ##########################################################
ifeq ($(SYSTEM),SOLARIS)
	X11=/usr/openwin
endif
ifeq ($(SYSTEM),LINUX)
	X11=/usr/X11
endif
IX11=-I$(X11)/include
LX11=-L$(X11)/lib -lX11 -lXext

ifeq ($(DISPLAY),y)
	XV=/usr/openwin
	IXV=-I$(XV)/include
	LXV=-L$(XV)/lib -lxview -lolgx

	ifeq ($(DEBUG),y)
		 XYUVLIB=xyuv
	else
		 XYUVLIB=xyuv
	endif
	XYUV=$(HOME)
	XYUVINCDIR=$(XYUV)/include
	XYUVLIBDIR=$(XYUV)/lib/$(SYSEXT)
	IXYUV=-I$(XYUVINCDIR)
	LXYUV=-L$(XYUVLIBDIR) -l$(XYUVLIB)

	IWIN=$(IX11) $(IXV) $(IXYUV)
	LWIN=$(LX11) $(LXV) $(LXYUV)
	DWIN =
else
	IWIN =
	LWIN =
	DWIN=-DNODISP
endif

### Magick ###################################################################
ifeq ($(USEMAGICK),y)
	ifeq ($(MAGICKCONFIG),y)
		IMAGICK=`Magick-config --cflags --cppflags`
		LMAGICK=`Magick-config --ldflags --libs`
		DMAGICK=
	else
		MAGICK=/usr/X11
		IMAGICK=-I$(MAGICK)/include/X11 $(IX11)
		LMAGICK=-L$(MAGICK)/lib -lMagick $(LX11) \
		-L/usr/local/lib \
		-ljbig -ldf -ltiff -lttf -ljpeg -lpng -ldpstk -ldps -lbz2 -lz
#-ljbig -ldf -ltiff -lttf -ljpeg -lpng -ldpstk -ldps -lXext \
#-lX11 -lsocket -lnsl -lbz2 -lz -lm
	endif
	ifeq ($(STATIC),y)
		LMAGICK +=-ldl
	endif
else
	IMAGICK=
	LMAGICK=
	DMAGICK=-DRANDOMIMAGE
endif

### AFSP ###################################################################
ifeq ($(USEAFSP),y)
#	ifeq ($(SYSTEM),LINUX)
		AFSP=../afsp
		IAFSP=-I$(AFSP)/include $(IX11)
		LAFSP=-L$(AFSP)/lib -lAO -ltsp
		DAFSP=-DAFLIB
#	 else
#	 endif
else
	IAFSP=
	LAFSP=
	DAFSP=-DRNDOMAUDIO
endif

### XML ######################################################################
ifeq ($(USEXML),y)
#	XML=../xml
	XML=/usr/local
	lXML=-lxerces-c
#	lXML +=-licu-uc
	IXML=-I$(XML)/include
	LXML=-L$(XML)/lib $(lXML)
# -lm  -lpthread  -L/usr/lib -L/usr/local/lib -L/usr/ccs/lib -lm -lw -lgen

#	set version of parser
ifeq ($(USEXML3),y)
	DXML=-DUSEXML -DUSEXML3
else
	DXML=-DUSEXML
endif
#	DXML=-DUSEXML -DXM_XERCES_VERSION=0x0160
	ifeq ($(STATIC),y)
		LXML +=
	endif
	XMLSET=
else
	IXML=
	LXML=
	DXML=
	#XMLSET+=_noXML
	XMLSET=
	DISABLEMDS=y
endif
#ifeq ($(CHECKER),y)
#	 IXML=
#	 LXML=
#	 DXML=
#endif

### OPENCV ###################################################################
ifeq ($(USEOPENCV),y)
	OPENCV=/usr/lib/
	#lOPENCV=-lcv -lcvaux -lcxcore
	lOPENCV=-lopencv_core -lopencv_imgproc
	IOPENCV=-I/usr/include/opencv
	LOPENCV=-L$(OPENCV) $(lOPENCV)
else
	lOPENCV=
	LOPENCV=
	LOPENCV=
endif 

### QHULL ###################################################################
ifeq ($(USEQHULL),y)
	lQHULL=-lqhull
	IQHULL=/usr/include
	LQHULL=-L/usr/lib $(lQHULL)
else
	lQHULL=
	IQHULL=
	LQHULL=
endif 

### MPEG2LIB ###################################################################
ifeq ($(USEMPEG2LIB),y)
	lMPEG2LIB=-lmpeg2dec
	LMPEG2LIB=-LDecoders/MPEG2Dec/src/mpeg2dec -lmpeg2dec
	MPEG2LIB=Decoders/MPEG2Dec/src/mpeg2dec/libmpeg2dec.a
else
	lMPEG2LIB=
	LMPEG2LIB=
endif 

### KLT ######################################################################
#To be removed a.s.a.p.
KLT=lib/kltlib
IKLT=-I$(KLT)
LKLT=-L$(KLT) -lklt


### Collect Settings##########################################################


#EXTRA_INCLUDE_DIR +=$(IMAGICK) $(IAFSP) $(IWIN) $(IXML) $(IKLT)
EXTRA_INCLUDE_DIR += $(IAFSP) $(IWIN) $(IXML) $(IOPENCV) $(IKLT) 
EXTRA_LIBS +=-lm $(LMAGICK) $(LAFSP) $(LWIN) $(LXML) $(LOPENCV) $(LMPEG2LIB) $(LKLT) $(LQHULL)
EXTRA_CDEFS +=-DSH_MEM $(DMAGICK) $(DAFSP) $(DWIN) $(DXML)
EXTRA_CFLAGS +=
#SETTINGS=$(SYSEXT)$(XMLSET)$(DBGEXT)$(M7XMVERSION)
SETTINGS=$(SYSEXT)$(XMLSET)$(DBGEXT)

# PK iprof complexity analysis of the MPEG-2 Reencoder (you need a modified gcc)
ifeq ($(M2ENCIPROF),y)
	M2ENCIPROFOPT=-a -DIPROF
	DEBUGOPT=-ggdb -DVERBOSE
	OPTIM=-O3
	IPROFCC=/home/soft/modgcc2723/lib/bin/gcc
	ifeq ($(SYSTEM),SOLARIS)
		OPTIM +=-mv8
		IPROFCC=/usr6/tmp/pkuhn/modgcc2723/lib/bin/gcc
	endif
else
	ifeq ($(DEBUG),y)
		M2ENCIPROFOPT=-ggdb
	else
		M2ENCIPROFOPT=-O3
	endif
	IPROFCC=gcc
endif

#Section three
## COMMANDS ##################################################################

STDCC=g++
STDCCC=g++
#STDCCC=g++ -DCPP
#other compilers for other OSs
#ifeq ($(SYSTEM),SOLARIS)
#	 STDCC=cc
#	 STDCCC=CC
#endif
STDLINK=g++
INDENT=indent -br -i2 -l80 -lc80

CC=$(STDCC)
CCC=$(STDCCC)
LINK=$(STDLINK)
ifeq ($(CHECKER),PURIFY)
	LINK=purify  -best-effort -g++ $(STDLINK)
##	LINK=purify -suppress-filenames="libX11.so.4" $(STDCCC)
##	CHECKCMD=purify $(STDCCC)
endif
ifeq ($(CHECKER),INSURE)
	CC=insure $(STDCC)
	CCC=insure $(STDCCC)
	LINK=insure $(STDLINK)
endif

ifeq ($(XMLIBSHARED),y)
	#AR=gcc -shared Wl,-soname=$@ -Wl,--no-undefined -o $@ $^
	AR=gcc -shared $(SHLFLAGS) -o $@ $^
else
	AR=ar -rs $@ $?
endif
MKDIR=/bin/mkdir -p
INSTALL=/bin/cp
RM=/bin/rm -f
CHMOD=chmod
UMASK=644
TAR=gtar
CVS=cvs -d :pserver:cvs@cvshost.lis.e-technik.tu-muenchen.de:/home/mpeg7-xm
MAKEDEPEND=makedepend
SYSINC=-I/usr/include -I/usr/include/linux -I/usr/include/g++ -I/usr/include/g++/backwardi -I/usr/lib/gcc-lib/i586-suse-linux/3.3.1/include -I/usr/include/g++/i586-suse-linux
#Section four
## MODULES ####################################################################

#ENABLE MODULES
#DummyType
DTINCL = n
#DummyTypeDS
DTDSINCL = n

ifneq ($(DISABLEVIDEO),y)
#ColorSpace
CSINCL = y
#ColorQuantizer
CQINCL = y
#GridLayout
GLINCL = y
#MultiView
2D3DINCL = y
#Spatial2DCoordinates
S2DINCL = y
#TemporalInterpolation
TIINCL = y
#TimeSeries
TSINCL =y


#ColorLayout
CLINCL = y
#ColorStrcuture
CTINCL = y
#ColorTemperature * client needs to be fixed * conflict with IIColor
CTBINCL = y
#DominantColor
DCINCL = y
#Dominant Color Temperature
DCTINCL = y
#GroupOfFramesHistogram
GFINCL = y
#GroupOfFramesFeature
GPFINCL = y
#IIColor
IICINCL= y
#ScalableColor
SCCINCL = y

#EdgeHistogram
EHINCL = y
#HomogeneousTexture
HTINCL = y
#TextureBrowsing
TBINCL = y

#3dShapeSpectrum
3DSSINCL = y
#ContourShape
SCINCL = y
#ImageSignature
ISINCL = y
#Perceptual3DShape
P3DSINCL = y
#RegionLocator
RLINCL = y
#RegionShape
SRINCL = y
#SpatioTemporalLocator
STLINCL = y
#ShapeVariation
SVINCL = y
#VideoSignature
VSINCL = y

#CameraMotion
CMINCL = y
#MotionActivity
MAINCL = y
#MotionTrajectory
MTINCL = y
#ParametricObjectMotion
POMINCL = y

#FaceRecognition
FRINCL = y
#AdvancedFaceRecognition
AFRINCL = y

#MultiImageFeature
MIINCL = n
endif #DISABLEVIDEO

ifneq ($(DISABLEMDS),y)
#AffectiveSegmentFilter
ASFINCL = y
#DependStructure *broken (Pattaya) GCC 2.95.2
DPSINCL =n
#ExtendedTextualType
ETTINCL = y
#GraphicalCS  Win32 broken
GCSINCL = y
#HierarchicalSummary
HSINCL = y
#ImportanceHint
IMHINCL = y
#InkSegmentDS
INKINCL = y
#MatchingHint
MHINCL = y
#MediaMetaDS
MMINCL = y
#MediaReview
MRINCL = y
#ModelDS
MOINCL = y
#MosaicDS
MKINCL = y
#MediaQuality
MQINCL = y
#OrderingKey *broken
ODKINCL =n
#Package
PACKINCL = y
#PhoneticTranslationHints
PTHINCL = y
#PointOfView
POVINCL = y
#SegmentDS
SGINCL =n
#SegmentRelationGraph
SRGINCL = y
#Semantic *broken
SEMINCL =n
#SeqSummaryFastMovingStoryBoard
SMSINCL = y
#SeqSummaryKeywords
SKWINCL = y
#SequentialSummary
SSZINCL = y
#SoundProperty
SPINCL = n
#SpaceFrequency
SFINCL = y
#SpatialResolutionHint
SRHINCL = y
#StateMember
STMINCL = y
#StateTransition
STXINCL = y
#StructuredCollection
STCINCL = y
#Summarization
SZINCL = y
#SummaryPreference
SPRINCL = y
#Transcoding Hints DS
THINCL = y
#Time
TMINCL = y
#UserPreference
UPINCL = y
#Variation
VAINCL = y
#VideoEditin
VEINCL = y
#VideoText *compiles only on windows
VTINCL =n
endif #DISABLEMDS

ifneq ($(DISABLEAUDIO),y)
#
# MDS modules implemented by audio
#

#TextualDescription
#TDINCL =y
#NameComponent
#NCINCL =y
#PersonName
#PNINCL =y
#Person
#PINCL =y
#Individual
#IINCL =y
#Probability vector
PVINCL = y
#Probability matrix
PMINCL = y

#
# Audio modules
#
#SpokenContentExtractionInfo
SPCINCL = y

#AudioSpectralFlatness
SFMINCL = y
#AudioSignature
ASTINCL = y
#MelodySequence
MSQINCL = y

# disable all DDL based Modules if no DDL support
#ifeq ($(USEXML),n)
#	 SZINCL=n
#	 MKINCL=n
#endif

endif #DISABLEAUDIO



#SUB-DESCRIPTORS OF DESCRIPTOR

#TEST IF DEPENDENCIES BREAK OTHER CODE
BREAKTEST=n
ifeq ($(BREAKTEST),y)
ifeq ($(CSINCL),y)
SCCINCL=y
DCINCL=y
MKINCL=y
SZINCL=y
endif
ifeq ($(CQINCL),y)
DCINCL=y
endif
ifeq ($(DCTINCL),y)
DCINCL=y
CTBINCL=y
endif
ifeq ($(SCINCL),y)
MIINCL=y
endif
ifeq ($(SCCINCL),y)
MIINCL=y
#GFINCL=y
endif
ifeq ($(TIINCL),y)
MTINCL=y
STLINCL=y
endif
ifeq ($(RLINCL),y)
STLINCL=y
endif
ifeq ($(POMINCL),y)
MKINCL=y
endif
ifeq ($(CLINCL),y)
TSINCL=n
endif
ifeq ($(MMINCL),y)
ETTINCL=y
PTHINCL=y
endif
endif #BREAKTEST


ifeq ($(MIINCL),y)
#CLINCL=y
#CTINCL=y
#DCINCL=y
#GFINCL=y
SCCINCL=y
#EHINCL=y
#HTINCL=y
#TBINCL=y
SCINCL=y
#SRINCL=y
#3DSSINCL=y
endif
ifeq ($(TSINCL),y)
CLINCL=y
endif
ifeq ($(SCCINCL),y)
CSINCL=y
endif
ifeq ($(GFINCL),y)
SCCINCL=y
endif
ifeq ($(MTINCL),y)
TIINCL=y
endif
ifeq ($(STLINCL),y)
RLINCL=y
TIINCL=y
endif
ifeq ($(DCINCL),y)
CSINCL=y
CQINCL=y
endif
ifeq ($(ETTINCL),y)
MMINCL=y
endif
ifeq ($(MKINCL),y)
POMINCL=y
CSINCL=y
endif
ifeq ($(PTHINCL),y)
MMINCL=y
endif
#ifeq ($(VEINCL),y)
#SZINCL=y
#endif
ifeq ($(SZINCL),y)
CSINCL=y
KLTINCL=y
endif
ifeq ($(SMSINCL),y)
CSINCL=y
KLTINCL=y
endif
ifeq ($(SKWINCL),y)
CSINCL=y
KLTINCL=y
endif
ifeq ($(USEOPENCV),n)
ISINCL=n
P3DSINCL=n
VSINCL=n
endif

#
# Audio dependencies
#

#SpokenContent needs SpokenContentLattice and SpokenContentHeader
ifeq ($(SPCINCL),y)
IVINCL=y
PVINCL=y
PMINCL=y
TDINCL=y
PINCL=y
PNINCL=y
NCINCL=y
TDINCL=y
endif

#Pathnames#
ADDRDIR=AddressLib
APPLDIR=Applications
BASDIR=BasicArchitecture
CSDIR=CodingSchemes
DDLDIR=DDL
DECDIR=Decoders
DSDIR=DescriptionSchemes
DDIR=Descriptors
ENCDIR=Encoders
EVADIR=EvaluationTools
EXTDIR=ExtractionUtilities
MEDDIR=Media
PARDIR=ParsingUtilities
SERDIR=SearchUtilities
SYTDIR=SystemTools
UTLDIR=Utilities
XMADIR=XMManager
MAINDIR=$(XMADIR)

# general components
BASOBJS=$(DDLDIR)/RegisteredDDLs.o\
	$(DECDIR)/RegisteredDecoders.o \
	$(DECDIR)/DecoderFileIO.o \
	$(ENCDIR)/RegisteredEncoders.o \
	$(ENCDIR)/EncoderFileIO.o \
	$(EVADIR)/RegisteredEvalTools.o \
	$(EVADIR)/EvaluationTools.o \
	$(MEDDIR)/MediaIO.o \
	$(MEDDIR)/WrlFileUtil.o \
	$(PARDIR)/RegisteredParsers.o\
	$(SERDIR)/MatchList.o \
	$(SYTDIR)/RegisteredSystems.o\
	$(XMADIR)/ConnectionControl.o

#	$(XMADIR)/Version1.o
#	$(UTLDIR)/Streams.o \

# general components application
APPOBJS=$(APPLDIR)/RegisteredClientApplications.o \
	$(APPLDIR)/RegisteredServerApplications.o \
	$(XMADIR)/ComponentDB.o \
	$(XMADIR)/Version1.o \
	$(XMADIR)/Version2.o \
	$(XMADIR)/Version3.o \
	$(XMADIR)/Version4.o


# general components required for module library
MCOMOBJS=$(ADDRDIR)/vopio.o \
	$(ADDRDIR)/address.o \
	$(ADDRDIR)/nhood.o \
	$(ADDRDIR)/fifo.o \
	$(ADDRDIR)/lifo.o \
	$(ADDRDIR)/intra.o \
	$(ADDRDIR)/inter.o \
	$(BASDIR)/XMABCInterfaces.o \
	$(BASDIR)/XMABCObjects.o \
	$(BASDIR)/XMInterfaces.o \
	$(BASDIR)/XMObjects.o \
	$(CSDIR)/RegisteredCodingSchemes.o \
	$(CSDIR)/GenericDSCS.o \
	$(DDLDIR)/XML4Cutils.o \
	$(DSDIR)/RegisteredDescriptionSchemes.o \
	$(DSDIR)/GenericDS.o \
	$(DSDIR)/VectorR.o \
	$(DDIR)/RegisteredDescriptors.o \
	$(EXTDIR)/RegisteredExtractionUtilities.o \
	$(MEDDIR)/MultiMedia.o \
	$(MEDDIR)/AudioFrame.o \
	$(SERDIR)/RegisteredSearchTools.o \
	$(UTLDIR)/BitBuffer.o \
	$(UTLDIR)/DoubleArray.o \
	$(UTLDIR)/DoubleArrayMatrix.o \
	$(UTLDIR)/DoubleVector.o \
	$(UTLDIR)/LinkedList.o \
	$(UTLDIR)/readparam.o \
	$(UTLDIR)/UUIDs.o

ifeq (Utilities/lismsmaster.h, $(wildcard Utilities/lismsmaster.h))
CFLAGS +=-DLISMS
BASOBJS +=$(UTLDIR)/lismsmaster.o
endif

ifeq ($(KLTINCL),y)
MCOMOBJS +=$(EXTDIR)/KLTFeaturePoints.o
endif

APPLDEF=

#DummyType
ifeq ($(DTINCL),y)
APPOBJS +=$(APPLDIR)/DummyTypeCAppl.o \
	$(APPLDIR)/DummyTypeSAppl.o
VIDOBJS +=$(CSDIR)/DummyTypeCS.o \
	$(DDIR)/DummyType.o \
	$(EXTDIR)/DummyTypeExtraction.o \
	$(SERDIR)/DummyTypeSearch.o
XMDEFS +=-DUSEDT
APPLDEF +=-DUSEDT
MODULES +=DummyType
endif

#DummyType
ifeq ($(DTDSINCL),y)
APPOBJS +=$(APPLDIR)/DSDummyTypeCAppl.o \
	$(APPLDIR)/DSDummyTypeSAppl.o
MDSOBJS +=$(EXTDIR)/DSDummyTypeExtraction.o \
	$(SERDIR)/DSDummyTypeSearch.o
XMDEFS +=-DUSEDTDS
APPLDEF +=-DUSEDTDS
MODULES +=DSDummyType
endif

#VISUAL Ds########################################################
#color space
ifeq ($(CSINCL),y)
VIDOBJS +=$(CSDIR)/ColorSpaceCS.o \
	$(DDIR)/ColorSpace.o \
	$(EXTDIR)/ColorSpaceExtraction.o
APPLDEF +=-DUSECS
MODULES +=colorspace
endif


#color qunatizer
ifeq ($(CQINCL),y)
VIDOBJS +=$(CSDIR)/ColorQuantCS.o \
	$(DDIR)/ColorQuant.o \
#	$(EXTDIR)/ColorQuantExtraction.o
APPLDEF +=-DUSECQ
MODULES +=colorquantizer
endif

##scalable histogram
#ifeq ($(SCHINCL),y)
#VIDOBJS +=$(CSDIR)/ScalableHistogramCS.o \
#	$(DDIR)/ScalableHistogram.o \
#	$(EXTDIR)/ScalableHistogramExtraction.o
#APPLDEF +=-DUSESCH
#MODULES +=scalablehistogram
#endif

#grid layout
ifeq ($(GLINCL),y)
APPOBJS += $(APPLDIR)/GridLayoutSAppl.o
VIDOBJS +=$(CSDIR)/GridLayoutCS.o \
	$(DDIR)/GridLayout.o \
	$(EXTDIR)/GridLayoutExtraction.o
APPLDEF +=-DUSEGL
MODULES +=gridlayout
endif

#IIColor
ifeq ($(IICINCL),y)
APPOBJS += $(APPLDIR)/IIColorSAppl.o
VIDOBJS +=$(CSDIR)/IIColorCS.o \
	$(DDIR)/IIColor.o \
	$(EXTDIR)/IIColorExtraction.o \
	$(EXTDIR)/IIColor_Extraction.o
APPLDEF += -DUSEIIC
MODULES += IIColor
endif
#	$(APPLDIR)/IIColorCAppl.o \
#	$(SERDIR)/IIColorSearch.o

#multiple view
ifeq ($(2D3DINCL),y)
APPOBJS +=$(APPLDIR)/MultiViewCAppl.o \
	$(APPLDIR)/MultiViewSAppl.o
VIDOBJS +=$(CSDIR)/MultiViewCS.o \
	$(DDIR)/MultiView.o \
	$(EXTDIR)/MultiViewExtraction.o \
	$(SERDIR)/MultiViewSearch.o
APPLDEF +=-DUSE2D3D
MODULES +=MultiView
endif

#Spatial2DCoordinates
ifeq ($(S2DINCL),y)
VIDOBJS +=$(CSDIR)/Spatial2DCoordinatesCS.o \
	$(DDIR)/Spatial2DCoordinates.o
APPLDEF +=-DUSES2D
MODULES +=Spatial2DCoordinates
endif

#TemporalInterpolation
ifeq ($(TIINCL),y)
VIDOBJS +=$(CSDIR)/TemporalInterpolationCS.o \
	$(DDIR)/TemporalInterpolation.o \
	$(EXTDIR)/TemporalInterpolationExtraction.o \
	$(EXTDIR)/temporal_interpolation_extraction.o
APPLDEF +=-DUSETI
MODULES +=TemporalInterpolation
endif

#time series
ifeq ($(TSINCL),y)
APPOBJS +=$(APPLDIR)/TimeSeriesCAppl.o \
	$(APPLDIR)/TimeSeriesSAppl.o
VIDOBJS +=$(CSDIR)/TimeSeriesCS.o \
	$(DDIR)/TimeSeries.o \
	$(EXTDIR)/TimeSeriesExtraction.o \
	$(SERDIR)/TimeSeriesSearch.o
APPLDEF +=-DUSETS
MODULES +=timeseries
endif

#color layout
ifeq ($(CLINCL),y)
APPOBJS +=$(APPLDIR)/ColorLayoutCAppl.o \
	$(APPLDIR)/ColorLayoutSAppl.o
VIDOBJS +=$(CSDIR)/ColorLayoutCS.o \
	$(DDIR)/ColorLayout.o \
	$(EXTDIR)/ColorLayoutExtraction.o \
	$(SERDIR)/ColorLayoutSearch.o
APPLDEF +=-DUSECL
MODULES +=colorlayout
endif

#color strcuture histogram
ifeq ($(CTINCL),y)
APPOBJS +=$(APPLDIR)/ColorStructureCAppl.o \
	$(APPLDIR)/ColorStructureSAppl.o
VIDOBJS +=$(CSDIR)/ColorStructureCS.o \
	$(DDIR)/ColorStructure.o \
	$(EXTDIR)/ColorStructureExtraction.o \
	$(SERDIR)/ColorStructureSearch.o
APPLDEF +=-DUSECT
MODULES +=colorstructure
endif

#color temperature
ifeq ($(CTBINCL),y)
APPOBJS +=$ \
	$(APPLDIR)/CTBrowsingSAppl.o \
	$(APPLDIR)/CTBrowsingCAppl.o
VIDOBJS +=$(CSDIR)/CTBrowsingCS.o \
	$(DDIR)/CTBrowsing.o \
	$(EXTDIR)/CTBrowsingExtraction.o \
	$(EXTDIR)/CTBrowsing_Extraction.o \
	$(SERDIR)/CTBrowsingSearch.o
APPLDEF +=-DUSECTB
MODULES +=colortemperature
endif

#dominant color
ifeq ($(DCINCL),y)
APPOBJS +=$(APPLDIR)/DominantColorCAppl.o \
	$(APPLDIR)/DominantColorSAppl.o
VIDOBJS +=$(CSDIR)/DominantColorCS.o \
	$(DDIR)/DominantColor.o \
	$(EXTDIR)/DominantColorExtraction.o \
	$(EXTDIR)/imgutil.o \
	$(SERDIR)/DominantColorSearch.o
APPLDEF +=-DUSEDC
MODULES +=dominatcolor
endif

#dominant color temperature
ifeq ($(DCTINCL),y)
APPOBJS +=$(APPLDIR)/DominantColorTemperatureCAppl.o
VIDOBJS +=$(SERDIR)/DominantColorTemperatureSearch.o
APPLDEF +=-DUSED_CT
MODULES +=dominatcolortemperature
endif

#groupe of frames histogram
ifeq ($(GFINCL),y)
APPOBJS +=$(APPLDIR)/GoFGoPColorCAppl.o \
	$(APPLDIR)/GoFGoPColorSAppl.o
VIDOBJS +=$(CSDIR)/GoFGoPColorCS.o \
	$(DDIR)/GoFGoPColor.o \
	$(EXTDIR)/GoFGoPColorExtraction.o \
	$(SERDIR)/GoFGoPColorSearch.o
APPLDEF +=-DUSEGF
MODULES +=gofhistogram
endif

#groupe of frames histogram
ifeq ($(GPINCL),y)
#APPOBJS +=$(APPLDIR)/GoFGoPColorCAppl.o \
#	$(APPLDIR)/GoFGoPColorSAppl.o
VIDOBJS +=$(CSDIR)/GofGopFeatureCS.o \
	$(DDIR)/GoFGopFeature.o #\
#	$(EXTDIR)/GoFGoPColorExtraction.o \
#	$(SERDIR)/GoFGoPColorSearch.o
APPLDEF +=-DUSEGP
MODULES +=goffeature
endif

#scalable color
ifeq ($(SCCINCL),y)
APPOBJS +=$(APPLDIR)/ScalableColorCAppl.o \
	$(APPLDIR)/ScalableColorSAppl.o
VIDOBJS +=$(CSDIR)/ScalableColorCS.o \
	$(DDIR)/ScalableColor.o \
	$(EXTDIR)/ScalableColorExtraction.o \
	$(SERDIR)/ScalableColorSearch.o
APPLDEF +=-DUSESCC
MODULES +=scalablecolor
endif

#homogeneous texture
ifeq ($(HTINCL),y)
APPOBJS +=$(APPLDIR)/HomoTextureCAppl.o \
	$(APPLDIR)/HomoTextureSAppl.o
VIDOBJS +=$(CSDIR)/HomoTextureCS.o \
	$(DDIR)/HomoTexture.o \
	$(EXTDIR)/HomoTextureExtraction.o \
	$(EXTDIR)/homo_texture_extractor.o \
	$(SERDIR)/HomoTextureSearch.o
APPLDEF +=-DUSEHT
MODULES +=homogeneoustexture
endif

#edge histogram
ifeq ($(EHINCL),y)
APPOBJS +=$(APPLDIR)/EdgeHistCAppl.o \
	$(APPLDIR)/EdgeHistSAppl.o
VIDOBJS +=$(CSDIR)/EdgeHistCS.o \
	$(DDIR)/EdgeHist.o \
	$(EXTDIR)/EdgeHistExtraction.o \
	$(SERDIR)/EdgeHistSearch.o
APPLDEF +=-DUSEEH
MODULES +=edgehistogram
endif

#texture browsing
ifeq ($(TBINCL),y)
APPOBJS +=$(APPLDIR)/TextureBrowsingCAppl.o \
	$(APPLDIR)/TextureBrowsingSAppl.o
VIDOBJS +=$(CSDIR)/TextureBrowsingCS.o \
	$(DDIR)/TextureBrowsing.o \
	$(EXTDIR)/TextureBrowsingExtraction.o \
	$(EXTDIR)/texture_browsing_extraction.o \
	$(SERDIR)/TextureBrowsingSearch.o
APPLDEF +=-DUSETB
MODULES +=texturebrowsing
endif


ifeq ($(3DSSINCL),y)
APPOBJS +=$(APPLDIR)/3DShapeSpectrumSAppl.o \
	$(APPLDIR)/3DShapeSpectrumCAppl.o
VIDOBJS +=$(DDIR)/3DShapeSpectrum.o \
	$(CSDIR)/3DShapeSpectrumCS.o \
	$(EXTDIR)/3DShapeSpectrumExtraction.o \
	$(EXTDIR)/3d_shape_spectrum_extraction.o \
	$(SERDIR)/3DShapeSpectrumSearch.o 
APPLDEF +=-DUSE3DSS
MODULES +=3DShapeSpectrum
endif

##bounding box
#ifeq ($(BBINCL),y)
#APPOBJS +=$(APPLDIR)/BBoxCAppl.o \
#	$(APPLDIR)/BBoxSAppl.o
#VIDOBJS +=$(CSDIR)/BBoxCS.o \
#	$(DDIR)/BBox.o \
#	$(EXTDIR)/BBoxExtraction.o \
#	$(SERDIR)/BBoxSearch.o
#APPLDEF +=-DUSEBB
#MODULES +=boundingbox
#endif

#image signature
ifeq ($(ISINCL),y)
APPOBJS +=$(APPLDIR)/ImageSignatureCAppl.o \
	$(APPLDIR)/ImageSignatureSAppl.o
VIDOBJS +=$(CSDIR)/ImageSignatureCS.o \
	$(DDIR)/ImageSignature.o \
	$(SERDIR)/ImageSignatureSearch.o \
	$(EXTDIR)/ImageSignatureExtraction.o
APPLDEF +=-DUSEIS
MODULES +=imagesignature
endif

#contour shape
ifeq ($(SCINCL),y)
APPOBJS +=$(APPLDIR)/ContourShapeCAppl.o \
	$(APPLDIR)/ContourShapeSAppl.o
VIDOBJS +=$(CSDIR)/ContourShapeCS.o \
	$(DDIR)/ContourShape.o \
	$(EXTDIR)/ContourShapeExtraction.o \
	$(SERDIR)/ContourShapeSearch.o
APPLDEF +=-DUSESC
MODULES +=contourshape
endif

#perceptual 3D shape
ifeq ($(P3DSINCL),y)
APPOBJS +=$(APPLDIR)/Perceptual3DShapeCAppl.o \
	$(APPLDIR)/Perceptual3DShapeSAppl.o
VIDOBJS +=$(CSDIR)/Perceptual3DShapeCS.o \
	$(DDIR)/Perceptual3DShape.o \
	$(DDIR)/Perceptual3DShape_Aux.o \
	$(EXTDIR)/Perceptual3DShapeExtraction.o \
	$(EXTDIR)/3d_shape_spectrum_extraction.o \
	$(SERDIR)/Perceptual3DShapeSearch.o
APPLDEF +=-DUSEP3DS
MODULES +=perceptual3Dshape
endif

#region shape
ifeq ($(SRINCL),y)
APPOBJS +=$(APPLDIR)/RegionShapeCAppl.o \
	$(APPLDIR)/RegionShapeSAppl.o
VIDOBJS +=$(CSDIR)/RegionShapeCS.o \
	$(DDIR)/RegionShape.o \
	$(EXTDIR)/RegionShapeExtraction.o \
	$(EXTDIR)/imgutil.o \
	$(SERDIR)/RegionShapeSearch.o
APPLDEF +=-DUSESR
MODULES +=regionshape
endif

#region locator
ifeq ($(RLINCL),y)
APPOBJS +=$(APPLDIR)/RegionLocatorCAppl.o \
	$(APPLDIR)/RegionLocatorSAppl.o
VIDOBJS +=$(CSDIR)/RegionLocatorCS.o \
	$(DDIR)/RegionLocator.o \
	$(EXTDIR)/RegionLocatorExtraction.o
# \
#	$(SERDIR)/RegionLocatorSearch.o
APPLDEF +=-DUSERL
MODULES +=regionlocator
endif

#spatio temporal locater
ifeq ($(STLINCL),y)
APPOBJS +=$(APPLDIR)/SpatioTemporalLocatorCAppl.o \
	$(APPLDIR)/SpatioTemporalLocatorSAppl.o
VIDOBJS +=$(CSDIR)/SpatioTemporalLocatorCS.o \
	$(DDIR)/SpatioTemporalLocator.o \
	$(EXTDIR)/SpatioTemporalLocatorExtraction.o \
	$(EXTDIR)/spatio_temporal_locator_extraction.o \
	$(SERDIR)/SpatioTemporalLocatorSearch.o
APPLDEF +=-DUSESTL
MODULES +=spatiotemporallocator
endif

#shape variation
ifeq ($(SVINCL),y)
APPOBJS +=$(APPLDIR)/ShapeVariationCAppl.o \
	$(APPLDIR)/ShapeVariationSAppl.o
VIDOBJS +=$(CSDIR)/ShapeVariationCS.o \
	$(DDIR)/ShapeVariation.o \
	$(EXTDIR)/ShapeVariationExtraction.o \
	$(SERDIR)/ShapeVariationSearch.o
APPLDEF +=-DUSESV
MODULES +=shapevariation
endif

#image signature
ifeq ($(VSINCL),y)
APPOBJS +=$(APPLDIR)/VideoSignatureCAppl.o \
	$(APPLDIR)/VideoSignatureSAppl.o
VIDOBJS +=$(CSDIR)/VideoSignatureCS.o \
	$(DDIR)/VideoSignature.o \
	$(SERDIR)/VideoSignatureSearch.o \
	$(EXTDIR)/VideoSignatureExtraction.o
APPLDEF +=-DUSEVS
MODULES +=videosignature
endif

#camera motion
ifeq ($(CMINCL),y)
APPOBJS += $(APPLDIR)/CameraMotionCAppl.o \
	$(APPLDIR)/CameraMotionSAppl.o
VIDOBJS +=$(CSDIR)/CameraMotionCS.o \
	$(DDIR)/CameraMotion.o \
	$(DDIR)/media_time_tool.o \
	$(EXTDIR)/CameraMotionExtraction.o \
	$(EXTDIR)/camera_motion_extraction.o \
	$(SERDIR)/CameraMotionSearch.o
APPLDEF +=-DUSECM
MODULES +=cameramotion
endif


#motion activity
ifeq ($(MAINCL),y)
APPOBJS += $(APPLDIR)/MotionActivityCAppl.o \
	$(APPLDIR)/MotionActivitySAppl.o
VIDOBJS +=$(CSDIR)/MotionActivityCS.o \
	$(DDIR)/MotionActivity.o \
	$(EXTDIR)/MotionActivityExtraction.o \
	$(EXTDIR)/motion_activity_extraction.o \
	$(EXTDIR)/motion_activity_categorization.o \
	$(SERDIR)/MotionActivitySearch.o
APPLDEF +=-DUSEMA
MODULES +=motionactivity
endif

#motion trajectory
ifeq ($(MTINCL),y)
APPOBJS += $(APPLDIR)/MotionTrajectoryCAppl.o \
	$(APPLDIR)/MotionTrajectorySAppl.o
VIDOBJS +=$(CSDIR)/MotionTrajectoryCS.o \
	$(DDIR)/MotionTrajectory.o \
	$(EXTDIR)/MotionTrajectoryExtraction.o \
	$(SERDIR)/MotionTrajectorySearch.o
APPLDEF +=-DUSEMT
MODULES +=objectmotiontrajectory
endif

#parametric objetc motion
ifeq ($(POMINCL),y)
APPOBJS +=$(APPLDIR)/ParametricObjectMotionCAppl.o \
	$(APPLDIR)/ParametricObjectMotionSAppl.o
VIDOBJS +=$(CSDIR)/ParametricObjectMotionCS.o \
	$(DDIR)/ParametricObjectMotion.o \
	$(EXTDIR)/ParametricObjectMotionExtraction.o \
	$(EXTDIR)/gme_for_iso.o \
	$(EXTDIR)/ParametricObjectMotionExtraction2.o \
	$(EXTDIR)/coortrans.o \
	$(SERDIR)/ParametricObjectMotionSearch.o \
	$(SERDIR)/ParametricObjectMotionUtilities.o
APPLDEF +=-DUSEPOM
MODULES +=parametricobjectmotion
endif

#face recognition
ifeq ($(FRINCL),y)
APPOBJS +=$(APPLDIR)/FaceRecognitionCAppl.o \
	 $(APPLDIR)/FaceRecognitionSAppl.o
VIDOBJS +=$(CSDIR)/FaceRecognitionCS.o \
	$(DDIR)/FaceRecognition.o \
	$(EXTDIR)/FaceRecognitionExtraction.o \
	$(EXTDIR)/face_detect.o \
	$(SERDIR)/FaceRecognitionSearch.o
APPLDEF +=-DUSEFR
MODULES +=facerecognition
endif

#advanced face recognition
ifeq ($(AFRINCL),y)
APPOBJS +=$(APPLDIR)/AdvancedFaceRecognitionCAppl.o \
	$(APPLDIR)/AdvancedFaceRecognitionSAppl.o
VIDOBJS +=$(CSDIR)/AdvancedFaceRecognitionCS.o \
	$(DDIR)/AdvancedFaceRecognition.o \
	$(EXTDIR)/AdvancedFaceRecognitionExtraction.o \
	$(SERDIR)/AdvancedFaceRecognitionSearch.o
APPLDEF +=-DUSEAFR
MODULES +=advancedfacerecognition
endif

#AUDIO#########################################################

#SpokenContent
ifeq ($(SPCINCL),y)
APPOBJS +=$(APPLDIR)/SpokenContentCAppl.o \
	$(APPLDIR)/SpokenContentSAppl.o
AUDOBJS +=$(CSDIR)/SpokenContentCS.o \
	$(DDIR)/SpokenContent.o \
	$(EXTDIR)/SpokenContentExtraction.o \
	$(SERDIR)/SpokenContentSearch.o \
	$(CSDIR)/SpokenContentLatticeCS.o \
	$(DDIR)/SpokenContentLattice.o \
	$(EXTDIR)/SpokenContentLatticeExtraction.o \
	$(CSDIR)/SpokenContentBlockCS.o \
	$(DDIR)/SpokenContentBlock.o \
	$(EXTDIR)/SpokenContentBlockExtraction.o \
	$(CSDIR)/SpokenContentNodeCS.o \
	$(DDIR)/SpokenContentNode.o \
	$(EXTDIR)/SpokenContentNodeExtraction.o \
	$(CSDIR)/SpokenContentHeaderCS.o \
	$(DDIR)/SpokenContentHeader.o \
	$(EXTDIR)/SpokenContentHeaderExtraction.o \
	$(CSDIR)/SpokenContentSpeakerCS.o \
	$(DDIR)/SpokenContentSpeaker.o \
	$(EXTDIR)/SpokenContentSpeakerExtraction.o \
	$(CSDIR)/SpokenContentWordIndexEntryCS.o \
	$(DDIR)/SpokenContentWordIndexEntry.o \
	$(EXTDIR)/SpokenContentWordIndexEntryExtraction.o \
	$(CSDIR)/SpokenContentPhoneIndexEntryCS.o \
	$(DDIR)/SpokenContentPhoneIndexEntry.o \
	$(EXTDIR)/SpokenContentPhoneIndexEntryExtraction.o \
	$(CSDIR)/SpokenContentPhoneLexiconCS.o \
	$(DDIR)/SpokenContentPhoneLexicon.o \
	$(EXTDIR)/SpokenContentPhoneLexiconExtraction.o \
	$(CSDIR)/SpokenContentConfusionInfoCS.o \
	$(DDIR)/SpokenContentConfusionInfo.o \
	$(EXTDIR)/SpokenContentConfusionInfoExtraction.o \
	$(CSDIR)/SpokenContentPhoneLinkCS.o \
	$(DDIR)/SpokenContentPhoneLink.o \
	$(EXTDIR)/SpokenContentPhoneLinkExtraction.o \
	$(CSDIR)/SpokenContentWordLinkCS.o \
	$(DDIR)/SpokenContentWordLink.o \
	$(EXTDIR)/SpokenContentWordLinkExtraction.o \
	$(CSDIR)/SpokenContentLinkCS.o \
	$(DDIR)/SpokenContentLink.o \
	$(EXTDIR)/SpokenContentLinkExtraction.o \
	$(CSDIR)/SpokenContentWordLexiconCS.o \
	$(DDIR)/SpokenContentWordLexicon.o \
	$(EXTDIR)/SpokenContentWordLexiconExtraction.o \
	$(CSDIR)/SpokenContentLexiconCS.o \
	$(DDIR)/SpokenContentLexicon.o \
	$(EXTDIR)/SpokenContentLexiconExtraction.o \
	$(CSDIR)/SpokenContentIndexEntryCS.o \
	$(DDIR)/SpokenContentIndexEntry.o \
	$(EXTDIR)/SpokenContentIndexEntryExtraction.o \
	$(CSDIR)/ProbabilityVectorCS.o \
	$(DDIR)/ProbabilityVector.o \
	$(EXTDIR)/ProbabilityVectorExtraction.o \
	$(CSDIR)/ProbabilityMatrixCS.o \
	$(DDIR)/ProbabilityMatrix.o \
	$(EXTDIR)/ProbabilityMatrixExtraction.o

#	$(SERDIR)/SpokenContentNodeSearch.o
#	$(APPLDIR)/SpokenContentNodeCAppl.o \
#	$(APPLDIR)/SpokenContentNodeSAppl.o \
#	$(SERDIR)/SpokenContentHeaderSearch.o
#	$(APPLDIR)/SpokenContentHeaderCAppl.o \
#	$(APPLDIR)/SpokenContentHeaderSAppl.o \
#	$(SERDIR)/SpokenContentSpeakerSearch.o
#	$(APPLDIR)/SpokenContentSpeakerCAppl.o \
#	$(APPLDIR)/SpokenContentSpeakerSAppl.o \
#	$(SERDIR)/SpokenContentWordIndexEntrySearch.o
#	$(APPLDIR)/SpokenContentWordIndexEntryCAppl.o \
#	$(APPLDIR)/SpokenContentWordIndexEntrySAppl.o \
#	$(SERDIR)/SpokenContentPhoneIndexEntrySearch.o
#	$(APPLDIR)/SpokenContentPhoneIndexEntryCAppl.o \
#	$(APPLDIR)/SpokenContentPhoneIndexEntrySAppl.o \
#	$(SERDIR)/SpokenContentPhoneLexiconSearch.o
#	$(APPLDIR)/SpokenContentPhoneLexiconCAppl.o \
#	$(APPLDIR)/SpokenContentPhoneLexiconSAppl.o \
#	$(SERDIR)/SpokenContentConfusionInfoSearch.o
#	$(APPLDIR)/SpokenContentConfusionInfoCAppl.o \
#	$(APPLDIR)/SpokenContentConfusionInfoSAppl.o \
#	$(SERDIR)/SpokenContentPhoneLinkSearch.o
#	$(APPLDIR)/SpokenContentPhoneLinkCAppl.o \
#	$(APPLDIR)/SpokenContentPhoneLinkSAppl.o \
#	$(SERDIR)/SpokenContentWordLinkSearch.o
#	$(APPLDIR)/SpokenContentWordLinkCAppl.o \
#	$(APPLDIR)/SpokenContentWordLinkSAppl.o \
#	$(SERDIR)/SpokenContentLinkSearch.o
#	$(APPLDIR)/SpokenContentLinkCAppl.o \
#	$(APPLDIR)/SpokenContentLinkSAppl.o \
#	$(SERDIR)/SpokenContentWordLexiconSearch.o
#	$(APPLDIR)/SpokenContentWordLexiconCAppl.o \
#	$(APPLDIR)/SpokenContentWordLexiconSAppl.o \
#	$(SERDIR)/SpokenContentLexiconSearch.o
#	$(APPLDIR)/SpokenContentLexiconCAppl.o \
#	$(APPLDIR)/SpokenContentLexiconSAppl.o \
#	$(CSDIR)/SpokenContentExtractionInfoCS.o \
#	$(DDIR)/SpokenContentExtractionInfo.o \
#	$(EXTDIR)/SpokenContentExtractionInfoExtraction.o \
#	$(SERDIR)/SpokenContentExtractionInfoSearch.o
#	$(APPLDIR)/SpokenContentExtractionInfoCAppl.o \
#	$(APPLDIR)/SpokenContentExtractionInfoSAppl.o \
#	$(SERDIR)/SpokenContentLatticeSearch.o
#	$(APPLDIR)/SpokenContentLatticeCAppl.o \
#	$(APPLDIR)/SpokenContentLatticeSAppl.o \
#	$(SERDIR)/SpokenContentBlockSearch.o
#	$(APPLDIR)/SpokenContentBlockCAppl.o \
#	$(APPLDIR)/SpokenContentBlockSAppl.o \
#	$(SERDIR)/SpokenContentIndexEntrySearch.o
#	$(APPLDIR)/SpokenContentIndexEntryCAppl.o \
#	$(APPLDIR)/SpokenContentIndexEntrySAppl.o \
CCFLAGS +=-DUSESPC
MODULES +=SpokenContent
endif

#AudioSpectralFlatness
ifeq ($(SFMINCL),y)
APPOBJS +=$(APPLDIR)/AudioSpectralFlatnessCAppl.o
AUDOBJS +=$(SERDIR)/AudioSpectralFlatnessSearch.o
APPLDEF +=-DUSESFM
MODULES +=AudioSpectralFlatness
endif

#AudioSignature
ifeq ($(ASTINCL),y)
APPOBJS +=$(APPLDIR)/AudioSignatureCAppl.o
AUDOBJS +=$(SERDIR)/AudioSignatureSearch.o
APPLDEF +=-DUSEAST
MODULES +=AudioSignature
endif

#MelodySequence
ifeq ($(MSQINCL),y)
APPOBJS +=$(APPLDIR)/MelodySequenceCAppl.o
AUDOBJS +=$(SERDIR)/MelodySequenceSearch.o
APPLDEF +=-DUSEMSQ
MODULES +=MelodySequence
endif

#MULTIFEATURE##################################################
#MultiImage
ifeq ($(MIINCL),y)
APPOBJS +=$(APPLDIR)/MultiImageCAppl.o \
	$(APPLDIR)/MultiImageSAppl.o
VIDOBJS +=$(CSDIR)/MultiImageCS.o \
	$(DDIR)/MultiImage.o \
	$(EXTDIR)/MultiImageExtraction.o \
	$(SERDIR)/MultiImageSearch.o
APPLDEF +=-DUSEMI
MODULES +=MultiImage
endif

#DS############################################################

#TextualDescription
#ifeq ($(TDINCL),y)
#APPOBJS +=$(APPLDIR)/TextualDescriptionCAppl.o \
#	$(APPLDIR)/TextualDescriptionSAppl.o
#MDSOBJS +=$(CSDIR)/TextualDescriptionCS.o \
#	$(DDIR)/TextualDescription.o \
#	$(EXTDIR)/TextualDescriptionExtraction.o \
#	$(SERDIR)/TextualDescriptionSearch.o
#CCFLAGS +=-DUSETD
#MODULES +=TextualDescription
#endif

#NameComponent
#ifeq ($(NCINCL),y)
#APPOBJS +=$(APPLDIR)/NameComponentCAppl.o \
#	$(APPLDIR)/NameComponentSAppl.o
#MDSOBJS +=$(CSDIR)/NameComponentCS.o \
#	$(DDIR)/NameComponent.o \
#	$(EXTDIR)/NameComponentExtraction.o \
#	$(SERDIR)/NameComponentSearch.o
#CCFLAGS +=-DUSENC
#MODULES +=NameComponent
#endif

#PersonName
#ifeq ($(PNINCL),y)
#APPOBJS +=$(APPLDIR)/PersonNameCAppl.o \
#	$(APPLDIR)/PersonNameSAppl.o
#MDSOBJS +=$(CSDIR)/PersonNameCS.o \
#	$(DDIR)/PersonName.o \
#	$(EXTDIR)/PersonNameExtraction.o \
#	$(SERDIR)/PersonNameSearch.o
#CCFLAGS +=-DUSEPN
#MODULES +=PersonName
#endif

#Person
#ifeq ($(PINCL),y)
#APPOBJS +=$(APPLDIR)/PersonCAppl.o \
#	$(APPLDIR)/PersonSAppl.o
#MDSOBJS +=$(CSDIR)/PersonCS.o \
#	$(DDIR)/Person.o \
#	$(EXTDIR)/PersonExtraction.o \
#	$(SERDIR)/PersonSearch.o
#CCFLAGS +=-DUSEP
#MODULES +=Person
#endif

#Individual
#ifeq ($(IINCL),y)
#APPOBJS +=$(APPLDIR)/IndividualCAppl.o \
#	$(APPLDIR)/IndividualSAppl.o
#MDSOBJS +=$(CSDIR)/IndividualCS.o \
#	$(DDIR)/Individual.o \
#	$(EXTDIR)/IndividualExtraction.o \
#	$(SERDIR)/IndividualSearch.o
#CCFLAGS +=-DUSEI
#MODULES +=Individual
#endif

#Probability vector
ifeq ($(PVINCL),y)
MDSOBJS +=$(CSDIR)/ProbabilityVectorCS.o \
	$(DDIR)/ProbabilityVector.o \
	$(EXTDIR)/ProbabilityVectorExtraction.o \

#	$(SERDIR)/ProbabilityVectorSearch.o
#	$(APPLDIR)/ProbabilityVectorCAppl.o \
#	$(APPLDIR)/ProbabilityVectorSAppl.o \
CCFLAGS +=-DUSEPV
MODULES +=ProbabilityVector
endif

#Probability matrix
ifeq ($(PMINCL),y)
MDSOBJS +=$(CSDIR)/ProbabilityMatrixCS.o \
	$(DDIR)/ProbabilityMatrix.o \
	$(EXTDIR)/ProbabilityMatrixExtraction.o

#	$(SERDIR)/ProbabilityMatrixSearch.o
#	$(APPLDIR)/ProbabilityMatrixCAppl.o \
#	$(APPLDIR)/ProbabilityMatrixSAppl.o \
CCFLAGS +=-DUSEPM
MODULES +=ProbabilityMatrix
endif

#affective segment filter
ifeq ($(ASFINCL),y)
APPOBJS +=$(APPLDIR)/AffectiveSegmentFilterCAppl.o
MDSOBJS +=$(SERDIR)/AffectiveSegmentFilterSearch.o
APPLDEF +=-DUSEASF
MODULES +=affectivesegmentfilter
endif

#dependency structure
ifeq ($(DPSINCL),y)
APPOBJS +=$(APPLDIR)/DependStructureCAppl.o
MDSOBJS +=$(SERDIR)/DependStructureSearch.o
APPLDEF +=-DUSEDPS
MODULES +=dependstructure
endif

#extended textual type
ifeq ($(ETTINCL),y)
APPOBJS +=$(APPLDIR)/ExtendedTextualTypeCAppl.o
MDSOBJS +=$(SERDIR)/ExtendedTextualTypeSearch.o
APPLDEF +=-DUSEETT
MODULES +=extendedtextualtype
endif

#graphical classification schemes
ifeq ($(GCSINCL),y)
APPOBJS +=$(APPLDIR)/GraphicalCSCAppl.o
MDSOBJS +=$(SERDIR)/GraphicalCSSearch.o
APPLDEF +=-DUSEGCS
MODULES +=GraphicalCS
endif

#hierarchical summary
ifeq ($(HSINCL),y)
APPOBJS +=$(APPLDIR)/HierarchicalKeyFrameSAppl.o
MDSOBJS +=$(EXTDIR)/HierarchicalKeyFrameExtraction.o
APPLDEF +=-DUSEHS
MODULES +=hierarchical summary
endif

#importancehint
ifeq ($(IMHINCL),y)
APPOBJS +=$(APPLDIR)/ImportanceHintCAppl.o
MDSOBJS +=$(SERDIR)/ImportanceHintSearch.o
APPLDEF +=-DUSEIMH
MODULES +=importancehint
endif

#ink segment
ifeq ($(INKINCL),y)
APPOBJS +=$(APPLDIR)/InkSegmentCAppl.o \
	$(APPLDIR)/InkSegmentSAppl.o
MDSOBJS +=$(EXTDIR)/InkSegmentExtraction.o \
	$(SERDIR)/InkSegmentSearch.o
APPLDEF +=-DUSEINK
MODULES +=inksegment
endif

#matchinghint
ifeq ($(MHINCL),y)
APPOBJS +=$(APPLDIR)/MatchingHintCAppl.o \
	$(APPLDIR)/MatchingHintSAppl.o
MDSOBJS +=$(EXTDIR)/MatchingHintExtraction.o \
	$(SERDIR)/MatchingHintSearch.o
APPLDEF +=-DUSEMH
MODULES +=matchinghint
endif

#media meta
ifeq ($(MMINCL),y)
APPOBJS +=$(APPLDIR)/MediaMetaCAppl.o
MDSOBJS +=$(SERDIR)/MediaMetaSearch.o
APPLDEF +=-DUSEMM
MODULES +=mediameta
endif

#media quality
ifeq ($(MQINCL),y)
APPOBJS +=$(APPLDIR)/MediaQualitySAppl.o
MDSOBJS +=$(EXTDIR)/MediaQualityExtraction.o
APPLDEF +=-DUSEMQ
MODULES +=mediaquality
endif

#media review
ifeq ($(MRINCL),y)
APPOBJS +=$(APPLDIR)/MediaReviewCAppl.o
MDSOBJS +=$(SERDIR)/MediaReviewSearch.o
APPLDEF +=-DUSEMR
MODULES +=mediameta
endif

#model
ifeq ($(MOINCL),y)
APPOBJS +=$(APPLDIR)/ModelSAppl.o
MDSOBJS +=$(EXTDIR)/ModelExtraction.o
APPLDEF +=-DUSEMO
MODULES +=model
endif

#mosaic
ifeq ($(MKINCL),y)
APPOBJS +=$(APPLDIR)/MosaicSAppl.o \
	$(APPLDIR)/MosaicCAppl.o
MDSOBJS +=$(EXTDIR)/MosaicExtraction.o \
	$(SERDIR)/MosaicSearch.o
APPLDEF +=-DUSEMK
MODULES +=mosaic
endif

#media quality
ifeq ($(MQINCL),y)
MODOBJS +=$(APPLDIR)/MediaQualitySAppl.o \
    $(EXTDIR)/MediaQualityExtraction.o
APPLDEF +=-DUSEMQ
MODULES +=mediaquality
endif

#ordering key
ifeq ($(ODKINCL),y)
APPOBJS +=$(APPLDIR)/OrderingKeyCAppl.o
MDSOBJS +=$(SERDIR)/OrderingKeySearch.o
APPLDEF +=-DUSEODK
MODULES +=orderingkey
endif

#package
ifeq ($(PACKINCL),y)
APPOBJS +=$(APPLDIR)/PackageCAppl.o
MDSOBJS +=$(SERDIR)/PackageSearch.o
APPLDEF +=-DUSEPACK
MODULES +=package
endif

#phonetic translation hints
ifeq ($(PTHINCL),y)
APPOBJS +=$(APPLDIR)/PhoneticTranslationHintsCAppl.o
MDSOBJS +=$(SERDIR)/PhoneticTranslationHintsSearch.o
APPLDEF +=-DUSEPTH
MODULES +=phonetictranslationhints
endif

#point of view
ifeq ($(POVINCL),y)
APPOBJS +=$(APPLDIR)/PointOfViewCAppl.o
MDSOBJS +=$(SERDIR)/PointOfViewSearch.o
#	$(DDIR)/PointOfView.o 
APPLDEF +=-DUSEPOV
MODULES +=pointofview
endif

#segment 
ifeq ($(SGINCL),y)
APPOBJS +=$(APPLDIR)/SegmentCAppl.o \
	$(APPLDIR)/SegmentSAppl.o
MDSOBJS +=$(EXTDIR)/SegmentExtraction.o \
	$(SERDIR)/SegmentSearch.o
APPLDEF +=-DUSESG
MODULES +=segment
endif

#segment relation graph
ifeq ($(SRGINCL),y)
APPOBJS +=$(APPLDIR)/SegmentRelGraphCAppl.o \
	$(APPLDIR)/SegmentRelGraphSAppl.o
MDSOBJS +=$(EXTDIR)/SegmentRelGraphExtraction.o \
	$(SERDIR)/SegmentRelGraphSearch.o
APPLDEF +=-DUSESRG
MODULES +=segrelgph
endif

#semantic
ifeq ($(SEMINCL),y)
APPOBJS +=$(APPLDIR)/SemanticCAppl.o
MDSOBJS +=$(SERDIR)/SemanticSearch.o
APPLDEF +=-DUSESEM
MODULES +=Semantic
endif

#seq summary fast moving story board
ifeq ($(SMSINCL),y)
APPOBJS +=$(APPLDIR)/SeqSummaryFastMovingStoryBoardSAppl.o
MDSOBJS +=$(EXTDIR)/SeqSummaryFastMovingStoryBoardExtraction.o
APPLDEF +=-DUSESMS
MODULES +=seqsummaryfastmovingstoryboard
endif

#seq summary keywords
ifeq ($(SKWINCL),y)
APPOBJS +=$(APPLDIR)/SeqSummaryKeywordsSAppl.o
MDSOBJS +=$(EXTDIR)/SeqSummaryKeywordsExtraction.o
APPLDEF +=-DUSESKW
MODULES +=seqsummarykeywords
endif

#sequential summary
ifeq ($(SSZINCL),y)
APPOBJS +=$(APPLDIR)/SequentialSummarySAppl.o
MDSOBJS +=$(EXTDIR)/SequentialSummaryExtraction.o
APPLDEF +=-DUSESSZ
MODULES +=sequentialsummary
endif

#soundproperty
ifeq ($(SPINCL),y)
APPOBJS +=$(APPLDIR)/SoundPropertySAppl.o
MDSOBJS +=$(EXTDIR)/SoundPropertyExtraction.o
APPLDEF +=-DUSESP
MODULES +=soundproperty
endif

#spacefrequency
ifeq ($(SFINCL),y)
APPOBJS +=$(APPLDIR)/SpaceFrequencyViewSAppl.o
MDSOBJS +=$(EXTDIR)/SpaceFrequencyViewExtraction.o
APPLDEF +=-DUSESF
MODULES +=spacefrequency
endif

#spatial resolution hint
ifeq ($(SRHINCL),y)
APPOBJS +=$(APPLDIR)/SpatialResolutionHintCAppl.o
MDSOBJS +=$(SERDIR)/SpatialResolutionHintSearch.o
APPLDEF +=-DUSESRH
MODULES +=spatialresolutionhint
endif

#semantic state membership function
ifeq ($(STMINCL),y)
APPOBJS +=$(APPLDIR)/StateMemberCAppl.o
MDSOBJS +=$(SERDIR)/StateMemberSearch.o
APPLDEF +=-DUSESTM
MODULES +=StateMember
endif

#statetransition
ifeq ($(STXINCL),y)
APPOBJS +=$(APPLDIR)/StateTransitionModelCAppl.o \
	$(APPLDIR)/StateTransitionModelSAppl.o
MDSOBJS +=$(EXTDIR)/StateTransitionModelExtraction.o \
	$(SERDIR)/StateTransitionModelSearch.o
APPLDEF +=-DUSESTX
MODULES +=statetransition
endif

#structured collection
ifeq ($(STCINCL),y)
APPOBJS +=$(APPLDIR)/StructuredCollectionSAppl.o \
	$(APPLDIR)/StructuredCollectionCAppl.o
MDSOBJS +=$(EXTDIR)/StructuredCollectionExtraction.o \
	$(SERDIR)/StructuredCollectionSearch.o
APPLDEF +=-DUSESTC
MODULES +=StructuredCollection
endif

#summarization
ifeq ($(SZINCL),y)
APPOBJS +=$(APPLDIR)/SummarizationCAppl.o \
	$(APPLDIR)/SummarizationSAppl.o
MDSOBJS +=$(EXTDIR)/SummarizationExtraction.o \
	$(SERDIR)/SummarizationSearch.o
APPLDEF +=-DUSESZ
MODULES +=summarization
endif

#summary preference
ifeq ($(SPRINCL),y)
APPOBJS +=$(APPLDIR)/SummaryPreferencesCAppl.o
MDSOBJS +=$(SERDIR)/SummaryPreferencesSearch.o
APPLDEF +=-DUSESPR
MODULES +=summarypreference
endif

#transcoding hints
ifeq ($(THINCL),y)
APPOBJS +=$(APPLDIR)/TranscodingSAppl.o
MDSOBJS +=$(EXTDIR)/TranscodingHintsExtraction.o
APPLDEF +=-DUSETH
MODULES +=transcoding
endif

#time
ifeq ($(TMINCL),y)
APPOBJS +=$(APPLDIR)/TimeCAppl.o \
	$(APPLDIR)/TimeSAppl.o
MDSOBJS +=$(EXTDIR)/TimeExtraction.o \
	$(SERDIR)/TimeSearch.o
APPLDEF +=-DUSETM
MODULES +=time
endif

#user preference
ifeq ($(UPINCL),y)
APPOBJS +=$(APPLDIR)/UserPreferenceCAppl.o
MDSOBJS +=$(SERDIR)/UserPreferenceSearch.o
APPLDEF +=-DUSEUP
MODULES +=userpreference
endif

#variation
ifeq ($(VAINCL),y)
APPOBJS +=$(APPLDIR)/VariationSAppl.o
MDSOBJS +=$(EXTDIR)/VariationExtraction.o
APPLDEF +=-DUSEVA
MODULES +=variation
endif

#video editing
ifeq ($(VEINCL),y)
APPOBJS +=$(APPLDIR)/VideoEditingSAppl.o
MDSOBJS +=$(EXTDIR)/VideoEditingExtraction.o 
APPLDEF +=-DUSEVE
MODULES +=videoediting
endif

#video text 
ifeq ($(VTINCL), y)
APPOBJS +=$(APPLDIR)/VideoTextSAppl.o
MDSOBJS +=$(EXTDIR)/VideoText_FrameFile.o\
	$(EXTDIR)/VideoText_Coordinate.o\
	$(EXTDIR)/VideoText_Line.o\
	$(EXTDIR)/VideoText_Area.o\
	$(EXTDIR)/VideoText_Parameters.o\
	$(EXTDIR)/VideoText_Detect.o\
	$(EXTDIR)/VideoTextExtraction.o
APPLDEF +=-DUSEVT
MODULES +=VideoText
endif

#Main###########################################################
MAINDIR=$(XMADIR)
MAINOBJS=$(MAINDIR)/XMMain.o

XMINCS=$(addprefix -I, . $(ADDRDIR))
XMBLIB=lib/libXMB_$(SETTINGS)$(LIBEXT)
XMBLIBS=-Llib -lXMB_$(SETTINGS)

XMAPLIB=lib/libXMAP_$(SETTINGS)$(LIBEXT)
XMAPLIBS=-lXMAP_$(SETTINGS)

XMCLIB=lib/libXMC_$(SETTINGS)$(LIBEXT)
XMCLIBS=-lXMC_$(SETTINGS)

XMVLIB=lib/libXMV_$(SETTINGS)$(LIBEXT)
XMVLIBS=-lXMV_$(SETTINGS)
XMALIB=lib/libXMA_$(SETTINGS)$(LIBEXT)
XMALIBS=-lXMA_$(SETTINGS)
XMMLIB=lib/libXMM_$(SETTINGS)$(LIBEXT)
XMMLIBS=-lXMM_$(SETTINGS)

XMLIB=lib/libXM_$(SETTINGS)$(LIBEXT)

XMPARTDEP=
XMPARTLIBS=
ifneq ($(VIDOBJS),)
	XMPARTDEP+=XMVLib
	XMPARTLIB+=$(XMVLIB)
	XMPARTLIBS+=$(XMVLIBS)
endif
ifneq ($(AUDOBJS),)
	XMPARTDEP+=XMALib
	XMPARTLIB+=$(XMALIB)
	XMPARTLIBS+=$(XMALIBS)
endif
ifneq ($(MDSOBJS),)
	XMPARTDEP+=XMMLib
	XMPARTLIB+=$(XMMLIB)
	XMPARTLIBS+=$(XMMLIBS)
endif

#BIN=$(addprefix $(BINDIR), $(addsuffix _$(SETTINGS).exe, XMMain))
BIN=$(addprefix $(BINDIR), $(addsuffix .exe, XMMain))

### Generate Flags #########################################################
CFLAGS +=$(CDEFS) $(XMINCS) $(XMDEFS) \
	$(EXTRA_INCLUDE_DIR) $(EXTRA_CFLAGS) $(EXTRA_CDEFS) \
	-DM7XMVERSION=\"$(M7XMVERSION)\"
CCFLAGS=$(CFLAGS)
#LFLAGS +=$(XMCLIBS) $(XMPARTLIBS) $(XMAPLIBS) $(XMBLIBS) $(EXTRA_LIBS) 
LFLAGS +=$(XMAPLIBS) $(XMPARTLIBS) $(XMCLIBS) $(XMBLIBS) $(EXTRA_LIBS) 
## DEPENDENCIES ############################################################

all : varlist XMMain \
	mpeg2decode.binary xmpg2enc.binary #\
#	TextualAccess_Encoder.binary AccessUnit.binary \
#	navigation_path
	@echo
	@echo '*************************************************'
	@echo 'To compile the Java BiM coder run: "make JavaBiM"'

varlist:
	@echo '****************************'
	@echo 'MPEG-7 Experimentation Model'
	@echo 'Copyright (c) 1998-1999'
	@echo '****************************'
	@echo 
	@echo '*******************'
	@echo 'DISPLAY    : $(DISPLAY)'
	@echo 'USEMAGICK  : $(USEMAGICK)'
	@echo 'USEXML     : $(USEXML)'
	@echo 'USEOPENCV  : $(USEOPENCV)'
	@echo 'PROF       : $(PROF)'
	@echo 'DEBUG      : $(DEBUG)'
	@echo 'PURIFY     : $(PURIFY)'
	@echo 'PURE       : $(PURE)'
	@echo 'IPROF      : $(IPROF)'
	@echo 'CC         : $(CC)'
	@echo 'CFLAGS     : $(CFLAGS)'
	@echo 'CCFLAGS    : $(CCFLAGS)'
	@echo 'LFLAGS     : $(LFLAGS)'
	@echo 'APPLDEF    : $(APPLDEF)'
	@echo 'LIBS       : $(LIBS)'
	@echo 'EXTRA_LIBS : $(EXTRA_LIBS)'
	@echo 'XMPARTS    : $(XMPARTDEP)'
	@echo '*******************'
	@echo

dep: Makefile
	$(RM) .depend
	touch .depend
#	find . -name \*.cpp -exec gcc $(CFLAGS) -MM {} >> .depend \;
#	find . -name \*.c -exec gcc $(CFLAGS) -MM {} >> .depend \;
	find . -name \*.cpp -exec $(MAKEDEPEND) $(CCFLAGS) $(SYSINC) \
		-Y -a -f.depend {} \;
	find . -name \*.c -exec $(MAKEDEPEND) $(CFLAGS) $(SYSINC) \
		-Y -a -f.depend {} \;

#.depend: dep

doc:
	cd Doc; perl format_all.pl . .

print_mod: 
	@echo
	@echo modules: $(MODULES)

print_moddirs: 
	@echo
	@echo module directories:  $(APPOBJS) $(VIDOBJS) \
	                           $(AUDOBJS) $(MDSOBJS)

print_modlibs: 
	@echo
	@echo module libraries: $(APPOBJS) $(VIDOBJS) \
                                $(AUDOBJS) $(MDSOBJS)

print_modobjs: 
	@echo
	@echo module objects: $(APPOBJS) $(VIDOBJS) \
                              $(AUDOBJS) $(MDSOBJS)

print_maindir: 
	@echo
	@echo main direcory:$(MAINDIR)

sleepclean:
	sleep 5
	make clean

clean: clean_bin clean_lib clean_obj

clean_bin:
	$(RM) $(BIN)
	$(RM) mpeg2decode	 
	$(RM) xmpg2enc	 
	$(RM) $(KLT)/libklt.a
	cd Decoders/MPEG2Dec; make clean
	cd Encoders/MPEG2Enc; make clean
	cd SystemTools/TextualAccessUnits/Encoder; make clean
	cd SystemTools/TextualAccessUnits/Decoder; make clean
	cd SystemTools/BiM/navigation_path; make clean
	cd $(KLT); make clean

clean_lib:
	$(RM) $(XMLIB)
	$(RM) $(XMBLIB)
	$(RM) $(XMAPLIB)
	$(RM) $(XMCLIB)
	$(RM) $(XMVLIB)
	$(RM) $(XMALIB)
	$(RM) $(XMMLIB)
	$(RM) lib/st*

clean_obj:
	$(RM) $(BASOBJS)
	$(RM) $(APPOBJS)
	$(RM) $(MCOMOBJS)
	$(RM) $(VIDOBJS)
	$(RM) $(AUDOBJS)
	$(RM) $(MDSOBJS)
	$(RM) $(MAINOBJS)

distclean:
	$(RM) .depend
	find . -name \*.o -exec $(RM) {} \;
	find . -name \*.a -exec $(RM) {} \;
	find . -name \*.exe -type f -exec $(RM) {} \;

#.binary added to executable name to force call of makefile
mpeg2decode.binary:
	cd Decoders/MPEG2Dec; make mpeg2decode

mpeg2decode.library:
	cd Decoders/MPEG2Dec; make libmpeg2dec

xmpg2enc.binary:
	cd Encoders/MPEG2Enc; make mpeg2encode 'IPROFCC=$(IPROFCC)'
#	cd Encoders/MPEG2Enc; make mpeg2encode 'IPROFCC=$(IPROFCC)' 'M2ENCIPROF'
TextualAccess_Encoder.binary:
	cd SystemTools/TextualAccessUnits/Encoder; make

AccessUnit.binary:
	cd SystemTools/TextualAccessUnits/Decoder; make

navigation_path:
	cd SystemTools/BiM/navigation_path; make

JavaBiM:
	cd SystemTools/BiM/src; make -f Makefile.unix
	cd SystemTools/BiM/src; make -f Makefile.unix jar

$(KLT)/libklt.a:
	cd $(KLT); make -f Makefile

install: XMMain mpeg2decode.binary xmpg2enc.binary installlib
	$(MKDIR) $(INSTALLDIR)/bin $(INSTALLDIR)/lib
	$(INSTALL) $(BIN) $(INSTALLDIR)/bin
	$(INSTALL) mpeg2decode $(INSTALLDIR)/bin
	$(INSTALL) xmpg2enc $(INSTALLDIR)/bin
	$(INSTALL) $(BIN) $(INSTALLDIR)/bin

installlib: $(XMPARTLIB) $(XMCLIB) $(XMBLIB)
	$(MKDIR) $(INSTALLDIR)/lib
	$(INSTALL) $(XMBLIB) $(XMCLIB) $(XMAPLIB) $(XMPARTLIB) $(INSTALLDIR)/lib
	$(INSTALL) $(MPEG2LIB) $(INSTALLDIR)/lib

installinc:
	$(MKDIR) $(INSTALLDIR)/include/xm
	$(INSTALL) --parents */*.h $(INSTALLDIR)/include/xm
	#$(INSTALL) Version $(INSTALLDIR)

# main executable#
XMMain: $(BIN)

$(BIN) : XMBLib XMAPLib XMCLib $(XMPARTDEP) $(MAINOBJS) $(KLT)/libklt.a mpeg2decode.library
	$(LINK) -o $(BIN) $(MAINOBJS) $(LFLAGS)
# -lstlport_gcc
#$(EXTRA_LIBS)

# basic Architecture library
XMLib: $(XMLIB)

$(XMLIB): $(MCOMOBJS) $(VIDOBJS) $(AUDOBJS) $(MDSOBJS)
	$(AR)
#	$(AR) $@ $?

# basic Architecture library
XMBLib: $(XMBLIB)

$(XMBLIB): $(BASOBJS)
	$(AR)
#	$(AR) $@ $?

# application library
XMAPLib: $(XMAPLIB)

$(XMAPLIB): $(APPOBJS)
	$(AR)
#	$(AR) $@ $?

# common module library
XMCLib: $(XMCLIB)

$(XMCLIB): $(MCOMOBJS)
	$(AR)
#	$(AR) $@ $?

# video module library
XMVLib: $(XMVLIB)

$(XMVLIB): $(VIDOBJS)
	$(AR)
#	$(AR) $@ $?

# audio module library
XMALib: $(XMALIB)

$(XMALIB): $(AUDOBJS)
	$(AR)
#	$(AR) $@ $?

# MDS module library
XMMLib: $(XMMLIB)

$(XMMLIB): $(MDSOBJS)
	$(AR)
#	$(AR) $@ $?

# main module
$(MAINDIR)/XMMain.o : $(MAINDIR)/XMMain.cpp
	$(CCC) $(IMAGICK) $(CCFLAGS) -c -o $@ $<
#$(MAINDIR)/XMMmain.o $(MAINDIR)/XMMain.cpp

$(XMADIR)/Version1.o : $(XMADIR)/Version1.cpp Makefile
	$(CCC) $(APPLDEF) $(IMAGICK) $(CCFLAGS) $(SHAREDFLAGS) -c -o $@ $<

$(XMADIR)/Version2.o : $(XMADIR)/Version2.cpp Makefile
	$(CCC) $(APPLDEF) $(IMAGICK) $(CCFLAGS) $(SHAREDFLAGS) -c -o $@ $<

$(XMADIR)/Version3.o : $(XMADIR)/Version3.cpp Makefile
	$(CCC) $(APPLDEF) $(IMAGICK) $(CCFLAGS) $(SHAREDFLAGS) -c -o $@ $<

$(XMADIR)/Version4.o : $(XMADIR)/Version4.cpp Makefile
	$(CCC) $(APPLDEF) $(IMAGICK) $(CCFLAGS) $(SHAREDFLAGS) -c -o $@ $<

ifeq (.depend, $(wildcard .depend))
include .depend
endif

# object files#
.SUFFIXES: .cpp
.cpp.o:
	$(CCC) $(CCFLAGS) $(SHAREDFLAGS) -c -o $@ $<

.SUFFIXES: .c
.c.o:
	$(CCC) $(CCFLAGS) $(SHAREDFLAGS) -c -o $@ $<

$(MEDDIR)/MediaIO.o: $(MEDDIR)/MediaIO.cpp
	$(CCC) $(CCFLAGS) $(SHAREDFLAGS) $(IMAGICK) -c -o $(MEDDIR)/MediaIO.o \
		$(MEDDIR)/MediaIO.cpp

$(DSDIR)/GenericDS.o: $(DSDIR)/GenericDS.cpp
	$(CCC) $(CCFLAGS) $(SHAREDFLAGS) $(IXML) -c -o $(DSDIR)/GenericDS.o \
		$(DSDIR)/GenericDS.cpp

$(DSDIR)/GenericDSCS.o: $(DSDIR)/GenericDSCS.cpp
	$(CCC) $(CCFLAGS) $(SHAREDFLAGS) $(IXML) -c -o $(DSDIR)/GenericDSCS.o \
		$(DSDIR)/GenericDSCS.cpp
