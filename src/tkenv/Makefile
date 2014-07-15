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
       $O/moduleinspector.o $O/loginspector.o $O/gateinspector.o $O/figurerenderers.o \
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
  ../../include/csimulation.h \
  ../envir/envirdefs.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  tkutil.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  ../../include/cmessageheap.h \
  ../../include/cstringpool.h \
  ../../include/simutil.h \
  ../../include/cownedobject.h \
  arrow.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cvisitor.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platdefs.h \
  ../../include/clistener.h \
  ../envir/visitor.h \
  ../../include/cobject.h \
  ../../include/platdep/platmisc.h \
  ../../include/opp_string.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h
$O/componenthistory.o: componenthistory.cc \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cownedobject.h \
  ../../include/simtime.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/cmodule.h \
  ../../include/csimplemodule.h \
  ../../include/cmessage.h \
  ../../include/carray.h \
  ../../include/opp_string.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponenttype.h \
  ../../include/cchannel.h \
  ../../include/clistener.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cnedmathfunction.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/csimulation.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cobject.h \
  ../../include/cgate.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  componenthistory.h
$O/figurerenderers.o: figurerenderers.cc \
  ../../include/platdep/platdefs.h \
  ../../include/cobject.h \
  ../../include/csimulation.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/simutil.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/cstringpool.h \
  ../../include/cmessageheap.h \
  ../../include/ccanvas.h \
  ../../include/platdep/intxtypes.h \
  ../../include/clistener.h \
  figurerenderers.h \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../../include/cexception.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/opp_string.h \
  ../envir/visitor.h \
  ../../include/platdep/platmisc.h \
  ../envir/envirdefs.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  tkutil.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h
$O/gateinspector.o: gateinspector.cc \
  inspectorfactory.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  componenthistory.h \
  logbuffer.h \
  ../../include/cobject.h \
  ../../include/cgate.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  ../../include/cresultlistener.h \
  ../../include/cexpression.h \
  ../../include/cdisplaystring.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../envir/eventlogfilemgr.h \
  gateinspector.h \
  ../envir/args.h \
  ../../include/csimulation.h \
  ../../include/cmessageheap.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  ../../include/globals.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cexception.h \
  ../../include/cvisitor.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/clistener.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cnedmathfunction.h \
  ../../include/platdep/platmisc.h \
  ../envir/visitor.h \
  ../../include/ccomponenttype.h \
  ../../include/cchannel.h \
  tkenv.h \
  inspector.h \
  ../../include/opp_string.h \
  ../../include/envirext.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  circularbuffer.h \
  ../../include/cmessage.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  ../envir/envirbase.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  tklib.h \
  ../../include/cmodule.h \
  ../envir/envirdefs.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  tkutil.h \
  ../../include/onstartup.h \
  ../../include/cregistrationlist.h \
  ../../include/cownedobject.h
$O/genericobjectinspector.o: genericobjectinspector.cc \
  tkutil.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  ../../include/cregistrationlist.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../envir/intervals.h \
  ../../include/carray.h \
  ../../include/cmessage.h \
  ../envir/envirdefs.h \
  tklib.h \
  ../../include/cmodule.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  tkenv.h \
  ../../include/cchannel.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platmisc.h \
  ../envir/visitor.h \
  circularbuffer.h \
  ../../include/opp_string.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/envirext.h \
  inspector.h \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../../include/cexception.h \
  ../../include/cvisitor.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cnedmathfunction.h \
  ../../include/platdep/intxtypes.h \
  ../envir/objectprinter.h \
  ../../include/clistener.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../../include/globals.h \
  ../../include/cobjectfactory.h \
  ../../include/cqueue.h \
  ../../include/cexpression.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../envir/args.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../../include/cgate.h \
  ../../include/cobject.h \
  ../../include/cconfiguration.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  inspectorfactory.h \
  genericobjectinspector.h \
  logbuffer.h \
  componenthistory.h \
  ../../include/cmsgpar.h \
  ../../include/platdep/platdefs.h
