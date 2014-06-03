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
LIBNAME=libopptkenv$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/tkenv
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../envir -I../layout -I../common $(TK_CFLAGS) $(ZLIB_CFLAGS) -DBUILDING_TKENV

IMPLIBS= -L$L -loppsim$D -loppenvir$D -lopplayout$D -loppcommon$D $(TK_LIBS) $(ZLIB_LIBS)

OBJS = $O/tkenv$(TKENV_EMBED_SUFFIX).o $O/tkcmd.o $O/tklib.o $O/inspector.o $O/logbuffer.o \
       $O/inspectorfactory.o $O/genericobjectinspector.o $O/watchinspector.o \
       $O/moduleinspector.o $O/loginspector.o $O/gateinspector.o \
       $O/outputvectorinspector.o $O/histograminspector.o $O/componenthistory.o \
       $O/layouterenv.o $O/arrow.o $O/tkutil.o $O/tkImgPNG.o $O/tkImgPNGInit.o

ifeq ("$(EMBED_TCL_CODE)","yes")
TKENV_EMBED_SUFFIX=-e
else
TKENV_EMBED_SUFFIX=
endif

# Required only on Mac OS X
ifeq ("$(PLATFORM)", "macosx")
COPTS += -framework Carbon
LDFLAGS += -framework Carbon
OBJS += $O/osxproctype.o
endif

#
# Automatic rules
#
.SUFFIXES : .cc .c

$O/%.o: %.c
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CC) -c $(COPTS) -o $@ $<

$O/%.o: %.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

#
# Targets
#
all: $(TKENV_IF_POSSIBLE)

tkenv : $L/$(LIBNAME)$(LIB_SUFFIX)
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
ifeq ("$(PLATFORM)", "macosx")
	$(Q)cp libtkdock1.0.dylib $(OMNETPP_LIB_DIR)
endif
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

$O/tkenv.o : tkenv.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -DOMNETPP_LIB_DIR="\"$(OMNETPP_LIB_DIR)\"" -DOMNETPP_TKENV_DIR="\"$(OMNETPP_TKENV_DIR)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" tkenv.cc -o $O/tkenv.o

$O/tkenv-e.o : tkenv.cc tclcode.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -DOMNETPP_LIB_DIR="\"$(OMNETPP_LIB_DIR)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" tkenv.cc -o $O/tkenv-e.o

tclcode.cc: *.tcl tcl2c.pl
	@# clearlooksimg.tcl must precede clearlooks.tcl (tcl2c filters out duplicates)
	$(PERL) tcl2c.pl tclcode.cc clearlooksimg.tcl *.tcl

missing-dependency:
	@echo '  *** No Tcl/Tk installation found -- skipping Tkenv.'
	@echo '  *** Install it, then run ./configure again.'

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc *.c

clean:
	$(qecho) Cleaning...
	$(Q)rm -f tclcode.cc
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arrow.o: arrow.cc \
  ../../include/simutil.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/opp_string.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h \
  arrow.h \
  ../../include/cvisitor.h \
  ../../include/cobject.h \
  tkdefs.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/clifetimelistener.h \
  ../../include/simkerneldefs.h \
  ../../include/cenvir.h \
  ../../include/errmsg.h \
  ../../include/platdep/intxtypes.h \
  tkutil.h \
  ../../include/clistener.h \
  ../../include/simtime_t.h \
  ../envir/envirdefs.h
$O/componenthistory.o: componenthistory.cc \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cexpression.h \
  ../../include/errmsg.h \
  ../../include/clog.h \
  ../../include/cgate.h \
  ../../include/csimplemodule.h \
  ../../include/regmacros.h \
  ../../include/cobject.h \
  ../../include/cnedmathfunction.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../../include/cchannel.h \
  ../../include/clistener.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/cexception.h \
  ../../include/ccomponent.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cevent.h \
  ../../include/cnamedobject.h \
  ../../include/cmsgpar.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  ../../include/cenvir.h \
  ../../include/clifetimelistener.h \
  tkdefs.h \
  ../../include/cvisitor.h \
  componenthistory.h \
  ../../include/cmessageheap.h \
  ../../include/cpar.h \
  ../../include/opp_string.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../../include/cmodule.h \
  ../../include/simutil.h
