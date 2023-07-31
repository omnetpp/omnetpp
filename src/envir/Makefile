
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
MAINLIBNAME=liboppmain$D
LIBNAME=liboppenvir$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/envir
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

ifeq ("$(PLATFORM)","win32.x86")
PSAPI_LIB=-lpsapi
endif

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../sim -I../common -DBUILDING_ENVIR -DHAVE_DLOPEN=$(HAVE_DLOPEN) -DSHARED_LIB_SUFFIX="\"$(SHARED_LIB_SUFFIX)\""

IMPLIBS= -L$L -loppsim$D -loppnedxml$D -loppcommon$D $(XML_LIBS)

OBJS= $O/appreg.o $O/args.o $O/startup.o $O/evmain.o $O/envirbase.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/fileoutvectormgr.o $O/fileoutscalarmgr.o $O/filesnapshotmgr.o \
      $O/indexedfileoutvectormgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/cxmldoccache.o $O/eventlogwriter.o $O/objectprinter.o \
      $O/eventlogfilemgr.o $O/runattributes.o $O/intervals.o \
      $O/statisticparser.o $O/visitor.o


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
all : $L/$(LIBNAME)$(LIB_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(Q)$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX) envir-implib
	@mkdir -p $L
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

envir-implib : $(OBJS)
	@mkdir -p $L
	-$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) 2>/dev/null
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

%ventlogwriter.cc %ventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

$O/envirbase.o: eventlogwriter.h

# Build a static lib that contains the main() function used with executable or dynamically linked models
$L/$(MAINLIBNAME)$(A_LIB_SUFFIX) : $O/main.o
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX) $O/main.o
	$(Q)$(RANLIB) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(MAINLIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(MAINLIBNAME)$(A_LIB_SUFFIX) && $(LN) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX) $(OMNETPP_LIB_DIR)

# default opp_run target - called only at the end once tkenv and cmdenv is done
# opp_run is useless with static libs
ifeq ("$(LIB_SUFFIX)","$(SHARED_LIB_SUFFIX)")
ifeq ("$(MODE)","debug")
opp_run_executable: $O/opp_run$(EXE_SUFFIX)
$O/opp_run$(EXE_SUFFIX) : $O/opp_run.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(qecho) "Creating executable: $@"
	$(Q)$(CXX) $(LDFLAGS) -o "$@" $O/opp_run.o $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS) $(PSAPI_LIB)
	$(Q)cp $O/opp_run$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
else
opp_run_executable: $O/opp_run_release$(EXE_SUFFIX)
$O/opp_run_release$(EXE_SUFFIX) : $O/main.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(qecho) "Creating executable: $@"
	$(Q)$(CXX) $(LDFLAGS) -o "$@" $O/main.o $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS)
	$(Q)cp $O/opp_run_release$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	$(Q)cp $O/opp_run_release$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)/opp_run$(EXE_SUFFIX)
endif

$O/opp_run.o : opp_run.cc
	@# Note: not in OBJS because main.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) $(COPTS) -c -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_INCL_DIR)/platdep" -I../sim  -I../common -o$O/opp_run.o opp_run.cc

$O/main.o : main.cc
	@# Note: not in OBJS because main.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) $(COPTS) -c -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_INCL_DIR)/platdep" -I../sim  -I../common -o$O/main.o main.cc

else
opp_run_executable:
	@echo "Note: We don't create opp_run.exe when building static libs"
endif

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../../include -P\$$O/ -- *.cc


clean:
	$(qecho) Cleaning...
	$(Q)rm -f eventlogwriter.cc eventlogwriter.h
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f opp_run$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/akaroarng.o: akaroarng.cc \
  envirdefs.h \
  ../../include/simutil.h \
  ../../include/onstartup.h \
  ../../include/platdep/platmisc.h \
  ../../include/cobjectfactory.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/csimulation.h \
  ../../include/cregistrationlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/clistener.h \
  ../../include/cenvir.h \
  akaroarng.h \
  ../../include/cnamedobject.h \
  ../../include/globals.h \
  ../../include/crng.h \
  ../../include/simtime_t.h \
  ../../include/cconfiguration.h \
  ../../include/platdep/intxtypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/regmacros.h \
  ../../include/cvisitor.h \
  ../../include/cmessageheap.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/cstatistic.h \
  args.h \
  ../../include/cpar.h \
  ../../include/cvisitor.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  runattributes.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/errmsg.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/envirext.h \
  ../../include/cresultlistener.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  objectprinter.h \
  ../../include/simkerneldefs.h \
  fileoutvectormgr.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  envirdefs.h \
  intervals.h \
  eventlogfilemgr.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  akoutvectormgr.h \
  ../../include/cexpression.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  envirbase.h \
  ../../include/simutil.h
