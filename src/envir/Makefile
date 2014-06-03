
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

OBJS= $O/appreg.o $O/args.o $O/startup.o $O/evmain.o $O/logformatter.o $O/envirbase.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/fileoutvectormgr.o $O/fileoutscalarmgr.o $O/filesnapshotmgr.o \
      $O/indexedfileoutvectormgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/cxmldoccache.o $O/eventlogwriter.o $O/objectprinter.o \
      $O/eventlogfilemgr.o $O/runattributes.o $O/intervals.o \
      $O/statisticparser.o $O/visitor.o

#TODO make configurable; put into separate lib? (src/httpserver)
OBJS += $O/httpserver.o $O/mongoosehttpserver.o $O/mongoose.o  $O/json.o

GENERATED_SOURCES= eventlogwriter.cc eventlogwriter.h

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

eventlogwriter.cc eventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

$O/envirbase.o: eventlogwriter.h

$O/mongoose.o : mongoose.c mongoose.h
	$(CXX) -c $(COPTS) -o $@ -DHAVE_STRTOUI64 mongoose.c

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

depend: $(GENERATED_SOURCES)
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../../include -P\$$O/ -- *.cc


clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f opp_run$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/akaroarng.o: akaroarng.cc \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/cregistrationlist.h \
  ../../include/regmacros.h \
  ../../include/simutil.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/platdep/platdefs.h \
  ../../include/cenvir.h \
  akaroarng.h \
  ../../include/opp_string.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platmisc.h \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/simtime.h \
  ../../include/errmsg.h \
  ../../include/crng.h \
  ../../include/cvisitor.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/cownedobject.h \
  ../../include/clifetimelistener.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  ../../include/clistener.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  envirdefs.h \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  ../../include/cmodule.h \
  envirbase.h \
  ../../include/clistener.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/carray.h \
  ../../include/opp_string.h \
  objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/csimulation.h \
  ../../include/cevent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/envirext.h \
  akoutvectormgr.h \
  ../../include/clog.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobjectfactory.h \
  ../../include/clifetimelistener.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cstatistic.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/ccomponent.h \
  ../../include/errmsg.h \
  ../../include/simtime.h \
  args.h \
  intervals.h \
  fileoutvectormgr.h \
  ../../include/cresultlistener.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  runattributes.h \
  ../../include/cgate.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  eventlogfilemgr.h \
  ../../include/globals.h \
  logformatter.h
$O/appreg.o: appreg.cc \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/clistener.h \
  ../../include/cvisitor.h \
  envirdefs.h \
  appreg.h \
  ../../include/cregistrationlist.h \
  ../../include/simutil.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/errmsg.h \
  ../../include/platdep/platmisc.h \
  ../../include/clifetimelistener.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cexception.h \
  ../../include/cenvir.h
$O/args.o: args.cc \
  envirdefs.h \
  args.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  cxmldoccache.h \
  ../../include/cxmlelement.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h \
  ../../include/clifetimelistener.h \
  ../../include/platdep/platmisc.h \
  ../../include/simtime.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/simutil.h \
  ../nedxml/saxparser.h \
  ../../include/cvisitor.h \
  envirdefs.h \
  ../nedxml/nedxmldefs.h \
  ../../include/clistener.h \
  ../../include/cobject.h