$O/gateinspector.o: gateinspector.cc \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/ccomponent.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  ../../include/clifetimelistener.h \
  tkenv.h \
  tkdefs.h \
  componenthistory.h \
  ../envir/envirbase.h \
  ../../include/cmessageheap.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  gateinspector.h \
  ../../include/clog.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/cobject.h \
  ../../include/cnedmathfunction.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/cchannel.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/cevent.h \
  ../../include/cmsgpar.h \
  ../../include/cenvir.h \
  ../../include/cvisitor.h \
  tklib.h \
  ../../include/cdisplaystring.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cpar.h \
  ../envir/args.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  tkutil.h \
  ../../include/cconfiguration.h \
  logbuffer.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  circularbuffer.h \
  inspectorfactory.h \
  ../../include/cresultlistener.h \
  ../envir/objectprinter.h \
  inspector.h
$O/genericobjectinspector.o: genericobjectinspector.cc \
  ../../include/cmsgpar.h \
  ../../include/cenvir.h \
  tklib.h \
  ../../include/cvisitor.h \
  ../../include/cpar.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../envir/args.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/cevent.h \
  inspectorfactory.h \
  circularbuffer.h \
  ../../include/cqueue.h \
  ../../include/cresultlistener.h \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  logbuffer.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  ../../include/clifetimelistener.h \
  ../../include/carray.h \
  ../../include/cnamedobject.h \
  ../../include/simkerneldefs.h \
  tkenv.h \
  tkdefs.h \
  ../envir/envirbase.h \
  componenthistory.h \
  ../../include/cmessageheap.h \
  genericobjectinspector.h \
  ../../include/envirext.h \
  ../../include/cmodule.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/ccomponent.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cobject.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/cnedmathfunction.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/cchannel.h \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  ../../include/clog.h
$O/histograminspector.o: histograminspector.cc \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/cresultlistener.h \
  inspectorfactory.h \
  circularbuffer.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  logbuffer.h \
  tkutil.h \
  ../../include/cconfiguration.h \
  histograminspector.h \
  ../../include/cstddev.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../envir/args.h \
  ../../include/cpar.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cvisitor.h \
  tklib.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cchannel.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../../include/cnedmathfunction.h \
  ../../include/cobject.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/clog.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/ctimestampedvalue.h \
  ../../include/envirext.h \
  ../../include/cmodule.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../envir/envirbase.h \
  componenthistory.h \
  tkdefs.h \
  tkenv.h \
  ../../include/clifetimelistener.h \
  ../../include/simkerneldefs.h \
  ../../include/carray.h \
  ../../include/cnamedobject.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponent.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/cdensityestbase.h