$O/histograminspector.o: histograminspector.cc \
  ../../include/cstringpool.h \
  ../../include/cmessageheap.h \
  ../../include/cdefaultlist.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  ../../include/globals.h \
  ../../include/cresultlistener.h \
  ../../include/cexpression.h \
  ../envir/args.h \
  ../../include/cdensityestbase.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../envir/eventlogfilemgr.h \
  ../../include/csimulation.h \
  ../../include/cobject.h \
  ../../include/cgate.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  inspectorfactory.h \
  ../../include/cmsgpar.h \
  componenthistory.h \
  ../../include/platdep/platdefs.h \
  histograminspector.h \
  logbuffer.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  tkutil.h \
  ../../include/cregistrationlist.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../envir/intervals.h \
  ../../include/cmessage.h \
  ../../include/carray.h \
  ../envir/envirbase.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../envir/envirdefs.h \
  ../../include/cmodule.h \
  tklib.h \
  ../../include/cstddev.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platmisc.h \
  ../envir/visitor.h \
  tkenv.h \
  ../../include/cchannel.h \
  ../../include/cenvir.h \
  ../../include/cstatistic.h \
  ../../include/opp_string.h \
  ../../include/envirext.h \
  ../../include/simtime_t.h \
  inspector.h \
  circularbuffer.h \
  ../../include/cexception.h \
  ../../include/cvisitor.h \
  ../../include/ctimestampedvalue.h \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../envir/objectprinter.h \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../../include/platdep/intxtypes.h
$O/inspector.o: inspector.cc \
  ../envir/eventlogfilemgr.h \
  ../../include/cresultfilter.h \
  ../../include/cexpression.h \
  ../../include/cdataratechannel.h \
  ../../include/cobjectfactory.h \
  ../../include/cqueue.h \
  ../../include/simutil.h \
  ../../include/ccoroutine.h \
  ../../include/cclassdescriptor.h \
  ../../include/cpsquare.h \
  ../../include/chistogram.h \
  ../../include/cvarhist.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/clongparimpl.h \
  ../../include/clcg32.h \
  componenthistory.h \
  ../../include/random.h \
  ../../include/cnedvalue.h \
  ../../include/cconfigoption.h \
  ../../include/cobject.h \
  ../../include/cdetect.h \
  ../../include/packing.h \
  ../../include/cpar.h \
  ../../include/cdynamicexpression.h \
  ../../include/errmsg.h \
  ../../include/cmodule.h \
  ../../include/nedsupport.h \
  ../../include/cstlwatch.h \
  ../../include/cmessage.h \
  ../../include/index.h \
  ../../include/cpacketqueue.h \
  ../../include/cregistrationlist.h \
  ../../include/cproperties.h \
  ../../include/clinkedlist.h \
  ../../include/ctopology.h \
  ../../include/platdep/timeutil.h \
  tkutil.h \
  ../../include/cksplit.h \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../../include/ctimestampedvalue.h \
  ../../include/ccommbuffer.h \
  ../../include/cvisitor.h \
  ../../include/mersennetwister.h \
  ../../include/cmessageprinter.h \
  ../../include/cstatistic.h \
  ../../include/opp_string.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../envir/args.h \
  ../../include/compat.h \
  ../../include/cdensityestbase.h \
  ../../include/omnetpp.h \
  ../../include/csimulation.h \
  ../../include/cresultlistener.h \
  ../../include/cmodelchange.h \
  ../../include/cdisplaystring.h \
  ../../include/cenum.h \
  ../../include/globals.h \
  ../../include/cparsimcomm.h \
  ../../include/cmessageheap.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  logbuffer.h \
  ../../include/cboolparimpl.h \
  ../../include/distrib.h \
  inspectorfactory.h \
  ../../include/cstringparimpl.h \
  ../envir/speedometer.h \
  ../../include/cdoubleparimpl.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  ../../include/cnedfunction.h \
  ../../include/cgate.h \
  tklib.h \
  ../../include/csimplemodule.h \
  ../envir/envirdefs.h \
  ../../include/carray.h \
  ../../include/cxmlparimpl.h \
  ../envir/intervals.h \
  ../../include/cparimpl.h \
  ../envir/envirbase.h \
  ../../include/cpatternmatcher.h \
  ../../include/cproperty.h \
  ../../include/coutvector.h \
  ../../include/onstartup.h \
  ../../include/cxmlelement.h \
  ../../include/cownedobject.h \
  ../envir/matchableobject.h \
  ../../include/cfsm.h \
  ../../include/simtime.h \
  ../../include/cwatch.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cscheduler.h \
  ../../include/ccanvas.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/cstringtokenizer.h \
  ../../include/simkerneldefs.h \
  ../../include/cmatchexpression.h \
  ../../include/cresultrecorder.h \
  inspector.h \
  ../../include/cnullenvir.h \
  ../../include/simtime_t.h \
  ../../include/envirext.h \
  ../../include/cenvir.h \
  circularbuffer.h \
  ../../include/chasher.h \
  ../../include/crng.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmersennetwister.h \
  ../../include/cstddev.h \
  ../../include/ccomponenttype.h \
  ../../include/cdelaychannel.h \
  tkenv.h