$O/envirbase.o: envirbase.cc \
  intervals.h \
  ../../include/simtime.h \
  statisticparser.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/clog.h \
  ../../include/cmessage.h \
  ../../include/cnedfunction.h \
  ../../include/globals.h \
  ../../include/cnedvalue.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/ccoroutine.h \
  ../../include/cobject.h \
  ../../include/clistener.h \
  ../nedxml/nedparser.h \
  ../nedxml/nedelement.h \
  ../nedxml/nedxmldefs.h \
  mongoose.h \
  ../../include/chasher.h \
  envirdefs.h \
  ../../include/cstddev.h \
  ../../include/cnedmathfunction.h \
  ../../include/platdep/timeutil.h \
  ../../include/simutil.h \
  ../../include/cevent.h \
  ../../include/cproperty.h \
  ../../include/cconfiguration.h \
  ../../include/cregistrationlist.h \
  ../../include/carray.h \
  args.h \
  ../../include/errmsg.h \
  ../../include/cmsgpar.h \
  ../../include/ccomponent.h \
  ../../include/cstatistic.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/onstartup.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/cownedobject.h \
  ../../include/cscheduler.h \
  logformatter.h \
  ../../include/cresultrecorder.h \
  ../common/ver.h \
  ../../include/simkerneldefs.h \
  eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/cdensityestbase.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  ../../include/cxmlelement.h \
  ../../include/cdynamicexpression.h \
  cxmldoccache.h \
  ../../include/cexpression.h \
  ../../include/cparsimcomm.h \
  ../../include/cresultlistener.h \
  ../nedxml/nederror.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/chistogram.h \
  ../../include/cmodule.h \
  envirbase.h \
  httpserver.h \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  ../../include/crng.h \
  ../../include/envirext.h \
  mongoosehttpserver.h \
  ../../include/ccomponenttype.h \
  appreg.h \
  ../../include/platdep/intxtypes.h \
  objectprinter.h \
  ../../include/csimulation.h \
  ../nedxml/nedyydefs.h \
  ../../include/opp_string.h \
  ../../include/random.h \
  ../../include/platdep/platdefs.h \
  ../../include/cproperties.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  ../../include/cnedmathfunction.h \
  ../../include/cevent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/cclassdescriptor.h \
  ../../include/simutil.h \
  ../../include/carray.h \
  ../../include/cobject.h \
  ../../include/clistener.h \
  envirdefs.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/cchannel.h \
  eventlogwriter.h \
  ../../include/clifetimelistener.h \
  ../../include/clog.h \
  ../../include/cmessage.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponenttype.h \
  ../../include/envirext.h \
  ../../include/opp_string.h \
  ../../include/csimulation.h \
  objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpacket.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  ../../include/cmodule.h \
  ../../include/csimplemodule.h \
  envirbase.h \
  ../../include/ctimestampedvalue.h \
  ../../include/simkerneldefs.h \
  eventlogfilemgr.h \
  ../../include/cdisplaystring.h \
  logformatter.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  ../../include/cgate.h \
  ../../include/cexpression.h \
  ../../include/cresultlistener.h \
  ../../include/errmsg.h \
  ../../include/cmsgpar.h \
  args.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/ccomponent.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h
$O/eventlogwriter.o: eventlogwriter.cc \
  ../../include/cexpression.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime_t.h \
  ../../include/cgate.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/csimulation.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmodule.h \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/globals.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/clog.h \
  ../../include/simtime.h \
  eventlogwriter.h \
  ../../include/simutil.h \
  ../../include/cregistrationlist.h \
  ../../include/clistener.h \
  envirdefs.h \
  ../../include/cobject.h
$O/evmain.o: evmain.cc \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cexception.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/simutil.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  startup.h \
  ../../include/platdep/platmisc.h \
  ../common/ver.h \
  ../../include/simkerneldefs.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cvisitor.h \
  ../../include/cstringpool.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/cexpression.h \
  ../../include/cresultlistener.h \
  logformatter.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  ../../include/cgate.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  ../../include/cdensityestbase.h \
  ../../include/onstartup.h \
  ../../include/cmessageheap.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  args.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/platdep/platmisc.h \
  ../../include/cstatistic.h \
  objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/csimulation.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/envirext.h \
  ../../include/ccomponenttype.h \
  envirbase.h \
  ../../include/cmodule.h \
  ../../include/cvisitor.h \
  ../../include/cconfigoption.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  runattributes.h \
  fileoutscalarmgr.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/globals.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/clog.h \
  intervals.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cstddev.h \
  ../../include/simutil.h \
  ../../include/cconfiguration.h \
  ../../include/cregistrationlist.h \
  ../../include/cevent.h \
  ../../include/clistener.h \
  envirdefs.h \
  ../../include/cobject.h
$O/fileoutvectormgr.o: fileoutvectormgr.cc \
  envirbase.h \
  ../../include/cmodule.h \
  ../../include/cvisitor.h \
  ../../include/cconfigoption.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/csimulation.h \
  objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/envirext.h \
  ../../include/ccomponenttype.h \
  ../../include/onstartup.h \
  ../../include/cmessageheap.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  args.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexpression.h \
  ../../include/cresultlistener.h \
  fileoutvectormgr.h \
  logformatter.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime_t.h \
  ../../include/cgate.h \
  ../../include/regmacros.h \
  ../../include/clistener.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/carray.h \
  ../../include/simutil.h \
  ../../include/cconfiguration.h \
  ../../include/cregistrationlist.h \
  ../../include/cevent.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/clog.h \
  intervals.h \
  ../../include/simtime.h \
  runattributes.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/globals.h
