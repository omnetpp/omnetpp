#
# Standalone executable support: you can have Tkenv's TCL script parts
# be compiled into the executable. Enable doing so, uncomment the
# TKAPP=tkapp-rt line below. Otherwise, uncomment TKAPP=tkapp.
#
TKAPP=tkapp
# TKAPP=tkapp-rt

# Definitions to be customized by 'configure'
CC=gcc -g -I. -c -x c++
AS=gcc -g -c
AR=ar rs
MAKE_SO=gcc -shared -o
OMNETPP_ROOT=/home/andras/omnetpp
OMNETPP_SRC_DIR=/home/andras/omnetpp/src
OMNETPP_INCL_PATH=-I$(OMNETPP_SRC_DIR)/sim
OMNETPP_INCLUDES=$(OMNETPP_SRC_DIR)/sim/*.h
OMNETPP_BITMAP_PATH=/home/andras/omnetpp/bitmaps
TK_INCL_PATH=-I/usr/include -I/usr/include
X_INCL_PATH=-I/usr/X11R6/include/X11
LIB_SUFFIX=a

# Local definitions
ENVIR_INCLUDES=../args.h ../cinifile.h ../omnetapp.h ../slaveapp.h
TKENV_INCLUDES=tkapp.h tkinsp.h tklib.h arrow.h
INCL_PATH=-I..

INCLUDES=$(OMNETPP_INCLUDES) $(ENVIR_INCLUDES) $(TKENV_INCLUDES)
CC_OPTS=-fwritable-strings $(OMNETPP_INCL_PATH) $(INCL_PATH) $(TK_INCL_PATH) $(X_INCL_PATH)

OMNETPP_TKENV_DIR=$(OMNETPP_SRC_DIR)/envir/tkenv

OBJS = $(TKAPP).o tkcmd.o tklib.o objinsp.o modinsp.o statinsp.o inspect.o arrow.o

all : libtkenv.$(LIB_SUFFIX)

libtkenv.a : $(OBJS)
	$(AR) libtkenv.a $(OBJS)

libtkenv.so : $(OBJS)
	$(MAKE_SO) libtkenv.so $(OBJS)

tkapp.o : tkapp.cc $(INCLUDES)
	$(CC) $(CC_OPTS) -DOMNETPP_TKENV_DIR=\"$(OMNETPP_TKENV_DIR)\" -DOMNETPP_BITMAP_PATH=\"$(OMNETPP_BITMAP_PATH)\" tkapp.cc

tkapp-rt.o : tkapp.cc $(INCLUDES) tclcode.cc
	$(CC) $(CC_OPTS) -DOMNETPP_BITMAP_PATH=\"$(OMNETPP_BITMAP_PATH)\" tkapp.cc -o tkapp-rt.o

tkcmd.o : tkcmd.cc $(INCLUDES)
	$(CC) $(CC_OPTS) tkcmd.cc

tklib.o : tklib.cc $(INCLUDES)
	$(CC) $(CC_OPTS) tklib.cc

objinsp.o: objinsp.cc $(INCLUDES)
	$(CC) $(CC_OPTS) objinsp.cc

modinsp.o: modinsp.cc $(INCLUDES)
	$(CC) $(CC_OPTS) modinsp.cc

statinsp.o: statinsp.cc $(INCLUDES)
	$(CC) $(CC_OPTS) statinsp.cc

inspect.o: inspect.cc $(INCLUDES)
	$(CC) $(CC_OPTS) inspect.cc

arrow.o: arrow.cc arrow.h
	$(CC) $(CC_OPTS) arrow.cc

tclcode.cc: *.tcl tcl2c
	./tcl2c tclcode.cc *.tcl

tcl2c: tcl2c.c
	cc tcl2c.c -o tcl2c
