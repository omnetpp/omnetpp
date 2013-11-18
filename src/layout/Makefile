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
LIBNAME=libopplayout$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/layout
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep

IMPLIBS= -L$L -loppcommon$D

OBJS= $O/geometry.o $O/graphcomponent.o $O/heapembedding.o $O/startreeembedding.o \
      $O/forcedirectedparametersbase.o $O/forcedirectedparameters.o $O/forcedirectedembedding.o \
      $O/graphlayouter.o $O/basicspringembedderlayout.o $O/forcedirectedgraphlayouter.o

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

#
# Targets
#
all: $L/$(LIBNAME)$(LIB_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(Q)$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/basicspringembedderlayout.o: basicspringembedderlayout.cc \
  graphlayouter.h \
  basicspringembedderlayout.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/lcgrandom.h \
  layoutdefs.h \
  ../common/commondefs.h
$O/concentrictreeembedding.o: concentrictreeembedding.cc \
  geometry.h \
  concentrictreeembedding.h \
  ../common/exception.h \
  ../common/commonutil.h \
  graphcomponent.h \
  ../common/commondefs.h \
  layoutdefs.h
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
  ../common/commonutil.h \
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
