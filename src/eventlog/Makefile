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

GENERATED_SOURCES= eventlogentries.cc eventlogentries.h eventlogfactory.cc

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
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

$O/eventlogtool$(EXE_SUFFIX) : $O/eventlogtool.o  $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o $O/eventlogtool$(EXE_SUFFIX) $O/eventlogtool.o -L$L $(LIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog eventlogtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

depend: $(GENERATED_SOURCES)
	$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

clean:
	rm -f $(GENERATED_SOURCES)
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	cd $(OMNETPP_BIN_DIR) && rm -f eventlogtool$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/event.o: event.cc \
  messagedependency.h \
  eventlogentry.h \
  ievent.h \
  ../common/commondefs.h \
  eventlog.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  eventlogentries.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ../common/filereader.h \
  ieventlog.h
$O/eventlog.o: eventlog.cc \
  messagedependency.h \
  eventlogentry.h \
  ievent.h \
  ../common/commondefs.h \
  eventlog.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  eventlogentries.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ieventlog.h \
  ../common/filereader.h
$O/eventlogentries.o: eventlogentries.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  ../common/commondefs.h \
  ../common/stringutil.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  eventlogentries.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/eventlogentry.o: eventlogentry.cc \
  messagedependency.h \
  eventlogentry.h \
  ievent.h \
  ../common/commondefs.h \
  eventlog.h \
  eventlogentryfactory.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  eventlogentries.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ../common/filereader.h \
  ieventlog.h
$O/eventlogentryfactory.o: eventlogentryfactory.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  ../common/commondefs.h \
  eventlogentryfactory.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/eventlogfacade.o: eventlogfacade.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  filteredeventlog.h \
  filteredevent.h \
  ../common/commondefs.h \
  eventlog.h \
  ../common/patternmatcher.h \
  eventlogindex.h \
  eventlogfacade.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ../common/filereader.h \
  ieventlog.h
$O/eventlogindex.o: eventlogindex.cc \
  eventlogentry.h \
  ../common/commondefs.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/eventlogtablefacade.o: eventlogtablefacade.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  ../common/commondefs.h \
  eventlogfacade.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogtablefacade.h \
  ieventlog.h
$O/eventlogtool.o: eventlogtool.cc \
  messagedependency.h \
  eventlogentry.h \
  ievent.h \
  filteredeventlog.h \
  filteredevent.h \
  ../common/commondefs.h \
  eventlog.h \
  ../common/patternmatcher.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/ver.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ieventlog.h \
  ../common/filereader.h
$O/filteredevent.o: filteredevent.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  filteredeventlog.h \
  filteredevent.h \
  ../common/commondefs.h \
  eventlog.h \
  ../common/patternmatcher.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ieventlog.h \
  ../common/filereader.h
$O/filteredeventlog.o: filteredeventlog.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  filteredeventlog.h \
  filteredevent.h \
  ../common/commondefs.h \
  eventlog.h \
  ../common/patternmatcher.h \
  eventlogindex.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ../common/filereader.h \
  ieventlog.h
$O/ievent.o: ievent.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/ieventlog.o: ieventlog.cc \
  messagedependency.h \
  eventlogentry.h \
  ievent.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  eventlogentries.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ieventlog.h
$O/messagedependency.o: messagedependency.cc \
  messagedependency.h \
  eventlogentry.h \
  ievent.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ieventlog.h
$O/sequencechartfacade.o: sequencechartfacade.cc \
  eventlogentry.h \
  messagedependency.h \
  ievent.h \
  ../common/commondefs.h \
  sequencechartfacade.h \
  eventlogfacade.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  event.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ieventlog.h \
  ../common/lcgrandom.h
