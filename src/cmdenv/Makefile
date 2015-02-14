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

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../common -I../envir -DBUILDING_CMDENV

IMPLIBS= -L$L -loppsim$D -loppenvir$D -loppcommon$D

OBJS = $O/cmdenv.o $O/serializer.o

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
	$(Q)$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/cmdenv.o: cmdenv.cc \
  ../../include/carray.h \
  ../../include/cchannel.h \
  ../../include/cclassdescriptor.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cdisplaystring.h \
  ../../include/cenum.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/checkandcast.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessage.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cmsgpar.h \
  ../../include/cnamedobject.h \
  ../../include/cnedmathfunction.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/coutvector.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cqueue.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultfilter.h \
  ../../include/cresultlistener.h \
  ../../include/cresultrecorder.h \
  ../../include/cscheduler.h \
  ../../include/csimplemodule.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstddev.h \
  ../../include/cstringpool.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/cwatch.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/intlimits.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/timeutil.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  ../envir/appreg.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/httpserver.h \
  ../envir/intervals.h \
  ../envir/json.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  cmddefs.h \
  cmdenv.h \
  serializer.h
$O/serializer.o: serializer.cc \
  ../../include/carray.h \
  ../../include/cchannel.h \
  ../../include/cclassdescriptor.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cdisplaystring.h \
  ../../include/cenum.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessage.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cmsgpar.h \
  ../../include/cnamedobject.h \
  ../../include/cnedmathfunction.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/coutvector.h \
  ../../include/cownedobject.h \
  ../../include/cpacket.h \
  ../../include/cpar.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cqueue.h \
  ../../include/cregistrationlist.h \
  ../../include/cscheduler.h \
  ../../include/csimplemodule.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstddev.h \
  ../../include/cstringpool.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/cwatch.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/intlimits.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/timeutil.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  ../envir/envirdefs.h \
  ../envir/json.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  cmddefs.h \
  serializer.h
