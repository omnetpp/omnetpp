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
LIBNAME=libsim_std$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/sim
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -DBUILDING_SIM $(MPI_CFLAGS)

IMPLIBS=-L$L -lenvir$D -lcommon$D

OBJS_STD=\
    $O/carray.o $O/cbasicchannel.o $O/cboolparimpl.o $O/cchannel.o \
    $O/cclassfactory.o $O/ccomponent.o $O/ccomponenttype.o $O/cconfiguration.o $O/cconfigkey.o \
    $O/ccompoundmodule.o $O/ccoroutine.o $O/cdefaultlist.o $O/cdensityestbase.o \
    $O/cdetect.o $O/cdisplaystring.o $O/cdoubleparimpl.o $O/cdynamicexpression.o \
    $O/cenum.o $O/cexception.o $O/cfsm.o $O/cmathfunction.o $O/cgate.o $O/chistogram.o \
    $O/cksplit.o $O/clcg32.o $O/clinkedlist.o $O/clongparimpl.o $O/cmersennetwister.o \
    $O/cmessage.o $O/cmsgpar.o $O/cmodule.o $O/cmessageheap.o $O/chasher.o \
    $O/cnedfunction.o $O/cobject.o $O/coutvector.o $O/cnamedobject.o \
    $O/cpar.o $O/cparimpl.o $O/cownedobject.o $O/cproperties.o $O/cproperty.o \
    $O/cpsquare.o $O/cqueue.o $O/cscheduler.o $O/csimplemodule.o \
    $O/csimulation.o $O/cstatistic.o $O/cstddev.o $O/cstlwatch.o $O/cstringparimpl.o \
    $O/cstringpool.o $O/cstringtokenizer.o $O/cclassdescriptor.o $O/ctopology.o $O/cvarhist.o \
    $O/cvisitor.o $O/cwatch.o $O/cxmlelement.o $O/cxmlparimpl.o $O/distrib.o $O/nedfunctions.o \
    $O/errmsg.o $O/globals.o $O/lex.expryy.o $O/minixpath.o $O/onstartup.o \
    $O/simtime.o $O/task.o $O/util.o $O/expr.tab.o $O/nedsupport.o $O/sim_std_m.o \
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
    $O/parsim/cparsimprotocolbase.o $O/parsim/cnosynchronization.o \
    $O/parsim/cnullmessageprot.o $O/parsim/clinkdelaylookahead.o \
    $O/parsim/cidealsimulationprot.o $O/parsim/cispeventlogger.o \
    $O/parsim/ccommbufferbase.o $O/parsim/cfilecomm.o \
    $O/parsim/cfilecommbuffer.o $O/parsim/cnamedpipecomm-win.o $O/parsim/parsimutil.o \
    $O/parsim/creceivedexception.o

OBJS_MPI=\
    $O/parsim/cmpicomm.o $O/parsim/cmpicommbuffer.o

OBJS= $(OBJS_STD)

ifeq ($(WITH_NETBUILDER),yes)
OBJS += $(OBJS_NETBUILDER)
COPTS += -I../nedxml
IMPLIBS += -lnedxml$D
endif

ifeq ($(WITH_PARSIM),yes)
OBJS += $(OBJS_PARSIM)
ifeq ($(HAVE_MPI),yes)
OBJS += $(OBJS_MPI)
COPTS += $(MPI_CFLAGS)
endif
endif

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O $O/netbuilder $O/parsim
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
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

expr.tab.h expr.tab.cc : expr.y
	$(YACC) -p expryy -b expr -d expr.y
	mv expr.tab.c expr.tab.cc

lex.expryy.cc: expr.lex expr.tab.h
	$(LEX) -Pexpryy expr.lex
	mv lex.expryy.c lex.expryy.cc

