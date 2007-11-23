#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

ifeq ($(strip $(CONFIGNAME)),)
O=.
L=$(OMNETPP_LIB_DIR)
else
O=$(OMNETPP_OBJ_DIR)/$(CONFIGNAME)/src/tkenv
L=$(OMNETPP_LIB_DIR)/$(CONFIGNAME)
endif

#
# Local definitions
#
LIBNAME=libtkenv

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../envir -I../layout -I../common $(TK_CFLAGS) $(ZLIB_CFLAGS) -DBUILDING_TKENV

IMPLIBS= -L$L -lsim_std -lenvir.lib -llayout -lcommon $(TK_LIBS) $(ZLIB_LIBS)

OBJS = $O/tkapp$(TKENV_EMBED_SUFFIX).o $O/tkcmd.o $O/tklib.o $O/inspector.o \
       $O/inspfactory.o $O/objinsp.o $O/modinsp.o $O/statinsp.o $O/layouterenv.o \
       $O/arrow.o $O/visitor.o $O/tkutil.o $O/tkImgPNG.o $O/tkImgPNGInit.o

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

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

tkapp.cc : tclcode.cc

$O/tkapp.o : tkapp.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -DOMNETPP_TKENV_DIR="\"$(OMNETPP_TKENV_DIR)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" tkapp.cc -o $O/tkapp.o

$O/tkapp-e.o : tkapp.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" tkapp.cc -o $O/tkapp-e.o

tclcode.cc: $O/tcl2c$(EXE_SUFFIX) *.tcl 
	@mkdir -p $O
	$O/tcl2c tclcode.cc *.tcl

$O/tcl2c$(EXE_SUFFIX): tcl2c.c
	@mkdir -p $O
	$(CC) tcl2c.c -o $@

missing-dependency:
	@echo '  *** No Tcl/Tk installation found -- skipping Tkenv.'
	@echo '  *** Install it, then run ./configure again.'

depend:
	$(MAKEDEPEND) -I. -I../envir -I../../include -P\$$O/ -- *.cc *.c

