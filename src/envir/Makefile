
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
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cregistrationlist.h \
  ../../include/crng.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  akaroarng.h \
  envirdefs.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  akoutvectormgr.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  fileoutvectormgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  runattributes.h
$O/appreg.o: appreg.cc \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cregistrationlist.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  appreg.h \
  envirdefs.h
$O/args.o: args.cc \
  args.h \
  envirdefs.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/cxmlelement.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  ../nedxml/nedxmldefs.h \
  ../nedxml/saxparser.h \
  cxmldoccache.h \
  envirdefs.h
$O/envirbase.o: envirbase.cc \
  ../../include/carray.h \
  ../../include/ccanvas.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/ccoroutine.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cdynamicexpression.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/chasher.h \
  ../../include/checkandcast.h \
  ../../include/chistogram.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessage.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cmsgpar.h \
  ../../include/cnamedobject.h \
  ../../include/cnedfunction.h \
  ../../include/cnedmathfunction.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cparsimcomm.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/cresultrecorder.h \
  ../../include/crng.h \
  ../../include/cscheduler.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstddev.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/cxmlelement.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/timeutil.h \
  ../../include/random.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  ../common/ver.h \
  ../nedxml/nedelement.h \
  ../nedxml/nederror.h \
  ../nedxml/nedparser.h \
  ../nedxml/nedxmldefs.h \
  ../nedxml/nedyydefs.h \
  appreg.h \
  args.h \
  cxmldoccache.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  httpserver.h \
  intervals.h \
  logformatter.h \
  mongoose.h \
  mongoosehttpserver.h \
  objectprinter.h \
  statisticparser.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  ../../include/carray.h \
  ../../include/cchannel.h \
  ../../include/cclassdescriptor.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cdisplaystring.h \
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
  ../../include/cownedobject.h \
  ../../include/cpacket.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimplemodule.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  eventlogwriter.h \
  intervals.h \
  logformatter.h \
  objectprinter.h
$O/eventlogwriter.o: eventlogwriter.cc \
  ../../include/ccomponent.h \
  ../../include/cconfigoption.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  eventlogwriter.h
$O/evmain.o: evmain.cc \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  ../common/ver.h \
  envirdefs.h \
  startup.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstddev.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  fileoutscalarmgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  runattributes.h
$O/fileoutvectormgr.o: fileoutvectormgr.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  fileoutvectormgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  runattributes.h
$O/filesnapshotmgr.o: filesnapshotmgr.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  filesnapshotmgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h
$O/fsutils.o: fsutils.cc \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  fsutils.h
$O/httpserver.o: httpserver.cc \
  ../../include/cconfiguration.h \
  ../../include/cexception.h \
  ../../include/clifecyclelistener.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  httpserver.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  fileoutvectormgr.h \
  indexedfileoutvectormgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  runattributes.h
$O/inifilereader.o: inifilereader.cc \
  ../../include/cconfigreader.h \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  fsutils.h \
  inifilereader.h
$O/intervals.o: intervals.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h
$O/json.o: json.cc \
  ../../include/platdep/platdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  envirdefs.h \
  json.h
$O/logformatter.o: logformatter.cc \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/chasher.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  logformatter.h
$O/main.o: main.cc \
  envirdefs.h
$O/matchablefield.o: matchablefield.cc \
  ../../include/cclassdescriptor.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  matchablefield.h
$O/matchableobject.o: matchableobject.cc \
  ../../include/cclassdescriptor.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cownedobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  matchableobject.h
$O/mongoosehttpserver.o: mongoosehttpserver.cc \
  ../../include/cconfiguration.h \
  ../../include/cexception.h \
  ../../include/clifecyclelistener.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  httpserver.h \
  mongoose.h \
  mongoosehttpserver.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/carray.h \
  ../../include/cclassdescriptor.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  matchablefield.h \
  matchableobject.h \
  objectprinter.h
$O/opp_run.o: opp_run.cc \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h
$O/runattributes.o: runattributes.cc \
  ../../include/carray.h \
  ../../include/ccomponent.h \
  ../../include/cconfigoption.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  runattributes.h
$O/scenario.o: scenario.cc \
  ../../include/cconfigreader.h \
  ../../include/cconfiguration.h \
  ../../include/cexception.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  scenario.h \
  sectionbasedconfig.h \
  valueiterator.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  ../../include/cconfigoption.h \
  ../../include/cconfigreader.h \
  ../../include/cconfiguration.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cregistrationlist.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  scenario.h \
  sectionbasedconfig.h \
  valueiterator.h
$O/speedometer.o: speedometer.cc \
  ../../include/platdep/platdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  envirdefs.h \
  speedometer.h
$O/startup.o: startup.cc \
  ../../include/ccomponent.h \
  ../../include/cconfigoption.h \
  ../../include/cconfigreader.h \
  ../../include/cconfiguration.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/clifecyclelistener.h \
  ../../include/clistener.h \
  ../../include/clog.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/cnamedobject.h \
  ../../include/cnedvalue.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/crng.h \
  ../../include/csimulation.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/distrib.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/random.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  akaroarng.h \
  akoutvectormgr.h \
  appreg.h \
  args.h \
  envirdefs.h \
  fileoutscalarmgr.h \
  fileoutvectormgr.h \
  filesnapshotmgr.h \
  fsutils.h \
  indexedfileoutvectormgr.h \
  inifilereader.h \
  intervals.h \
  matchableobject.h \
  runattributes.h \
  sectionbasedconfig.h \
  speedometer.h \
  startup.h
$O/statisticparser.o: statisticparser.cc \
  envirdefs.h \
  statisticparser.h
$O/valueiterator.o: valueiterator.cc \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  valueiterator.h
$O/visitor.o: visitor.cc \
  ../../include/carray.h \
  ../../include/cchannel.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cevent.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cfsm.h \
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
  ../../include/cpar.h \
  ../../include/cqueue.h \
  ../../include/cregistrationlist.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstringpool.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/cwatch.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/regmacros.h \
  ../../include/simkerneldefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simutil.h \
  envirdefs.h \
  matchableobject.h \
  visitor.h
