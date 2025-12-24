# -*-Makefile-*-

# XERCES CONFIGURATION
XCERES_ROOT=/u/formes/share
XCERES_ROOT_INCLUDE=$(XCERES_ROOT)/include
XCERES_ROOT_LIB=$(XCERES_ROOT)/lib/$(SYS)
XCERES_LIB = $(XCERES_ROOT_LIB)/libxerces-c1_3.so
INCLDIR=$(XCERES_ROOT_INCLUDE)
LIBDIR=$(XCERES_ROOT_LIB)

CXXFLAGS= -I$(INCLDIR) -g -Wall
LDFLAGS= -L$(LIBDIR) -lxerces-c1_3
OBJ_DIR=.

SRC_MATLAB= outputXML.cpp
OBJS_MATLAB= DOMPrint.o DOMTreeErrorReporter.o outputXMLFunction.o treeFunction.o

# SPECIAL FLAGS FOR MATLAB
MEX = /usr/local/matlab-6/bin/mex
MEX_OPTIONS = -v -g CC=gcc LD=gcc 
MEX_LDFLAGS = "LDFLAGS=-shared -nodefaultlibs -t"

CFLAGS_MATLAB= -I$(INCLDIR) -I/usr/local/matlab-6/extern/include -I/usr/include -I/usr/local/include/

CXX=g++
INSTALL=/bin/cp
INSTALL_DIR= ../

CXXFLAGS += -DTIMBRE
CFLAGS_MATLAB += -DTIMBRE


all: matlab


matlab: $(OBJS_MATLAB)
	$(MEX) $(MEX_OPTIONS) $(CFLAGS_MATLAB) $(MEX_LDFLAGS) $(SRC_MATLAB) $(OBJS_MATLAB) $(XCERES_LIB)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

install: matlab
	$(INSTALL) outputXML.mexglx $(INSTALL_DIR)

clean:
	-rm -f $(OBJS_MATLAB) outputXML.mewxglx $(INSTALL_DIR)/outputXML.mexglx *~ core 


