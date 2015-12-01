
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

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

ifeq ("$(PLATFORM)","win32.x86")
PSAPI_LIB=-lpsapi
endif

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) $(INCL_FLAGS) -DBUILDING_ENVIR -DHAVE_DLOPEN=$(HAVE_DLOPEN) -DSHARED_LIB_SUFFIX="\"$(SHARED_LIB_SUFFIX)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\""

IMPLIBS= -L$L -loppsim$D -loppnedxml$D -loppcommon$D $(XML_LIBS)

OBJS= $O/appreg.o $O/args.o $O/startup.o $O/evmain.o $O/logformatter.o $O/envirbase.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/fileoutvectormgr.o $O/fileoutscalarmgr.o $O/filesnapshotmgr.o \
      $O/indexedfileoutvectormgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/cxmldoccache.o $O/eventlogwriter.o $O/objectprinter.o \
      $O/eventlogfilemgr.o $O/runattributes.o $O/intervals.o \
      $O/statisticparser.o $O/visitor.o

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
	$(Q)$(CXX) $(COPTS) -c $(INCL_FLAGS) -o$O/opp_run.o opp_run.cc

$O/main.o : main.cc
	@# Note: not in OBJS because main.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) $(COPTS) -c $(INCL_FLAGS) -o$O/main.o main.cc

else
opp_run_executable:
	@echo "Note: We don't create opp_run.exe when building static libs"
endif

depend: $(GENERATED_SOURCES)
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I../../include -I.. -P\$$O/ -- *.cc


clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f opp_run$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/akaroarng.o: akaroarng.cc \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  akaroarng.h \
  envirdefs.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/matchexpression.h \
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
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  appreg.h \
  envirdefs.h
$O/args.o: args.cc \
  ../../include/omnetpp/platdep/platdefs.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/opp_ctype.h \
  args.h \
  envirdefs.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cxmlelement.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/fileutil.h \
  ../nedxml/nedxmldefs.h \
  ../nedxml/saxparser.h \
  cxmldoccache.h \
  envirdefs.h
$O/envirbase.o: envirbase.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/ccoroutine.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdensityestbase.h \
  ../../include/omnetpp/cdynamicexpression.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/chasher.h \
  ../../include/omnetpp/checkandcast.h \
  ../../include/omnetpp/chistogram.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedfunction.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cparsimcomm.h \
  ../../include/omnetpp/cproperties.h \
  ../../include/omnetpp/cproperty.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultfilter.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/cresultrecorder.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/cscheduler.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstddev.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cxmlelement.h \
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
  ../common/expression.h \
  ../common/fileglobber.h \
  ../common/fileutil.h \
  ../common/fnamelisttokenizer.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/stringpool.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  ../common/unitconversion.h \
  ../common/ver.h \
  ../nedxml/nedelement.h \
  ../nedxml/nederror.h \
  ../nedxml/nedparser.h \
  ../nedxml/nedxmldefs.h \
  ../nedxml/nedyydefs.h \
  ../sim/parsim/cparsimpartition.h \
  ../sim/parsim/cparsimsynchr.h \
  ../sim/parsim/creceivedexception.h \
  ../sim/resultfilters.h \
  ../sim/resultrecorders.h \
  appreg.h \
  args.h \
  cxmldoccache.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  statisticparser.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
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
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpacket.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  eventlogwriter.h \
  intervals.h \
  logformatter.h \
  objectprinter.h
$O/eventlogwriter.o: eventlogwriter.cc \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/stringutil.h \
  envirdefs.h \
  eventlogwriter.h
$O/evmain.o: evmain.cc \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/ver.h \
  envirdefs.h \
  startup.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdensityestbase.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstddev.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/stringutil.h \
  ../common/unitconversion.h \
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
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/stringutil.h \
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
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  filesnapshotmgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h
$O/fsutils.o: fsutils.cc \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/fileutil.h \
  envirdefs.h \
  fsutils.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/stringutil.h \
  ../common/unitconversion.h \
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
  ../../include/omnetpp/cconfigreader.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/opp_ctype.h \
  envirdefs.h \
  fsutils.h \
  inifilereader.h
$O/intervals.o: intervals.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/stringtokenizer.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h
$O/logformatter.o: logformatter.cc \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/chasher.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  envirdefs.h \
  logformatter.h
$O/main.o: main.cc \
  ../../include/omnetpp/platdep/platdefs.h \
  ../common/commondefs.h \
  ../common/exception.h \
  envirdefs.h
$O/matchablefield.o: matchablefield.cc \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  envirdefs.h \
  matchablefield.h
$O/matchableobject.o: matchableobject.cc \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  envirdefs.h \
  matchableobject.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  envirdefs.h \
  matchablefield.h \
  matchableobject.h \
  objectprinter.h
$O/opp_run.o: opp_run.cc \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h
$O/runattributes.o: runattributes.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/stringutil.h \
  args.h \
  envirbase.h \
  envirdefs.h \
  eventlogfilemgr.h \
  intervals.h \
  logformatter.h \
  objectprinter.h \
  runattributes.h
$O/scenario.o: scenario.cc \
  ../../include/omnetpp/cconfigreader.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/expression.h \
  ../common/stringpool.h \
  ../common/stringutil.h \
  ../common/unitconversion.h \
  envirdefs.h \
  scenario.h \
  sectionbasedconfig.h \
  valueiterator.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfigreader.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/expression.h \
  ../common/opp_ctype.h \
  ../common/patternmatcher.h \
  ../common/stringpool.h \
  ../common/stringtokenizer.h \
  ../common/stringtokenizer2.h \
  ../common/stringutil.h \
  envirdefs.h \
  scenario.h \
  sectionbasedconfig.h \
  valueiterator.h
$O/speedometer.o: speedometer.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../common/commondefs.h \
  ../common/exception.h \
  envirdefs.h \
  speedometer.h
$O/startup.o: startup.cc \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfigreader.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
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
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/fnamelisttokenizer.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/stringpool.h \
  ../common/stringutil.h \
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
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultfilter.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/cresultrecorder.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/expression.h \
  ../common/stringpool.h \
  ../common/stringutil.h \
  ../sim/resultfilters.h \
  ../sim/resultrecorders.h \
  envirdefs.h \
  statisticparser.h
$O/valueiterator.o: valueiterator.cc \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/expression.h \
  ../common/opp_ctype.h \
  ../common/stringpool.h \
  ../common/stringtokenizer2.h \
  ../common/stringutil.h \
  ../common/unitconversion.h \
  envirdefs.h \
  valueiterator.h
$O/visitor.o: visitor.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfsm.h \
  ../../include/omnetpp/cgate.h \
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
  ../../include/omnetpp/cqueue.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  envirdefs.h \
  matchableobject.h \
  visitor.h
