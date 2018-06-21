#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppeventlog$D
TARGET_LIB_FILES=$(OMNETPP_LIB_DIR)/$(LIBNAME)$(LIB_SUFFIX)
# On Windows, the targets are different (DLLs go into the bin dir and separate export libs are created)
ifeq ($(PLATFORM),win32.x86_64)
  ifeq ($(SHARED_LIBS),yes)
    TARGET_LIB_FILES=$(OMNETPP_BIN_DIR)/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(A_LIB_SUFFIX)
    ifeq ($(TOOLCHAIN_NAME),clangc2)
      ifeq ($(MODE),debug)
        TARGET_LIB_FILES+=$(OMNETPP_BIN_DIR)/$(LIBNAME).pdb
      endif
    endif
  endif
endif
TARGET_EXE_FILES=$(OMNETPP_BIN_DIR)/eventlogtool$(EXE_SUFFIX) \
                 $(OMNETPP_BIN_DIR)/opp_eventlogtool

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/eventlog
INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS)

IMPLIBS= -loppcommon$D

OBJS= $O/ievent.o $O/ieventlog.o $O/eventlogfacade.o $O/eventlogtablefacade.o $O/sequencechartfacade.o \
      $O/eventlog.o $O/eventlogindex.o $O/messagedependency.o $O/event.o $O/eventlogentry.o \
      $O/eventlogentries.o $O/filteredevent.o $O/filteredeventlog.o $O/eventlogentryfactory.o

GENERATED_SOURCES= eventlogentries.csv eventlogentries.h eventlogentries.cc eventlogentryfactory.cc

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DEVENTLOG_EXPORT

.PHONY: all clean
.SECONDARY: $(OBJS) $O/$(LIBNAME)$(LIB_SUFFIX) $(GENERATED_SOURCES)

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc $(GENERATED_SOURCES) $(CONFIGFILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

#
# Targets
#
all : $(TARGET_LIB_FILES) $(TARGET_EXE_FILES)

ifeq ($(SHARED_LIBS),yes)
# dynamically linked library (on all platforms except Windows)
$O/%$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

# dynamically linked library + import lib (on Windows)
$O/%$(DLL_LIB_SUFFIX) $O/%$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating DLL and import library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)
else
# statically linked library (on all platforms)
$O/%$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
endif

$O/eventlogtool$(EXE_SUFFIX) : eventlogtool.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) eventlogtool.cc -o $@ $(LDFLAGS) $(IMPLIBS) -loppeventlog$D

$O/opp_eventlogtool:
	@mkdir -p $O
	$(Q)cp opp_eventlogtool $O && chmod +x $O/opp_eventlogtool

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning eventlog
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES) $(TARGET_EXE_FILES)

# generated source files
# NOTE: This is a trick: creating a muti-target pattern rule that prevents executing this target multiple times when running make in parallel
%ventlogentries.h %ventlogentries.cc %ventlogentries.csv %ventlogentryfactory.cc : eventlogentries.txt eventlogentries.pl $(CONFIGFILE)
	@echo Generating $(GENERATED_SOURCES) from $<
	$(Q)$(PERL) eventlogentries.pl

-include $(OBJS:%.o=%.d)