$O/appreg.o: appreg.cc \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/simtime_t.h \
  ../../include/cvisitor.h \
  appreg.h \
  ../../include/cenvir.h \
  ../../include/platdep/platdefs.h \
  ../../include/cnamedobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  envirdefs.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/regmacros.h \
  ../../include/cstringpool.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cmessageheap.h \
  ../../include/clistener.h \
  ../../include/globals.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/simutil.h \
  ../../include/platdep/platmisc.h
$O/args.o: args.cc \
  args.h \
  envirdefs.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/csimulation.h \
  ../../include/simutil.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cxmlelement.h \
  ../../include/regmacros.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/cmessageheap.h \
  ../../include/clistener.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  envirdefs.h \
  ../nedxml/saxparser.h \
  ../../include/simtime_t.h \
  ../nedxml/nedxmldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  cxmldoccache.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h
$O/envirbase.o: envirbase.cc \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../common/ver.h \
  ../../include/cmessageheap.h \
  ../../include/cnedvalue.h \
  ../../include/random.h \
  ../../include/cconfigoption.h \
  ../../include/clistener.h \
  ../../include/globals.h \
  ../../include/cresultrecorder.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccoroutine.h \
  ../nedxml/nedxmldefs.h \
  ../../include/simtime_t.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  ../../include/cscheduler.h \
  ../../include/cobjectfactory.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/onstartup.h \
  ../../include/chasher.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cxmlelement.h \
  ../../include/regmacros.h \
  ../../include/cproperty.h \
  ../../include/chistogram.h \
  ../../include/cparsimcomm.h \
  ../../include/cexpression.h \
  ../../include/cnedmathfunction.h \
  statisticparser.h \
  ../../include/cstddev.h \
  ../../include/cnedfunction.h \
  ../nedxml/nedelement.h \
  ../../include/platdep/timeutil.h \
  ../../include/ccomponent.h \
  envirbase.h \
  ../nedxml/nedyydefs.h \
  ../nedxml/nederror.h \
  ../../include/platdep/platmisc.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cstatistic.h \
  cxmldoccache.h \
  ../../include/cvisitor.h \
  args.h \
  ../../include/cresultlistener.h \
  ../../include/envirext.h \
  appreg.h \
  ../../include/cnamedobject.h \
  ../../include/crng.h \
  ../nedxml/nedparser.h \
  ../../include/cdensityestbase.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  objectprinter.h \
  eventlogfilemgr.h \
  envirdefs.h \
  ../../include/cproperties.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  ../../include/cmessageheap.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/ctimestampedvalue.h \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessage.h \
  ../../include/simutil.h \
  ../../include/cpar.h \
  ../../include/cconfiguration.h \
  eventlogwriter.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/csimplemodule.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  intervals.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/regmacros.h \
  ../../include/cnedmathfunction.h \
  ../../include/cexpression.h \
  ../../include/platdep/platmisc.h \
  envirbase.h \
  ../../include/ccomponent.h \
  ../../include/cchannel.h \
  args.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cnamedobject.h \
  ../../include/cresultlistener.h \
  ../../include/envirext.h \
  objectprinter.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/cdisplaystring.h \
  ../../include/cclassdescriptor.h \
  eventlogfilemgr.h \
  envirdefs.h
$O/eventlogwriter.o: eventlogwriter.cc \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  ../../include/globals.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/csimulation.h \
  ../../include/cregistrationlist.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  eventlogwriter.h \
  ../../include/simtime_t.h \
  ../../include/cpar.h \
  ../../include/onstartup.h \
  ../../include/simtime.h \
  ../../include/cobjectfactory.h \
  ../../include/cgate.h \
  ../../include/cexpression.h \
  ../../include/regmacros.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponent.h \
  ../../include/platdep/platmisc.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cvisitor.h \
  envirdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h
