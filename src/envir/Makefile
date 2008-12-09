
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
LIBNAME=liboppenvir$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/envir
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../sim -I../common -DBUILDING_ENVIR -DHAVE_DLOPEN=$(HAVE_DLOPEN) -DSHARED_LIB_SUFFIX="\"$(SHARED_LIB_SUFFIX)\""

IMPLIBS= -L$L -loppsim$D -loppnedxml$D -loppcommon$D $(XML_LIBS)

OBJS= $O/args.o $O/bootenv.o $O/main.o $O/envirbase.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/fileoutvectormgr.o $O/fileoutscalarmgr.o $O/filesnapshotmgr.o \
      $O/indexedfileoutvectormgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/cxmldoccache.o $O/eventlogwriter.o $O/objectprinter.o \
      $O/eventlogfilemgr.o $O/runattributes.o $O/intervals.o


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
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS) envir-implib
	@mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

envir-implib : $(OBJS)
	@mkdir -p $L
	-$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) 2>/dev/null
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

eventlogwriter.cc eventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

$O/envirbase.o: eventlogwriter.h

# default opp_run target - called only at the end once tkenv and cmdenv is done
opp_run_executable: $O/opp_run$(EXE_SUFFIX)

# opp_run is useless with static libs
ifeq ("$(LIB_SUFFIX)","$(SHARED_LIB_SUFFIX)")
$O/opp_run$(EXE_SUFFIX) : $O/opp_run.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o "$@" $O/opp_run.o $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS)
	cp $O/opp_run$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

$O/opp_run.o : opp_run.cc
	@# Note: not in OBJS because opp_run.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(CXX) $(CFLAGS) -c -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_INCL_DIR)/platdep" -I../sim  -I../common -o$O/opp_run.o opp_run.cc
else
$O/opp_run$(EXE_SUFFIX):
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
  ../../include/cclassfactory.h \
  ../../include/globals.h \
  akaroarng.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cregistrationlist.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/errmsg.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  akoutvectormgr.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  envirdefs.h \
  ../../include/cclassfactory.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cstatistic.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/args.o: args.cc \
  envirdefs.h \
  args.h
$O/bootenv.o: bootenv.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  akoutvectormgr.h \
  ../../include/platdep/platdefs.h \
  sectionbasedconfig.h \
  fileoutvectormgr.h \
  inifilereader.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  akaroarng.h \
  ../../include/distrib.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  matchableobject.h \
  ../../include/cconfigkey.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  indexedfileoutvectormgr.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  bootenv.h \
  ../../include/cconfigreader.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  speedometer.h \
  fsutils.h \
  appreg.h \
  ../../include/crng.h \
  ../../include/platdep/platmisc.h \
  fileoutscalarmgr.h \
  ../../include/cmodule.h \
  ../../include/cexception.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  filesnapshotmgr.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../nedxml/saxparser.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  cxmldoccache.h \
  ../../include/cxmlelement.h \
  ../nedxml/nedxmldefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/envirbase.o: envirbase.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cproperties.h \
  ../../include/cregistrationlist.h \
  ../../include/platdep/timeutil.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/chasher.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/onstartup.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  cxmldoccache.h \
  objectprinter.h \
  appreg.h \
  ../../include/cxmlelement.h \
  ../../include/crng.h \
  ../../include/platdep/platmisc.h \
  ../common/ver.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/cproperty.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cparsimcomm.h \
  ../../include/ccomponenttype.h \
  args.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cmathfunction.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/ccompoundmodule.h \
  ../../include/cgate.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cmsgpar.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cregistrationlist.h \
  eventlogwriter.h \
  ../../include/cchannel.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/cdisplaystring.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/csimplemodule.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  args.h
$O/eventlogwriter.o: eventlogwriter.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  eventlogwriter.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cstatistic.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  bootenv.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/cstddev.h \
  ../../include/platdep/platmisc.h \
  fileoutscalarmgr.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/ccomponenttype.h
$O/fileoutvectormgr.o: fileoutvectormgr.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  bootenv.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h \
  ../../include/ccomponenttype.h
$O/filesnapshotmgr.o: filesnapshotmgr.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  intervals.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  bootenv.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  filesnapshotmgr.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/fsutils.o: fsutils.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/onstartup.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  fsutils.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  fileoutvectormgr.h \
  envirdefs.h \
  ../../include/cclassfactory.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  intervals.h \
  ../../include/cstatistic.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  indexedfileoutvectormgr.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/inifilereader.o: inifilereader.cc \
  ../../include/platdep/platdefs.h \
  inifilereader.h \
  envirdefs.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cconfigreader.h \
  fsutils.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/intervals.o: intervals.cc \
  ../../include/regmacros.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cclassfactory.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  intervals.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/onstartup.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/main.o: main.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  bootenv.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../common/ver.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h
$O/matchablefield.o: matchablefield.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/cclassdescriptor.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  matchablefield.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h
$O/matchableobject.o: matchableobject.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  matchableobject.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/cclassdescriptor.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cclassfactory.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  matchableobject.h \
  ../../include/cgate.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/ccomponent.h \
  ../../include/cclassdescriptor.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  matchablefield.h \
  ../../include/cstringpool.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cdefaultlist.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/cexception.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h
$O/opp_run.o: opp_run.cc \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  bootenv.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/platdep/intxtypes.h
$O/runattributes.o: runattributes.cc \
  ../../include/regmacros.h \
  ../../include/ccoroutine.h \
  envirbase.h \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/cclassfactory.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  intervals.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  runattributes.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  eventlogfilemgr.h \
  ../../include/envirext.h \
  ../../include/cstringpool.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  objectprinter.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  args.h
$O/scenario.o: scenario.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  sectionbasedconfig.h \
  envirdefs.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cconfigreader.h \
  valueiterator.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  scenario.h \
  ../../include/platdep/intxtypes.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  sectionbasedconfig.h \
  ../../include/cclassfactory.h \
  envirdefs.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cconfigreader.h \
  ../../include/globals.h \
  valueiterator.h \
  ../../include/cvisitor.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  scenario.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/speedometer.o: speedometer.cc \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/simtime.h \
  ../../include/simtime_t.h \
  ../../include/simkerneldefs.h \
  speedometer.h \
  ../../include/platdep/intxtypes.h
$O/valueiterator.o: valueiterator.cc \
  ../../include/platdep/platdefs.h \
  envirdefs.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  valueiterator.h \
  ../../include/platdep/platmisc.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
