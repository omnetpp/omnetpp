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

%ventlogentries.cc %ventlogentries.h %ventlogentryfactory.cc : eventlogentries.txt eventlogentries.pl
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
  eventlogdefs.h \
  eventlog.h \
  ../common/stringpool.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  eventlogentry.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  ../common/bigdecimal.h \
  messagedependency.h \
  ievent.h \
  ieventlog.h \
  eventlogindex.h \
  ../common/filereader.h \
  event.h \
  ../common/commondefs.h
$O/eventlog.o: eventlog.cc \
  eventlogentry.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/stringpool.h \
  eventlog.h \
  eventlogdefs.h \
  ../common/filereader.h \
  event.h \
  ../common/commondefs.h \
  eventlogindex.h \
  ieventlog.h \
  ievent.h \
  ../common/bigdecimal.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogentries.h
$O/eventlogentries.o: eventlogentries.cc \
  eventlogdefs.h \
  eventlogentry.h \
  ../common/stringutil.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ievent.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  ../common/commondefs.h \
  event.h \
  ../common/filereader.h
$O/eventlogentry.o: eventlogentry.cc \
  eventlogdefs.h \
  eventlogentryfactory.h \
  eventlog.h \
  eventlogentry.h \
  ../common/stringpool.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ievent.h \
  ieventlog.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/filereader.h \
  event.h \
  eventlogindex.h
$O/eventlogentryfactory.o: eventlogentryfactory.cc \
  ../common/commondefs.h \
  event.h \
  ../common/filereader.h \
  ievent.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  eventlogentry.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  eventlogdefs.h \
  eventlogentryfactory.h
$O/eventlogfacade.o: eventlogfacade.cc \
  eventlogdefs.h \
  eventlog.h \
  filteredeventlog.h \
  ../common/stringpool.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  filteredevent.h \
  eventlogentry.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  ../common/patternmatcher.h \
  ../common/bigdecimal.h \
  messagedependency.h \
  ievent.h \
  ieventlog.h \
  eventlogfacade.h \
  eventlogindex.h \
  ../common/filereader.h \
  event.h \
  ../common/commondefs.h
$O/eventlogindex.o: eventlogindex.cc \
  eventlogindex.h \
  ../common/filereader.h \
  ../common/commondefs.h \
  ../common/bigdecimal.h \
  ../common/matchexpression.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  eventlogentry.h \
  eventlogdefs.h
$O/eventlogtablefacade.o: eventlogtablefacade.cc \
  eventlogentry.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  eventlogdefs.h \
  ../common/commondefs.h \
  event.h \
  ../common/filereader.h \
  eventlogtablefacade.h \
  eventlogfacade.h \
  ieventlog.h \
  ievent.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  eventlogentries.h \
  ../common/matchexpression.h
$O/eventlogtool.o: eventlogtool.cc \
  ../common/commondefs.h \
  event.h \
  ../common/filereader.h \
  eventlogindex.h \
  ../common/ver.h \
  ieventlog.h \
  ievent.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  ../common/patternmatcher.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  eventlogentry.h \
  filteredevent.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/stringpool.h \
  filteredeventlog.h \
  eventlog.h \
  eventlogdefs.h
$O/filteredevent.o: filteredevent.cc \
  ieventlog.h \
  ievent.h \
  ../common/bigdecimal.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  ../common/patternmatcher.h \
  event.h \
  ../common/filereader.h \
  ../common/commondefs.h \
  eventlogindex.h \
  filteredeventlog.h \
  eventlog.h \
  eventlogdefs.h \
  filteredevent.h \
  eventlogentry.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/stringpool.h
$O/filteredeventlog.o: filteredeventlog.cc \
  filteredeventlog.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentry.h \
  filteredevent.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/stringpool.h \
  ieventlog.h \
  ievent.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  ../common/patternmatcher.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  ../common/commondefs.h \
  event.h \
  ../common/filereader.h \
  eventlogindex.h
$O/ievent.o: ievent.cc \
  ../common/commondefs.h \
  ../common/filereader.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  ievent.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  eventlogentry.h \
  eventlogdefs.h
$O/ieventlog.o: ieventlog.cc \
  ../common/filereader.h \
  ../common/commondefs.h \
  ieventlog.h \
  ievent.h \
  ../common/bigdecimal.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  eventlogentry.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  eventlogdefs.h
$O/messagedependency.o: messagedependency.cc \
  event.h \
  ../common/filereader.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogentries.h \
  ../common/bigdecimal.h \
  messagedependency.h \
  ievent.h \
  ieventlog.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  eventlogentry.h \
  eventlogdefs.h
$O/sequencechartfacade.o: sequencechartfacade.cc \
  ievent.h \
  ieventlog.h \
  eventlogentries.h \
  ../common/matchexpression.h \
  ../common/lcgrandom.h \
  messagedependency.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/filereader.h \
  event.h \
  eventlogfacade.h \
  eventlogdefs.h \
  eventlogentry.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  sequencechartfacade.h \
  ../common/commonutil.h
