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

OBJS= $O/$(QTENVUILIB) $O/qtenv.o $O/canvasrenderer.o $O/figurerenderers.o $O/graphicspatharrowitem.o

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<


#
# Targets
#
all: $(QTENV_IF_POSSIBLE)

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

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm $(OBJS)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

missing-dependency:
	@echo '  *** No QT installation found -- skipping Qtenv.'
	@echo '  *** Install it, then run ./configure again.'

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/canvasrenderer.o: canvasrenderer.cc \
  canvasrenderer.h \
  figurerenderers.h
$O/figurerenderers.o: figurerenderers.cc \
  figurerenderers.h \
  graphicspatharrowitem.h
$O/graphicspatharrowitem.o: graphicspatharrowitem.cc \
  graphicspatharrowitem.h
$O/qtenv.o: qtenv.cc \
  ../../include/omnetpp/carray.h \
  ../../include/omnetpp/ccomponent.h \
  ../../include/omnetpp/cconfiguration.h \
  ../../include/omnetpp/cdefaultlist.h \
  ../../include/omnetpp/cenvir.h \
  ../../include/omnetpp/cevent.h \
  ../../include/omnetpp/cexception.h \
  ../../include/omnetpp/cexpression.h \
  ../../include/omnetpp/clifecyclelistener.h \
  ../../include/omnetpp/clistener.h \
  ../../include/omnetpp/clog.h \
  ../../include/omnetpp/cmessageheap.h \
  ../../include/omnetpp/cnamedobject.h \
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
  canvasrenderer.h \
  figurerenderers.h \
  mainwindow.h \
  qtdefs.h \
  qtenv.h