$O/inspectorfactory.o: inspectorfactory.cc \
  tklib.h \
  ../../include/csimplemodule.h \
  ../envir/envirdefs.h \
  ../envir/envirbase.h \
  ../../include/cproperty.h \
  ../../include/cpatternmatcher.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  ../../include/cparimpl.h \
  ../../include/cxmlparimpl.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/coutvector.h \
  ../../include/cxmlelement.h \
  ../../include/cwatch.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h \
  ../../include/cfsm.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccanvas.h \
  ../../include/cscheduler.h \
  ../envir/objectprinter.h \
  tkdefs.h \
  ../../include/cstringtokenizer.h \
  ../../include/cmatchexpression.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  circularbuffer.h \
  ../../include/chasher.h \
  ../../include/crng.h \
  ../../include/cresultrecorder.h \
  inspector.h \
  ../../include/envirext.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/cnullenvir.h \
  ../../include/cdelaychannel.h \
  tkenv.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmersennetwister.h \
  ../../include/cstddev.h \
  ../../include/ccomponenttype.h \
  ../../include/csimulation.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/compat.h \
  ../envir/args.h \
  ../../include/cdensityestbase.h \
  ../../include/omnetpp.h \
  ../../include/cmodelchange.h \
  ../../include/cdisplaystring.h \
  ../../include/cresultlistener.h \
  ../../include/globals.h \
  ../../include/cparsimcomm.h \
  ../../include/cenum.h \
  ../../include/cmessageheap.h \
  ../../include/cboolparimpl.h \
  logbuffer.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  inspectorfactory.h \
  ../../include/cstringparimpl.h \
  ../../include/distrib.h \
  ../../include/cconfiguration.h \
  ../../include/ccomponent.h \
  ../envir/speedometer.h \
  ../../include/cdoubleparimpl.h \
  ../../include/cgate.h \
  ../../include/cnedfunction.h \
  ../../include/cmodule.h \
  ../../include/nedsupport.h \
  ../../include/cpar.h \
  ../../include/cdynamicexpression.h \
  ../../include/errmsg.h \
  ../../include/cpacketqueue.h \
  ../../include/cstlwatch.h \
  ../../include/cmessage.h \
  ../../include/index.h \
  ../../include/cproperties.h \
  ../../include/clinkedlist.h \
  ../../include/cregistrationlist.h \
  ../../include/platdep/timeutil.h \
  ../../include/ctopology.h \
  tkutil.h \
  ../../include/cnedmathfunction.h \
  ../../include/clistener.h \
  ../../include/cksplit.h \
  ../../include/mersennetwister.h \
  ../../include/cmessageprinter.h \
  ../../include/ccommbuffer.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/cstatistic.h \
  ../../include/cchannel.h \
  ../envir/visitor.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cexpression.h \
  ../../include/cdataratechannel.h \
  ../../include/cresultfilter.h \
  ../../include/simutil.h \
  ../../include/cclassdescriptor.h \
  ../../include/ccoroutine.h \
  ../../include/cqueue.h \
  ../../include/cobjectfactory.h \
  ../../include/cdefaultlist.h \
  ../../include/clongparimpl.h \
  ../../include/cpsquare.h \
  ../../include/chistogram.h \
  ../../include/cvarhist.h \
  ../../include/cstringpool.h \
  ../../include/clcg32.h \
  componenthistory.h \
  ../../include/random.h \
  ../../include/cnedvalue.h \
  ../../include/packing.h \
  ../../include/cconfigoption.h \
  ../../include/cobject.h \
  ../../include/cdetect.h
