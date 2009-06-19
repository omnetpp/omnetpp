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


#XXX should come from configuser.vc
#TODO: in the released source, don't run swig (except via an explicit target)
GUIENV_IF_POSSIBLE = guienv
SWIG = swig
JAVA_LIBS = -L/usr/lib/jvm/java-6-sun/jre/lib/i386/client -ljvm

#
# Local definitions
#
LIBNAME=liboppguienv$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/guienv
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../envir -I../nedxml -I../layout -I../eventlog -I../common $(JAVA_CFLAGS)
IMPLIBS= -L$L -loppsim$D -loppenvir$D -lopplayout$D -loppcommon$D $(JAVA_LIBS)

OBJS = $O/visitor.o $O/jutil.o $O/logbuffer.o $O/logbufferview.o \
       $O/jcallback.o $O/guienv.o $O/wrappertable.o \
       $O/simkernel_wrap.o $O/registernatives.o

JAVAPKG = org.omnetpp.runtime.nativelibs.simkernel
JAVADIR = $(OMNETPP_UI_DIR)/org.omnetpp.runtime.nativelibs/src/$(subst .,/,$(JAVAPKG))

JAVA_COBJECTS = \
    cAccuracyDetection.java cADByStddev.java cArray.java cChannel.java \
    cChannelType.java cClassDescriptor.java cClassFactory.java cComponent.java \
    cComponentType.java cCompoundModule.java cConfigOption.java \
    cConfiguration.java cConfigurationEx.java cDatarateChannel.java \
    cDefaultList.java cDelayChannel.java cDensityEstBase.java cDoubleHistogram.java \
    cEnum.java cEqdHistogramBase.java cFSM.java cGate.java cHistogramBase.java \
    cIdealChannel.java cLongHistogram.java cMathFunction.java cMessage.java \
    cMessageHeap.java cModule.java cModuleType.java cMsgPar.java \
    cNamedObject.java cNEDFunction.java cNoncopyableOwnedObject.java \
    cObject.java cOutVector.java cOwnedObject.java cPacket.java cPar.java \
    cProperties.java cProperty.java cPSquare.java cQueue.java \
    cRegistrationList.java cSimpleModule.java \
    cSimulation.java cStatistic.java cStdDev.java cStdVectorWatcherBase.java \
    cTDExpandingWindows.java cTopology.java cTransientDetection.java \
    cVarHistogram.java cWatchBase.java cWatch_bool.java \
    cWatch_char.java cWatch_cObject.java cWatch_cObjectPtr.java \
    cWatch_stdstring.java cWatch_uchar.java cWeightedStdDev.java

#
# Automatic rules
#
.SUFFIXES : .cc .cxx

$O/%.o: %.cxx
	@mkdir -p $O
	$(CC) -c $(COPTS) -o $@ $<

$O/%.o: %.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $(GUIENV_IF_POSSIBLE)

guienv : $O/$(LIBNAME)$(LIB_SUFFIX)

guienv : $L/$(LIBNAME)$(LIB_SUFFIX)
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

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

