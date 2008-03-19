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
LIBNAME=libenvir$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/envir
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../sim -I../common -DBUILDING_ENVIR -DHAVE_DLOPEN=$(HAVE_DLOPEN)

IMPLIBS= -L$L -lsim_std$D -lnedxml$D -lcommon$D $(XML_LIBS)

OBJS= $O/args.o $O/cenvir.o $O/main.o $O/omnetapp.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/fileoutvectormgr.o $O/fileoutscalarmgr.o $O/filesnapshotmgr.o \
      $O/indexedfileoutvectormgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/cxmldoccache.o $O/eventlogwriter.o $O/objectprinter.o


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
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(LDFLAGS)
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
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME)* .

eventlogwriter.cc eventlogwriter.h : eventlogentries.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogentries.pl

$O/omnetapp.o: eventlogwriter.h

# opp_run is useless with static libs
ifeq ("$(LIB_SUFFIX)","$(SHARED_LIB_SUFFIX)")
$O/opp_run$(EXE_SUFFIX) : $O/opp_run.o $O/$(LIBNAME)$(LIB_SUFFIX)
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
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/cenvir.h \
  omnetapp.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/cstatistic.h \
  akoutvectormgr.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  args.h \
  envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  filemgrs.h
$O/args.o: args.cc \
  args.h \
  envdefs.h
$O/cenvir.o: cenvir.cc \
  args.h \
  ../../include/distrib.h \
  ../../include/cconfigkey.h \
  inifilereader.h \
  sectionbasedconfig.h \
  ../../include/cenvir.h \
  omnetapp.h \
  appreg.h \
  ../../include/cmodule.h \
  fsutils.h \
  speedometer.h \
  filemgrs.h \
  akaroarng.h \
  akoutvectormgr.h \
  matchableobject.h \
  envdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/csimulation.h \
  ../../include/crng.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/errmsg.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/cconfigreader.h \
  ../../include/cconfiguration.h \
  ../../include/envirext.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
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
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  envdefs.h \
  ../../include/cenvir.h \
  ../../include/opp_string.h \
  ../nedxml/nedxmldefs.h
$O/eventlogwriter.o: eventlogwriter.cc \
  eventlogwriter.h \
  envdefs.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h
$O/filemgrs.o: filemgrs.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  omnetapp.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/cstatistic.h \
  ../../include/cdensityestbase.h \
  filemgrs.h \
  ../../include/ccomponenttype.h \
  ivfilemgr.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cstddev.h
$O/fsutils.o: fsutils.cc \
  fsutils.h \
  ../../include/onstartup.h \
  ../../include/cexception.h \
  ../../include/util.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/inifilereader.o: inifilereader.cc \
  inifilereader.h \
  fsutils.h \
  ../../include/cexception.h \
  envdefs.h \
  ../../include/cconfigreader.h \
  ../../include/util.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/opp_string.h
$O/ivfilemgr.o: ivfilemgr.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  omnetapp.h \
  ../../include/csimulation.h \
  ../../include/regmacros.h \
  ../../include/cmodule.h \
  ../../include/cstatistic.h \
  ivfilemgr.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  filemgrs.h
$O/main.o: main.cc \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../utils/ver.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h
$O/matchablefield.o: matchablefield.cc \
  matchablefield.h \
  ../../include/cclassdescriptor.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
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
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
$O/objectprinter.o: objectprinter.cc \
  ../../include/cconfiguration.h \
  ../../include/cmodule.h \
  ../../include/cclassdescriptor.h \
  objectprinter.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  matchablefield.h \
  matchableobject.h
$O/omnetapp.o: omnetapp.cc \
  args.h \
  omnetapp.h \
  appreg.h \
  fsutils.h \
  eventlogwriter.h \
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
  ../../include/cmessage.h \
  ../../include/cdisplaystring.h \
  ../../include/cxmlelement.h \
  cxmldoccache.h \
  ../../include/chasher.h \
  ../../include/cconfigkey.h \
  ../../include/regmacros.h \
  ../../include/simtime.h \
  ../../include/cparsimcomm.h \
  objectprinter.h \
  envdefs.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/timeutil.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cdefaultlist.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  ../../include/platdep/inttypes.h \
  matchablefield.h \
  matchableobject.h
$O/opp_run.o: opp_run.cc \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h
$O/scenario.o: scenario.cc \
  scenario.h \
  valueiterator.h \
  ../../include/cexception.h \
  sectionbasedconfig.h \
  ../../include/cconfiguration.h \
  ../../include/cconfigreader.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  envdefs.h \
  ../../include/opp_string.h
$O/sectionbasedconfig.o: sectionbasedconfig.cc \
  sectionbasedconfig.h \
  ../../include/cconfigreader.h \
  valueiterator.h \
  ../../include/cexception.h \
  scenario.h \
  ../../include/globals.h \
  ../../include/cconfigkey.h \
  ../../include/cconfiguration.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  envdefs.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cstringpool.h
$O/speedometer.o: speedometer.cc \
  speedometer.h \
  ../../include/simkerneldefs.h \
  envdefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
$O/valueiterator.o: valueiterator.cc \
  valueiterator.h \
  ../../include/cexception.h \
  envdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h
