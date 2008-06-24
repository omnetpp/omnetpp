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
LIBNAME=libtkenv$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/tkenv
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../envir -I../layout -I../common $(TK_CFLAGS) $(ZLIB_CFLAGS) -DBUILDING_TKENV

IMPLIBS= -L$L -lsim_std$D -lenvir$D -llayout$D -lcommon$D $(TK_LIBS) $(ZLIB_LIBS)

OBJS = $O/tkenv$(TKENV_EMBED_SUFFIX).o $O/tkcmd.o $O/tklib.o $O/inspector.o \
       $O/inspfactory.o $O/objinsp.o $O/modinsp.o $O/statinsp.o $O/layouterenv.o \
       $O/arrow.o $O/visitor.o $O/tkutil.o $O/tkImgPNG.o $O/tkImgPNGInit.o

ifeq ("$(EMBED_TCL_CODE)","yes")
TKENV_EMBED_SUFFIX=-e
else
TKENV_EMBED_SUFFIX=
endif

# Required only on Mac OS X 
ifneq (,$(findstring dylib,$(SO_LIB_SUFFIX)))
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
	$(CC) -c $(COPTS) -o $@ $<

$O/%.o: %.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $(TKENV_IF_POSSIBLE)

tkenv : $L/$(LIBNAME)$(LIB_SUFFIX)
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

$O/tkenv.o : tkenv.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -DOMNETPP_TKENV_DIR="\"$(OMNETPP_TKENV_DIR)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" tkenv.cc -o $O/tkenv.o

$O/tkenv-e.o : tkenv.cc tclcode.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" tkenv.cc -o $O/tkenv-e.o

tclcode.cc: *.tcl tcl2c.pl
	$(PERL) tcl2c.pl tclcode.cc *.tcl

missing-dependency:
	@echo '  *** No Tcl/Tk installation found -- skipping Tkenv.'
	@echo '  *** Install it, then run ./configure again.'

depend:
	$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc *.c

clean:
	rm -f tclcode.cc
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arrow.o: arrow.cc \
  ../../include/regmacros.h \
  ../../include/cownedobject.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/cstringpool.h \
  ../../include/cnamedobject.h \
  ../../include/simtime.h \
  ../../include/cvisitor.h \
  ../../include/cenvir.h \
  arrow.h \
  tkutil.h \
  ../../include/platdep/platmisc.h \
  ../../include/opp_string.h \
  ../../include/simutil.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  ../../include/errmsg.h
$O/inspector.o: inspector.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../../include/clinkedlist.h \
  ../envir/envirdefs.h \
  ../../include/cksplit.h \
  ../../include/cdoubleparimpl.h \
  ../../include/distrib.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cproperties.h \
  ../../include/cregistrationlist.h \
  ../../include/cenum.h \
  ../../include/cdynamicexpression.h \
  ../../include/platdep/timeutil.h \
  ../../include/cconfiguration.h \
  ../../include/chasher.h \
  ../../include/nedsupport.h \
  ../../include/ccommbuffer.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/clongparimpl.h \
  ../../include/onstartup.h \
  ../../include/cstlwatch.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cvarhist.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cfsm.h \
  ../../include/omnetpp.h \
  ../../include/cvisitor.h \
  ../../include/cmersennetwister.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../../include/compat.h \
  ../../include/clcg32.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/cpsquare.h \
  ../../include/ctopology.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/cparsimcomm.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/cdetect.h \
  ../../include/platdep/platdefs.h \
  ../../include/coutvector.h \
  ../../include/cqueue.h \
  ../../include/cwatch.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cconfigkey.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cstringparimpl.h \
  ../../include/cenvir.h \
  ../../include/mersennetwister.h \
  ../../include/cboolparimpl.h \
  ../../include/cmsgpar.h \
  ../../include/cclassdescriptor.h \
  ../../include/cstatistic.h \
  ../../include/cstringtokenizer.h \
  inspector.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/csimulation.h \
  ../../include/chistogram.h \
  ../../include/cdisplaystring.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../../include/cnedfunction.h \
  ../../include/cbasicchannel.h \
  ../envir/bootenv.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cxmlparimpl.h \
  ../../include/cparimpl.h \
  ../../include/cstddev.h \
  ../../include/cxmlelement.h \
  ../../include/crng.h \
  ../../include/cpacketqueue.h \
  ../../include/index.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../../include/cproperty.h \
  ../envir/matchablefield.h