$O/inspector.o: inspector.cc \
  ../../include/cdelaychannel.h \
  ../../include/cstlwatch.h \
  ../../include/ccomponenttype.h \
  ../../include/cresultfilter.h \
  ../../include/packing.h \
  ../../include/cstatistic.h \
  ../../include/ctimestampedvalue.h \
  ../../include/compat.h \
  ../../include/cparimpl.h \
  ../../include/clog.h \
  ../../include/random.h \
  ../../include/regmacros.h \
  ../../include/cksplit.h \
  ../envir/visitor.h \
  ../../include/cpatternmatcher.h \
  ../../include/cobject.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/clcg32.h \
  ../../include/cchannel.h \
  ../../include/cdynamicexpression.h \
  ../../include/cpacketqueue.h \
  ../../include/cnedmathfunction.h \
  ../../include/ccommbuffer.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../../include/ccomponent.h \
  ../../include/cparsimcomm.h \
  ../../include/mersennetwister.h \
  ../../include/cdensityestbase.h \
  ../../include/chasher.h \
  ../../include/cmessage.h \
  ../../include/cdoubleparimpl.h \
  ../../include/csimulation.h \
  ../../include/cwatch.h \
  ../../include/cnullenvir.h \
  ../../include/platdep/intxtypes.h \
  ../../include/nedsupport.h \
  ../../include/platdep/timeutil.h \
  ../../include/ctopology.h \
  ../../include/cpacket.h \
  ../../include/cpsquare.h \
  ../../include/cmodelchange.h \
  tkenv.h \
  tkdefs.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  ../../include/cstringparimpl.h \
  ../../include/clifetimelistener.h \
  ../../include/cboolparimpl.h \
  ../../include/cstringtokenizer.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../../include/omnetpp.h \
  ../../include/cnedfunction.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  ../envir/envirbase.h \
  ../../include/cmersennetwister.h \
  componenthistory.h \
  ../../include/cmessageheap.h \
  ../../include/cxmlparimpl.h \
  ../../include/cstddev.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../../include/cvarhist.h \
  ../../include/cexpression.h \
  ../envir/logformatter.h \
  ../../include/distrib.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  ../../include/cdetect.h \
  logbuffer.h \
  ../../include/crng.h \
  ../../include/cresultlistener.h \
  ../../include/index.h \
  ../../include/cqueue.h \
  circularbuffer.h \
  ../../include/clongparimpl.h \
  ../../include/cproperty.h \
  inspectorfactory.h \
  ../../include/csimplemodule.h \
  ../../include/cmatchexpression.h \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/cclassdescriptor.h \
  ../../include/coutvector.h \
  ../../include/cconfigoption.h \
  ../../include/cnedvalue.h \
  ../../include/cownedobject.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cfsm.h \
  ../../include/cscheduler.h \
  ../../include/cexception.h \
  ../../include/cobjectfactory.h \
  ../../include/checkandcast.h \
  ../../include/chistogram.h \
  ../../include/cevent.h \
  ../../include/cproperties.h \
  ../../include/globals.h \
  ../../include/ccoroutine.h \
  ../../include/cvisitor.h \
  ../../include/cenum.h \
  tklib.h \
  ../../include/cmsgpar.h \
  ../../include/cresultrecorder.h \
  ../../include/cenvir.h \
  ../envir/args.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/cdataratechannel.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cdisplaystring.h \
  ../../include/cxmlelement.h \
  ../../include/cpar.h
$O/inspectorfactory.o: inspectorfactory.cc \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/cmatchexpression.h \
  ../../include/cconfigoption.h \
  ../../include/coutvector.h \
  ../../include/cclassdescriptor.h \
  ../../include/cnedvalue.h \
  ../../include/cresultlistener.h \
  ../../include/index.h \
  ../../include/cqueue.h \
  ../../include/clongparimpl.h \
  ../../include/cproperty.h \
  circularbuffer.h \
  ../../include/csimplemodule.h \
  inspectorfactory.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../../include/distrib.h \
  ../../include/cdetect.h \
  logbuffer.h \
  ../../include/crng.h \
  ../../include/cstddev.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  ../../include/onstartup.h \
  ../../include/cvarhist.h \
  ../../include/simtime_t.h \
  ../envir/args.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../../include/cdisplaystring.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cdataratechannel.h \
  ../../include/cpar.h \
  ../../include/cxmlelement.h \
  ../../include/ccoroutine.h \
  tklib.h \
  ../../include/cenum.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cresultrecorder.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/checkandcast.h \
  ../../include/chistogram.h \
  ../../include/globals.h \
  ../../include/cproperties.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  ../../include/cscheduler.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cfsm.h \
  ../../include/clcg32.h \
  ../../include/cchannel.h \
  ../../include/cdynamicexpression.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/cpacketqueue.h \
  ../../include/cnedmathfunction.h \
  ../../include/ccommbuffer.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/cksplit.h \
  ../../include/cobject.h \
  ../../include/cpatternmatcher.h \
  ../../include/compat.h \
  ../../include/cparimpl.h \
  ../../include/clog.h \
  ../../include/random.h \
  ../../include/cstlwatch.h \
  ../../include/cdelaychannel.h \
  ../../include/cresultfilter.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/packing.h \
  ../../include/ctimestampedvalue.h \
  ../../include/omnetpp.h \
  ../envir/intervals.h \
  ../../include/cstringtokenizer.h \
  ../../include/opp_string.h \
  ../../include/envirext.h \
  ../../include/cnedfunction.h \
  ../../include/cmodule.h \
  ../../include/cmessageheap.h \
  ../envir/envirbase.h \
  componenthistory.h \
  ../../include/cmersennetwister.h \
  ../../include/cxmlparimpl.h \
  tkdefs.h \
  tkenv.h \
  ../../include/simkerneldefs.h \
  ../../include/cstringparimpl.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/cboolparimpl.h \
  ../../include/clifetimelistener.h \
  ../../include/cnullenvir.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cwatch.h \
  ../../include/nedsupport.h \
  ../../include/platdep/timeutil.h \
  ../../include/ctopology.h \
  ../../include/cpsquare.h \
  ../../include/cpacket.h \
  ../../include/cmodelchange.h \
  ../../include/cparsimcomm.h \
  ../../include/ccomponent.h \
  ../../include/mersennetwister.h \
  ../../include/cmessage.h \
  ../../include/cdoubleparimpl.h \
  ../../include/csimulation.h \
  ../../include/cdensityestbase.h \
  ../../include/chasher.h
