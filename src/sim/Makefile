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
ifeq ($(PLATFORM),win32.x86_64)
  ifeq ($(SHARED_LIBS),yes)
    TARGET_LIB_FILES=$(OMNETPP_BIN_DIR)/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(A_LIB_SUFFIX)
    ifeq ($(TOOLCHAIN_NAME),clangc2)
      ifeq ($(MODE),debug)
        TARGET_LIB_FILES+=$(OMNETPP_BIN_DIR)/$(LIBNAME).pdb
      endif
    endif
  endif
endif

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/sim

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=-Wno-unused-function $(CFLAGS) $(INCL_FLAGS)

IMPLIBS= -loppcommon$D

OBJS_STD=\
    $O/carray.o $O/cdelaychannel.o $O/cdataratechannel.o $O/cboolparimpl.o $O/cchannel.o \
    $O/cobjectfactory.o $O/ccomponent.o $O/ccomponenttype.o $O/cconfiguration.o $O/cconfigoption.o \
    $O/ccanvas.o $O/ccoroutine.o $O/cdefaultlist.o $O/cabstracthistogram.o $O/cfutureeventset.o \
    $O/cdisplaystring.o $O/cdoubleparimpl.o $O/cdynamicexpression.o $O/cexpression.o $O/cenvir.o \
    $O/cenum.o $O/cevent.o $O/cexception.o $O/cfsm.o $O/cnedmathfunction.o $O/cgate.o \
    $O/chistogram.o $O/chistogramstrategy.o $O/clegacyhistogram.o \
    $O/cksplit.o $O/clcg32.o $O/clistener.o $O/clog.o $O/cintparimpl.o $O/cmersennetwister.o \
    $O/cmessage.o $O/cpacket.o $O/cmsgpar.o $O/cmodule.o $O/ceventheap.o $O/chasher.o $O/cfingerprint.o $O/ctimestampedvalue.o \
    $O/cmatchexpression.o $O/cpatternmatcher.o $O/cmessageprinter.o $O/cnullenvir.o $O/envirext.o \
    $O/cnedfunction.o $O/cnedvalue.o $O/cobject.o $O/coutvector.o $O/cnamedobject.o $O/cosgcanvas.o \
    $O/cpar.o $O/cparimpl.o $O/cownedobject.o $O/cproperties.o $O/cproperty.o $O/crandom.o \
    $O/cresultfilter.o $O/cresultlistener.o $O/cresultrecorder.o $O/clifecyclelistener.o \
    $O/cprecolldensityest.o $O/cpsquare.o $O/cqueue.o $O/cpacketqueue.o $O/cscheduler.o $O/csimplemodule.o \
    $O/csimulation.o $O/cstatistic.o $O/cstddev.o $O/cstlwatch.o $O/cstringparimpl.o \
    $O/cstringpool.o $O/cstringtokenizer.o $O/cclassdescriptor.o $O/ctopology.o $O/cvarhist.o \
    $O/cvisitor.o $O/cwatch.o $O/cxmlelement.o $O/cxmlparimpl.o $O/distrib.o $O/nedfunctions.o \
    $O/errmsg.o $O/globals.o $O/cregistrationlist.o $O/lex.expryy.o $O/minixpath.o $O/onstartup.o \
    $O/simtime.o $O/task.o $O/util.o $O/gettime.o $O/expr.tab.o $O/nedsupport.o $O/sim_std_m.o \
    $O/cstatisticbuilder.o $O/statisticparsing.o $O/resultfilters.o $O/resultrecorders.o \
    $O/ccommbuffer.o $O/cparsimcomm.o

OBJS_NETBUILDER=\
    $O/netbuilder/cexpressionbuilder.o \
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

GENERATED_SOURCES= expr.tab.hh expr.tab.cc lex.expryy.cc sim_std_m.cc sim_std_m.h

ifeq ($(WITH_NETBUILDER),yes)
  OBJS += $(OBJS_NETBUILDER)
  IMPLIBS += -loppnedxml$D
endif

ifeq ($(WITH_PARSIM),yes)
  OBJS += $(OBJS_PARSIM)
  COPTS += $(MPI_CFLAGS)
  IMPLIBS += $(MPI_LIBS)
endif

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DSIM_EXPORT

.PHONY: all clean
.SECONDARY: $(OBJS) $O/$(LIBNAME)$(LIB_SUFFIX) $(GENERATED_SOURCES)

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc $(GENERATED_SOURCES) $(CONFIGFILE)
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
$O/%$(DLL_LIB_SUFFIX) $O/%$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating DLL and import library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)
else
# statically linked library (on all platforms)
$O/%$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
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

# rules for generated sources
lex.expryy.cc: expr.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.expryy.cc -Pexpryy expr.lex

# NOTE: This is a trick: creating a muti-target pattern rule that prevents executing this target multiple times when running make in parallel
%xpr.tab.hh %xpr.tab.cc : expr.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o expr.tab.cc -p expryy -d expr.y

%im_std_m.h %im_std_m.cc : sim_std.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) --msg6 sim_std.msg

-include $(OBJS:%.o=%.d)