# only run swig in developer mode (when ui/ source tree exists)
ifneq ("$(wildcard $(OMNETPP_UI_DIR))","")
simkernel_wrap.cxx registernatives.cc : *.i *.h Makefile.vc $(OMNETPP_ROOT)/include/*.h  # and many other things
	rm -rf $(JAVADIR)
	mkdir $(JAVADIR)
	$(SWIG) -c++ -java -package $(JAVAPKG) -I$(OMNETPP_ROOT)/include -I$(OMNETPP_ROOT)/src/envir -I$(OMNETPP_ROOT) -outdir $(JAVADIR) simkernel.i
	@# Patch the generated Java files so that they access swigCPtr via checkZombie() that detects null pointers (see WrapperTable!)
	for i in $(JAVA_COBJECTS); do $(PERL) -i.bak -pe "s!\(swigCPtr, this!(checkZombie(swigCPtr), this!g" $(JAVADIR)/$$i; done
	for i in $(JAVA_COBJECTS); do $(PERL) -i.bak -pe "s!0 : obj.swigCPtr;!0 : obj.checkZombie(obj.swigCPtr);!g" $(JAVADIR)/$$i; done
	rm -f $(JAVADIR)/*.bak
	$(PERL) registernatives.pl $(JAVADIR)/SimkernelJNI.java registernatives.cc
endif

missing-dependency:
	@echo '  *** No JDK installation found, skipping GUIEnv.'
	@echo '  *** Install it, and re-run configure.'

depend:
	$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc *.cxx

clean:
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)\$(LIBNAME).*

veryclean: clean
	rm -f simkernel_wrap.cxx registernatives.cc


# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/guienv.obj: guienv.cc \
  ../../include/platdep/timeutil.h \
  ../../include/cboolparimpl.h \
  ../../include/cpsquare.h \
  ../../include/carray.h \
  ../../include/ccompoundmodule.h \
  ../../include/cgate.h \
  ../../include/cdataratechannel.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/cmodule.h \
  ../../include/cclassdescriptor.h \
  ../../include/cmessage.h \
  logbuffer.h \
  ../../include/csimulation.h \
  ../../include/clcg32.h \
  guienv.h \
  ../../include/simtime.h \
  ../../include/simkerneldefs.h \
  ../../include/simutil.h \
  ../../include/cconfigoption.h \
  ../../include/cwatch.h \
  ../../include/cenum.h \
  ../../include/cstlwatch.h \
  ../../include/cmsgpar.h \
  ../../include/cobject.h \
  ../../include/cfsm.h \
  ../../include/nedsupport.h \
  ../../include/cclassfactory.h \
  ../../include/cnullenvir.h \
  ../../include/cstddev.h \
  ../../include/cdoubleparimpl.h \
  ../../include/errmsg.h \
  ../../include/packing.h \
  ../../include/ctopology.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/distrib.h \
  ../../include/cnedfunction.h \
  ../../include/cpacketqueue.h \
  ../../include/cownedobject.h \
  ../../include/cdisplaystring.h \
  ../../include/cexpression.h \
  wrappertable.h \
  ../../include/compat.h \
  ../../include/crng.h \
  ../../include/cdetect.h \
  ../../include/random.h \
  ../../include/cproperty.h \
  ../../include/clinkedlist.h \
  ../../include/cparimpl.h \
  ../../include/cchannel.h \
  ../../include/cparsimcomm.h \
  ../../include/cproperties.h \
  ../../include/cqueue.h \
  ../../include/chistogram.h \
  ../../include/ccomponent.h \
  ../../include/cstringtokenizer.h \
  ../../include/omnetpp.h \
  ../../include/cregistrationlist.h \
  ../../include/ccommbuffer.h \
  ../../include/cscheduler.h \
  ../../include/clongparimpl.h \
  ../../include/mersennetwister.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/cxmlelement.h \
  ../../include/cmathfunction.h \
  jcallback.h \
  ../../include/coutvector.h \
  ../../include/onstartup.h \
  ../../include/cxmlparimpl.h \
  ../../include/simtime_t.h \
  ../../include/csimplemodule.h \
  ../../include/cdelaychannel.h \
  ../../include/cdensityestbase.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cmessageheap.h \
  ../../include/cvarhist.h \
  ../../include/globals.h \
  ../../include/cstringparimpl.h \
  ../../include/platdep/platmisc.h \
  ../../include/chasher.h \
  ../../include/cdefaultlist.h \
  ../../include/index.h \
  ../../include/cmersennetwister.h \
  ../../include/opp_string.h \
  ../../include/envirext.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cdynamicexpression.h \
  ../../include/cksplit.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  ../../include/ccoroutine.h \
  guienvdefs.h \
  ../../include/regmacros.h
$O/jcallback.obj: jcallback.cc \
  ../../include/platdep/timeutil.h \
  ../../include/cpsquare.h \
  ../../include/cboolparimpl.h \
  ../../include/carray.h \
  ../../include/ccompoundmodule.h \
  ../../include/cgate.h \
  ../../include/cdataratechannel.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/cmodule.h \
  ../../include/cclassdescriptor.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/clcg32.h \
  ../../include/simtime.h \
  ../../include/simkerneldefs.h \
  ../../include/simutil.h \
  ../../include/cwatch.h \
  ../../include/cconfigoption.h \
  ../../include/cstlwatch.h \
  ../../include/cenum.h \
  ../../include/cmsgpar.h \
  ../../include/cobject.h \
  ../../include/cfsm.h \
  ../../include/nedsupport.h \
  ../../include/cstddev.h \
  ../../include/cnullenvir.h \
  ../../include/cclassfactory.h \
  ../../include/cdoubleparimpl.h \
  ../../include/errmsg.h \
  ../../include/packing.h \
  ../../include/ctopology.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/distrib.h \
  ../../include/cnedfunction.h \
  ../../include/cpacketqueue.h \
  ../../include/cownedobject.h \
  ../../include/cdisplaystring.h \
  ../../include/cexpression.h \
  ../../include/compat.h \
  ../../include/crng.h \
  ../../include/random.h \
  ../../include/cdetect.h \
  ../../include/cproperty.h \
  ../../include/cparimpl.h \
  ../../include/clinkedlist.h \
  ../../include/cqueue.h \
  ../../include/cproperties.h \
  ../../include/cparsimcomm.h \
  ../../include/cchannel.h \
  ../../include/chistogram.h \
  ../../include/ccomponent.h \
  ../../include/cstringtokenizer.h \
  ../../include/omnetpp.h \
  ../../include/cregistrationlist.h \
  ../../include/ccommbuffer.h \
  ../../include/cscheduler.h \
  ../../include/clongparimpl.h \
  ../../include/mersennetwister.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/cxmlelement.h \
  ../../include/cmathfunction.h \
  jcallback.h \
  ../../include/coutvector.h \
  ../../include/onstartup.h \
  ../../include/cxmlparimpl.h \
  ../../include/simtime_t.h \
  ../../include/cdelaychannel.h \
  ../../include/csimplemodule.h \
  ../../include/cdensityestbase.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cvarhist.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/platdep/platmisc.h \
  ../../include/cstringparimpl.h \
  ../../include/chasher.h \
  ../../include/index.h \
  ../../include/cdefaultlist.h \
  ../../include/opp_string.h \
  ../../include/cmersennetwister.h \
  ../../include/envirext.h \
  ../../include/cexception.h \
  ../../include/cdynamicexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cksplit.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  ../../include/ccoroutine.h \
  guienvdefs.h \
  ../../include/regmacros.h
$O/jutil.obj: jutil.cc \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  ../../include/csimulation.h \
  ../../include/simtime.h \
  ../../include/simkerneldefs.h \
  ../../include/simutil.h \
  ../../include/cobject.h \
  ../../include/errmsg.h \
  ../../include/ccomponenttype.h \
  ../../include/cownedobject.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  jutil.h \
  ../../include/simtime_t.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platmisc.h \
  ../../include/cdefaultlist.h \
  ../../include/opp_string.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  guienvdefs.h \
  ../../include/regmacros.h
$O/logbuffer.obj: logbuffer.cc \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  logbuffer.h \
  ../../include/csimulation.h \
  ../../include/simtime.h \
  ../../include/simutil.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/cclassfactory.h \
  ../../include/errmsg.h \
  ../../include/cexpression.h \
  ../../include/cownedobject.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cvisitor.h \
  ../../include/cstringpool.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/intxtypes.h \
  ../../include/globals.h \
  ../../include/platdep/platmisc.h \
  ../../include/cdefaultlist.h \
  ../../include/opp_string.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  guienvdefs.h \
  ../../include/regmacros.h
$O/logbufferview.obj: logbufferview.cc \
  logbuffer.h \
  ../../include/simtime.h \
  ../../include/simkerneldefs.h \
  logbufferview.h \
  ../../include/simtime_t.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  guienvdefs.h
$O/registernatives.obj: registernatives.cc \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  guienvdefs.h
$O/visitor.obj: visitor.cc \
  ../../include/carray.h \
  ../../include/cgate.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/simtime.h \
  ../../include/simutil.h \
  ../../include/simkerneldefs.h \
  ../../include/cwatch.h \
  ../../include/cmsgpar.h \
  ../../include/cobject.h \
  ../../include/cfsm.h \
  ../../include/cclassfactory.h \
  ../../include/errmsg.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/cexpression.h \
  ../../include/cownedobject.h \
  ../../include/cchannel.h \
  ../../include/cqueue.h \
  ../../include/ccomponent.h \
  visitor.h \
  ../../include/cregistrationlist.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/cmathfunction.h \
  jutil.h \
  ../../include/coutvector.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/intxtypes.h \
  ../../include/globals.h \
  ../../include/platdep/platmisc.h \
  ../../include/cdefaultlist.h \
  ../../include/opp_string.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  guienvdefs.h \
  ../../include/regmacros.h
$O/wrappertable.obj: wrappertable.cc \
  ../../include/platdep/timeutil.h \
  ../../include/cpsquare.h \
  ../../include/cboolparimpl.h \
  ../../include/carray.h \
  ../../include/ccompoundmodule.h \
  ../../include/cgate.h \
  ../../include/cdataratechannel.h \
  ../../include/cenvir.h \
  ../../include/cconfiguration.h \
  ../../include/cmodule.h \
  ../../include/cclassdescriptor.h \
  ../../include/cmessage.h \
  logbuffer.h \
  ../../include/csimulation.h \
  ../../include/clcg32.h \
  guienv.h \
  ../../include/simtime.h \
  ../../include/simkerneldefs.h \
  ../../include/simutil.h \
  ../../include/cwatch.h \
  ../../include/cconfigoption.h \
  ../../include/cstlwatch.h \
  ../../include/cenum.h \
  ../../include/cmsgpar.h \
  ../../include/cobject.h \
  ../../include/cfsm.h \
  ../../include/nedsupport.h \
  ../../include/cstddev.h \
  ../../include/cnullenvir.h \
  ../../include/cclassfactory.h \
  ../../include/cdoubleparimpl.h \
  ../../include/packing.h \
  ../../include/errmsg.h \
  ../../include/ctopology.h \
  ../../include/distrib.h \
  ../../include/cstatistic.h \
  ../../include/ccomponenttype.h \
  ../../include/cnedfunction.h \
  ../../include/cpacketqueue.h \
  ../../include/cownedobject.h \
  ../../include/cdisplaystring.h \
  ../../include/cexpression.h \
  wrappertable.h \
  ../../include/compat.h \
  ../../include/crng.h \
  ../../include/random.h \
  ../../include/cdetect.h \
  ../../include/cproperty.h \
  ../../include/cparimpl.h \
  ../../include/clinkedlist.h \
  ../../include/cqueue.h \
  ../../include/cproperties.h \
  ../../include/cparsimcomm.h \
  ../../include/cchannel.h \
  ../../include/chistogram.h \
  ../../include/ccomponent.h \
  ../../include/cstringtokenizer.h \
  ../../include/omnetpp.h \
  ../../include/cregistrationlist.h \
  ../../include/ccommbuffer.h \
  ../../include/cscheduler.h \
  ../../include/mersennetwister.h \
  ../../include/clongparimpl.h \
  ../../include/cxmlelement.h \
  ../../include/cvisitor.h \
  ../../include/cstringpool.h \
  ../../include/cmathfunction.h \
  jcallback.h \
  ../../include/coutvector.h \
  ../../include/onstartup.h \
  ../../include/cxmlparimpl.h \
  ../../include/simtime_t.h \
  ../../include/cdelaychannel.h \
  ../../include/csimplemodule.h \
  ../../include/cdensityestbase.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cvarhist.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/platdep/platmisc.h \
  ../../include/cstringparimpl.h \
  ../../include/chasher.h \
  ../../include/index.h \
  ../../include/cdefaultlist.h \
  ../../include/opp_string.h \
  ../../include/cmersennetwister.h \
  ../../include/envirext.h \
  ../../include/cexception.h \
  ../../include/cdynamicexpression.h \
  ../../include/cnamedobject.h \
  ../../include/cksplit.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  ../../include/ccoroutine.h \
  guienvdefs.h \
  ../../include/regmacros.h
$O/simkernel_wrap.obj: simkernel_wrap.cxx \
  ../../include/platdep/timeutil.h \
  ../../include/cboolparimpl.h \
  ../../include/cpsquare.h \
  ../../include/carray.h \
  ../../include/ccompoundmodule.h \
  ../../include/cdataratechannel.h \
  ../../include/cgate.h \
  ../../include/cconfiguration.h \
  ../../include/cenvir.h \
  ../../include/cmodule.h \
  ../../include/cclassdescriptor.h \
  ../../include/cmessage.h \
  logbuffer.h \
  ../../include/csimulation.h \
  ../../include/clcg32.h \
  guienv.h \
  ../../include/simtime.h \
  ../../include/simkerneldefs.h \
  ../../include/simutil.h \
  ../../include/cconfigoption.h \
  ../../include/cwatch.h \
  ../../include/cenum.h \
  ../../include/cstlwatch.h \
  ../../include/cmsgpar.h \
  ../../include/cobject.h \
  ../../include/cfsm.h \
  ../../include/nedsupport.h \
  ../../include/cclassfactory.h \
  ../../include/cnullenvir.h \
  ../../include/cstddev.h \
  ../../include/cdoubleparimpl.h \
  ../../include/errmsg.h \
  ../../include/packing.h \
  ../../include/ctopology.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/distrib.h \
  ../../include/cnedfunction.h \
  ../../include/cpacketqueue.h \
  ../../include/cexpression.h \
  ../../include/cdisplaystring.h \
  ../../include/cownedobject.h \
  wrappertable.h \
  ../../include/compat.h \
  ../../include/crng.h \
  ../../include/cdetect.h \
  ../../include/random.h \
  ../../include/cproperty.h \
  logbufferview.h \
  innerclasses.h \
  ../../include/clinkedlist.h \
  ../../include/cparimpl.h \
  ../../include/cchannel.h \
  ../../include/cparsimcomm.h \
  ../../include/cproperties.h \
  ../../include/cqueue.h \
  ../../include/chistogram.h \
  ../../include/ccomponent.h \
  ../../include/cstringtokenizer.h \
  visitor.h \
  ../../include/omnetpp.h \
  ../../include/cregistrationlist.h \
  ../../include/ccommbuffer.h \
  ../../include/cscheduler.h \
  ../../include/clongparimpl.h \
  ../../include/mersennetwister.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/cxmlelement.h \
  ../../include/cmathfunction.h \
  jutil.h \
  jcallback.h \
  ../../include/coutvector.h \
  ../../include/cxmlparimpl.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/csimplemodule.h \
  ../../include/cdelaychannel.h \
  ../../include/cdensityestbase.h \
  ../../include/cmessageheap.h \
  ../../include/cvarhist.h \
  ../../include/platdep/intxtypes.h \
  ../../include/globals.h \
  ../../include/cstringparimpl.h \
  ../../include/platdep/platmisc.h \
  ../../include/chasher.h \
  ../../include/cdefaultlist.h \
  ../../include/index.h \
  ../../include/cmersennetwister.h \
  ../../include/opp_string.h \
  ../../include/envirext.h \
  ../../include/cexception.h \
  ../../include/cnamedobject.h \
  ../../include/cdynamicexpression.h \
  ../../include/cksplit.h \
  ../../include/platdep/platdefs.h \
  ../../include/cpar.h \
  ../../include/ccoroutine.h \
  guienvdefs.h \
  ../../include/regmacros.h