clean:
	rm -f tclcode.cc
	rm -f $O/*.o $O/$(LIBNAME)$(A_LIB_SUFFIX) $O/$(LIBNAME)$(SO_LIB_SUFFIX) $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $O/tcl2c$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arrow.o: arrow.cc \
  arrow.h
$O/inspector.o: inspector.cc \
  ../../include/omnetpp.h \
  tkapp.h \
  tklib.h \
  inspector.h \
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
  ../../include/platdep/platdefs.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  tkutil.h
$O/inspfactory.o: inspfactory.cc \
  ../../include/omnetpp.h \
  tkapp.h \
  tklib.h \
  inspector.h \
  inspfactory.h \
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
  ../../include/platdep/platdefs.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  tkutil.h
$O/layouterenv.o: layouterenv.cc \
  layouterenv.h \
  tklib.h \
  modinsp.h \
  ../../include/cmodule.h \
  ../../include/cdisplaystring.h \
  ../../include/csimplemodule.h \
  ../../include/cchannel.h \
  ../../include/cgate.h \
  ../../include/cmessage.h \
  ../../include/cpar.h \
  ../../include/carray.h \
  ../../include/coutvector.h \
  ../../include/cstat.h \
  ../../include/cdensity.h \
  ../../include/cqueue.h \
  ../../include/ccompoundmodule.h \
  ../../include/cbasicchannel.h \
  inspector.h \
  ../envir/omnetapp.h \
  ../../include/ccomponent.h \
  ../../include/ccomponenttype.h \
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
  ../../include/cneddeclarationbase.h \
  ../../include/globals.h \
  ../../include/cparvalue.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cexpression.h \
  ../../include/cmsgpar.h \
  ../../include/csimulation.h \
  ../../include/cfunction.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  tkutil.h \
  tkdefs.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h
$O/modinsp.o: modinsp.cc \
  modinsp.h \
  tkapp.h \
  tklib.h \
  inspfactory.h \
  arrow.h \
  layouterenv.h \
  ../../include/csimplemodule.h \
  ../../include/cchannel.h \
  ../../include/cgate.h \
  ../../include/cmessage.h \
  ../../include/cpar.h \
  ../../include/carray.h \
  ../../include/coutvector.h \
  ../../include/cstat.h \
  ../../include/cdensity.h \
  ../../include/cdisplaystring.h \
  ../../include/cqueue.h \
  ../../include/ccompoundmodule.h \
  ../../include/cbasicchannel.h \
  inspector.h \
  ../envir/omnetapp.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
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
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponenttype.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cexpression.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  tkutil.h \
  tkdefs.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h
$O/objinsp.o: objinsp.cc \
  ../../include/omnetpp.h \
  tkapp.h \
  tklib.h \
  inspfactory.h \
  objinsp.h \
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
  ../../include/platdep/platdefs.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  inspector.h \
  tkutil.h
$O/statinsp.o: statinsp.cc \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/cpar.h \
  ../../include/carray.h \
  ../../include/coutvector.h \
  ../../include/cstat.h \
  ../../include/cdensity.h \
  tkapp.h \
  tklib.h \
  inspfactory.h \
  statinsp.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
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
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  ../../include/cexpression.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  inspector.h \
  tkutil.h
$O/tkapp.o: tkapp.cc \
  ../envir/appreg.h \
  ../../include/cenvir.h \
  ../../include/csimplemodule.h \
  ../../include/cmessage.h \
  ../envir/speedometer.h \
  ../../include/cscheduler.h \
  ../../include/ccomponenttype.h \
  ../../include/csimulation.h \
  ../../include/cconfigkey.h \
  ../../include/regmacros.h \
  tkdefs.h \
  tkapp.h \
  tklib.h \
  inspector.h \
  inspfactory.h \
  modinsp.h \
  ../utils/ver.h \
  ../../include/globals.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
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
  ../../include/opp_string.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  ../envir/envdefs.h \
  ../../include/platdep/timeutil.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  ../../include/cchannel.h \
  ../../include/coutvector.h \
  ../../include/cstat.h \
  ../../include/cdensity.h \
  ../../include/cdisplaystring.h \
  ../../include/cqueue.h \
  ../../include/ccompoundmodule.h \
  ../../include/cbasicchannel.h
$O/tkcmd.o: tkcmd.cc \
  ../../include/cenvir.h \
  ../../include/carray.h \
  ../../include/csimulation.h \
  ../../include/csimplemodule.h \
  ../../include/ccompoundmodule.h \
  ../../include/ccomponenttype.h \
  ../../include/cpar.h \
  ../../include/cmessage.h \
  ../../include/cchannel.h \
  ../../include/cstat.h \
  ../../include/cwatch.h \
  ../../include/cclassdescriptor.h \
  ../../include/cdisplaystring.h \
  ../../include/cqueue.h \
  ../../include/coutvector.h \
  ../../include/cenum.h \
  tkapp.h \
  tklib.h \
  inspector.h \
  inspfactory.h \
  visitor.h \
  ../envir/fsutils.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/globals.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cdefaultlist.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cexpression.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  tkutil.h \
  ../envir/matchableobject.h
$O/tklib.o: tklib.cc \
  tklib.h \
  tkapp.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h
$O/tkutil.o: tkutil.cc \
  ../../include/cownedobject.h \
  ../../include/cmodule.h \
  ../../include/csimulation.h \
  tkapp.h \
  visitor.h \
  tkutil.h \
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
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../envir/matchableobject.h
$O/visitor.o: visitor.cc \
  ../../include/cenvir.h \
  ../../include/cobject.h \
  ../../include/cmodule.h \
  ../../include/cmessage.h \
  ../../include/cqueue.h \
  ../../include/cstat.h \
  ../../include/coutvector.h \
  ../../include/cwatch.h \
  ../../include/cfsm.h \
  ../../include/cpar.h \
  ../../include/cchannel.h \
  ../../include/cgate.h \
  visitor.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cdefaultlist.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cstringpool.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  ../../include/cexpression.h \
  ../../include/ccomponenttype.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  tkapp.h \
  ../envir/matchableobject.h \
  tkdefs.h \
  ../envir/omnetapp.h \
  ../envir/args.h \
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h
$O/tkImgPNG.o: tkImgPNG.c
$O/tkImgPNGInit.o: tkImgPNGInit.c
