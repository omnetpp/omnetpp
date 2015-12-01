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

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS) $(TK_CFLAGS) $(ZLIB_CFLAGS) -DBUILDING_TKENV

IMPLIBS= -L$L -loppsim$D -loppenvir$D -lopplayout$D -loppcommon$D $(TK_LIBS) $(ZLIB_LIBS)

OBJS = $O/tkenv$(TKENV_EMBED_SUFFIX).o $O/tkcmd.o $O/tklib.o $O/inspector.o $O/logbuffer.o \
       $O/inspectorfactory.o $O/genericobjectinspector.o $O/watchinspector.o $O/canvasinspector.o \
       $O/moduleinspector.o $O/loginspector.o $O/gateinspector.o $O/figurerenderers.o \
       $O/outputvectorinspector.o $O/histograminspector.o $O/componenthistory.o \
       $O/canvasrenderer.o $O/layouterenv.o $O/arrow.o \
       $O/tkutil.o $O/tkImgPNG.o $O/tkImgPNGInit.o

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

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS) tcl-libs
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS) tcl-libs
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(Q)$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS) tcl-libs
	@mkdir -p $L
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

tcl-libs:
	@echo Copying Tcl extensions to lib/
	$(Q)cp -R tkpath $(OMNETPP_LIB_DIR)
ifeq ("$(PLATFORM)", "macosx")
	$(Q)cp -R tkdock $(OMNETPP_LIB_DIR)
endif

$O/tkenv.o : tkenv.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -DOMNETPP_LIB_DIR="\"$(OMNETPP_LIB_DIR)\"" -DOMNETPP_TKENV_DIR="\"$(OMNETPP_TKENV_DIR)\"" tkenv.cc -o $O/tkenv.o

$O/tkenv-e.o : tkenv.cc tclcode.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -DOMNETPP_LIB_DIR="\"$(OMNETPP_LIB_DIR)\"" tkenv.cc -o $O/tkenv-e.o

tclcode.cc: *.tcl tcl2c.pl
	@# clearlooksimg.tcl must precede clearlooks.tcl (tcl2c filters out duplicates)
	$(PERL) tcl2c.pl tclcode.cc clearlooksimg.tcl *.tcl

missing-dependency:
	@echo '  *** No Tcl/Tk installation found -- skipping Tkenv.'
	@echo '  *** Install it, then run ./configure again.'

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I../../include -I.. -P\$$O/ -- *.cc *.c

clean:
	$(qecho) Cleaning...
	$(Q)rm -f tclcode.cc
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arrow.o: arrow.cc \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/envirdefs.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  arrow.h \
  tkdefs.h \
  tkutil.h
$O/canvasinspector.o: canvasinspector.cc \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../envir/envirdefs.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  canvasinspector.h \
  canvasrenderer.h \
  figurerenderers.h \
  inspector.h \
  inspectorfactory.h \
  tkdefs.h \
  tklib.h \
  tkutil.h
$O/canvasrenderer.o: canvasrenderer.cc \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../envir/envirdefs.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  canvasrenderer.h \
  figurerenderers.h \
  tkdefs.h \
  tklib.h \
  tkutil.h
$O/componenthistory.o: componenthistory.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  componenthistory.h \
  tkdefs.h
$O/figurerenderers.o: figurerenderers.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/checkandcast.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  figurerenderers.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tkutil.h
$O/gateinspector.o: gateinspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  gateinspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/genericobjectinspector.o: genericobjectinspector.cc \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/envirdefs.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  genericobjectinspector.h \
  inspector.h \
  inspectorfactory.h \
  tkdefs.h \
  tklib.h \
  tkutil.h
$O/histograminspector.o: histograminspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdensityestbase.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstddev.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  histograminspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/inspector.o: inspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/inspectorfactory.o: inspectorfactory.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/layouterenv.o: layouterenv.cc \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/lcgrandom.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/envirdefs.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  ../layout/graphlayouter.h \
  ../layout/layoutdefs.h \
  layouterenv.h \
  tkdefs.h \
  tklib.h \
  tkutil.h
$O/logbuffer.o: logbuffer.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/envirdefs.h \
  ../envir/matchableobject.h \
  ../envir/visitor.h \
  circularbuffer.h \
  logbuffer.h \
  tkdefs.h \
  tklib.h \
  tkutil.h
$O/loginspector.o: loginspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmessageprinter.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringtokenizer.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  loginspector.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/moduleinspector.o: moduleinspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cqueue.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/lcgrandom.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  ../layout/basicspringembedderlayout.h \
  ../layout/forcedirectedembedding.h \
  ../layout/forcedirectedgraphlayouter.h \
  ../layout/forcedirectedparameters.h \
  ../layout/forcedirectedparametersbase.h \
  ../layout/geometry.h \
  ../layout/graphcomponent.h \
  ../layout/graphlayouter.h \
  ../layout/layoutdefs.h \
  arrow.h \
  canvasrenderer.h \
  circularbuffer.h \
  componenthistory.h \
  figurerenderers.h \
  inspector.h \
  inspectorfactory.h \
  layouterenv.h \
  logbuffer.h \
  moduleinspector.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/osxproctype.o: osxproctype.cc
$O/outputvectorinspector.o: outputvectorinspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/coutvector.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  outputvectorinspector.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/tclcode.o: tclcode.cc
$O/tkcmd.o: tkcmd.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenum.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/coutvector.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpacket.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cqueue.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/patternmatcher.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  ../common/unitconversion.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/fsutils.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/tkenv.o: tkenv.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmessageprinter.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cproperties.h \
  ../../include/omnetpp/cproperty.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/cscheduler.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  ../common/ver.h \
  ../envir/appreg.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  gateinspector.h \
  genericobjectinspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  loginspector.h \
  moduleinspector.h \
  tclcode.cc \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h \
  watchinspector.h
$O/tklib.o: tklib.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h
$O/tkutil.o: tkutil.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cfutureeventset.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/coutvector.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cqueue.h \
  ../../include/omnetpp/crandom.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../common/ver.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tkutil.h
$O/watchinspector.o: watchinspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmodule.h \
  ../../include/omnetpp/cmsgpar.h \
  ../../include/omnetpp/cnamedobject.h \
  ../../include/omnetpp/cnedmathfunction.h \
  ../../include/omnetpp/cnedvalue.h \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cobjectfactory.h \
  ../../include/omnetpp/cownedobject.h \
  ../../include/omnetpp/cpar.h \
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/cresultlistener.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstlwatch.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
  ../../include/omnetpp/distrib.h \
  ../../include/omnetpp/envirext.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/globals.h \
  ../../include/omnetpp/onstartup.h \
  ../../include/omnetpp/opp_string.h \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../envir/args.h \
  ../envir/envirbase.h \
  ../envir/envirdefs.h \
  ../envir/eventlogfilemgr.h \
  ../envir/intervals.h \
  ../envir/logformatter.h \
  ../envir/matchableobject.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../envir/visitor.h \
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  tkdefs.h \
  tkenv.h \
  tklib.h \
  tkutil.h \
  watchinspector.h
$O/tkImgPNG.o: tkImgPNG.c
$O/tkImgPNGInit.o: tkImgPNGInit.c
