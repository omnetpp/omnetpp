#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

# check if the output directories are not empty                                                                   
ifeq ("$(strip $(OMNETPP_BIN_DIR))","")                                                                           
$(error OMNETPP_BIN_DIR must be correctly set)                                                                    
endif                                                                                                             
ifeq ("$(strip $(OMNETPP_OUT_DIR))","")                                                                           
$(error OMNETPP_OUT_DIR must be correctly set)                                                                    
endif                                                                                                             
ifeq ("$(strip $(OMNETPP_LIB_DIR))","")                                                                           
$(error OMNETPP_LIB_DIR must be correctly set)                                                                    
endif 

O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/layout
L=$(OMNETPP_LIB_DIR)/$(CONFIGNAME)

#
# Local definitions
#
LIBNAME=liblayout

COPTS=$(CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep

IMPLIBS= -L$L -lcommon

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
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(CONFIGNAME)/$(LIBNAME)$(LIB_SUFFIX)* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

clean:
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/basicspringembedderlayout.o: basicspringembedderlayout.cc \
  ../common/commonutil.h \
  basicspringembedderlayout.h \
  ../common/commondefs.h \
  ../common/exception.h \
  graphlayouter.h \
  layoutdefs.h \
  ../common/lcgrandom.h
$O/forcedirectedembedding.o: forcedirectedembedding.cc \
  forcedirectedembedding.h \
  forcedirectedparameters.h \
  ../common/lcgrandom.h \
  geometry.h \
  forcedirectedparametersbase.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/forcedirectedgraphlayouter.o: forcedirectedgraphlayouter.cc \
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
$O/forcedirectedparameters.o: forcedirectedparameters.cc \
  forcedirectedparameters.h \
  geometry.h \
  forcedirectedparametersbase.h \
  forcedirectedembedding.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/forcedirectedparametersbase.o: forcedirectedparametersbase.cc \
  forcedirectedparametersbase.h \
  geometry.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/geometry.o: geometry.cc \
  geometry.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/graphcomponent.o: graphcomponent.cc \
  graphcomponent.h \
  geometry.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/graphlayouter.o: graphlayouter.cc \
  graphlayouter.h \
  layoutdefs.h \
  ../common/lcgrandom.h \
  ../common/commondefs.h \
  ../common/exception.h
$O/heapembedding.o: heapembedding.cc \
  heapembedding.h \
  geometry.h \
  graphcomponent.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/startreeembedding.o: startreeembedding.cc \
  startreeembedding.h \
  geometry.h \
  graphcomponent.h \
  layoutdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/commondefs.h