$O/filesnapshotmgr.o: filesnapshotmgr.cc \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  envirbase.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  objectprinter.h \
  ../../include/csimulation.h \
  ../../include/platdep/intxtypes.h \
  ../../include/envirext.h \
  ../../include/cownedobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cpar.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  ../../include/errmsg.h \
  args.h \
  ../../include/cresultlistener.h \
  ../../include/cexpression.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  logformatter.h \
  envirdefs.h \
  ../../include/clistener.h \
  ../../include/cobject.h \
  ../../include/carray.h \
  ../../include/cevent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/clifetimelistener.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  filesnapshotmgr.h \
  ../../include/globals.h
$O/fsutils.o: fsutils.cc \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platmisc.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  ../../include/cobject.h \
  fsutils.h \
  envirdefs.h \
  ../../include/simutil.h \
  ../../include/simtime.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h
$O/httpserver.o: httpserver.cc \
  ../../include/cvisitor.h \
  httpserver.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h \
  ../../include/envirext.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/simutil.h \
  ../../include/cconfiguration.h \
  ../../include/clifetimelistener.h \
  ../../include/simtime.h \
  ../../include/cexception.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  ../../include/cexpression.h \
  ../../include/cresultlistener.h \
  fileoutvectormgr.h \
  ../../include/simkerneldefs.h \
  eventlogfilemgr.h \
  logformatter.h \
  ../../include/cgate.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cmessageheap.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  args.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/cstatistic.h \
  ../../include/ccomponent.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  objectprinter.h \
  ../../include/csimulation.h \
  ../../include/platdep/platdefs.h \
  ../../include/envirext.h \
  ../../include/cvisitor.h \
  ../../include/cconfigoption.h \
  envirbase.h \
  ../../include/cmodule.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  runattributes.h \
  ../../include/cenvir.h \
  ../../include/globals.h \
  ../../include/clifetimelistener.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/carray.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/cevent.h \
  ../../include/simutil.h \
  ../../include/clistener.h \
  envirdefs.h \
  indexedfileoutvectormgr.h \
  ../../include/cobject.h
$O/inifilereader.o: inifilereader.cc \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h \
  inifilereader.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h \
  ../../include/cconfigreader.h \
  ../../include/cexception.h \
  ../../include/simtime.h \
  ../../include/simutil.h \
  envirdefs.h \
  fsutils.h
$O/intervals.o: intervals.cc \
  ../../include/envirext.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  objectprinter.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  envirbase.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  logformatter.h \
  ../../include/cresultlistener.h \
  ../../include/cexpression.h \
  ../../include/cpar.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  ../../include/errmsg.h \
  args.h \
  ../../include/cownedobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cevent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/carray.h \
  ../../include/cobject.h \
  envirdefs.h \
  ../../include/clistener.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/clifetimelistener.h
$O/json.o: json.cc \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  json.h \
  ../../include/simtime.h
$O/logformatter.o: logformatter.cc \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/cmodule.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cgate.h \
  ../../include/simkerneldefs.h \
  logformatter.h \
  ../../include/cexpression.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/ccomponent.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessageheap.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/cobject.h \
  ../../include/chasher.h \
  envirdefs.h \
  ../../include/clistener.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/simtime.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/clifetimelistener.h
$O/main.o: main.cc \
  envirdefs.h
$O/matchablefield.o: matchablefield.cc \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cexception.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/cclassdescriptor.h \
  ../../include/simutil.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  ../../include/platdep/platmisc.h \
  ../../include/simkerneldefs.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cvisitor.h \
  matchablefield.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h
$O/matchableobject.o: matchableobject.cc \
  ../../include/cclassdescriptor.h \
  ../../include/simutil.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/cexception.h \
  matchableobject.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/cvisitor.h \
  ../../include/cstringpool.h \
  ../../include/simkerneldefs.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  ../../include/platdep/platmisc.h
$O/mongoosehttpserver.o: mongoosehttpserver.cc \
  ../../include/simtime.h \
  ../../include/clifetimelistener.h \
  ../../include/cexception.h \
  ../../include/cobject.h \
  envirdefs.h \
  mongoose.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h \
  ../../include/cvisitor.h \
  httpserver.h \
  mongoosehttpserver.h \
  ../../include/envirext.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  ../../include/cexpression.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  ../../include/cgate.h \
  ../../include/cvisitor.h \
  ../../include/cmodule.h \
  matchablefield.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  objectprinter.h \
  ../../include/csimulation.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/clifetimelistener.h \
  matchableobject.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/globals.h \
  ../../include/clistener.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/carray.h \
  ../../include/cclassdescriptor.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h
