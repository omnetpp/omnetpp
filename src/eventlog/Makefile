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

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS)

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
	$(Q)$(MAKEDEPEND) -I../../include -I.. -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f eventlogtool$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/event.o: event.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/stringpool.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogindex.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/eventlog.o: eventlog.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/stringpool.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogindex.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/eventlogentries.o: eventlogentries.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/stringutil.h \
  enums.h \
  event.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  ievent.h \
  messagedependency.h
$O/eventlogentry.o: eventlogentry.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/stringpool.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogentryfactory.h \
  eventlogindex.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/eventlogentryfactory.o: eventlogentryfactory.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  event.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogentryfactory.h \
  ievent.h \
  messagedependency.h
$O/eventlogfacade.o: eventlogfacade.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringpool.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogfacade.h \
  eventlogindex.h \
  filteredevent.h \
  filteredeventlog.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/eventlogindex.o: eventlogindex.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  eventlogdefs.h \
  eventlogentry.h \
  eventlogindex.h
$O/eventlogtablefacade.o: eventlogtablefacade.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  event.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogfacade.h \
  eventlogtablefacade.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/eventlogtool.o: eventlogtool.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringpool.h \
  ../common/ver.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogindex.h \
  filteredevent.h \
  filteredeventlog.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/filteredevent.o: filteredevent.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringpool.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogindex.h \
  filteredevent.h \
  filteredeventlog.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/filteredeventlog.o: filteredeventlog.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/stringpool.h \
  enums.h \
  event.h \
  eventlog.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogindex.h \
  filteredevent.h \
  filteredeventlog.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/ievent.o: ievent.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  ievent.h \
  messagedependency.h
$O/ieventlog.o: ieventlog.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/messagedependency.o: messagedependency.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  event.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h
$O/sequencechartfacade.o: sequencechartfacade.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/lcgrandom.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  enums.h \
  event.h \
  eventlogdefs.h \
  eventlogentries.h \
  eventlogentry.h \
  eventlogfacade.h \
  ievent.h \
  ieventlog.h \
  messagedependency.h \
  sequencechartfacade.h