$O/evmain.o: evmain.cc \
  ../../include/simtime.h \
  ../../include/errmsg.h \
  startup.h \
  ../../include/simtime_t.h \
  ../../include/platdep/platdefs.h \
  ../../include/simutil.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../common/ver.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  envirdefs.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/platdep/intxtypes.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/globals.h \
  ../../include/cconfigoption.h \
  ../../include/clistener.h \
  ../../include/cmessageheap.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  runattributes.h \
  ../../include/simtime_t.h \
  ../../include/cconfiguration.h \
  intervals.h \
  ../../include/onstartup.h \
  ../../include/cgate.h \
  ../../include/cobjectfactory.h \
  ../../include/simtime.h \
  ../../include/cstddev.h \
  ../../include/cexpression.h \
  ../../include/regmacros.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platmisc.h \
  envirbase.h \
  ../../include/ccomponent.h \
  ../../include/cdensityestbase.h \
  ../../include/cnamedobject.h \
  fileoutscalarmgr.h \
  ../../include/envirext.h \
  ../../include/cresultlistener.h \
  ../../include/cvisitor.h \
  args.h \
  ../../include/cstatistic.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  envirdefs.h \
  eventlogfilemgr.h \
  objectprinter.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h
$O/fileoutvectormgr.o: fileoutvectormgr.cc \
  ../../include/cmessageheap.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cconfigoption.h \
  ../../include/clistener.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../../include/cpar.h \
  ../../include/simtime_t.h \
  runattributes.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/cobjectfactory.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  fileoutvectormgr.h \
  intervals.h \
  ../../include/onstartup.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  envirbase.h \
  ../../include/cvisitor.h \
  args.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cnamedobject.h \
  ../../include/cresultlistener.h \
  ../../include/envirext.h \
  objectprinter.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  eventlogfilemgr.h
$O/filesnapshotmgr.o: filesnapshotmgr.cc \
  eventlogfilemgr.h \
  envirdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  objectprinter.h \
  ../../include/cresultlistener.h \
  ../../include/envirext.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  args.h \
  ../../include/cvisitor.h \
  envirbase.h \
  ../../include/ccomponent.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexpression.h \
  filesnapshotmgr.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  intervals.h \
  ../../include/simtime.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cmessageheap.h
$O/fsutils.o: fsutils.cc \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  ../../include/cvisitor.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platmisc.h \
  fsutils.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/simtime.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simutil.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  indexedfileoutvectormgr.h \
  ../../include/ccomponent.h \
  envirbase.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/simkerneldefs.h \
  ../../include/carray.h \
  ../../include/cexception.h \
  objectprinter.h \
  eventlogfilemgr.h \
  envirdefs.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstatistic.h \
  args.h \
  ../../include/cvisitor.h \
  ../../include/cresultlistener.h \
  ../../include/envirext.h \
  ../../include/cnamedobject.h \
  ../../include/csimulation.h \
  ../../include/cregistrationlist.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/cmessageheap.h \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  ../../include/globals.h \
  ../../include/simtime.h \
  fileoutvectormgr.h \
  ../../include/cobjectfactory.h \
  ../../include/cgate.h \
  ../../include/onstartup.h \
  intervals.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  runattributes.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h
$O/inifilereader.o: inifilereader.cc \
  envirdefs.h \
  ../../include/cconfigreader.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platmisc.h \
  fsutils.h \
  ../../include/platdep/intxtypes.h \
  inifilereader.h \
  ../../include/simtime.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h
$O/intervals.o: intervals.cc \
  ../../include/ccomponent.h \
  envirbase.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexpression.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  envirdefs.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  objectprinter.h \
  ../../include/cresultlistener.h \
  ../../include/envirext.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  args.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/csimulation.h \
  ../../include/cregistrationlist.h \
  ../../include/clistener.h \
  ../../include/globals.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  intervals.h \
  ../../include/simtime.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/cpar.h
$O/main.o: main.cc \
  envirdefs.h
$O/matchablefield.o: matchablefield.cc \
  matchablefield.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/cclassdescriptor.h \
  envirdefs.h \
  ../../include/cvisitor.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cnamedobject.h \
  ../../include/simutil.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h
$O/matchableobject.o: matchableobject.cc \
  ../../include/cclassdescriptor.h \
  envirdefs.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/simtime.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/simutil.h \
  matchableobject.h \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  ../../include/cpar.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  ../../include/csimulation.h \
  ../../include/cregistrationlist.h \
  matchableobject.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/globals.h \
  ../../include/clistener.h \
  objectprinter.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/cclassdescriptor.h \
  envirdefs.h \
  ../../include/cvisitor.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  ../../include/cnamedobject.h \
  matchablefield.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h
