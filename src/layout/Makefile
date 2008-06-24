#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

# Make sure that output locations are set
ifeq ("$(strip $(OMNETPP_BIN_DIR))","")
$(error OMNETPP_BIN_DIR must be correctly set)
endif
ifeq ("$(strip $(OMNETPP_OUT_DIR))","")
$(error OMNETPP_OUT_DIR must be correctly set)
endif
ifeq ("$(strip $(OMNETPP_LIB_DIR))","")
$(error OMNETPP_LIB_DIR must be correctly set)
endif

#
# Local definitions
#
LIBNAME=liblayout$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/layout
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep

IMPLIBS= -L$L -lcommon$D

OBJS= $O/geometry.o $O/graphcomponent.o $O/heapembedding.o $O/startreeembedding.o \
      $O/forcedirectedparametersbase.o $O/forcedirectedparameters.o $O/forcedirectedembedding.o \
      $O/graphlayouter.o $O/basicspringembedderlayout.o $O/forcedirectedgraphlayouter.o

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $L/$(LIBNAME)$(LIB_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

clean:
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/basicspringembedderlayout.o: basicspringembedderlayout.cc \
  graphlayouter.h \
  basicspringembedderlayout.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/lcgrandom.h \
  layoutdefs.h \
  ../common/commondefs.h
$O/forcedirectedembedding.o: forcedirectedembedding.cc \
  geometry.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  layoutdefs.h \
  forcedirectedparameters.h \
  forcedirectedparametersbase.h \
  forcedirectedembedding.h \
  ../common/lcgrandom.h
$O/forcedirectedgraphlayouter.o: forcedirectedgraphlayouter.cc \
  startreeembedding.h \
  geometry.h \
  graphlayouter.h \
  heapembedding.h \
  ../common/exception.h \
  ../common/commonutil.h \
  graphcomponent.h \
  layoutdefs.h \
  ../common/commondefs.h \
  forcedirectedparameters.h \
  forcedirectedparametersbase.h \
  forcedirectedembedding.h \
  forcedirectedgraphlayouter.h \
  ../common/lcgrandom.h
$O/forcedirectedparameters.o: forcedirectedparameters.cc \
  geometry.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  layoutdefs.h \
  forcedirectedparameters.h \
  forcedirectedparametersbase.h \
  forcedirectedembedding.h
$O/forcedirectedparametersbase.o: forcedirectedparametersbase.cc \
  geometry.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  layoutdefs.h \
  forcedirectedparametersbase.h
$O/geometry.o: geometry.cc \
  geometry.h \
  ../common/commonutil.h \
  ../common/exception.h \
  layoutdefs.h \
  ../common/commondefs.h
$O/graphcomponent.o: graphcomponent.cc \
  geometry.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  layoutdefs.h \
  graphcomponent.h
$O/graphlayouter.o: graphlayouter.cc \
  graphlayouter.h \
  ../common/exception.h \
  layoutdefs.h \
  ../common/commondefs.h \
  ../common/lcgrandom.h
$O/heapembedding.o: heapembedding.cc \
  geometry.h \
  heapembedding.h \
  ../common/exception.h \
  ../common/commonutil.h \
  graphcomponent.h \
  ../common/commondefs.h \
  layoutdefs.h
$O/startreeembedding.o: startreeembedding.cc \
  startreeembedding.h \
  geometry.h \
  ../common/exception.h \
  ../common/commonutil.h \
  graphcomponent.h \
  ../common/commondefs.h \
  layoutdefs.h
