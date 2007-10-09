#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -DBUILDING_SIM $(MPI_CFLAGS)

OBJS_STD=\
    carray.o cbasicchannel.o cboolpar.o cchannel.o \
    cclassfactory.o ccomponent.o ccomponenttype.o cconfiguration.o cconfigkey.o \
    ccompoundmodule.o ccoroutine.o cdefaultlist.o cdensity.o \
    cdetect.o cdisplaystring.o cdoublepar.o cdynamicexpression.o \
    cenum.o cexception.o cfsm.o cfunction.o cgate.o chist.o \
    cksplit.o clcg32.o cllist.o clongpar.o cmersennetwister.o \
    cmessage.o cmsgpar.o cmodule.o cmsgheap.o chasher.o \
    cnedfunction.o cobject.o coutvector.o cnamedobject.o \
    cpar.o cparvalue.o cownedobject.o cproperties.o cproperty.o \
    cpsquare.o cqueue.o cscheduler.o csimplemodule.o \
    csimulation.o cstat.o cstlwatch.o cstringpar.o \
    cstringpool.o cstrtokenizer.o cclassdescriptor.o ctopo.o cvarhist.o \
    cvisitor.o cwatch.o cxmlelement.o cxmlpar.o distrib.o \
    errmsg.o globals.o lex.expryy.o minixpath.o onstartup.o \
    simtime.o task.o util.o expr.tab.o nedsupport.o sim_std_m.o \
    ccommbuffer.o cparsimcomm.o

OBJS_NETBUILDER=\
    netbuilder/cexpressionbuilder.o \
    netbuilder/cneddeclaration.o \
    netbuilder/cnedloader.o \
    netbuilder/cdynamicchanneltype.o \
    netbuilder/cdynamicmoduletype.o \
    netbuilder/cnednetworkbuilder.o

OBJS_PARSIM=\
    parsim\cmemcommbuffer.o \
    parsim\cparsimpartition.o parsim/cplaceholdermod.o parsim/cproxygate.o \
    parsim/cparsimprotocolbase.o parsim/cnosynchronization.o \
    parsim/cnullmessageprot.o parsim/clinkdelaylookahead.o \
    parsim/cidealsimulationprot.o parsim/cispeventlogger.o \
    parsim/ccommbufferbase.o parsim/cfilecomm.o \
    parsim/cfilecommbuffer.o parsim/cnamedpipecomm-win.o parsim/parsimutil.o \
    parsim/creceivedexception.o

OBJS_MPI=\
    parsim/cmpicomm.o parsim/cmpicommbuffer.o

OBJS= $(OBJS_STD)

ifeq ($(WITH_NETBUILDER),yes)
OBJS += $(OBJS_NETBUILDER)
COPTS += -I../nedxml
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

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
default : sim_std

sim_std : libsim_std$(LIB_SUFFIX)

libsim_std$(A_LIB_SUFFIX) : $(OBJS)
	$(AR) libsim_std$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libsim_std$(A_LIB_SUFFIX)
	../utils/install-lib libsim_std$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libsim_std$(A_LIB_SUFFIX)

libsim_std$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o libsim_std$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libsim_std$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libsim_std$(SO_LIB_SUFFIX)

expr.tab.h expr.tab.cc : expr.y
	$(YACC) -p expryy -b expr -d expr.y
	mv expr.tab.c expr.tab.cc

lex.expryy.cc: expr.lex expr.tab.h
	$(LEX) -Pexpryy expr.lex
	mv lex.expryy.c lex.expryy.cc