$O/opp_run.o: opp_run.cc \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/intxtypes.h
$O/runattributes.o: runattributes.cc \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/ccomponent.h \
  ../../include/errmsg.h \
  args.h \
  ../../include/cownedobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/regmacros.h \
  eventlogfilemgr.h \
  ../../include/simkerneldefs.h \
  logformatter.h \
  ../../include/cresultlistener.h \
  ../../include/cexpression.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  envirbase.h \
  ../../include/envirext.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  objectprinter.h \
  ../../include/csimulation.h \
  ../../include/platdep/intxtypes.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/clifetimelistener.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  runattributes.h \
  ../../include/cobject.h \
  envirdefs.h \
  ../../include/clistener.h \
  ../../include/cevent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/carray.h
$O/scenario.o: scenario.cc \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/simtime.h \
  ../../include/cconfigreader.h \
  ../../include/cexception.h \
  sectionbasedconfig.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platdefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  scenario.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  valueiterator.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  ../../include/simutil.h \
  ../../include/cconfiguration.h \
  ../../include/cregistrationlist.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/cexception.h \
  ../../include/cconfigreader.h \
  ../../include/globals.h \
  sectionbasedconfig.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/cconfigoption.h \
  ../../include/cvisitor.h \
  ../../include/cstringpool.h \
  valueiterator.h \
  ../../include/simkerneldefs.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  scenario.h \
  ../../include/onstartup.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  ../../include/platdep/platmisc.h
$O/speedometer.o: speedometer.cc \
  envirdefs.h \
  ../../include/simtime.h \
  speedometer.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime_t.h
$O/startup.o: startup.cc \
  ../../include/cvisitor.h \
  ../../include/crng.h \
  ../../include/cconfigoption.h \
  ../../include/cmodule.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  speedometer.h \
  ../../include/platdep/platdefs.h \
  ../../include/random.h \
  ../../include/opp_string.h \
  ../../include/platdep/intxtypes.h \
  ../../include/csimulation.h \
  appreg.h \
  ../../include/envirext.h \
  ../../include/cownedobject.h \
  ../../include/cmessageheap.h \
  ../../include/cobjectfactory.h \
  ../../include/onstartup.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpar.h \
  ../../include/ccomponent.h \
  inifilereader.h \
  ../../include/distrib.h \
  startup.h \
  ../../include/errmsg.h \
  args.h \
  fileoutvectormgr.h \
  ../../include/cexpression.h \
  ../../include/simtime_t.h \
  ../../include/cgate.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  ../../include/clistener.h \
  fsutils.h \
  indexedfileoutvectormgr.h \
  ../../include/cobject.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/simutil.h \
  ../../include/clog.h \
  akoutvectormgr.h \
  ../../include/cnamedobject.h \
  ../../include/clifetimelistener.h \
  matchableobject.h \
  ../../include/simtime.h \
  intervals.h \
  ../../include/cconfigreader.h \
  akaroarng.h \
  ../../include/cenvir.h \
  fileoutscalarmgr.h \
  ../../include/cexception.h \
  runattributes.h \
  sectionbasedconfig.h \
  ../../include/globals.h \
  filesnapshotmgr.h
$O/statisticparser.o: statisticparser.cc \
  envirdefs.h \
  statisticparser.h
$O/valueiterator.o: valueiterator.cc \
  ../../include/cexception.h \
  ../../include/simtime.h \
  ../../include/simutil.h \
  envirdefs.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h \
  valueiterator.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/opp_string.h
$O/visitor.o: visitor.cc \
  ../../include/clifetimelistener.h \
  matchableobject.h \
  ../../include/coutvector.h \
  ../../include/cmessage.h \
  ../../include/clog.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cwatch.h \
  ../../include/cchannel.h \
  ../../include/cqueue.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/globals.h \
  ../../include/clistener.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/carray.h \
  ../../include/cnedmathfunction.h \
  ../../include/cevent.h \
  ../../include/cregistrationlist.h \
  ../../include/simutil.h \
  ../../include/cobjectfactory.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/errmsg.h \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  ../../include/cpar.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponent.h \
  ../../include/cexpression.h \
  ../../include/ctimestampedvalue.h \
  ../../include/simkerneldefs.h \
  ../../include/cfsm.h \
  ../../include/cgate.h \
  ../../include/regmacros.h \
  ../../include/simtime_t.h \
  ../../include/cvisitor.h \
  ../../include/cmodule.h \
  visitor.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/csimulation.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/ccomponenttype.h