depend:
	$(MAKEDEPEND) -I../../include -P\$$O/ -- *.cc netbuilder/*.cc parsim/*.cc

sim_std_m.cc sim_std_m.h : sim_std.msg
	$(MSGC) -Xnc -Xns sim_std.msg

clean:
	rm -f expr.tab.h expr.tab.cc lex.expryy.cc *_m.cc *_m.h
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/carray.o: carray.cc \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
$O/cbasicchannel.o: cbasicchannel.cc \
  ../../include/cbasicchannel.h \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/csimulation.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/cchannel.h \
  ../../include/cpar.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  ../../include/opp_string.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
$O/cboolparimpl.o: cboolparimpl.cc \
  ../../include/cboolparimpl.h \
  ../../include/cstringtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparimpl.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cdefaultlist.h
$O/cchannel.o: cchannel.cc \
  ../../include/cchannel.h \
  ../../include/cpar.h \
  ../../include/cgate.h \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  ../../include/csimulation.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
$O/cclassdescriptor.o: cclassdescriptor.cc \
  ../../include/cclassdescriptor.h \
  ../../include/carray.h \
  ../../include/cenum.h \
  ../../include/util.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/onstartup.h
$O/cclassfactory.o: cclassfactory.cc \
  ../../include/cclassfactory.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  parsim/cplaceholdermod.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cmodule.h \
  ../../include/csimulation.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/ccommbuffer.o: ccommbuffer.cc \
  ../../include/ccommbuffer.h \
  ../../include/util.h \
  ../../include/cownedobject.h \
  ../../include/cclassfactory.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/carray.h
$O/ccomponent.o: ccomponent.cc \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cmodule.h \
  ../../include/cproperties.h \
  ../../include/cpar.h \
  ../../include/cparimpl.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/cconfigkey.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cgate.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cexpression.h
$O/ccomponenttype.o: ccomponenttype.cc \
  ../../include/ccomponenttype.h \
  ../../include/cmodule.h \
  ../../include/cchannel.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/globals.h \
  ../../include/ccommbuffer.h \
  parsim/cplaceholdermod.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cgate.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/ccompoundmodule.o: ccompoundmodule.cc \
  ../../include/ccompoundmodule.h \
  ../../include/ccomponenttype.h \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h
$O/cconfigkey.o: cconfigkey.cc \
  ../../include/cconfigkey.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cconfiguration.o: cconfiguration.cc \
  ../../include/cconfiguration.h \
  ../../include/cconfigkey.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
$O/ccoroutine.o: ccoroutine.cc \
  ../../include/ccoroutine.h \
  task.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
$O/cdefaultlist.o: cdefaultlist.cc \
  ../../include/globals.h \
  ../../include/carray.h \
  ../../include/cexception.h \
  ../../include/cdefaultlist.h \
  ../../include/cwatch.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cdensityestbase.o: cdensityestbase.cc \
  ../../include/globals.h \
  ../../include/cdensityestbase.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cstddev.h \
  ../../include/cstatistic.h
$O/cdetect.o: cdetect.cc \
  ../../include/csimulation.h \
  ../../include/cdetect.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
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
  ../../include/cstatistic.h
$O/cdisplaystring.o: cdisplaystring.cc \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cdisplaystring.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cdoubleparimpl.o: cdoubleparimpl.cc \
  ../../include/cdoubleparimpl.h \
  ../../include/cstringtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparimpl.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cdefaultlist.h
$O/cdynamicexpression.o: cdynamicexpression.cc \
  ../../include/cdynamicexpression.h \
  ../../include/cxmlelement.h \
  ../../include/cmathfunction.h \
  ../../include/cnedfunction.h \
  ../../include/cexception.h \
  expryydefs.h \
  ../../include/cpar.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cenum.o: cenum.cc \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/cenum.h \
  ../../include/cexception.h \
  ../../include/cstringtokenizer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cexception.o: cexception.cc \
  ../../include/cexception.h \
  ../../include/csimulation.h \
  ../../include/ccomponent.h \
  ../../include/cmodule.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cgate.h
$O/cfsm.o: cfsm.cc \
  ../../include/globals.h \
  ../../include/cfsm.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cgate.o: cgate.cc \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/csimulation.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/cchannel.h \
  ../../include/cbasicchannel.h \
  ../../include/cdisplaystring.h \
  ../../include/ccomponenttype.h \
  ../../include/util.h \
  ../../include/cownedobject.h \
  ../../include/cmsgpar.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
$O/chasher.o: chasher.cc \
  ../../include/chasher.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cexception.h \
  ../../include/platdep/inttypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
$O/chistogram.o: chistogram.cc \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/globals.h \
  ../../include/cdetect.h \
  ../../include/chistogram.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cstatistic.h \
  ../../include/cdensityestbase.h \
  ../../include/cstddev.h
$O/cksplit.o: cksplit.cc \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cksplit.h \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/cdensityestbase.h \
  ../../include/cstddev.h \
  ../../include/cstatistic.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
$O/clcg32.o: clcg32.cc \
  ../../include/clcg32.h \
  ../../include/util.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cconfigkey.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
$O/clinkedlist.o: clinkedlist.cc \
  ../../include/globals.h \
  ../../include/clinkedlist.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/clongparimpl.o: clongparimpl.cc \
  ../../include/clongparimpl.h \
  ../../include/cstringtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparimpl.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cdefaultlist.h
$O/cmathfunction.o: cmathfunction.cc \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/random.h \
  ../../include/ccommbuffer.h \
  parsim/cplaceholdermod.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
$O/cmersennetwister.o: cmersennetwister.cc \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cexception.h \
  ../../include/cmersennetwister.h \
  ../../include/cmessage.h \
  ../../include/cconfigkey.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/mersennetwister.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/cmathfunction.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cmessage.o: cmessage.cc \
  ../../include/globals.h \
  ../../include/cmodule.h \
  ../../include/csimplemodule.h \
  ../../include/cmessage.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h
$O/cmessageheap.o: cmessageheap.cc \
  ../../include/globals.h \
  ../../include/cmessage.h \
  ../../include/cmessageheap.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/cmathfunction.h \
  ../../include/ccoroutine.h
$O/cmodule.o: cmodule.cc \
  ../../include/cmodule.h \
  ../../include/csimplemodule.h \
  ../../include/cgate.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/carray.h \
  ../../include/ccomponenttype.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cdisplaystring.h \
  ../../include/cchannel.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/util.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h
$O/cmsgpar.o: cmsgpar.cc \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  ../../include/globals.h \
  ../../include/cmathfunction.h \
  ../../include/cxmlelement.h \
  ../../include/cconfiguration.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/csimulation.h \
  ../../include/ccommbuffer.h \
  ../../include/cownedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cnamedobject.o: cnamedobject.cc \
  ../../include/cnamedobject.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/ccommbuffer.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/cenvir.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/cnedfunction.o: cnedfunction.cc \
  ../../include/cnedfunction.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cdynamicexpression.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/cobject.o: cobject.cc \
  ../../include/cobject.h \
  ../../include/cexception.h \
  ../../include/cclassdescriptor.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cstringpool.h
$O/coutvector.o: coutvector.cc \
  ../../include/globals.h \
  ../../include/coutvector.h \
  ../../include/cmodule.h \
  ../../include/csimulation.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cenum.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h
$O/cownedobject.o: cownedobject.cc \
  ../../include/cownedobject.h \
  ../../include/csimulation.h \
  ../../include/cenvir.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/util.h \
  ../../include/cdefaultlist.h \
  ../../include/cclassdescriptor.h \
  ../../include/ccommbuffer.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/errmsg.h
$O/cpar.o: cpar.cc \
  ../../include/cpar.h \
  ../../include/cparimpl.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cmodule.h \
  ../../include/csimulation.h \
  ../../include/ccommbuffer.h \
  ../../include/cownedobject.h \
  ../../include/cexpression.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cdefaultlist.h \
  ../../include/cgate.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cparimpl.o: cparimpl.cc \
  ../../include/cparimpl.h \
  ../../include/cproperties.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/ccommbuffer.h \
  ../../include/cboolparimpl.h \
  ../../include/clongparimpl.h \
  ../../include/cdoubleparimpl.h \
  ../../include/cstringparimpl.h \
  ../../include/cxmlparimpl.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cparsimcomm.o: cparsimcomm.cc \
  ../../include/errmsg.h \
  ../../include/ccommbuffer.h \
  ../../include/cparsimcomm.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h
$O/cproperties.o: cproperties.cc \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cproperty.o: cproperty.cc \
  ../../include/cproperty.h \
  ../../include/cproperties.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cpsquare.o: cpsquare.cc \
  ../../include/globals.h \
  ../../include/cpsquare.h \
  ../../include/cexception.h \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cdensityestbase.h \
  ../../include/cstddev.h \
  ../../include/cstatistic.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
$O/cqueue.o: cqueue.cc \
  ../../include/globals.h \
  ../../include/cqueue.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cscheduler.o: cscheduler.cc \
  ../../include/cscheduler.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/cconfigkey.h \
  ../../include/cobject.h \
  ../../include/platdep/timeutil.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
$O/csimplemodule.o: csimplemodule.cc \
  ../../include/csimplemodule.h \
  ../../include/cgate.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/cqueue.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmathfunction.h \
  ../../include/opp_string.h
$O/csimulation.o: csimulation.cc \
  ../../include/cmodule.h \
  ../../include/csimplemodule.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/cscheduler.h \
  ../../include/cenvir.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/cexception.h \
  ../../include/cparimpl.h \
  ../../include/chasher.h \
  ../../include/cconfiguration.h \
  ../../include/ccommbuffer.h \
  netbuilder/cnedloader.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/timeutil.h \
  ../../include/opp_string.h \
  netbuilder/cneddeclaration.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h
$O/cstatistic.o: cstatistic.cc \
  ../../include/cstatistic.h \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/globals.h \
  ../../include/cdetect.h \
  ../../include/csimplemodule.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/ccommbuffer.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h
$O/cstddev.o: cstddev.cc \
  ../../include/cstddev.h \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/globals.h \
  ../../include/cdetect.h \
  ../../include/csimplemodule.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/ccommbuffer.h \
  ../../include/cstatistic.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h
$O/cstlwatch.o: cstlwatch.cc \
  ../../include/cstlwatch.h \
  ../../include/cclassdescriptor.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/cwatch.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/cstringparimpl.o: cstringparimpl.cc \
  ../../include/cstringparimpl.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparimpl.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cdefaultlist.h
$O/cstringpool.o: cstringpool.cc \
  ../../include/cstringpool.h \
  ../../include/cownedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cstringtokenizer.o: cstringtokenizer.cc \
  ../../include/cstringtokenizer.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/util.h
$O/ctopology.o: ctopology.cc \
  ../../include/ctopology.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/clinkedlist.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/cownedobject.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/cvarhist.o: cvarhist.cc \
  ../../include/globals.h \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/cvarhist.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/chistogram.h \
  ../../include/cdensityestbase.h \
  ../../include/cstddev.h \
  ../../include/cstatistic.h
$O/cvisitor.o: cvisitor.cc \
  ../../include/cvisitor.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h
$O/cwatch.o: cwatch.cc \
  ../../include/cwatch.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/cxmlelement.o: cxmlelement.cc \
  ../../include/cxmlelement.h \
  minixpath.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/util.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/cxmlparimpl.o: cxmlparimpl.cc \
  ../../include/cxmlparimpl.h \
  ../../include/cxmlelement.h \
  ../../include/cstringtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparimpl.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cdefaultlist.h
$O/distrib.o: distrib.cc \
  ../../include/distrib.h \
  ../../include/globals.h \
  ../../include/cmathfunction.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/util.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/errmsg.o: errmsg.cc \
  ../../include/errmsg.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
$O/expr.tab.o: expr.tab.cc \
  expryydefs.h \
  ../../include/cdynamicexpression.h \
  ../../include/cpar.h \
  ../../include/cxmlelement.h \
  ../../include/cexception.h \
  ../../include/globals.h \
  ../../include/cmathfunction.h \
  ../../include/cnedfunction.h \
  ../../include/nedsupport.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
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
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/globals.o: globals.cc \
  ../../include/globals.h \
  ../../include/carray.h \
  ../../include/onstartup.h \
  ../../include/ccomponenttype.h \
  ../../include/cclassfactory.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cpar.h \
  ../../include/cgate.h \
  ../../include/cexpression.h
$O/lex.expryy.o: lex.expryy.cc \
  expryydefs.h \
  ../../include/cexception.h \
  expr.tab.h \
  ../../include/util.h \
  ../../include/cdynamicexpression.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
$O/minixpath.o: minixpath.cc \
  minixpath.h \
  ../../include/cexception.h \
  ../../include/cxmlelement.h \
  ../../include/simkerneldefs.h \
  ../../include/cenvir.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/opp_string.h \
  ../../include/util.h \
  ../../include/errmsg.h
$O/nedfunctions.o: nedfunctions.cc \
  ../../include/distrib.h \
  ../../include/cmathfunction.h \
  ../../include/cnedfunction.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
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
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cdynamicexpression.h
$O/nedsupport.o: nedsupport.cc \
  ../../include/cdynamicexpression.h \
  ../../include/cxmlelement.h \
  ../../include/cmathfunction.h \
  ../../include/cnedfunction.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/nedsupport.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/onstartup.o: onstartup.cc \
  ../../include/onstartup.h \
  ../../include/cexception.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/sim_std_m.o: sim_std_m.cc \
  sim_std_m.h \
  ../../include/omnetpp.h \
  ../../include/carray.h \
  ../../include/cbasicchannel.h \
  ../../include/cboolparimpl.h \
  ../../include/cchannel.h \
  ../../include/cclassfactory.h \
  ../../include/ccommbuffer.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/ccompoundmodule.h \
  ../../include/cconfiguration.h \
  ../../include/ccoroutine.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cdetect.h \
  ../../include/cconfigkey.h \
  ../../include/cdisplaystring.h \
  ../../include/cdoubleparimpl.h \
  ../../include/cdynamicexpression.h \
  ../../include/cenum.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/chasher.h \
  ../../include/cfsm.h \
  ../../include/cmathfunction.h \
  ../../include/cgate.h \
  ../../include/chistogram.h \
  ../../include/cksplit.h \
  ../../include/clcg32.h \
  ../../include/clinkedlist.h \
  ../../include/clongparimpl.h \
  ../../include/cmersennetwister.h \
  ../../include/simtime.h \
  ../../include/cmessage.h \
  ../../include/cmsgpar.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../../include/cnedfunction.h \
  ../../include/cobject.h \
  ../../include/cnamedobject.h \
  ../../include/cownedobject.h \
  ../../include/coutvector.h \
  ../../include/cpar.h \
  ../../include/cparimpl.h \
  ../../include/cparsimcomm.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cpsquare.h \
  ../../include/cqueue.h \
  ../../include/crng.h \
  ../../include/cscheduler.h \
  ../../include/csimplemodule.h \
  ../../include/csimulation.h \
  ../../include/cstatistic.h \
  ../../include/cstddev.h \
  ../../include/cstlwatch.h \
  ../../include/cstringparimpl.h \
  ../../include/cstringpool.h \
  ../../include/cstringtokenizer.h \
  ../../include/cclassdescriptor.h \
  ../../include/ctopology.h \
  ../../include/cvarhist.h \
  ../../include/cvisitor.h \
  ../../include/cwatch.h \
  ../../include/cxmlelement.h \
  ../../include/cxmlparimpl.h \
  ../../include/simkerneldefs.h \
  ../../include/distrib.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/index.h \
  ../../include/mersennetwister.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/random.h \
  ../../include/regmacros.h \
  ../../include/util.h \
  ../../include/nedsupport.h \
  ../../include/compat.h \
  ../../include/platdep/inttypes.h \
  ../../include/platdep/timeutil.h \
  ../../include/platdep/platdefs.h
$O/simtime.o: simtime.cc \
  ../../include/simtime.h \
  ../../include/cexception.h \
  ../../include/cpar.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/inttypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/util.h \
  ../../include/cownedobject.h \
  ../../include/cexpression.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
$O/task.o: task.cc \
  task.h \
  ../../include/platdep/platdefs.h
$O/util.o: util.cc \
  ../../include/util.h \
  ../../include/cenvir.h \
  ../../include/csimulation.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/cmathfunction.h \
  ../../include/cscheduler.h \
  ../../include/distrib.h \
  ../../include/cwatch.h \
  ../../include/cstlwatch.h \
  ../../include/clcg32.h \
  ../../include/cmersennetwister.h \
  ../../include/cksplit.h \
  ../../include/cpsquare.h \
  ../../include/cstringtokenizer.h \
  ../../include/cxmlelement.h \
  ../../include/cbasicchannel.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/platdep/timeutil.h \
  ../../include/random.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cconfiguration.h \
  ../../include/mersennetwister.h \
  ../../include/cdensityestbase.h \
  ../../include/cstddev.h \
  ../../include/cstatistic.h \
  ../../include/cchannel.h \
  ../../include/ccomponenttype.h
$O/netbuilder/cdynamicchanneltype.o: netbuilder/cdynamicchanneltype.cc \
  netbuilder/cdynamicchanneltype.h \
  netbuilder/cnedloader.h \
  netbuilder/cnednetworkbuilder.h \
  ../../include/cchannel.h \
  ../../include/ccomponenttype.h \
  netbuilder/cneddeclaration.h \
  ../../include/ccomponent.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/globals.h \
  ../../include/cparimpl.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
$O/netbuilder/cdynamicmoduletype.o: netbuilder/cdynamicmoduletype.cc \
  netbuilder/cdynamicmoduletype.h \
  netbuilder/cneddeclaration.h \
  netbuilder/cnedloader.h \
  netbuilder/cnednetworkbuilder.h \
  ../../include/ccompoundmodule.h \
  ../../include/cmodule.h \
  ../../include/ccomponenttype.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cparimpl.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h
$O/netbuilder/cexpressionbuilder.o: netbuilder/cexpressionbuilder.cc \
  netbuilder/cexpressionbuilder.h \
  ../../include/cmathfunction.h \
  ../../include/cnedfunction.h \
  ../../include/cparimpl.h \
  ../../include/nedsupport.h \
  ../../include/cdynamicexpression.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cpar.h
$O/netbuilder/cneddeclaration.o: netbuilder/cneddeclaration.cc \
  netbuilder/cneddeclaration.h \
  ../../include/cexception.h \
  ../../include/cproperties.h \
  ../../include/cdynamicexpression.h \
  ../../include/cgate.h \
  ../../include/cmodule.h \
  ../../include/cdisplaystring.h \
  netbuilder/cnedloader.h \
  ../../include/simkerneldefs.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/cparimpl.h \
  ../../include/cproperty.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cpar.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h
$O/netbuilder/cnedloader.o: netbuilder/cnedloader.cc \
  netbuilder/cnedloader.h \
  ../../include/cproperty.h \
  ../../include/cproperties.h \
  ../../include/ccomponenttype.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  netbuilder/cexpressionbuilder.h \
  ../../include/cpar.h \
  ../../include/clongparimpl.h \
  ../../include/cdoubleparimpl.h \
  ../../include/globals.h \
  netbuilder/cdynamicmoduletype.h \
  netbuilder/cdynamicchanneltype.h \
  ../../include/cdisplaystring.h \
  ../../include/simkerneldefs.h \
  netbuilder/cneddeclaration.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/cparimpl.h \
  ../../include/cgate.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/opp_string.h \
  ../../include/cdynamicexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmodule.h \
  netbuilder/cnednetworkbuilder.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cchannel.h
$O/netbuilder/cnednetworkbuilder.o: netbuilder/cnednetworkbuilder.cc \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cchannel.h \
  ../../include/cbasicchannel.h \
  ../../include/ccomponenttype.h \
  ../../include/clongparimpl.h \
  ../../include/cboolparimpl.h \
  ../../include/cstringparimpl.h \
  ../../include/cdisplaystring.h \
  netbuilder/cneddeclaration.h \
  netbuilder/cnednetworkbuilder.h \
  netbuilder/cnedloader.h \
  netbuilder/cexpressionbuilder.h \
  ../../include/nedsupport.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cparimpl.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cdynamicexpression.h
$O/parsim/cadvlinkdelaylookahead.o: parsim/cadvlinkdelaylookahead.cc \
  parsim/cadvlinkdelaylookahead.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/cenvir.h \
  parsim/cnullmessageprot.h \
  ../../include/cparsimcomm.h \
  parsim/cparsimpartition.h \
  parsim/cplaceholdermod.h \
  parsim/cproxygate.h \
  ../../include/cchannel.h \
  parsim/cnmplookahead.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/ccomponenttype.h
$O/parsim/ccommbufferbase.o: parsim/ccommbufferbase.cc \
  parsim/ccommbufferbase.h \
  ../../include/cexception.h \
  ../../include/ccommbuffer.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
$O/parsim/cfilecomm.o: parsim/cfilecomm.cc \
  ../../include/cexception.h \
  parsim/cfilecomm.h \
  parsim/cfilecommbuffer.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  parsim/parsimutil.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cparsimcomm.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h
$O/parsim/cfilecommbuffer.o: parsim/cfilecommbuffer.cc \
  parsim/cfilecommbuffer.h \
  ../../include/util.h \
  ../../include/cownedobject.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
$O/parsim/cidealsimulationprot.o: parsim/cidealsimulationprot.cc \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  parsim/cidealsimulationprot.h \
  parsim/cplaceholdermod.h \
  parsim/cparsimpartition.h \
  ../../include/cparsimcomm.h \
  ../../include/ccommbuffer.h \
  parsim/messagetags.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
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
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
$O/parsim/cispeventlogger.o: parsim/cispeventlogger.cc \
  parsim/cispeventlogger.h \
  parsim/cidealsimulationprot.h \
  ../../include/cparsimcomm.h \
  parsim/cplaceholdermod.h \
  ../../include/cmodule.h \
  parsim/cnullmessageprot.h \
  parsim/cparsimprotocolbase.h \
  ../../include/cmessage.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/cobject.h \
  ../../include/platdep/timeutil.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
$O/parsim/clinkdelaylookahead.o: parsim/clinkdelaylookahead.cc \
  parsim/clinkdelaylookahead.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/cenvir.h \
  parsim/cnullmessageprot.h \
  ../../include/cparsimcomm.h \
  parsim/cparsimpartition.h \
  parsim/cplaceholdermod.h \
  parsim/cproxygate.h \
  ../../include/cchannel.h \
  parsim/cnmplookahead.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/ccomponenttype.h
$O/parsim/cmemcommbuffer.o: parsim/cmemcommbuffer.cc \
  parsim/cmemcommbuffer.h \
  ../../include/util.h \
  ../../include/cownedobject.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
$O/parsim/cmpicomm.o: parsim/cmpicomm.cc \
  parsim/cmpicomm.h \
  parsim/cmpicommbuffer.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/cparsimcomm.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h \
  ../../include/opp_string.h
$O/parsim/cmpicommbuffer.o: parsim/cmpicommbuffer.cc \
  parsim/cmpicommbuffer.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h
$O/parsim/cnamedpipecomm-win.o: parsim/cnamedpipecomm-win.cc \
  parsim/cnamedpipecomm.h \
  ../../include/cexception.h \
  parsim/cmemcommbuffer.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  parsim/parsimutil.h \
  ../../include/util.h \
  ../../include/opp_string.h \
  ../../include/cparsimcomm.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h
$O/parsim/cnamedpipecomm.o: parsim/cnamedpipecomm.cc \
  parsim/cnamedpipecomm.h \
  ../../include/cexception.h \
  parsim/cmemcommbuffer.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  parsim/parsimutil.h \
  ../../include/util.h \
  ../../include/opp_string.h \
  ../../include/cparsimcomm.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h
$O/parsim/cnosynchronization.o: parsim/cnosynchronization.cc \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  parsim/cnosynchronization.h \
  parsim/cparsimpartition.h \
  ../../include/cparsimcomm.h \
  ../../include/ccommbuffer.h \
  parsim/messagetags.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
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
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
$O/parsim/cnullmessageprot.o: parsim/cnullmessageprot.cc \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  parsim/cnullmessageprot.h \
  parsim/clinkdelaylookahead.h \
  parsim/cparsimpartition.h \
  ../../include/cparsimcomm.h \
  ../../include/ccommbuffer.h \
  parsim/messagetags.h \
  parsim/cplaceholdermod.h \
  parsim/cproxygate.h \
  ../../include/cchannel.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
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
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h \
  parsim/cnmplookahead.h \
  ../../include/ccomponenttype.h
$O/parsim/cparsimpartition.o: parsim/cparsimpartition.cc \
  ../../include/cmessage.h \
  ../../include/errmsg.h \
  parsim/cplaceholdermod.h \
  parsim/cproxygate.h \
  parsim/cparsimpartition.h \
  ../../include/ccommbuffer.h \
  ../../include/cparsimcomm.h \
  parsim/cparsimsynchr.h \
  parsim/creceivedexception.h \
  parsim/messagetags.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
$O/parsim/cparsimprotocolbase.o: parsim/cparsimprotocolbase.cc \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  parsim/cparsimpartition.h \
  ../../include/cparsimcomm.h \
  ../../include/ccommbuffer.h \
  parsim/messagetags.h \
  parsim/cparsimprotocolbase.h \
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
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
  ../../include/opp_string.h \
  ../../include/cmathfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
$O/parsim/cplaceholdermod.o: parsim/cplaceholdermod.cc \
  parsim/cplaceholdermod.h \
  parsim/cproxygate.h \
  parsim/cfilecomm.h \
  parsim/cnamedpipecomm.h \
  parsim/cnosynchronization.h \
  parsim/cnullmessageprot.h \
  parsim/cispeventlogger.h \
  parsim/cidealsimulationprot.h \
  parsim/clinkdelaylookahead.h \
  ../../include/cmodule.h \
  ../../include/csimulation.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/simkerneldefs.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cparsimcomm.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h \
  ../../include/cmessage.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h \
  parsim/cnmplookahead.h
$O/parsim/cproxygate.o: parsim/cproxygate.cc \
  parsim/cproxygate.h \
  parsim/cparsimpartition.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/cgate.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmessageheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cmathfunction.h
$O/parsim/cptrpassingmpicomm.o: parsim/cptrpassingmpicomm.cc \
  parsim/cptrpassingmpicomm.h \
  parsim/cptrpassingmpicommbuffer.h \
  ../../include/cparsimcomm.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  parsim/cmpicommbuffer.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h
$O/parsim/cptrpassingmpicommbuffer.o: parsim/cptrpassingmpicommbuffer.cc \
  parsim/cmpicommbuffer.h \
  parsim/ccommbufferbase.h \
  ../../include/ccommbuffer.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h
$O/parsim/creceivedexception.o: parsim/creceivedexception.cc \
  parsim/creceivedexception.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h
$O/parsim/parsimutil.o: parsim/parsimutil.cc \
  ../../include/cenvir.h \
  parsim/parsimutil.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h