$O/inspfactory.o: inspfactory.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../../include/clinkedlist.h \
  ../envir/envirdefs.h \
  ../../include/cksplit.h \
  ../../include/cdoubleparimpl.h \
  ../../include/distrib.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cproperties.h \
  ../../include/cregistrationlist.h \
  ../../include/cenum.h \
  ../../include/cdynamicexpression.h \
  ../../include/platdep/timeutil.h \
  ../../include/cconfiguration.h \
  ../../include/chasher.h \
  ../../include/nedsupport.h \
  ../../include/ccommbuffer.h \
  ../../include/errmsg.h \
  ../../include/cmessage.h \
  ../../include/cownedobject.h \
  ../../include/clongparimpl.h \
  ../../include/cstlwatch.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cvarhist.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cfsm.h \
  ../../include/omnetpp.h \
  ../../include/cvisitor.h \
  ../../include/cmersennetwister.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../../include/compat.h \
  ../../include/clcg32.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/cpsquare.h \
  ../../include/ctopology.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/cparsimcomm.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/cdetect.h \
  ../../include/platdep/platdefs.h \
  ../../include/cwatch.h \
  ../../include/cqueue.h \
  ../../include/coutvector.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/cconfigkey.h \
  ../../include/cstringparimpl.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cboolparimpl.h \
  ../../include/mersennetwister.h \
  ../../include/cclassdescriptor.h \
  ../../include/cstatistic.h \
  ../../include/cstringtokenizer.h \
  inspector.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/csimulation.h \
  ../../include/chistogram.h \
  ../../include/cdisplaystring.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../../include/cnedfunction.h \
  ../../include/cbasicchannel.h \
  ../envir/bootenv.h \
  inspfactory.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cxmlparimpl.h \
  ../../include/cparimpl.h \
  ../../include/cstddev.h \
  ../../include/cxmlelement.h \
  ../../include/crng.h \
  ../../include/cpacketqueue.h \
  ../../include/index.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../../include/cproperty.h \
  ../envir/matchablefield.h
$O/layouterenv.o: layouterenv.cc \
  ../../include/regmacros.h \
  ../envir/args.h \
  layouterenv.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cmessage.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  modinsp.h \
  ../../include/coutvector.h \
  ../../include/cqueue.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  inspector.h \
  ../../include/cchannel.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cdisplaystring.h \
  ../../include/cobject.h \
  tkdefs.h \
  ../../include/cbasicchannel.h \
  ../../include/cstringpool.h \
  ../../include/cdensityestbase.h \
  ../../include/cdefaultlist.h \
  ../../include/cstddev.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h \
  ../envir/matchablefield.h
$O/modinsp.o: modinsp.cc \
  ../../include/regmacros.h \
  ../envir/args.h \
  layouterenv.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  arrow.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cmessage.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  modinsp.h \
  ../../include/coutvector.h \
  ../../include/cqueue.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  inspector.h \
  ../../include/cchannel.h \
  ../../include/opp_string.h \
  ../envir/intervals.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cdisplaystring.h \
  ../../include/cobject.h \
  tkdefs.h \
  ../../include/cbasicchannel.h \
  ../envir/bootenv.h \
  inspfactory.h \
  ../../include/cstringpool.h \
  ../../include/cdensityestbase.h \
  ../../include/cdefaultlist.h \
  ../../include/cstddev.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h \
  tkenv.h \
  ../envir/matchablefield.h
$O/objinsp.o: objinsp.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../../include/clinkedlist.h \
  ../envir/envirdefs.h \
  ../../include/cksplit.h \
  ../../include/cdoubleparimpl.h \
  ../../include/distrib.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  objinsp.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cproperties.h \
  ../../include/cregistrationlist.h \
  ../../include/cenum.h \
  ../../include/cdynamicexpression.h \
  ../../include/platdep/timeutil.h \
  ../../include/cconfiguration.h \
  ../../include/chasher.h \
  ../../include/nedsupport.h \
  ../../include/ccommbuffer.h \
  ../../include/errmsg.h \
  ../../include/cmessage.h \
  ../../include/cownedobject.h \
  ../../include/clongparimpl.h \
  ../../include/cstlwatch.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cvarhist.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cfsm.h \
  ../../include/omnetpp.h \
  ../../include/cvisitor.h \
  ../../include/cmersennetwister.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../../include/compat.h \
  ../../include/clcg32.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/cpsquare.h \
  ../../include/ctopology.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/cparsimcomm.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/cdetect.h \
  ../../include/platdep/platdefs.h \
  ../../include/cwatch.h \
  ../../include/cqueue.h \
  ../../include/coutvector.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/cconfigkey.h \
  ../../include/cstringparimpl.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cboolparimpl.h \
  ../../include/mersennetwister.h \
  ../../include/cclassdescriptor.h \
  ../../include/cstatistic.h \
  inspector.h \
  ../../include/cstringtokenizer.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/random.h \
  ../../include/csimulation.h \
  ../../include/chistogram.h \
  ../../include/cdisplaystring.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../../include/cnedfunction.h \
  ../../include/cbasicchannel.h \
  ../envir/bootenv.h \
  inspfactory.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cxmlparimpl.h \
  ../../include/cparimpl.h \
  ../../include/cstddev.h \
  ../../include/cxmlelement.h \
  ../../include/crng.h \
  ../../include/cpacketqueue.h \
  ../../include/index.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../../include/cproperty.h \
  ../envir/matchablefield.h
