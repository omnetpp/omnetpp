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
  ../../include/cdensityestbase.h \
  ../../include/cpar.h \
  ../../include/cmessage.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cproperty.h \
  ../../include/csimulation.h \
  ../../include/checkandcast.h \
  ../../include/cenum.h \
  ../../include/coutvector.h \
  cmddefs.h \
  ../../include/ccomponent.h \
  ../../include/cobjectfactory.h \
  ../../include/cresultlistener.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/cqueue.h \
  ../../include/globals.h \
  ../envir/matchableobject.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/cexpression.h \
  ../../include/cresultfilter.h \
  ../envir/logformatter.h \
  ../envir/speedometer.h \
  ../../include/cnedmathfunction.h \
  ../../include/cmsgpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cobject.h \
  ../envir/appreg.h \
  ../envir/intervals.h \
  ../../include/errmsg.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cscheduler.h \
  ../../include/cchannel.h \
  ../../include/cconfigoption.h \
  ../envir/visitor.h \
  ../../include/platdep/timeutil.h \
  ../../include/cenvir.h \
  ../../include/cresultrecorder.h \
  ../../include/simtime.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../envir/json.h \
  ../../include/cexception.h \
  ../../include/regmacros.h \
  ../envir/envirbase.h \
  ../../include/carray.h \
  ../../include/cownedobject.h \
  ../envir/args.h \
  ../../include/cregistrationlist.h \
  ../../include/cmessageheap.h \
  ../../include/csimplemodule.h \
  serializer.h \
  ../envir/httpserver.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cmodule.h \
  ../../include/simutil.h \
  ../../include/cevent.h \
  ../../include/cproperties.h \
  ../envir/objectprinter.h \
  ../../include/ccomponenttype.h \
  ../../include/cclassdescriptor.h \
  ../../include/clog.h \
  ../../include/clistener.h \
  ../../include/cstringpool.h \
  ../../include/cstddev.h \
  cmdenv.h \
  ../../include/clifetimelistener.h \
  ../../include/envirext.h \
  ../../include/cnamedobject.h \
  ../../include/cwatch.h \
  ../envir/envirdefs.h \
  ../../include/cstatistic.h \
  ../../include/cdisplaystring.h \
  ../../include/cgate.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h
$O/serializer.o: serializer.cc \
  ../../include/globals.h \
  ../../include/cqueue.h \
  ../envir/matchableobject.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime_t.h \
  ../../include/csimulation.h \
  ../../include/cenum.h \
  ../../include/coutvector.h \
  cmddefs.h \
  ../../include/cpacket.h \
  ../../include/ccomponent.h \
  ../../include/cobjectfactory.h \
  ../../include/cdensityestbase.h \
  ../../include/cmessage.h \
  ../../include/cpar.h \
  ../../include/cproperty.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cscheduler.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/errmsg.h \
  ../../include/cnedmathfunction.h \
  ../../include/cdefaultlist.h \
  ../../include/cmsgpar.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cexpression.h \
  ../../include/cmodule.h \
  ../../include/simutil.h \
  ../../include/cregistrationlist.h \
  ../../include/csimplemodule.h \
  ../../include/cmessageheap.h \
  serializer.h \
  ../envir/json.h \
  ../../include/platdep/platmisc.h \
  ../../include/carray.h \
  ../../include/cexception.h \
  ../../include/regmacros.h \
  ../../include/cownedobject.h \
  ../../include/platdep/timeutil.h \
  ../../include/cenvir.h \
  ../../include/cvisitor.h \
  ../../include/simtime.h \
  ../../include/cstatistic.h \
  ../../include/cdisplaystring.h \
  ../../include/cgate.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/cnamedobject.h \
  ../envir/envirdefs.h \
  ../../include/cwatch.h \
  ../../include/cstringpool.h \
  ../../include/cstddev.h \
  ../../include/clistener.h \
  ../../include/clifetimelistener.h \
  ../../include/cproperties.h \
  ../../include/cevent.h \
  ../../include/ccomponenttype.h \
  ../../include/clog.h \
  ../../include/cclassdescriptor.h
