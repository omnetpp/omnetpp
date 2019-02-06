#
# Global definitions
#
include ../../Makefile.inc

#
# Local definitions
#
COPTS = $(CFLAGS) -I../../include -I../../src

OBJS= filereaderconsumer.o filereaderproducer.o
LIBS= $(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)/liboppcommon$D$(SO_LIB_SUFFIX)
IMPLIBS= -L $(OMNETPP_LIB_DIR) -loppcommon$D

EXECUTABLES = fileechotest$(EXE_SUFFIX) filereadertest$(EXE_SUFFIX) filereaderconsumer$(EXE_SUFFIX) filereaderproducer$(EXE_SUFFIX)

#
# Automatic rules
#
.SUFFIXES : .cc

%.o: %.cc
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $(EXECUTABLES)

fileechotest$(EXE_SUFFIX): fileechotest.o $(LIBS)
	$(CXX) $(LDFLAGS) -o fileechotest$(EXE_SUFFIX) fileechotest.o $(IMPLIBS)

filereadertest$(EXE_SUFFIX): filereadertest.o $(LIBS)
	$(CXX) $(LDFLAGS) -o filereadertest$(EXE_SUFFIX) filereadertest.o $(IMPLIBS)

filereaderconsumer$(EXE_SUFFIX): filereaderconsumer.o $(LIBS)
	$(CXX) $(LDFLAGS) -o filereaderconsumer$(EXE_SUFFIX) filereaderconsumer.o $(IMPLIBS)

filereaderproducer$(EXE_SUFFIX): filereaderproducer.o $(LIBS)
	$(CXX) $(LDFLAGS) -o filereaderproducer$(EXE_SUFFIX) filereaderproducer.o $(IMPLIBS)

depend:
	$(MAKEDEPEND) -I. -fMakefile -- *.cc

clean:
	- rm -f *.o
	- rm -f $(EXECUTABLES)


# DO NOT DELETE THIS LINE -- make depend depends on it.
