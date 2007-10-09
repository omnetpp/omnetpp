#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep
LIBS= -lcommon

OBJS= geometry.o graphcomponent.o heapembedding.o startreeembedding.o \
      forcedirectedparametersbase.o forcedirectedparameters.o forcedirectedembedding.o \
      graphlayouter.o basicspringembedderlayout.o forcedirectedgraphlayouter.o


#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: liblayout$(LIB_SUFFIX)

liblayout$(A_LIB_SUFFIX) : $(OBJS)
	$(AR) liblayout$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) liblayout$(A_LIB_SUFFIX)
	../utils/install-lib liblayout$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) liblayout$(A_LIB_SUFFIX)

liblayout$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o liblayout$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib liblayout$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) liblayout$(SO_LIB_SUFFIX)

depend:
	$(MAKEDEPEND) -I. -I../common -- *.cc

clean:
	rm -f *.o liblayout$(A_LIB_SUFFIX) liblayout$(SO_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
basicspringembedderlayout.o: basicspringembedderlayout.cc \
  ../common/commonutil.h \
  basicspringembedderlayout.h \
  ../common/commondefs.h \
  ../common/exception.h \
  graphlayouter.h \
  layoutdefs.h \
  ../common/lcgrandom.h
forcedirectedembedding.o: forcedirectedembedding.cc \
  forcedirectedembedding.h \
  forcedirectedparameters.h \
  ../common/lcgrandom.h \
  geometry.h \
  forcedirectedparametersbase.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
forcedirectedgraphlayouter.o: forcedirectedgraphlayouter.cc \
  ../common/commonutil.h \
  forcedirectedgraphlayouter.h \
  startreeembedding.h \
  heapembedding.h \
  ../common/commondefs.h \
  ../common/exception.h \
  graphlayouter.h \
  forcedirectedembedding.h \
  forcedirectedparameters.h \
  graphcomponent.h \
  layoutdefs.h \
  ../common/lcgrandom.h \
  geometry.h \
  forcedirectedparametersbase.h
forcedirectedparametersbase.o: forcedirectedparametersbase.cc \
  forcedirectedparametersbase.h \
  geometry.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
forcedirectedparameters.o: forcedirectedparameters.cc \
  forcedirectedparameters.h \
  geometry.h \
  forcedirectedparametersbase.h \
  forcedirectedembedding.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
geometry.o: geometry.cc \
  geometry.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
graphcomponent.o: graphcomponent.cc \
  graphcomponent.h \
  geometry.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
graphlayouter.o: graphlayouter.cc \
  graphlayouter.h \
  layoutdefs.h \
  ../common/lcgrandom.h \
  ../common/commondefs.h \
  ../common/exception.h
heapembedding.o: heapembedding.cc \
  heapembedding.h \
  geometry.h \
  graphcomponent.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
startreeembedding.o: startreeembedding.cc \
  startreeembedding.h \
  geometry.h \
  graphcomponent.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
