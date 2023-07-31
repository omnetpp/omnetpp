
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
      $O/statisticparser.o


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
all : $L/$(LIBNAME)$(LIB_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX)
	@mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX) envir-implib
	@mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

envir-implib : $(OBJS)
	@mkdir -p $L
	-$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) 2>/dev/null
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

%ventlogwriter.cc %ventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

$O/envirbase.o: eventlogwriter.h

# Build a static lib that contains the main() function used with executable or dynamically linked models
$L/$(MAINLIBNAME)$(A_LIB_SUFFIX) : $O/main.o
	@mkdir -p $L
	$(AR) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX) $O/main.o
	$(RANLIB) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(MAINLIBNAME)$(A_LIB_SUFFIX) $L
	rm -f $(OMNETPP_LIB_DIR)/$(MAINLIBNAME)$(A_LIB_SUFFIX) && $(LN) $L/$(MAINLIBNAME)$(A_LIB_SUFFIX) $(OMNETPP_LIB_DIR)

# default opp_run target - called only at the end once tkenv and cmdenv is done
# opp_run is useless with static libs
ifeq ("$(LIB_SUFFIX)","$(SHARED_LIB_SUFFIX)")
ifeq ("$(MODE)","debug")
opp_run_executable: $O/opp_run$(EXE_SUFFIX)
$O/opp_run$(EXE_SUFFIX) : $O/opp_run.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o "$@" $O/opp_run.o $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS) $(PSAPI_LIB)
	cp $O/opp_run$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
else
opp_run_executable: $O/opp_run_release$(EXE_SUFFIX)
$O/opp_run_release$(EXE_SUFFIX) : $O/main.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o "$@" $O/main.o $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS)
	cp $O/opp_run_release$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	cp $O/opp_run_release$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)/opp_run$(EXE_SUFFIX)
endif

$O/opp_run.o : opp_run.cc
	@# Note: not in OBJS because main.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(CXX) $(COPTS) -c -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_INCL_DIR)/platdep" -I../sim  -I../common -o$O/opp_run.o opp_run.cc

$O/main.o : main.cc
	@# Note: not in OBJS because main.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(CXX) $(COPTS) -c -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_INCL_DIR)/platdep" -I../sim  -I../common -o$O/main.o main.cc

else
opp_run_executable:
	@echo "Note: We don't create opp_run.exe when building static libs"
endif

depend:
	$(MAKEDEPEND) -I. -I../../include -P\$$O/ -- *.cc


clean:
	rm -f eventlogwriter.cc eventlogwriter.h
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	cd $(OMNETPP_BIN_DIR) && rm -f opp_run$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/akaroarng.o: akaroarng.cc \
  ../../include/regmacros.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cstringpool.h \
  envirdefs.h \
  ../../include/globals.h \
  akaroarng.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cvisitor.h \
  ../../include/cregistrationlist.h \
  ../../include/cobjectfactory.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/platdep/platmisc.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/regmacros.h \
  akoutvectormgr.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cconfigoption.h \
  ../../include/cstatistic.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/cobjectfactory.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/appreg.o: appreg.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cregistrationlist.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  appreg.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h