$O/layouterenv.o: layouterenv.cc \
  ../../include/cmodule.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  tkdefs.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  layouterenv.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/cobject.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/clog.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../../include/simutil.h \
  ../../include/cpar.h \
  ../../include/cdisplaystring.h \
  tklib.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  ../../include/cobjectfactory.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  ../../include/cexpression.h \
  tkutil.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h
$O/logbuffer.o: logbuffer.cc \
  ../../include/platdep/intxtypes.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/ccomponent.h \
  ../../include/cmessageheap.h \
  ../../include/opp_string.h \
  ../../include/cmodule.h \
  ../../include/carray.h \
  ../../include/cnamedobject.h \
  ../../include/simkerneldefs.h \
  ../../include/clifetimelistener.h \
  tkdefs.h \
  ../../include/clog.h \
  ../../include/cnedmathfunction.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/cobject.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/cevent.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../../include/cmsgpar.h \
  ../../include/cenvir.h \
  ../../include/cvisitor.h \
  tklib.h \
  logbuffer.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  tkutil.h \
  circularbuffer.h
$O/loginspector.o: loginspector.cc \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/cobject.h \
  ../../include/cchannel.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../../include/cnedmathfunction.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/ctimestampedvalue.h \
  ../../include/ccomponenttype.h \
  ../../include/clog.h \
  tkdefs.h \
  tkenv.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/clifetimelistener.h \
  ../envir/intervals.h \
  ../../include/cmessageprinter.h \
  ../../include/opp_string.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  ../../include/cmessageheap.h \
  ../envir/envirbase.h \
  componenthistory.h \
  ../../include/ccomponent.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cresultlistener.h \
  circularbuffer.h \
  inspectorfactory.h \
  ../envir/objectprinter.h \
  inspector.h \
  tkutil.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/simtime_t.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  loginspector.h \
  logbuffer.h \
  ../../include/cvisitor.h \
  tklib.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../envir/args.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cpar.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/globals.h
$O/moduleinspector.o: moduleinspector.cc \
  tkutil.h \
  ../../include/cconfiguration.h \
  moduleinspector.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  logbuffer.h \
  ../../include/cqueue.h \
  ../../include/cresultlistener.h \
  inspectorfactory.h \
  ../../include/csimplemodule.h \
  circularbuffer.h \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/globals.h \
  tklib.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../envir/args.h \
  ../../include/cpar.h \
  arrow.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cdisplaystring.h \
  ../../include/ctimestampedvalue.h \
  ../../include/ccomponenttype.h \
  ../../include/clog.h \
  ../../include/cobject.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/cchannel.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../../include/cnedmathfunction.h \
  ../../include/ccomponent.h \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  layouterenv.h \
  ../../include/platdep/intxtypes.h \
  tkdefs.h \
  tkenv.h \
  ../../include/clifetimelistener.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/envirext.h \
  ../../include/cmodule.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  ../envir/envirbase.h \
  componenthistory.h
