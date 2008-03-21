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
LIBNAME=libeventlog$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/eventlog
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR)/platdep -I. -I../common

IMPLIBS= -L$L -lcommon$D

OBJS= $O/ievent.o $O/ieventlog.o $O/eventlogfacade.o $O/eventlogtablefacade.o $O/sequencechartfacade.o \
      $O/eventlog.o $O/eventlogindex.o $O/messagedependency.o $O/event.o $O/eventlogentry.o \
      $O/eventlogentries.o $O/filteredevent.o $O/filteredeventlog.o $O/eventlogentryfactory.o
LIBS= -leventlog$D -lcommon$D

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: eventlogentries.cc $L/$(LIBNAME)$(LIB_SUFFIX) $O/eventlogtool$(EXE_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

eventlogentries.cc eventlogentries.h eventlogfactory.cc : eventlogentries.txt eventlogentries.pl
	$(PERL) eventlogentries.pl

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

$O/eventlogtool$(EXE_SUFFIX) : $O/eventlogtool.o  $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o $O/eventlogtool$(EXE_SUFFIX) $O/eventlogtool.o -L$L $(LIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog eventlogtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

clean:
	rm -f eventlogentries.cc eventlogentries.h eventlogfactory.cc
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	cd $(OMNETPP_BIN_DIR) && rm -f eventlogtool$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/event.o: event.cc \
  ../common/filereader.h \
  event.h \
  eventlog.h \
  eventlogentry.h \
  ../common/commondefs.h \
  ievent.h \
  eventlogentries.h \
  messagedependency.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ieventlog.h \
  eventlogindex.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h
$O/eventlog.o: eventlog.cc \
  ../common/filereader.h \
  ../common/stringpool.h \
  eventlog.h \
  ../common/commondefs.h \
  event.h \
  ieventlog.h \
  eventlogindex.h \
  ievent.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h
$O/eventlogentries.o: eventlogentries.cc \
  event.h \
  eventlogentries.h \
  ../common/stringutil.h \
  ievent.h \
  ../common/filereader.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h
$O/eventlogentry.o: eventlogentry.cc \
  eventlog.h \
  eventlogentry.h \
  eventlogentryfactory.h \
  ../common/stringpool.h \
  ../common/filereader.h \
  event.h \
  ieventlog.h \
  eventlogindex.h \
  ../common/commondefs.h \
  ievent.h \
  eventlogentries.h \
  messagedependency.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h
$O/eventlogentryfactory.o: eventlogentryfactory.cc \
  event.h \
  eventlogentryfactory.h \
  ievent.h \
  ../common/filereader.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h
$O/eventlogfacade.o: eventlogfacade.cc \
  ievent.h \
  ieventlog.h \
  event.h \
  eventlog.h \
  filteredevent.h \
  filteredeventlog.h \
  eventlogfacade.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h \
  ../common/stringpool.h \
  eventlogindex.h \
  ../common/patternmatcher.h
$O/eventlogindex.o: eventlogindex.cc \
  eventlogindex.h \
  ../common/exception.h \
  eventlogdefs.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h
$O/eventlogtablefacade.o: eventlogtablefacade.cc \
  ievent.h \
  ieventlog.h \
  event.h \
  eventlogtablefacade.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h \
  eventlogfacade.h
$O/eventlogtool.o: eventlogtool.cc \
  ../utils/ver.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  eventlogindex.h \
  eventlog.h \
  filteredeventlog.h \
  ../common/commondefs.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  event.h \
  ieventlog.h \
  ievent.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  filteredevent.h
$O/filteredevent.o: filteredevent.cc \
  event.h \
  filteredevent.h \
  filteredeventlog.h \
  ievent.h \
  ../common/filereader.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/patternmatcher.h \
  ieventlog.h \
  eventlog.h \
  ../common/stringpool.h \
  eventlogindex.h
$O/filteredeventlog.o: filteredeventlog.cc \
  filteredeventlog.h \
  ../common/patternmatcher.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ieventlog.h \
  eventlog.h \
  filteredevent.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ievent.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/linetokenizer.h \
  eventlogentries.h \
  ../common/stringpool.h \
  event.h \
  eventlogindex.h
$O/ievent.o: ievent.cc \
  ievent.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h
$O/ieventlog.o: ieventlog.cc \
  ieventlog.h \
  ../common/filereader.h \
  eventlogdefs.h \
  ievent.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h \
  eventlogentries.h
$O/messagedependency.o: messagedependency.cc \
  eventlogdefs.h \
  ievent.h \
  ieventlog.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/filereader.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h \
  eventlogentries.h
$O/sequencechartfacade.o: sequencechartfacade.cc \
  ../common/lcgrandom.h \
  ievent.h \
  ieventlog.h \
  event.h \
  messagedependency.h \
  sequencechartfacade.h \
  ../common/commondefs.h \
  ../common/exception.h \
  eventlogentry.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h \
  eventlogfacade.h