$O/args.o: args.cc \
  envirdefs.h \
  args.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../nedxml/saxparser.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  cxmldoccache.h \
  ../../include/cxmlelement.h \
  ../nedxml/nedxmldefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/envirbase.o: envirbase.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cnamedobject.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cproperties.h \
  ../../include/cregistrationlist.h \
  ../../include/cdynamicexpression.h \
  ../../include/platdep/timeutil.h \
  ../../include/cconfiguration.h \
  ../../include/chasher.h \
  ../nedxml/nedparser.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cscheduler.h \
  ../../include/cresultrecorder.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cnedmathfunction.h \
  ../../include/cobjectfactory.h \
  appreg.h \
  ../nedxml/nedxmldefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../nedxml/nedelement.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cparsimcomm.h \
  ../../include/ccomponenttype.h \
  args.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/cnedvalue.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  intervals.h \
  ../../include/cconfigoption.h \
  ../../include/cstatistic.h \
  ../nedxml/nedyydefs.h \
  ../../include/opp_string.h \
  ../nedxml/nederror.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/csimulation.h \
  statisticparser.h \
  ../../include/chistogram.h \
  ../../include/cobject.h \
  ../../include/cnedfunction.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  cxmldoccache.h \
  objectprinter.h \
  ../../include/cxmlelement.h \
  ../../include/cstddev.h \
  ../../include/crng.h \
  ../common/ver.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h \
  ../../include/cproperty.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cnedmathfunction.h \
  ../../include/csimplemodule.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  args.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cmsgpar.h \
  intervals.h \
  ../../include/cclassdescriptor.h \
  ../../include/cconfigoption.h \
  eventlogwriter.h \
  ../../include/cchannel.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cdisplaystring.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/eventlogwriter.o: eventlogwriter.cc \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/clistener.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cconfigoption.h \
  eventlogwriter.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  intervals.h \
  ../../include/cstatistic.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  objectprinter.h \
  ../../include/cstddev.h \
  fileoutscalarmgr.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/fileoutvectormgr.o: fileoutvectormgr.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  envirdefs.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  intervals.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/filesnapshotmgr.o: filesnapshotmgr.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  filesnapshotmgr.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  intervals.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/fsutils.o: fsutils.cc \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  fsutils.h \
  ../../include/simutil.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  envirdefs.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  intervals.h \
  ../../include/cstatistic.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  indexedfileoutvectormgr.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/inifilereader.o: inifilereader.cc \
  ../../include/errmsg.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  inifilereader.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cconfigreader.h \
  fsutils.h \
  ../../include/simutil.h
$O/intervals.o: intervals.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cnamedobject.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/main.o: main.cc \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  startup.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../common/ver.h \
  ../../include/simutil.h
$O/matchablefield.o: matchablefield.cc \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  matchablefield.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/cclassdescriptor.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/simutil.h
$O/matchableobject.o: matchableobject.cc \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  matchableobject.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/cclassdescriptor.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/simutil.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  matchableobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  matchablefield.h \
  ../../include/clistener.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cclassdescriptor.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/opp_run.o: opp_run.cc
$O/runattributes.o: runattributes.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cresultlistener.h \
  eventlogfilemgr.h \
  ../../include/clistener.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  intervals.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  objectprinter.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h
$O/scenario.o: scenario.cc \
  ../../include/regmacros.h \
  sectionbasedconfig.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  valueiterator.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cconfigreader.h \
  ../../include/simutil.h \
  scenario.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  ../../include/regmacros.h \
  sectionbasedconfig.h \
  ../../include/cnamedobject.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/globals.h \
  valueiterator.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cconfigreader.h \
  ../../include/simutil.h \
  scenario.h
$O/speedometer.o: speedometer.cc \
  speedometer.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h
$O/startup.o: startup.cc \
  ../../include/regmacros.h \
  sectionbasedconfig.h \
  ../../include/distrib.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  matchableobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/envirext.h \
  ../../include/clistener.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  startup.h \
  ../../include/cvisitor.h \
  ../../include/cobjectfactory.h \
  speedometer.h \
  appreg.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  filesnapshotmgr.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  akoutvectormgr.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  inifilereader.h \
  envirdefs.h \
  akaroarng.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  intervals.h \
  ../../include/cconfigoption.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  indexedfileoutvectormgr.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cconfigreader.h \
  ../../include/cdefaultlist.h \
  fsutils.h \
  ../../include/crng.h \
  fileoutscalarmgr.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h
$O/statisticparser.o: statisticparser.cc \
  envirdefs.h \
  statisticparser.h
$O/valueiterator.o: valueiterator.cc \
  ../../include/errmsg.h \
  valueiterator.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/simutil.h