$O/osxproctype.o: osxproctype.cc
$O/outputvectorinspector.o: outputvectorinspector.cc \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/ccomponent.h \
  ../../include/platdep/intxtypes.h \
  ../../include/clifetimelistener.h \
  ../../include/simkerneldefs.h \
  ../../include/carray.h \
  ../../include/cnamedobject.h \
  tkdefs.h \
  tkenv.h \
  ../../include/cmessageheap.h \
  ../envir/envirbase.h \
  componenthistory.h \
  ../../include/envirext.h \
  ../../include/cmodule.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  ../../include/clog.h \
  ../../include/cobject.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/cnedmathfunction.h \
  ../../include/cchannel.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  tklib.h \
  ../../include/cvisitor.h \
  ../../include/cpar.h \
  ../envir/eventlogfilemgr.h \
  outputvectorinspector.h \
  ../envir/speedometer.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../envir/args.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  tkutil.h \
  ../../include/cconfiguration.h \
  logbuffer.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  inspectorfactory.h \
  circularbuffer.h \
  ../../include/cresultlistener.h \
  ../../include/coutvector.h \
  ../envir/objectprinter.h \
  inspector.h
$O/tkcmd.o: tkcmd.cc \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cstatistic.h \
  ../../include/clog.h \
  ../../include/cobject.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/cchannel.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/cnedmathfunction.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/cwatch.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cpacket.h \
  tkdefs.h \
  tkenv.h \
  ../../include/clifetimelistener.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../envir/fsutils.h \
  ../../include/envirext.h \
  ../../include/cmodule.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  componenthistory.h \
  ../envir/envirbase.h \
  tkutil.h \
  ../../include/cconfiguration.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  logbuffer.h \
  ../../include/cqueue.h \
  ../../include/cresultlistener.h \
  inspectorfactory.h \
  ../../include/csimplemodule.h \
  circularbuffer.h \
  ../../include/coutvector.h \
  ../../include/cclassdescriptor.h \
  inspector.h \
  ../envir/objectprinter.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/cenum.h \
  tklib.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../envir/args.h \
  ../../include/cpar.h \
  ../../include/cdisplaystring.h \
  ../envir/eventlogfilemgr.h
$O/tkenv.o: tkenv.cc \
  ../../include/cmessage.h \
  ../../include/csimulation.h \
  ../../include/ccomponent.h \
  ../../include/platdep/intxtypes.h \
  ../envir/appreg.h \
  ../../include/platdep/timeutil.h \
  ../../include/simkerneldefs.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/clifetimelistener.h \
  tkdefs.h \
  tkenv.h \
  ../../include/cmessageheap.h \
  componenthistory.h \
  ../envir/envirbase.h \
  ../../include/cmessageprinter.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  genericobjectinspector.h \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  gateinspector.h \
  ../../include/clog.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/cobject.h \
  watchinspector.h \
  ../../include/cnedmathfunction.h \
  ../../include/cstringpool.h \
  ../../include/simtime.h \
  ../../include/cchannel.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/cscheduler.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cproperties.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cvisitor.h \
  tklib.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cpar.h \
  ../envir/args.h \
  ../envir/speedometer.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../../include/onstartup.h \
  moduleinspector.h \
  ../../include/simtime_t.h \
  tkutil.h \
  ../../include/cconfiguration.h \
  loginspector.h \
  logbuffer.h \
  ../../include/cexpression.h \
  ../envir/logformatter.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  ../../include/cproperty.h \
  circularbuffer.h \
  inspectorfactory.h \
  ../../include/csimplemodule.h \
  ../../include/cresultlistener.h \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/cconfigoption.h