depend:
	$(MAKEDEPEND) -I../../include -- *.cc netbuilder/*.cc parsim/*.cc

sim_std_m.cc sim_std_m.h : sim_std.msg
	$(MSGC) -Xnc -Xns sim_std.msg

clean:
	rm -f expr.tab.h expr.tab.cc lex.expryy.cc
	rm -f *.o parsim/*.o netbuilder/*.o
	rm -f libsim_std$(A_LIB_SUFFIX) libsim_std$(SO_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
carray.o: carray.cc \
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
cbasicchannel.o: cbasicchannel.cc \
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
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cexpression.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  ../../include/opp_string.h \
  ../../include/crng.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
cboolpar.o: cboolpar.cc \
  ../../include/cboolpar.h \
  ../../include/cstrtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparvalue.h \
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
cchannel.o: cchannel.cc \
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
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cexpression.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  ../../include/opp_string.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
cclassdescriptor.o: cclassdescriptor.cc \
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
cclassfactory.o: cclassfactory.cc \
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
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
ccommbuffer.o: ccommbuffer.cc \
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
ccomponent.o: ccomponent.cc \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/cproperties.h \
  ../../include/cpar.h \
  ../../include/cparvalue.h \
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
  ../../include/cneddeclarationbase.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cexpression.h
ccomponenttype.o: ccomponenttype.cc \
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
  ../../include/cneddeclarationbase.h \
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
  ../../include/cparvalue.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cdefaultlist.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
ccompoundmodule.o: ccompoundmodule.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h
cconfigkey.o: cconfigkey.cc \
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
cconfiguration.o: cconfiguration.cc \
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
ccoroutine.o: ccoroutine.cc \
  ../../include/ccoroutine.h \
  task.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
cdefaultlist.o: cdefaultlist.cc \
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
cdensity.o: cdensity.cc \
  ../../include/globals.h \
  ../../include/cdensity.h \
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
  ../../include/cstat.h
cdetect.o: cdetect.cc \
  ../../include/csimulation.h \
  ../../include/cdetect.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmsgheap.h \
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
  ../../include/cstat.h
cdisplaystring.o: cdisplaystring.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cdoublepar.o: cdoublepar.cc \
  ../../include/cdoublepar.h \
  ../../include/cstrtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparvalue.h \
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
cdynamicexpression.o: cdynamicexpression.cc \
  ../../include/cdynamicexpression.h \
  ../../include/cxmlelement.h \
  ../../include/cfunction.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cenum.o: cenum.cc \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/cenum.h \
  ../../include/cexception.h \
  ../../include/cstrtokenizer.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cexception.o: cexception.cc \
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
  ../../include/cmsgheap.h \
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
cfsm.o: cfsm.cc \
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
cfunction.o: cfunction.cc \
  ../../include/cfunction.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
cgate.o: cgate.cc \
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
  ../../include/util.h \
  ../../include/ccomponenttype.h \
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
  ../../include/cfunction.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h
chasher.o: chasher.cc \
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
chist.o: chist.cc \
  ../../include/random.h \
  ../../include/distrib.h \
  ../../include/globals.h \
  ../../include/cdetect.h \
  ../../include/chist.h \
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
  ../../include/cmsgheap.h \
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
  ../../include/cstat.h \
  ../../include/cdensity.h
cksplit.o: cksplit.cc \
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
  ../../include/cdensity.h \
  ../../include/cstat.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
clcg32.o: clcg32.cc \
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
cllist.o: cllist.cc \
  ../../include/globals.h \
  ../../include/cllist.h \
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
clongpar.o: clongpar.cc \
  ../../include/clongpar.h \
  ../../include/cstrtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparvalue.h \
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
cmersennetwister.o: cmersennetwister.cc \
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
  ../../include/cfunction.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cmessage.o: cmessage.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h
cmodule.o: cmodule.cc \
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
  ../../include/cfunction.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/opp_string.h \
  ../../include/errmsg.h
cmsgheap.o: cmsgheap.cc \
  ../../include/globals.h \
  ../../include/cmessage.h \
  ../../include/cmsgheap.h \
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
  ../../include/cfunction.h \
  ../../include/ccoroutine.h
cmsgpar.o: cmsgpar.cc \
  ../../include/cmsgpar.h \
  ../../include/cstat.h \
  ../../include/globals.h \
  ../../include/cfunction.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cnamedobject.o: cnamedobject.cc \
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
cnedfunction.o: cnedfunction.cc \
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
cobject.o: cobject.cc \
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
coutvector.o: coutvector.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h
cownedobject.o: cownedobject.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/errmsg.h
cpar.o: cpar.cc \
  ../../include/cpar.h \
  ../../include/cparvalue.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
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
  ../../include/cneddeclarationbase.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cparsimcomm.o: cparsimcomm.cc \
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
cparvalue.o: cparvalue.cc \
  ../../include/cparvalue.h \
  ../../include/cproperties.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/ccommbuffer.h \
  ../../include/cboolpar.h \
  ../../include/clongpar.h \
  ../../include/cdoublepar.h \
  ../../include/cstringpar.h \
  ../../include/cxmlpar.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cproperties.o: cproperties.cc \
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
cproperty.o: cproperty.cc \
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
cpsquare.o: cpsquare.cc \
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
  ../../include/cdensity.h \
  ../../include/cstat.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/crng.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
cqueue.o: cqueue.cc \
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
cscheduler.o: cscheduler.cc \
  ../../include/cscheduler.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/cmsgheap.h \
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
  ../../include/cfunction.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h
csimplemodule.o: csimplemodule.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cfunction.h \
  ../../include/opp_string.h
csimulation.o: csimulation.cc \
  ../../include/cmodule.h \
  ../../include/csimplemodule.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/cscheduler.h \
  ../../include/cenvir.h \
  ../../include/ccomponenttype.h \
  ../../include/cstat.h \
  ../../include/cexception.h \
  ../../include/cparvalue.h \
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
  ../../include/cfunction.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/timeutil.h \
  ../../include/opp_string.h \
  ../../include/cneddeclarationbase.h \
  netbuilder/cneddeclaration.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h
cstat.o: cstat.cc \
  ../../include/cstat.h \
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
  ../../include/cmsgheap.h \
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
cstlwatch.o: cstlwatch.cc \
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
cstringpar.o: cstringpar.cc \
  ../../include/cstringpar.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparvalue.h \
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
cstringpool.o: cstringpool.cc \
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
cstrtokenizer.o: cstrtokenizer.cc \
  ../../include/cstrtokenizer.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/util.h
ctopo.o: ctopo.cc \
  ../../include/ctopo.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cllist.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h
cvarhist.o: cvarhist.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/chist.h \
  ../../include/cdensity.h \
  ../../include/cstat.h
cvisitor.o: cvisitor.cc \
  ../../include/cvisitor.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h
cwatch.o: cwatch.cc \
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
cxmlelement.o: cxmlelement.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
cxmlpar.o: cxmlpar.cc \
  ../../include/cxmlpar.h \
  ../../include/cxmlelement.h \
  ../../include/cstrtokenizer.h \
  ../../include/cdynamicexpression.h \
  ../../include/ccomponent.h \
  ../../include/cparvalue.h \
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
distrib.o: distrib.cc \
  ../../include/distrib.h \
  ../../include/globals.h \
  ../../include/cfunction.h \
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
  ../../include/cmsgheap.h \
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
errmsg.o: errmsg.cc \
  ../../include/errmsg.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
globals.o: globals.cc \
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
  ../../include/cneddeclarationbase.h \
  ../../include/cexpression.h \
  ../../include/cparvalue.h
minixpath.o: minixpath.cc \
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
nedsupport.o: nedsupport.cc \
  ../../include/cdynamicexpression.h \
  ../../include/cxmlelement.h \
  ../../include/cfunction.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
onstartup.o: onstartup.cc \
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
sim_std_m.o: sim_std_m.cc \
  sim_std_m.h \
  ../../include/omnetpp.h \
  ../../include/carray.h \
  ../../include/cbasicchannel.h \
  ../../include/cboolpar.h \
  ../../include/cchannel.h \
  ../../include/cclassfactory.h \
  ../../include/ccommbuffer.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
  ../../include/ccompoundmodule.h \
  ../../include/cconfiguration.h \
  ../../include/ccoroutine.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensity.h \
  ../../include/cdetect.h \
  ../../include/cconfigkey.h \
  ../../include/cdisplaystring.h \
  ../../include/cdoublepar.h \
  ../../include/cdynamicexpression.h \
  ../../include/cenum.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/cexpression.h \
  ../../include/chasher.h \
  ../../include/cfsm.h \
  ../../include/cfunction.h \
  ../../include/cgate.h \
  ../../include/chist.h \
  ../../include/cksplit.h \
  ../../include/clcg32.h \
  ../../include/cllist.h \
  ../../include/clongpar.h \
  ../../include/cmersennetwister.h \
  ../../include/simtime.h \
  ../../include/cmessage.h \
  ../../include/cmsgpar.h \
  ../../include/cmodule.h \
  ../../include/cmsgheap.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cnedfunction.h \
  ../../include/cobject.h \
  ../../include/cnamedobject.h \
  ../../include/cownedobject.h \
  ../../include/coutvector.h \
  ../../include/cpar.h \
  ../../include/cparvalue.h \
  ../../include/cparsimcomm.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cpsquare.h \
  ../../include/cqueue.h \
  ../../include/crng.h \
  ../../include/cscheduler.h \
  ../../include/csimplemodule.h \
  ../../include/csimulation.h \
  ../../include/cstat.h \
  ../../include/cstlwatch.h \
  ../../include/cstringpar.h \
  ../../include/cstringpool.h \
  ../../include/cstrtokenizer.h \
  ../../include/cclassdescriptor.h \
  ../../include/ctopo.h \
  ../../include/cvarhist.h \
  ../../include/cvisitor.h \
  ../../include/cwatch.h \
  ../../include/cxmlelement.h \
  ../../include/cxmlpar.h \
  ../../include/simkerneldefs.h \
  ../../include/distrib.h \
  ../../include/envirext.h \
  ../../include/errmsg.h \
  ../../include/globals.h \
  ../../include/index.h \
  ../../include/mersennetwister.h \
  ../../include/onstartup.h \
  ../../include/opp_string.h \
  ../../include/protocol.h \
  ../../include/random.h \
  ../../include/regmacros.h \
  ../../include/util.h \
  ../../include/nedsupport.h \
  ../../include/compat.h \
  ../../include/platdep/inttypes.h \
  ../../include/platdep/timeutil.h \
  ../../include/platdep/platdefs.h
simtime.o: simtime.cc \
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
task.o: task.cc \
  task.h
util.o: util.cc \
  ../../include/util.h \
  ../../include/cenvir.h \
  ../../include/csimulation.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  ../../include/cfunction.h \
  ../../include/cscheduler.h \
  ../../include/distrib.h \
  ../../include/cwatch.h \
  ../../include/cstlwatch.h \
  ../../include/clcg32.h \
  ../../include/cmersennetwister.h \
  ../../include/cksplit.h \
  ../../include/cpsquare.h \
  ../../include/cstrtokenizer.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/opp_string.h \
  ../../include/cmsgheap.h \
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
  ../../include/cdensity.h \
  ../../include/cstat.h
xxx.o: xxx.cc \
  ../../include/stlwrap.h \
  ../../include/cobject.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h
netbuilder/cdynamicchanneltype.o: netbuilder/cdynamicchanneltype.cc \
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
  ../../include/cneddeclarationbase.h \
  ../../include/globals.h \
  ../../include/cparvalue.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h
netbuilder/cdynamicmoduletype.o: netbuilder/cdynamicmoduletype.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h
netbuilder/cexpressionbuilder.o: netbuilder/cexpressionbuilder.cc \
  netbuilder/cexpressionbuilder.h \
  ../../include/cfunction.h \
  ../../include/cnedfunction.h \
  ../../include/cparvalue.h \
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
netbuilder/cneddeclaration.o: netbuilder/cneddeclaration.cc \
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
  ../../include/cparvalue.h \
  ../../include/cneddeclarationbase.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h
netbuilder/cnedloader.o: netbuilder/cnedloader.cc \
  netbuilder/cnedloader.h \
  ../../include/cproperty.h \
  ../../include/cproperties.h \
  ../../include/ccomponenttype.h \
  ../../include/cexception.h \
  ../../include/cenvir.h \
  netbuilder/cexpressionbuilder.h \
  ../../include/cpar.h \
  ../../include/clongpar.h \
  ../../include/cdoublepar.h \
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
  ../../include/cparvalue.h \
  ../../include/cgate.h \
  ../../include/cneddeclarationbase.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cchannel.h
netbuilder/cnednetworkbuilder.o: netbuilder/cnednetworkbuilder.cc \
  ../../include/cmodule.h \
  ../../include/cgate.h \
  ../../include/cchannel.h \
  ../../include/cbasicchannel.h \
  ../../include/ccomponenttype.h \
  ../../include/clongpar.h \
  ../../include/cboolpar.h \
  ../../include/cstringpar.h \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/cdynamicexpression.h
parsim/cadvlinkdelaylookahead.o: parsim/cadvlinkdelaylookahead.cc \
  parsim/cadvlinkdelaylookahead.h \
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
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
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
  ../../include/ccomponenttype.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h
parsim/ccommbufferbase.o: parsim/ccommbufferbase.cc \
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
parsim/cfilecommbuffer.o: parsim/cfilecommbuffer.cc \
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
parsim/cfilecomm.o: parsim/cfilecomm.cc \
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
parsim/cidealsimulationprot.o: parsim/cidealsimulationprot.cc \
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
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
parsim/cispeventlogger.o: parsim/cispeventlogger.cc \
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
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
parsim/clinkdelaylookahead.o: parsim/clinkdelaylookahead.cc \
  parsim/clinkdelaylookahead.h \
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
  ../../include/cownedobject.h \
  ../../include/carray.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/opp_string.h \
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
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
  ../../include/ccomponenttype.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h
parsim/cmemcommbuffer.o: parsim/cmemcommbuffer.cc \
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
parsim/cmpicommbuffer.o: parsim/cmpicommbuffer.cc \
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
parsim/cmpicomm.o: parsim/cmpicomm.cc \
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
parsim/cnamedpipecomm.o: parsim/cnamedpipecomm.cc \
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
parsim/cnamedpipecomm-win.o: parsim/cnamedpipecomm-win.cc \
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
parsim/cnosynchronization.o: parsim/cnosynchronization.cc \
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
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
parsim/cnullmessageprot.o: parsim/cnullmessageprot.cc \
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
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
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
  ../../include/ccomponenttype.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h
parsim/cparsimpartition.o: parsim/cparsimpartition.cc \
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
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
parsim/cparsimprotocolbase.o: parsim/cparsimprotocolbase.cc \
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
  ../../include/cfunction.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h
parsim/cplaceholdermod.o: parsim/cplaceholdermod.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cparsimcomm.h \
  parsim/cparsimprotocolbase.h \
  parsim/cparsimsynchr.h \
  ../../include/cscheduler.h \
  ../../include/platdep/timeutil.h \
  ../../include/cmessage.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  parsim/cnmplookahead.h
parsim/cproxygate.o: parsim/cproxygate.cc \
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
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h
parsim/cptrpassingmpicommbuffer.o: parsim/cptrpassingmpicommbuffer.cc \
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
parsim/cptrpassingmpicomm.o: parsim/cptrpassingmpicomm.cc \
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
parsim/creceivedexception.o: parsim/creceivedexception.cc \
  parsim/creceivedexception.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h
parsim/parsimutil.o: parsim/parsimutil.cc \
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
