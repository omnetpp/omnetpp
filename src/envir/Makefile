#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) $(AKAROA_CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../sim -I../common -DBUILDING_ENVIR -DHAVE_DLOPEN=$(HAVE_DLOPEN)

OBJS=args.o cenvir.o main.o omnetapp.o fsutils.o \
     sectionbasedconfig.o inifilereader.o scenario.o valueiterator.o \
     speedometer.o filemgrs.o akoutvectormgr.o matchableobject.o \
     akaroarng.o cxmldoccache.o ivfilemgr.o eventlogwriter.o objectprinter.o

#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all : libenvir$(LIB_SUFFIX)

libenvir$(A_LIB_SUFFIX) : $(OBJS)
	$(AR) libenvir$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libenvir$(A_LIB_SUFFIX)
	../utils/install-lib libenvir$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libenvir$(A_LIB_SUFFIX)

libenvir$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o libenvir$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libenvir$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libenvir$(SO_LIB_SUFFIX)

eventlogwriter.cc eventlogwriter.h : eventlogentries.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogentries.pl

omnetapp.o: eventlogwriter.h

depend:
	$(MAKEDEPEND) -I. -I../../include -- *.cc

clean:
	rm -f eventlogwriter.cc eventlogwriter.h 2> nul
	rm -f *.o libenvir$(A_LIB_SUFFIX) libenvir$(SO_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
akaroarng.o: akaroarng.cc \
  ../../include/cenvir.h \
  ../../include/util.h \
  ../../include/cexception.h \
  akaroarng.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  ../../include/crng.h \
  ../../include/cconfiguration.h \
  ../../include/cobject.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
akoutvectormgr.o: akoutvectormgr.cc \
  ../../include/cenvir.h \
  omnetapp.h \
  ../../include/cmodule.h \
  ../../include/cstat.h \
  akoutvectormgr.h \
  ../../include/simkerneldefs.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/errmsg.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cexception.h \
  args.h \
  envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  filemgrs.h
args.o: args.cc \
  args.h \
  envdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
cenvir.o: cenvir.cc \
  args.h \
  ../../include/distrib.h \
  ../../include/cconfigkey.h \
  inifilereader.h \
  sectionbasedconfig.h \
  ../../include/cenvir.h \
  omnetapp.h \
  appreg.h \
  ../../include/cmodule.h \
  fsutils.h \
  speedometer.h \
  filemgrs.h \
  akaroarng.h \
  akoutvectormgr.h \
  matchableobject.h \
  envdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/random.h \
  ../../include/csimulation.h \
  ../../include/crng.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/errmsg.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/cconfigreader.h \
  ../../include/cconfiguration.h \
  ../../include/envirext.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h
cxmldoccache.o: cxmldoccache.cc \
  ../../include/cobject.h \
  cxmldoccache.h \
  ../../include/cxmlelement.h \
  ../../include/cexception.h \
  ../nedxml/saxparser.h \
  ../../include/simkerneldefs.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/errmsg.h \
  envdefs.h \
  ../../include/cenvir.h \
  ../../include/opp_string.h \
  ../nedxml/nedxmldefs.h
filemgrs.o: filemgrs.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  omnetapp.h \
  ../../include/csimulation.h \
  ../../include/cmodule.h \
  ../../include/cstat.h \
  ../../include/cdensity.h \
  filemgrs.h \
  ../../include/ccomponenttype.h \
  ivfilemgr.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/opp_string.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h
fsutils.o: fsutils.cc \
  fsutils.h \
  ../../include/onstartup.h \
  ../../include/cexception.h \
  ../../include/util.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/opp_string.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h
inifilereader.o: inifilereader.cc \
  inifilereader.h \
  fsutils.h \
  ../../include/cexception.h \
  ../../include/cconfigreader.h \
  ../../include/util.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/opp_string.h
ivfilemgr.o: ivfilemgr.cc \
  ../../include/simkerneldefs.h \
  ../../include/cconfigkey.h \
  ../../include/cenvir.h \
  omnetapp.h \
  ../../include/csimulation.h \
  ../../include/regmacros.h \
  ../../include/cmodule.h \
  ../../include/cstat.h \
  ivfilemgr.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/opp_string.h \
  ../../include/carray.h \
  ../../include/globals.h \
  args.h \
  envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  filemgrs.h
main.o: main.cc \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../utils/ver.h \
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
  ../../include/opp_string.h
matchableobject.o: matchableobject.cc \
  matchableobject.h \
  ../../include/cclassdescriptor.h \
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
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h
objectprinter.o: objectprinter.cc \
  ../../include/cconfiguration.h \
  ../../include/cmodule.h \
  ../../include/cclassdescriptor.h \
  objectprinter.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/ccomponent.h \
  ../../include/globals.h \
  ../../include/cgate.h \
  ../../include/csimulation.h \
  ../../include/cownedobject.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cexception.h \
  ../../include/opp_string.h \
  ../../include/cstringpool.h \
  ../../include/cexpression.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
  matchableobject.h
omnetapp.o: omnetapp.cc \
  args.h \
  omnetapp.h \
  appreg.h \
  fsutils.h \
  ../../include/ccoroutine.h \
  ../../include/csimulation.h \
  ../../include/cscheduler.h \
  ../../include/cpar.h \
  ../../include/cproperties.h \
  ../../include/cproperty.h \
  ../../include/random.h \
  ../../include/crng.h \
  ../../include/cmodule.h \
  ../../include/ccomponenttype.h \
  ../../include/cmessage.h \
  ../../include/cdisplaystring.h \
  ../../include/cxmlelement.h \
  cxmldoccache.h \
  ../../include/chasher.h \
  ../../include/cconfigkey.h \
  ../../include/regmacros.h \
  ../../include/cparsimcomm.h \
  objectprinter.h \
  envdefs.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/carray.h \
  ../../include/globals.h \
  ../../include/cenvir.h \
  ../../include/cexception.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cownedobject.h \
  ../../include/util.h \
  ../../include/cnamedobject.h \
  ../../include/errmsg.h \
  ../../include/cobject.h \
  ../../include/cstringpool.h \
  ../../include/cvisitor.h \
  ../../include/onstartup.h \
  ../../include/cclassfactory.h \
  ../../include/opp_string.h \
  ../../include/cmsgheap.h \
  ../../include/platdep/timeutil.h \
  ../../include/cexpression.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cdefaultlist.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h \
  matchableobject.h
scenario.o: scenario.cc \
  scenario.h \
  valueiterator.h \
  ../../include/cexception.h \
  sectionbasedconfig.h \
  ../../include/cconfiguration.h \
  ../../include/cconfigreader.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h
sectionbasedconfig.o: sectionbasedconfig.cc \
  sectionbasedconfig.h \
  ../../include/cconfigreader.h \
  valueiterator.h \
  ../../include/cexception.h \
  scenario.h \
  ../../include/globals.h \
  ../../include/cconfigkey.h \
  ../../include/cconfiguration.h \
  ../../include/simkerneldefs.h \
  ../../include/cobject.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h \
  ../../include/cvisitor.h \
  ../../include/regmacros.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/onstartup.h \
  ../../include/carray.h \
  ../../include/cclassfactory.h \
  ../../include/cownedobject.h \
  ../../include/cenvir.h \
  ../../include/cnamedobject.h \
  ../../include/cstringpool.h
speedometer.o: speedometer.cc \
  speedometer.h \
  ../../include/simkerneldefs.h \
  envdefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h
valueiterator.o: valueiterator.cc \
  valueiterator.h \
  ../../include/cexception.h \
  ../../include/simkerneldefs.h \
  ../../include/errmsg.h \
  ../../include/opp_string.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/util.h
