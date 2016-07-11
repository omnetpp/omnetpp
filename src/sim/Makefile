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
LIBNAME=liboppsim$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/sim
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=-Wno-unused-function $(CFLAGS) $(INCL_FLAGS) -DBUILDING_SIM

IMPLIBS=-L$L -loppcommon$D

OBJS_STD=\
    $O/carray.o $O/cdelaychannel.o $O/cdataratechannel.o $O/cboolparimpl.o $O/cchannel.o \
    $O/cobjectfactory.o $O/ccomponent.o $O/ccomponenttype.o $O/cconfiguration.o $O/cconfigoption.o \
    $O/ccanvas.o $O/ccoroutine.o $O/cdefaultlist.o $O/cdensityestbase.o $O/cfutureeventset.o \
    $O/cdetect.o $O/cdisplaystring.o $O/cdoubleparimpl.o $O/cdynamicexpression.o $O/cenvir.o \
    $O/cenum.o $O/cevent.o $O/cexception.o $O/cfsm.o $O/cnedmathfunction.o $O/cgate.o $O/chistogram.o \
    $O/cksplit.o $O/clcg32.o $O/clistener.o $O/clog.o $O/clongparimpl.o $O/cmersennetwister.o \
    $O/cmessage.o $O/cpacket.o $O/cmsgpar.o $O/cmodule.o $O/ceventheap.o $O/chasher.o $O/cfingerprint.o $O/ctimestampedvalue.o \
    $O/cmatchexpression.o $O/cpatternmatcher.o $O/cmessageprinter.o $O/cnullenvir.o $O/envirext.o \
    $O/cnedfunction.o $O/cnedvalue.o $O/cobject.o $O/coutvector.o $O/cnamedobject.o $O/cosgcanvas.o \
    $O/cpar.o $O/cparimpl.o $O/cownedobject.o $O/cproperties.o $O/cproperty.o $O/crandom.o \
    $O/cresultfilter.o $O/cresultlistener.o $O/cresultrecorder.o $O/clifecyclelistener.o \
    $O/cpsquare.o $O/cqueue.o $O/cpacketqueue.o $O/cscheduler.o $O/csimplemodule.o \
    $O/csimulation.o $O/cstatistic.o $O/cstddev.o $O/cstlwatch.o $O/cstringparimpl.o \
    $O/cstringpool.o $O/cstringtokenizer.o $O/cclassdescriptor.o $O/ctopology.o $O/cvarhist.o \
    $O/cvisitor.o $O/cwatch.o $O/cxmlelement.o $O/cxmlparimpl.o $O/distrib.o $O/nedfunctions.o \
    $O/errmsg.o $O/globals.o $O/cregistrationlist.o $O/lex.expryy.o $O/minixpath.o $O/onstartup.o \
    $O/simtime.o $O/task.o $O/util.o $O/expr.tab.o $O/nedsupport.o $O/sim_std_m.o \
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

ifneq ($(OSG_LIBS),)
IMPLIBS += -losg -lOpenThreads
GENERATED_SOURCES += sim_std_osg_m.cc sim_std_osg_m.h
OBJS_STD += $O/sim_std_osg_m.o 
endif

ifneq ($(OSGEARTH_LIBS),)
IMPLIBS += -losgEarth
endif

ifeq ($(WITH_PARSIM),yes)
OBJS += $(OBJS_PARSIM)
ifeq ($(HAVE_MPI),yes)
COPTS += $(MPI_CFLAGS)
IMPLIBS += $(MPI_LIBS)
endif
endif


#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc | $(GENERATED_SOURCES)
	@mkdir -p $O $O/netbuilder $O/parsim
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

#
# Targets
#
all : $L/$(LIBNAME)$(LIB_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(Q)$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

expr.tab.hh expr.tab.cc : expr.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o expr.tab.cc -p expryy -d expr.y

lex.expryy.cc: expr.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.expryy.cc -Pexpryy expr.lex

sim_std_m.cc sim_std_m.h : sim_std.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -Xnc -Xns sim_std.msg

sim_std_osg_m.cc sim_std_osg_m.h : sim_std_osg.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -Xnc -Xns sim_std_osg.msg

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

-include $(OBJS:%.o=%.d)
