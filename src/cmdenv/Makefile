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
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/cmdenv
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS) -DBUILDING_CMDENV

IMPLIBS= -L$L -loppsim$D -loppenvir$D -loppcommon$D

OBJS = $O/cmdenv.o

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
	$(Q)$(MAKEDEPEND) -I../../include -I.. -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/cmdenv.o: cmdenv.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdensityestbase.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenum.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/checkandcast.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/coutvector.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cproperties.h \
  ../../include/omnetpp/cproperty.h \
  ../../include/omnetpp/cqueue.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultfilter.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/cresultrecorder.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/cscheduler.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstddev.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/enumstr.h \
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/patternmatcher.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  ../envir/appreg.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  cmddefs.h \
  cmdenv.h
