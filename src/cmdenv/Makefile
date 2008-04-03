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
LIBNAME=libcmdenv$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/cmdenv
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../common -I../envir -DBUILDING_CMDENV

IMPLIBS= -L$L -lsim_std$D -lenvir$D -lcommon$D

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
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc

clean:
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/cmdenv.o: cmdenv.cc \
  cmddefs.h \
  cmdenv.h \
  ../envir/appreg.h \
  ../../include/csimplemodule.h \
  ../../include/ccomponenttype.h \
  ../../include/cmessage.h \
  ../envir/args.h \
  ../envir/speedometer.h \
  ../../include/cconfigkey.h \
  ../../include/csimulation.h \
  ../envir/omnetapp.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/carray.h \
  ../envir/envirdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h