$O/opp_run.o: opp_run.cc \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h
$O/runattributes.o: runattributes.cc \
  ../../include/envirext.h \
  ../../include/cresultlistener.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  args.h \
  envirdefs.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  objectprinter.h \
  ../../include/cexpression.h \
  ../../include/regmacros.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponent.h \
  envirbase.h \
  ../../include/platdep/platmisc.h \
  ../../include/cenvir.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  ../../include/cconfiguration.h \
  runattributes.h \
  ../../include/simtime_t.h \
  ../../include/cpar.h \
  ../../include/onstartup.h \
  intervals.h \
  ../../include/simtime.h \
  ../../include/cobjectfactory.h \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h
$O/scenario.o: scenario.cc \
  ../../include/cconfigreader.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  ../../include/cvisitor.h \
  valueiterator.h \
  ../../include/cobject.h \
  ../../include/opp_string.h \
  sectionbasedconfig.h \
  ../../include/platdep/platmisc.h \
  scenario.h \
  ../../include/regmacros.h \
  ../../include/platdep/intxtypes.h \
  ../../include/simtime.h \
  ../../include/errmsg.h \
  ../../include/simtime_t.h \
  ../../include/cconfiguration.h \
  ../../include/platdep/platdefs.h \
  ../../include/simutil.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  sectionbasedconfig.h \
  scenario.h \
  ../../include/platdep/platmisc.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  valueiterator.h \
  ../../include/cnamedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/cconfigreader.h \
  envirdefs.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cconfigoption.h \
  ../../include/globals.h \
  ../../include/cregistrationlist.h \
  ../../include/simutil.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h
$O/speedometer.o: speedometer.cc \
  ../../include/simtime_t.h \
  ../../include/platdep/platdefs.h \
  speedometer.h \
  ../../include/simtime.h \
  ../../include/platdep/intxtypes.h \
  ../../include/simkerneldefs.h \
  envirdefs.h
$O/startup.o: startup.cc \
  inifilereader.h \
  ../../include/onstartup.h \
  intervals.h \
  fileoutvectormgr.h \
  ../../include/simtime.h \
  ../../include/cobjectfactory.h \
  ../../include/cgate.h \
  speedometer.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/cpar.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  runattributes.h \
  startup.h \
  ../../include/errmsg.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../../include/csimulation.h \
  ../../include/cregistrationlist.h \
  matchableobject.h \
  ../../include/globals.h \
  ../../include/distrib.h \
  ../../include/clistener.h \
  ../../include/cconfigoption.h \
  ../../include/cmessageheap.h \
  ../../include/random.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  envirdefs.h \
  ../../include/cconfigreader.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/crng.h \
  ../../include/cnamedobject.h \
  ../../include/envirext.h \
  fileoutscalarmgr.h \
  appreg.h \
  args.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  sectionbasedconfig.h \
  indexedfileoutvectormgr.h \
  ../../include/cexpression.h \
  filesnapshotmgr.h \
  akoutvectormgr.h \
  akaroarng.h \
  fsutils.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h
$O/statisticparser.o: statisticparser.cc \
  envirdefs.h \
  statisticparser.h
$O/valueiterator.o: valueiterator.cc \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  envirdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  valueiterator.h \
  ../../include/opp_string.h \
  ../../include/simutil.h \
  ../../include/simtime.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  ../../include/simtime_t.h
$O/visitor.o: visitor.cc \
  ../../include/ccomponent.h \
  ../../include/cchannel.h \
  ../../include/cqueue.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexpression.h \
  ../../include/cnedmathfunction.h \
  ../../include/cwatch.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/intxtypes.h \
  ../../include/regmacros.h \
  ../../include/cfsm.h \
  envirdefs.h \
  ../../include/carray.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstatistic.h \
  ../../include/cvisitor.h \
  ../../include/cmessage.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/csimulation.h \
  matchableobject.h \
  ../../include/cregistrationlist.h \
  ../../include/coutvector.h \
  ../../include/clistener.h \
  ../../include/ctimestampedvalue.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cgate.h \
  ../../include/cobjectfactory.h \
  ../../include/simtime.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/simtime_t.h \
  ../../include/errmsg.h \
  visitor.h \
  ../../include/cpar.h
