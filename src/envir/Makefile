
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
LIBNAME=libenvir$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/envir
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../sim -I../common -DBUILDING_ENVIR -DHAVE_DLOPEN=$(HAVE_DLOPEN)

IMPLIBS= -L$L -lsim_std$D -lnedxml$D -lcommon$D $(XML_LIBS)

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
all : $L/$(LIBNAME)$(LIB_SUFFIX) $O/opp_run$(EXE_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS) envir-implib
	@mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

envir-implib : $(OBJS)
	@mkdir -p $L
	-$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) $(LDFLAGS) 2>/dev/null
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

eventlogwriter.cc eventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

$O/envirbase.o: eventlogwriter.h

# opp_run is useless with static libs
ifeq ("$(LIB_SUFFIX)","$(SHARED_LIB_SUFFIX)")
$O/opp_run$(EXE_SUFFIX) : $O/opp_run.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o "$@" $O/opp_run.o -lenvir$D $(IMPLIBS) $(SYS_LIBS)
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
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cexception.h \
  akaroarng.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  envirdefs.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/cenvir.h \
  envirbase.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/cstatistic.h \
  akoutvectormgr.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  args.h \
  envirdefs.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  fileoutvectormgr.h \
  runattributes.h
$O/args.o: args.cc \
  args.h \
  envirdefs.h
$O/bootenv.o: bootenv.cc \
  args.h \
  ../../include/distrib.h \
  ../../include/cconfigkey.h \
  inifilereader.h \
  sectionbasedconfig.h \
  bootenv.h \
  appreg.h \
  ../../include/cmodule.h \
  fsutils.h \
  speedometer.h \
  fileoutvectormgr.h \
  fileoutscalarmgr.h \
  filesnapshotmgr.h \
  indexedfileoutvectormgr.h \
  akaroarng.h \
  akoutvectormgr.h \
  matchableobject.h \
  envirdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/simtime.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/csimulation.h \
  ../../include/crng.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platmisc.h \
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
  ../../include/cconfigreader.h \
  ../../include/cconfiguration.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/envirext.h \
  runattributes.h \
  intervals.h
$O/cxmldoccache.o: cxmldoccache.cc \
  ../../include/cobject.h \
  cxmldoccache.h \
  ../../include/cxmlelement.h \
  ../../include/cexception.h \
  ../nedxml/saxparser.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  envirdefs.h \
  ../../include/cenvir.h \
  ../../include/opp_string.h \
  ../nedxml/nedxmldefs.h
$O/envirbase.o: envirbase.cc \
  args.h \
  envirbase.h \
  appreg.h \
  ../../include/ccoroutine.h \
  ../../include/csimulation.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/random.h \
  ../../include/crng.h \
  ../../include/cmodule.h \
  ../../include/ccomponenttype.h \
  ../../include/cxmlelement.h \
  cxmldoccache.h \
  ../../include/chasher.h \
  ../../include/cconfigkey.h \
  ../../include/regmacros.h \
  ../../include/simtime.h \
  ../../include/cparsimcomm.h \
  envirdefs.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/timeutil.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cdefaultlist.h
$O/eventlogfilemgr.o: eventlogfilemgr.cc \
  eventlogfilemgr.h \
  ../../include/cconfigkey.h \
  ../../include/cconfiguration.h \
  envirbase.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/cgate.h \
  ../../include/cchannel.h \
  ../../include/csimplemodule.h \
  ../../include/ccompoundmodule.h \
  ../../include/cdisplaystring.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  ../../include/opp_string.h \
  objectprinter.h \
  intervals.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  ../../include/envirext.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  ../../include/ccomponenttype.h
$O/fileoutscalarmgr.o: fileoutscalarmgr.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  bootenv.h \
  envirbase.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/cstatistic.h \
  ../../include/cdensityestbase.h \
  fileoutscalarmgr.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  envirdefs.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cstddev.h \
  runattributes.h
$O/fileoutvectormgr.o: fileoutvectormgr.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  bootenv.h \
  envirbase.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  fileoutvectormgr.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  envirdefs.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  runattributes.h
$O/filesnapshotmgr.o: filesnapshotmgr.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  bootenv.h \
  envirbase.h \
  filesnapshotmgr.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  envirdefs.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h
$O/fsutils.o: fsutils.cc \
  fsutils.h \
  ../../include/onstartup.h \
  ../../include/cexception.h \
  envirdefs.h \
  ../../include/util.h \
  ../../include/platdep/platmisc.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/inttypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/indexedfileoutvectormgr.o: indexedfileoutvectormgr.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  envirbase.h \
  ../../include/csimulation.h \
  ../../include/regmacros.h \
  ../../include/cmodule.h \
  ../../include/cstatistic.h \
  indexedfileoutvectormgr.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  envirdefs.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  fileoutvectormgr.h \
  runattributes.h
$O/inifilereader.o: inifilereader.cc \
  inifilereader.h \
  fsutils.h \
  ../../include/cexception.h \
  envirdefs.h \
  ../../include/cconfigreader.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
$O/intervals.o: intervals.cc \
  envirbase.h \
  intervals.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  args.h \
  envirdefs.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h \
  objectprinter.h \
  matchablefield.h \
  matchableobject.h
$O/main.o: main.cc \
  ../../include/cownedobject.h \
  bootenv.h \
  ../utils/ver.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  envirdefs.h
$O/matchablefield.o: matchablefield.cc \
  matchablefield.h \
  ../../include/cclassdescriptor.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
$O/matchableobject.o: matchableobject.cc \
  matchableobject.h \
  ../../include/cclassdescriptor.h \
  envirdefs.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/cconfiguration.h \
  ../../include/carray.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cclassdescriptor.h \
  objectprinter.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  matchablefield.h \
  matchableobject.h \
  envirdefs.h
$O/opp_run.o: opp_run.cc \
  bootenv.h \
  envirdefs.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h
$O/runattributes.o: runattributes.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  envirbase.h \
  ../../include/csimulation.h \
  runattributes.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  envirdefs.h \
  ../../include/envirext.h \
  eventlogfilemgr.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  objectprinter.h \
  intervals.h \
  matchablefield.h \
  matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/scenario.o: scenario.cc \
  scenario.h \
  valueiterator.h \
  ../../include/cexception.h \
  envirdefs.h \
  sectionbasedconfig.h \
  ../../include/cconfiguration.h \
  ../../include/cconfigreader.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  sectionbasedconfig.h \
  ../../include/cconfigreader.h \
  valueiterator.h \
  ../../include/cexception.h \
  scenario.h \
  ../../include/globals.h \
  ../../include/cconfigkey.h \
  envirdefs.h \
  ../../include/cconfiguration.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platmisc.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cstringpool.h
$O/speedometer.o: speedometer.cc \
  speedometer.h \
  ../../include/simkerneldefs.h \
  envirdefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h
$O/valueiterator.o: valueiterator.cc \
  valueiterator.h \
  ../../include/cexception.h \
  envirdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/platdep/platmisc.h
