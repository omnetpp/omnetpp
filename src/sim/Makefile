#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppsim$D
TARGET_LIB_FILES=$(OMNETPP_LIB_DIR)/$(LIBNAME)$(LIB_SUFFIX)
# On Windows, the targets are different (DLLs go into the bin dir and separate export libs are created)
ifeq ($(PLATFORM),win32)
  ifeq ($(SHARED_LIBS),yes)
    TARGET_LIB_FILES=$(OMNETPP_BIN_DIR)/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(IMPLIB_SUFFIX)
    ifeq ($(TOOLCHAIN_NAME),clang-msabi)
      ifeq ($(MODE),debug)
        TARGET_LIB_FILES+=$(OMNETPP_BIN_DIR)/$(LIBNAME).pdb
      endif
    endif
  endif
endif

O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/sim

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=-Wno-unused-function $(CFLAGS) $(INCL_FLAGS)

IMPLIBS= -loppcommon$D

OBJS_STD=\
    $O/carray.o $O/cdelaychannel.o $O/cdataratechannel.o $O/cboolparimpl.o $O/cchannel.o \
    $O/cobjectfactory.o $O/ccomponent.o $O/ccomponenttype.o $O/cconfiguration.o $O/cconfigoption.o \
    $O/ccanvas.o $O/ccoroutine.o $O/csoftowner.o $O/cabstracthistogram.o $O/cfutureeventset.o \
    $O/cdisplaystring.o $O/cdoubleparimpl.o $O/cdynamicexpression.o $O/cexpression.o $O/cenvir.o \
    $O/cenum.o $O/cevent.o $O/cexception.o $O/cfsm.o $O/cnedmathfunction.o $O/cgate.o \
    $O/ccontextswitcher.o $O/chistogram.o $O/chistogramstrategy.o $O/cksplit.o \
    $O/clcg32.o $O/clistener.o $O/clog.o $O/cintparimpl.o $O/cmersennetwister.o \
    $O/cmessage.o $O/cpacket.o $O/cmsgpar.o $O/cmodule.o $O/ceventheap.o $O/chasher.o $O/cfingerprint.o $O/ctimestampedvalue.o \
    $O/cmatchexpression.o $O/cpatternmatcher.o $O/cmessageprinter.o $O/cnullenvir.o $O/envirext.o \
    $O/cnedfunction.o $O/cvalue.o $O/cvaluecontainer.o $O/cvaluearray.o $O/cvaluemap.o $O/cvalueholder.o $O/cobject.o \
    $O/cobjectparimpl.o $O/coutvector.o $O/cnamedobject.o $O/cosgcanvas.o \
    $O/cpar.o $O/cparimpl.o $O/cownedobject.o $O/cproperties.o $O/cproperty.o $O/crandom.o \
    $O/cresultfilter.o $O/cresultlistener.o $O/cresultrecorder.o $O/clifecyclelistener.o \
    $O/cprecolldensityest.o $O/cpsquare.o $O/cqueue.o $O/cpacketqueue.o $O/cscheduler.o $O/csimplemodule.o \
    $O/csimulation.o $O/cstatistic.o $O/cstddev.o $O/cstlwatch.o $O/cstringparimpl.o \
    $O/cstringtokenizer.o $O/cclassdescriptor.o $O/ctemporaryowner.o $O/ctopology.o \
    $O/cvisitor.o $O/cwatch.o $O/cxmlelement.o $O/cxmlparimpl.o $O/any_ptr.o $O/distrib.o $O/nedfunctions.o \
    $O/errmsg.o $O/globals.o $O/cregistrationlist.o $O/minixpath.o $O/onstartup.o $O/opp_pooledstring.o \
    $O/simtime.o $O/simtimemath.o $O/task.o $O/util.o $O/gettime.o $O/nedsupport.o $O/sim_std_m.o \
    $O/cstatisticbuilder.o $O/statisticsourceparser.o $O/statisticrecorderparser.o $O/stringutil.o \
    $O/resultfilters.o $O/resultrecorders.o $O/expressionfilter.o $O/ccommbuffer.o $O/cparsimcomm.o

OBJS_NETBUILDER=\
    $O/netbuilder/cneddeclaration.o \
    $O/netbuilder/cnedloader.o \
    $O/netbuilder/cdynamicchanneltype.o \
    $O/netbuilder/cdynamicmoduletype.o \
    $O/netbuilder/cnednetworkbuilder.o

OBJS_PARSIM=\
    $O/parsim/cmemcommbuffer.o \
    $O/parsim/cparsimpartition.o $O/parsim/cplaceholdermod.o $O/parsim/cproxygate.o \
    $O/parsim/cparsimsynchr.o $O/parsim/cparsimprotocolbase.o $O/parsim/cnosynchronization.o \
    $O/parsim/cnullmessageprot.o $O/parsim/clinkdelaylookahead.o \
    $O/parsim/cidealsimulationprot.o $O/parsim/cispeventlogger.o \
    $O/parsim/ccommbufferbase.o $O/parsim/cfilecomm.o \
    $O/parsim/cfilecommbuffer.o $O/parsim/cnamedpipecomm-win.o $O/parsim/cnamedpipecomm.o $O/parsim/parsimutil.o \
    $O/parsim/creceivedexception.o $O/parsim/cmpicomm.o $O/parsim/cmpicommbuffer.o

OBJS= $(OBJS_STD)

GENERATED_SOURCES= sim_std_m.cc $(OMNETPP_INCL_DIR)/omnetpp/sim_std_m.h

ifeq ($(WITH_NETBUILDER),yes)
  OBJS += $(OBJS_NETBUILDER)
  IMPLIBS += -loppnedxml$D
endif

ifeq ($(WITH_PARSIM),yes)
  OBJS += $(OBJS_PARSIM)
  COPTS += $(MPI_CFLAGS)
  IMPLIBS += $(MPI_LIBS)
endif

# macro is used in $(EXPORT_DEFINES) with clang-msabi when building a shared lib
EXPORT_MACRO = -DSIM_EXPORT

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

.PHONY: all clean
.SECONDARY: $(OBJS) $O/$(LIBNAME)$(LIB_SUFFIX) $(GENERATED_SOURCES)

# disabling all implicit rules
.SUFFIXES :

#
# Automatic rules
#

# compile *_m.cc files generated from *.msg files
$O/%_m.o: $(GENERATED_SOURCES) $(CONFIGFILE) $(COPTS_FILE)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -I"$(OMNETPP_INCL_DIR)/omnetpp" $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

$O/%.o: %.cc $(CONFIGFILE) $(COPTS_FILE)
	@mkdir -p $O $O/netbuilder $O/parsim
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

#
# Targets
#
all : $(TARGET_LIB_FILES)

ifeq ($(SHARED_LIBS),yes)
# dynamically linked library (on all platforms except Windows)
$O/%$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

# dynamically linked library + import lib (on Windows)
$O/%$(DLL_LIB_SUFFIX) $O/%$(IMPLIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating DLL and import library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(IMPLIB_SUFFIX)
else
# statically linked library (on all platforms)
$O/%$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR_CR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
endif

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning sim
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES)

# NOTE: This is a trick: creating a multi-target pattern rule that prevents executing this target multiple times when running parallel make
$(OMNETPP_INCL_DIR)/omnetpp/%im_std_m.h %im_std_m.cc : sim_std.msg ../nedxml/*
	$(qecho) MSGC: $<
	$(Q)$(MSGC) --msg6 sim_std.msg
	$(Q)mv sim_std_m.h $(OMNETPP_INCL_DIR)/omnetpp/sim_std_m.h

-include $(OBJS:%=%.d)

