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
LIBNAME=liboppeventlog$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/eventlog
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR)/platdep -I. -I../common

IMPLIBS= -L$L -loppcommon$D

OBJS= $O/ievent.o $O/ieventlog.o $O/eventlogfacade.o $O/eventlogtablefacade.o $O/sequencechartfacade.o \
      $O/eventlog.o $O/eventlogindex.o $O/messagedependency.o $O/event.o $O/eventlogentry.o \
      $O/eventlogentries.o $O/filteredevent.o $O/filteredeventlog.o $O/eventlogentryfactory.o
LIBS= -loppeventlog$D -loppcommon$D

GENERATED_SOURCES= eventlogentries.cc eventlogentries.h eventlogentryfactory.cc

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
all: eventlogentries.cc $L/$(LIBNAME)$(LIB_SUFFIX) $O/eventlogtool$(EXE_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$(GENERATED_SOURCES) : eventlogentries.txt eventlogentries.pl
	$(PERL) eventlogentries.pl

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
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

$O/eventlogtool$(EXE_SUFFIX) : $O/eventlogtool.o  $L/$(LIBNAME)$(LIB_SUFFIX)
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/eventlogtool$(EXE_SUFFIX) $O/eventlogtool.o -L$L $(LIBS)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-prog eventlogtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

depend: $(GENERATED_SOURCES)
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f eventlogtool$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/event.o: event.cc \
  eventlogindex.h \
  eventlog.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/stringpool.h \
  ../common/matchexpression.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  eventlogentries.h \
  ieventlog.h \
  ../common/filereader.h \
  event.h \
  eventlogdefs.h \
  ievent.h \
  messagedependency.h
$O/eventlog.o: eventlog.cc \
  eventlogindex.h \
  eventlog.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/stringpool.h \
  ../common/matchexpression.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ieventlog.h \
  eventlogentries.h \
  ../common/filereader.h \
  event.h \
  eventlogdefs.h \
  ievent.h \
  messagedependency.h
$O/eventlogentries.o: eventlogentries.cc \
  eventlogentries.h \
  ../common/filereader.h \
  event.h \
  ievent.h \
  messagedependency.h \
  eventlogdefs.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/matchexpression.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/stringutil.h \
  ../common/linetokenizer.h
$O/eventlogentry.o: eventlogentry.cc \
  eventlogentries.h \
  ieventlog.h \
  ../common/filereader.h \
  event.h \
  messagedependency.h \
  ievent.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  eventlogentryfactory.h \
  ../common/commondefs.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/stringpool.h \
  ../common/matchexpression.h \
  eventlogindex.h \
  eventlog.h \
  ../common/linetokenizer.h
$O/eventlogentryfactory.o: eventlogentryfactory.cc \
  eventlogentry.h \
  ../common/bigdecimal.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h \
  ../common/filereader.h \
  eventlogentries.h \
  ievent.h \
  eventlogdefs.h \
  messagedependency.h \
  event.h \
  ../common/exception.h \
  eventlogentryfactory.h \
  ../common/commonutil.h \
  ../common/commondefs.h
$O/eventlogfacade.o: eventlogfacade.cc \
  ../common/linetokenizer.h \
  eventlogindex.h \
  eventlog.h \
  ../common/matchexpression.h \
  ../common/stringpool.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  filteredevent.h \
  ../common/exception.h \
  filteredeventlog.h \
  eventlogfacade.h \
  event.h \
  messagedependency.h \
  eventlogdefs.h \
  ievent.h \
  eventlogentries.h \
  ../common/patternmatcher.h \
  ieventlog.h \
  ../common/filereader.h
$O/eventlogindex.o: eventlogindex.cc \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/filereader.h \
  eventlogdefs.h \
  eventlogindex.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/bigdecimal.h \
  eventlogentry.h
$O/eventlogtablefacade.o: eventlogtablefacade.cc \
  ../common/exception.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  event.h \
  eventlogfacade.h \
  eventlogtablefacade.h \
  eventlogdefs.h \
  ievent.h \
  messagedependency.h \
  eventlogentries.h \
  ieventlog.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/matchexpression.h
$O/eventlogtool.o: eventlogtool.cc \
  ../common/matchexpression.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/ver.h \
  ../common/stringpool.h \
  eventlogindex.h \
  eventlog.h \
  ../common/linetokenizer.h \
  filteredeventlog.h \
  ../common/patternmatcher.h \
  ieventlog.h \
  eventlogentries.h \
  ../common/filereader.h \
  event.h \
  ievent.h \
  messagedependency.h \
  eventlogdefs.h \
  filteredevent.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ../common/exception.h
$O/filteredevent.o: filteredevent.cc \
  eventlogentry.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ../common/matchexpression.h \
  eventlog.h \
  eventlogindex.h \
  ../common/linetokenizer.h \
  ../common/filereader.h \
  ../common/patternmatcher.h \
  eventlogentries.h \
  ieventlog.h \
  messagedependency.h \
  ievent.h \
  eventlogdefs.h \
  event.h \
  filteredeventlog.h \
  ../common/exception.h \
  filteredevent.h \
  ../common/commonutil.h \
  ../common/commondefs.h
$O/filteredeventlog.o: filteredeventlog.cc \
  ../common/matchexpression.h \
  ../common/stringpool.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/linetokenizer.h \
  eventlogindex.h \
  eventlog.h \
  filteredeventlog.h \
  event.h \
  messagedependency.h \
  ievent.h \
  eventlogdefs.h \
  eventlogentries.h \
  ../common/patternmatcher.h \
  ieventlog.h \
  ../common/filereader.h \
  ../common/commondefs.h \
  filteredevent.h \
  ../common/commonutil.h \
  ../common/exception.h
$O/ievent.o: ievent.cc \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h \
  eventlogentries.h \
  ../common/filereader.h \
  eventlogdefs.h \
  ievent.h \
  messagedependency.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h
$O/ieventlog.o: ieventlog.cc \
  ../common/exception.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  eventlogdefs.h \
  ievent.h \
  messagedependency.h \
  eventlogentries.h \
  ieventlog.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  eventlogentry.h \
  ../common/matchexpression.h
$O/messagedependency.o: messagedependency.cc \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ../common/filereader.h \
  eventlogentries.h \
  ieventlog.h \
  eventlogdefs.h \
  messagedependency.h \
  ievent.h \
  event.h \
  ../common/linetokenizer.h \
  eventlogentry.h \
  ../common/bigdecimal.h \
  ../common/matchexpression.h
$O/sequencechartfacade.o: sequencechartfacade.cc \
  ../common/filereader.h \
  eventlogentries.h \
  ieventlog.h \
  messagedependency.h \
  eventlogdefs.h \
  ievent.h \
  eventlogfacade.h \
  event.h \
  ../common/lcgrandom.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  eventlogentry.h \
  ../common/bigdecimal.h \
  sequencechartfacade.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h