$O/layouterenv.o: layouterenv.cc \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/platdep/platdefs.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../../include/csimulation.h \
  ../../include/cdisplaystring.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  tklib.h \
  ../envir/envirdefs.h \
  ../../include/cobject.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/opp_string.h \
  ../envir/visitor.h \
  layouterenv.h \
  ../../include/clistener.h \
  ../../include/cvisitor.h \
  ../../include/cregistrationlist.h \
  tkutil.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  ../../include/cmodule.h
$O/logbuffer.o: logbuffer.cc \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/simutil.h \
  ../../include/cobjectfactory.h \
  ../../include/cexpression.h \
  ../../include/cobject.h \
  tkutil.h \
  ../../include/cregistrationlist.h \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  ../envir/visitor.h \
  ../../include/opp_string.h \
  ../../include/cvisitor.h \
  ../../include/cnedmathfunction.h \
  ../../include/clistener.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/cgate.h \
  ../../include/ccomponent.h \
  logbuffer.h \
  ../../include/cmsgpar.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../envir/envirdefs.h \
  tklib.h \
  ../../include/platdep/platmisc.h \
  circularbuffer.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h
$O/loginspector.o: loginspector.cc \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../envir/args.h \
  ../../include/cgate.h \
  ../../include/cconfiguration.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  inspectorfactory.h \
  logbuffer.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  loginspector.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  tklib.h \
  ../envir/envirdefs.h \
  tkenv.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponenttype.h \
  circularbuffer.h \
  inspector.h \
  ../../include/cenvir.h \
  ../../include/envirext.h \
  ../../include/simtime_t.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../envir/objectprinter.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/simutil.h \
  ../../include/cobjectfactory.h \
  ../../include/cexpression.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cobject.h \
  componenthistory.h \
  tkutil.h \
  ../../include/cregistrationlist.h \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  ../../include/cchannel.h \
  ../envir/visitor.h \
  ../../include/opp_string.h \
  ../../include/cmessageprinter.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/cnedmathfunction.h \
  ../../include/clistener.h
$O/moduleinspector.o: moduleinspector.cc \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../../include/cexception.h \
  ../../include/ccanvas.h \
  ../../include/platdep/intxtypes.h \
  ../envir/objectprinter.h \
  tkenv.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platmisc.h \
  circularbuffer.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/envirext.h \
  inspector.h \
  ../envir/envirbase.h \
  ../envir/intervals.h \
  ../../include/carray.h \
  ../envir/envirdefs.h \
  tklib.h \
  ../../include/csimplemodule.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h \
  ../../include/cownedobject.h \
  arrow.h \
  ../../include/onstartup.h \
  inspectorfactory.h \
  logbuffer.h \
  ../../include/cmsgpar.h \
  ../../include/platdep/platdefs.h \
  ../../include/cgate.h \
  ../../include/cconfiguration.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cdisplaystring.h \
  ../../include/cresultlistener.h \
  ../../include/csimulation.h \
  ../envir/args.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cnedmathfunction.h \
  ../../include/clistener.h \
  figurerenderers.h \
  ../../include/cchannel.h \
  layouterenv.h \
  ../envir/visitor.h \
  ../../include/opp_string.h \
  ../../include/cmessage.h \
  ../../include/cmodule.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  tkutil.h \
  moduleinspector.h \
  ../../include/cregistrationlist.h \
  componenthistory.h \
  ../../include/cobject.h \
  ../../include/cexpression.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../../include/simutil.h \
  ../../include/cobjectfactory.h \
  ../../include/cqueue.h
