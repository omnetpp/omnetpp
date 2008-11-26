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
LIBNAME=liboppcmdenv$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/cmdenv
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../common -I../envir -DBUILDING_CMDENV

IMPLIBS= -L$L -loppsim$D -loppenvir$D -loppcommon$D

OBJS = $O/cmdenv.o

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
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc

clean:
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/cmdenv.o: cmdenv.cc \
  ../../include/regmacros.h \
  ../envir/args.h \
  ../../include/ccoroutine.h \
  cmdenv.h \
  ../../include/platdep/platdefs.h \
  ../envir/envirdefs.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../envir/eventlogfilemgr.h \
  cmddefs.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../envir/objectprinter.h \
  ../../include/carray.h \
  ../envir/speedometer.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/ccomponent.h \
  ../../include/cproperties.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/errmsg.h \
  ../../include/cmessage.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../envir/appreg.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  ../../include/csimplemodule.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/cproperty.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h
