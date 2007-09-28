#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_INCL_DIR)/platdep -I../common -I../envir -DBUILDING_CMDENV

OBJS = cmdenv.o enumstr.o

#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: libcmdenv$(LIB_SUFFIX)

libcmdenv$(A_LIB_SUFFIX) : $(OBJS)
	$(AR) libcmdenv$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libcmdenv$(A_LIB_SUFFIX)
	../utils/install-lib libcmdenv$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libcmdenv$(A_LIB_SUFFIX)

libcmdenv$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o libcmdenv$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libcmdenv$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libcmdenv$(SO_LIB_SUFFIX)

depend:
	$(MAKEDEPEND) -I. -I../envir -I../../include -- *.cc

clean:
	rm -f *.o libcmdenv$(A_LIB_SUFFIX) libcmdenv$(SO_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
cmdenv.o: cmdenv.cc \
  cmddefs.h \
  cmdenv.h \
  enumstr.h \
  ../envir/appreg.h \
  ../../include/csimplemodule.h \
  ../../include/ccomponenttype.h \
  ../../include/cmessage.h \
  ../envir/args.h \
  ../envir/speedometer.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/platdefs.h \
  ../../include/simtime.h \
  ../../include/platdep/inttypes.h \
  ../../include/csimulation.h \
  ../envir/omnetapp.h \
  ../../include/util.h \
  ../../include/globals.h \
  ../../include/cmsgheap.h \
  ../../include/ccoroutine.h \
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
  ../envir/envdefs.h \
  ../../include/envirext.h \
  ../../include/cconfiguration.h \
  ../../include/cconfigkey.h \
  ../../include/cmodule.h \
  ../../include/ccomponent.h \
  ../../include/cgate.h \
  ../../include/cpar.h \
  ../../include/cdefaultlist.h \
  ../../include/cexpression.h \
  ../../include/cneddeclarationbase.h \
  ../../include/cparvalue.h \
  ../../include/cmsgpar.h \
  ../../include/cfunction.h
enumstr.o: enumstr.cc \
  enumstr.h