$O/osxproctype.o: osxproctype.cc
$O/outputvectorinspector.o: outputvectorinspector.cc \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../envir/args.h \
  ../../include/csimulation.h \
  ../../include/cresultlistener.h \
  ../../include/ccomponent.h \
  ../envir/speedometer.h \
  ../../include/cconfiguration.h \
  ../../include/cgate.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  logbuffer.h \
  inspectorfactory.h \
  ../../include/coutvector.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  tklib.h \
  ../envir/envirdefs.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  ../envir/envirbase.h \
  inspector.h \
  ../../include/cenvir.h \
  ../../include/simtime_t.h \
  ../../include/envirext.h \
  circularbuffer.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponenttype.h \
  tkenv.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  outputvectorinspector.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cexpression.h \
  ../../include/cobject.h \
  componenthistory.h \
  ../../include/cregistrationlist.h \
  tkutil.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/opp_string.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h
$O/tkcmd.o: tkcmd.cc \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  logbuffer.h \
  inspectorfactory.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  ../../include/cgate.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../envir/args.h \
  ../../include/csimulation.h \
  ../../include/cresultlistener.h \
  ../../include/cdisplaystring.h \
  ../../include/cenum.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccanvas.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  inspector.h \
  ../../include/simtime_t.h \
  ../../include/envirext.h \
  ../../include/cenvir.h \
  circularbuffer.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponenttype.h \
  tkenv.h \
  ../../include/csimplemodule.h \
  tklib.h \
  ../envir/envirdefs.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  ../envir/envirbase.h \
  ../../include/onstartup.h \
  ../../include/coutvector.h \
  ../../include/cownedobject.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  ../../include/cwatch.h \
  componenthistory.h \
  ../envir/fsutils.h \
  ../../include/cobject.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cexpression.h \
  ../../include/cqueue.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  ../../include/cclassdescriptor.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/cstatistic.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/cregistrationlist.h \
  tkutil.h
$O/tkenv.o: tkenv.cc \
  ../../include/cresultlistener.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../envir/args.h \
  ../../include/csimulation.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../envir/appreg.h \
  inspectorfactory.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  logbuffer.h \
  ../../include/cgate.h \
  ../../include/ccomponent.h \
  ../envir/speedometer.h \
  ../../include/cconfiguration.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  ../envir/envirbase.h \
  ../../include/cproperty.h \
  ../../include/csimplemodule.h \
  tklib.h \
  ../envir/envirdefs.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  loginspector.h \
  ../../include/onstartup.h \
  watchinspector.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cscheduler.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponenttype.h \
  tkenv.h \
  inspector.h \
  ../../include/envirext.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  circularbuffer.h \
  ../../include/cexpression.h \
  ../envir/eventlogfilemgr.h \
  gateinspector.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  genericobjectinspector.h \
  componenthistory.h \
  ../../include/cobject.h \
  ../../include/cconfigoption.h \
  ../../include/cmessage.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/cmodule.h \
  ../../include/platdep/timeutil.h \
  tkutil.h \
  ../../include/cregistrationlist.h \
  ../../include/cproperties.h \
  moduleinspector.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/cmessageprinter.h \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/opp_string.h
$O/tklib.o: tklib.cc \
  ../../include/cconfiguration.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  logbuffer.h \
  ../../include/cmsgpar.h \
  ../../include/platdep/platdefs.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h \
  ../../include/csimulation.h \
  ../envir/args.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../../include/cresultlistener.h \
  circularbuffer.h \
  ../../include/envirext.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  tkenv.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platmisc.h \
  ../../include/platdep/intxtypes.h \
  ../envir/objectprinter.h \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../../include/cexception.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../envir/matchableobject.h \
  ../../include/simtime.h \
  ../envir/envirdefs.h \
  tklib.h \
  ../envir/envirbase.h \
  ../envir/intervals.h \
  ../../include/carray.h \
  ../../include/cobject.h \
  componenthistory.h \
  ../../include/simutil.h \
  ../../include/cobjectfactory.h \
  ../../include/cdefaultlist.h \
  ../../include/cstringpool.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cexpression.h \
  ../../include/opp_string.h \
  ../../include/cchannel.h \
  ../envir/visitor.h \
  ../../include/cnedmathfunction.h \
  ../../include/clistener.h \
  ../../include/cvisitor.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cregistrationlist.h \
  tkutil.h \
  ../../include/cmodule.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/cmessage.h