$O/osxproctype.o: osxproctype.cc
$O/statinsp.o: statinsp.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/coutvector.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  inspector.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../envir/bootenv.h \
  inspfactory.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cdensityestbase.h \
  ../../include/cstddev.h \
  ../../include/simutil.h \
  ../../include/cmessageheap.h \
  tkenv.h \
  statinsp.h \
  ../envir/matchablefield.h
$O/tkcmd.o: tkcmd.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  visitor.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cenum.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../envir/fsutils.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/coutvector.h \
  ../../include/cqueue.h \
  ../../include/cwatch.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cclassdescriptor.h \
  ../../include/cstatistic.h \
  inspector.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cdisplaystring.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../envir/bootenv.h \
  inspfactory.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../envir/matchablefield.h
$O/tkenv.o: tkenv.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../../include/ccompoundmodule.h \
  ../envir/objectprinter.h \
  ../envir/speedometer.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/platdep/timeutil.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../envir/appreg.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/csimplemodule.h \
  ../utils/ver.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  modinsp.h \
  ../../include/coutvector.h \
  ../../include/cqueue.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/cconfigkey.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  inspector.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  ../../include/cdisplaystring.h \
  ../../include/cobject.h \
  tkdefs.h \
  ../../include/cbasicchannel.h \
  inspfactory.h \
  ../envir/bootenv.h \
  ../../include/cstringpool.h \
  ../../include/cdensityestbase.h \
  ../../include/cdefaultlist.h \
  ../../include/cstddev.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../envir/matchablefield.h
$O/tklib.o: tklib.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/cmodule.h \
  ../../include/platdep/platmisc.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  tklib.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../envir/bootenv.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../envir/matchablefield.h
$O/tkutil.o: tkutil.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  visitor.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/cclassfactory.h \
  ../envir/eventlogfilemgr.h \
  ../../include/cgate.h \
  ../../include/simtime.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../envir/bootenv.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  tkenv.h \
  ../envir/matchablefield.h
$O/visitor.o: visitor.cc \
  ../envir/args.h \
  ../../include/regmacros.h \
  visitor.h \
  ../envir/envirdefs.h \
  ../../include/cexpression.h \
  ../../include/cnamedobject.h \
  ../envir/objectprinter.h \
  ../../include/ccomponent.h \
  ../../include/cregistrationlist.h \
  ../../include/cconfiguration.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cmessage.h \
  ../../include/onstartup.h \
  ../envir/envirbase.h \
  ../../include/envirext.h \
  ../../include/cpar.h \
  ../../include/globals.h \
  ../../include/cfsm.h \
  ../../include/cvisitor.h \
  tkutil.h \
  ../../include/platdep/platmisc.h \
  ../../include/cmodule.h \
  ../envir/matchableobject.h \
  ../../include/cexception.h \
  ../../include/platdep/intxtypes.h \
  ../../include/ccomponenttype.h \
  ../../include/ccoroutine.h \
  ../../include/platdep/platdefs.h \
  ../../include/cwatch.h \
  ../../include/coutvector.h \
  ../../include/cqueue.h \
  ../../include/cclassfactory.h \
  ../../include/cmathfunction.h \
  ../envir/eventlogfilemgr.h \
  ../../include/simtime.h \
  ../../include/cgate.h \
  ../../include/carray.h \
  ../../include/cenvir.h \
  ../../include/cmsgpar.h \
  ../../include/cstatistic.h \
  ../../include/cchannel.h \
  ../envir/intervals.h \
  ../../include/opp_string.h \
  ../../include/simkerneldefs.h \
  ../../include/csimulation.h \
  tkdefs.h \
  ../../include/cobject.h \
  ../envir/bootenv.h \
  ../../include/cstringpool.h \
  ../../include/cdefaultlist.h \
  tkenv.h \
  ../../include/cmessageheap.h \
  ../../include/simutil.h \
  ../envir/matchablefield.h
$O/tkImgPNG.o: tkImgPNG.c
$O/tkImgPNGInit.o: tkImgPNGInit.c