$O/tklib.o: tklib.cc \
  ../../include/ccomponenttype.h \
  ../../include/ctimestampedvalue.h \
  ../../include/clog.h \
  ../../include/cobject.h \
  ../envir/visitor.h \
  ../../include/regmacros.h \
  ../../include/cchannel.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/platdefs.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/cnedmathfunction.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/platdep/intxtypes.h \
  tkdefs.h \
  tkenv.h \
  ../../include/clifetimelistener.h \
  ../../include/simkerneldefs.h \
  ../../include/carray.h \
  ../../include/cnamedobject.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/cmessageheap.h \
  componenthistory.h \
  ../envir/envirbase.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  logbuffer.h \
  ../../include/cresultlistener.h \
  circularbuffer.h \
  ../envir/objectprinter.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  ../envir/envirdefs.h \
  ../../include/clistener.h \
  ../../include/cevent.h \
  ../../include/cobjectfactory.h \
  ../../include/globals.h \
  tklib.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../envir/speedometer.h \
  ../../include/simutil.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../envir/args.h \
  ../../include/cpar.h \
  ../envir/eventlogfilemgr.h
$O/tkutil.o: tkutil.cc \
  ../../include/coutvector.h \
  ../envir/objectprinter.h \
  ../../include/cqueue.h \
  ../../include/cresultlistener.h \
  ../../include/csimplemodule.h \
  circularbuffer.h \
  ../../include/cgate.h \
  ../../include/errmsg.h \
  ../envir/logformatter.h \
  ../../include/cexpression.h \
  logbuffer.h \
  ../common/ver.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cdefaultlist.h \
  ../../include/simutil.h \
  ../envir/speedometer.h \
  ../envir/args.h \
  ../../include/cpar.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cvisitor.h \
  ../../include/cmsgpar.h \
  ../../include/cenvir.h \
  ../../include/cobjectfactory.h \
  ../../include/cevent.h \
  ../../include/globals.h \
  ../../include/cownedobject.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cexception.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cchannel.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  ../../include/cnedmathfunction.h \
  ../../include/cobject.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/clog.h \
  ../../include/ctimestampedvalue.h \
  ../../include/ccomponenttype.h \
  ../../include/cstatistic.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../envir/envirbase.h \
  componenthistory.h \
  ../../include/cmessageheap.h \
  tkenv.h \
  tkdefs.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cwatch.h \
  ../../include/ccomponent.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h
$O/watchinspector.o: watchinspector.cc \
  ../../include/simtime_t.h \
  ../../include/onstartup.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  logbuffer.h \
  ../../include/errmsg.h \
  ../../include/cgate.h \
  ../../include/cexpression.h \
  ../envir/logformatter.h \
  inspectorfactory.h \
  circularbuffer.h \
  ../../include/cresultlistener.h \
  ../envir/objectprinter.h \
  inspector.h \
  ../../include/clistener.h \
  ../envir/envirdefs.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/cevent.h \
  ../../include/cmsgpar.h \
  ../../include/cenvir.h \
  tklib.h \
  ../../include/cvisitor.h \
  ../../include/cpar.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cdefaultlist.h \
  ../../include/cregistrationlist.h \
  ../envir/speedometer.h \
  ../../include/simutil.h \
  ../envir/args.h \
  ../../include/ctimestampedvalue.h \
  ../../include/ccomponenttype.h \
  ../../include/cstlwatch.h \
  ../../include/clog.h \
  ../../include/cobject.h \
  ../../include/regmacros.h \
  ../envir/visitor.h \
  ../../include/simtime.h \
  ../../include/cstringpool.h \
  watchinspector.h \
  ../../include/cnedmathfunction.h \
  ../envir/matchableobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/platdep/platmisc.h \
  ../../include/cchannel.h \
  ../../include/csimulation.h \
  ../../include/cmessage.h \
  ../../include/ccomponent.h \
  ../../include/cwatch.h \
  ../../include/platdep/intxtypes.h \
  ../../include/clifetimelistener.h \
  ../../include/cnamedobject.h \
  ../../include/carray.h \
  ../../include/simkerneldefs.h \
  tkenv.h \
  tkdefs.h \
  componenthistory.h \
  ../envir/envirbase.h \
  ../../include/cmessageheap.h \
  ../../include/cmodule.h \
  ../../include/envirext.h \
  ../../include/opp_string.h \
  ../envir/intervals.h
$O/tkImgPNG.o: tkImgPNG.c
$O/tkImgPNGInit.o: tkImgPNGInit.c