$O/tkutil.o: tkutil.cc \
  ../../include/cresultlistener.h \
  ../../include/cnamedobject.h \
  ../../include/regmacros.h \
  ../envir/args.h \
  ../../include/csimulation.h \
  ../../include/cmessageheap.h \
  ../../include/globals.h \
  ../../include/platdep/platdefs.h \
  ../../include/cmsgpar.h \
  logbuffer.h \
  ../../include/cgate.h \
  ../../include/ccomponent.h \
  ../envir/speedometer.h \
  ../../include/cconfiguration.h \
  ../../include/carray.h \
  ../envir/intervals.h \
  ../envir/envirbase.h \
  ../../include/csimplemodule.h \
  ../envir/envirdefs.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  ../../include/cwatch.h \
  ../../include/onstartup.h \
  ../../include/coutvector.h \
  ../../include/cownedobject.h \
  ../../include/cexception.h \
  tkdefs.h \
  ../../include/simkerneldefs.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/platdep/platmisc.h \
  ../../include/ccomponenttype.h \
  tkenv.h \
  ../../include/cenvir.h \
  ../../include/envirext.h \
  ../../include/simtime_t.h \
  circularbuffer.h \
  ../../include/cexpression.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cqueue.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  componenthistory.h \
  ../../include/cobject.h \
  ../../include/cmessage.h \
  ../../include/cpar.h \
  ../../include/errmsg.h \
  ../../include/cmodule.h \
  tkutil.h \
  ../common/ver.h \
  ../../include/cregistrationlist.h \
  ../../include/ctimestampedvalue.h \
  ../../include/cvisitor.h \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/opp_string.h \
  ../../include/cstatistic.h
$O/watchinspector.o: watchinspector.cc \
  ../../include/clistener.h \
  ../../include/cnedmathfunction.h \
  ../../include/cvisitor.h \
  ../../include/ctimestampedvalue.h \
  ../../include/opp_string.h \
  ../envir/visitor.h \
  ../../include/cchannel.h \
  ../../include/errmsg.h \
  ../../include/cpar.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/cstlwatch.h \
  ../../include/cregistrationlist.h \
  tkutil.h \
  componenthistory.h \
  ../../include/cobject.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cexpression.h \
  ../../include/cobjectfactory.h \
  ../../include/simutil.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../envir/objectprinter.h \
  ../../include/platdep/intxtypes.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  tkdefs.h \
  ../../include/simtime_t.h \
  ../../include/cenvir.h \
  ../../include/envirext.h \
  inspector.h \
  circularbuffer.h \
  ../../include/ccomponenttype.h \
  ../../include/platdep/platmisc.h \
  tkenv.h \
  ../envir/envirdefs.h \
  tklib.h \
  ../envir/intervals.h \
  ../../include/carray.h \
  ../envir/envirbase.h \
  watchinspector.h \
  ../../include/onstartup.h \
  ../../include/cownedobject.h \
  ../../include/simtime.h \
  ../envir/matchableobject.h \
  ../../include/cwatch.h \
  ../../include/cmsgpar.h \
  ../../include/platdep/platdefs.h \
  logbuffer.h \
  inspectorfactory.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cconfiguration.h \
  ../../include/cgate.h \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../../include/cnamedobject.h \
  ../../include/csimulation.h \
  ../../include/cresultlistener.h \
  ../../include/globals.h \
  ../../include/cmessageheap.h
$O/tkImgPNG.o: tkImgPNG.c
$O/tkImgPNGInit.o: tkImgPNGInit.c
