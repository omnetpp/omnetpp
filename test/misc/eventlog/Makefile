#
# Global definitions
#
include ../../../Makefile.inc

#
# Local definitions
#
COPTS = $(CXXFLAGS) -I../../../include -I../../../src

OBJS= eventlogtest.o eventlogindextest.o
LIBS= $(OMNETPP_LIB_DIR)/liboppcommon$D$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/liboppeventlog$D$(SO_LIB_SUFFIX)
IMPLIBS= -L $(OMNETPP_LIB_DIR) -loppcommon$D -loppeventlog$D

EXECUTABLES = eventlogtest$(EXE_SUFFIX) eventlogindextest$(EXE_SUFFIX)

# disabling all implicit rules
.SUFFIXES :

#
# Automatic rules
#

%.o: %.cc
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $(EXECUTABLES)

eventlogtest$(EXE_SUFFIX): eventlogtest.o $(LIBS)
	$(CXX) $(LDFLAGS) -o eventlogtest$(EXE_SUFFIX) eventlogtest.o $(IMPLIBS)

eventlogindextest$(EXE_SUFFIX): eventlogindextest.o $(LIBS)
	$(CXX) $(LDFLAGS) -o eventlogindextest$(EXE_SUFFIX) eventlogindextest.o $(IMPLIBS)

depend:
	$(MAKEDEPEND) -I. -fMakefile -- *.cc

clean:
	- rm -f *.o
	- rm -f $(EXECUTABLES)


# DO NOT DELETE THIS LINE -- make depend depends on it.
