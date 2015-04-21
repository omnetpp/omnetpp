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
LIBNAME=liboppqtenv$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/qtenv
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)


# when expanded, this is the output of
# pkg-config --cflags Qt5Core Qt5Gui Qt5Widgets
# FIXME hardcoded path. Shoud be handled with pkg-config 
#QTPREFIX= /usr/include/x86_64-linux-gnu
#QTINCLUDES= -I$(QTPREFIX)/qt5/QtGui -I$(QTPREFIX)/qt5 -I$(QTPREFIX)/qt5/QtCore -I$(QTPREFIX)/qt5/QtWidgets

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_SRC_DIR) -DBUILDING_QTENV $(QT_CFLAGS)

IMPLIBS= -L$L -loppsim$D -loppenvir$D -loppcommon$D $(QT_LIBS)

QTENVUILIB= libqtenv-ui.a

OBJS = $O/$(QTENVUILIB) $O/qtenv.o $O/tkcmd.o $O/tklib.o $O/inspector.o $O/logbuffer.o \
       $O/inspectorfactory.o $O/genericobjectinspector.o $O/watchinspector.o $O/canvasinspector.o \
       $O/moduleinspector.o $O/loginspector.o $O/gateinspector.o $O/figurerenderers.o \
       $O/outputvectorinspector.o $O/histograminspector.o $O/componenthistory.o \
       $O/canvasrenderer.o $O/layouterenv.o $O/arrow.o $O/graphicspatharrowitem.o \
       $O/tkutil.o


qtenv: $L/$(LIBNAME)$(LIB_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .


$O/$(QTENVUILIB):
	@echo Creating static UI library from the sources in qtcreator...
	$(Q)qmake -Wall -o $O/Makefile && (cd $O && make)

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(O)/*.o
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) -Wl,--whole-archive $(OBJS) -Wl,--no-whole-archive $(IMPLIBS)
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

#
# Automatic rules
#
.SUFFIXES : .cc

$O/qtenv.o : qtenv.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" qtenv.cc -o $O/qtenv.o

$O/%.o: %.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<


#
# Targets
#
all: $(QTENV_IF_POSSIBLE)
depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(OBJS)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

missing-dependency:
	@echo '  *** No QT installation found -- skipping Qtenv.'
	@echo '  *** Install it, then run ./configure again.'

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arrow.o: arrow.cc \
  ../../include/omnetpp/cobject.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/errmsg.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simutil.h \
  arrow.h \
  qtenvdefs.h \
  tk-dummy.h \
  tkutil.h
$O/canvasinspector.o: canvasinspector.cc \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cexception.h \
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
  canvasinspector.h \
  canvasrenderer.h \
  figurerenderers.h \
  inspector.h \
  inspectorfactory.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/canvasrenderer.o: canvasrenderer.cc \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cexception.h \
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
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  canvasrenderer.h \
  figurerenderers.h
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
  ../../include/omnetpp/cmessageheap.h \
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
  qtenvdefs.h \
  tk-dummy.h
$O/figurerenderers.o: figurerenderers.cc \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cexception.h \
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
  ../../include/omnetpp/regmacros.h \
  ../../include/omnetpp/simkerneldefs.h \
  ../../include/omnetpp/simtime.h \
  ../../include/omnetpp/simtime_t.h \
  ../../include/omnetpp/simutil.h \
  figurerenderers.h \
  graphicspatharrowitem.h
$O/gateinspector.o: gateinspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmessageheap.h \
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
  circularbuffer.h \
  componenthistory.h \
  gateinspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/genericobjectinspector.o: genericobjectinspector.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/crng.h \
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
  circularbuffer.h \
  componenthistory.h \
  genericobjectinspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/graphicspatharrowitem.o: graphicspatharrowitem.cc \
  graphicspatharrowitem.h
$O/histograminspector.o: histograminspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstddev.h \
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
  circularbuffer.h \
  componenthistory.h \
  histograminspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/inspector.o: inspector.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/inspectorfactory.o: inspectorfactory.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
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
  ../../include/omnetpp/cmessageheap.h \
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
  layouterenv.h \
  qtenvdefs.h \
  tk-dummy.h \
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
  ../../include/omnetpp/cmessageheap.h \
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
  circularbuffer.h \
  logbuffer.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/loginspector.o: loginspector.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/crng.h \
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
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  loginspector.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/moduleinspector.o: moduleinspector.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmessageheap.h \
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
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/osxproctype.o: osxproctype.cc
$O/outputvectorinspector.o: outputvectorinspector.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/crng.h \
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
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  outputvectorinspector.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/qtenv.o: qtenv.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cconfigoption.h \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/cscheduler.h \
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
  circularbuffer.h \
  componenthistory.h \
  gateinspector.h \
  genericobjectinspector.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  loginspector.h \
  moduleinspector.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h \
  watchinspector.h
$O/tkcmd.o: tkcmd.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
  ../../include/omnetpp/cchannel.h \
  ../../include/omnetpp/cclassdescriptor.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/ccomponenttype.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cdisplaystring.h \
  ../../include/omnetpp/cenum.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/cgate.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessage.h \
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
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
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/tklib.o: tklib.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  circularbuffer.h \
  componenthistory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h
$O/tkutil.o: tkutil.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccanvas.h \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/cregistrationlist.h \
  ../../include/omnetpp/crng.h \
  ../../include/omnetpp/csimplemodule.h \
  ../../include/omnetpp/csimulation.h \
  ../../include/omnetpp/cstatistic.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
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
  circularbuffer.h \
  componenthistory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tkutil.h
$O/watchinspector.o: watchinspector.cc \
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
  ../../include/omnetpp/cmessageheap.h \
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
  ../../include/omnetpp/cstlwatch.h \
  ../../include/omnetpp/cstringpool.h \
  ../../include/omnetpp/ctimestampedvalue.h \
  ../../include/omnetpp/cvisitor.h \
  ../../include/omnetpp/cwatch.h \
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
  circularbuffer.h \
  componenthistory.h \
  inspector.h \
  inspectorfactory.h \
  logbuffer.h \
  qtenv.h \
  qtenvdefs.h \
  tk-dummy.h \
  tklib.h \
  tkutil.h \
  watchinspector.h
