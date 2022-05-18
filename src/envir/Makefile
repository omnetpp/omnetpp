
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
MAINLIBNAME=$(LIB_PREFIX)oppmain$D
LIBNAME=$(LIB_PREFIX)oppenvir$D

TARGET_LIB_FILES=$(OMNETPP_LIB_DIR)/$(LIBNAME)$(LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(MAINLIBNAME)$(A_LIB_SUFFIX)
# On Windows, the targets are different (DLLs go into the bin dir and separate export libs are created)
ifeq ($(PLATFORM),win32)
  ifeq ($(SHARED_LIBS),yes)
    TARGET_LIB_FILES=$(OMNETPP_BIN_DIR)/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(IMPLIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(MAINLIBNAME)$(A_LIB_SUFFIX)
    ifeq ($(TOOLCHAIN_NAME),clang-msabi)
      ifeq ($(MODE),debug)
        TARGET_LIB_FILES+=$(OMNETPP_BIN_DIR)/$(LIBNAME).pdb
      endif
    endif
  endif
endif

# build opp_run, opp_run_dbg and opp_run_release depending on MODE (or nothing if we are building static libs)
ifeq ($(SHARED_LIBS),yes)
TARGET_EXE_FILES=$(OMNETPP_BIN_DIR)/opp_run$(D)$(EXE_SUFFIX)
  ifeq ($(MODE),release)
    TARGET_EXE_FILES+=$(OMNETPP_BIN_DIR)/opp_run_release$(EXE_SUFFIX)
  endif
endif

O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/envir

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

ifeq ($(PLATFORM),win32)
  PSAPI_LIB=-lpsapi
endif

ifeq ($(PREFER_SQLITE_RESULT_FILES),yes)
  DEFINES += -DPREFER_SQLITE_RESULT_FILES
endif

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) $(INCL_FLAGS) -DHAVE_DLOPEN=$(HAVE_DLOPEN) -DSHARED_LIB_SUFFIX="\"$(SHARED_LIB_SUFFIX)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\"" -DLIBSUFFIX="\"$D\""

IMPLIBS= -loppsim$D -loppnedxml$D -loppcommon$D

OBJS= $O/appreg.o $O/args.o $O/startup.o $O/evmain.o $O/logformatter.o $O/envirbase.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/filesnapshotmgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/stopwatch.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/xmldoccache.o $O/eventlogwriter.o $O/objectprinter.o \
      $O/eventlogfilemgr.o $O/resultfileutils.o $O/intervals.o \
      $O/omnetppoutscalarmgr.o $O/omnetppoutvectormgr.o \
      $O/sqliteoutscalarmgr.o $O/sqliteoutvectormgr.o \
      $O/visitor.o $O/envirutils.o

GENERATED_SOURCES= eventlogwriter.cc eventlogwriter.h

# macro is used in $(EXPORT_DEFINES) with clang-msabi when building a shared lib
EXPORT_MACRO = -DENVIR_EXPORT

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

.PHONY: all clean opp_run_executable
.SECONDARY: $(OBJS) $O/$(LIBNAME)$(LIB_SUFFIX) $(GENERATED_SOURCES)

# disabling all implicit rules
.SUFFIXES :

#
# Automatic rules
#

$O/%.o: %.cc $(GENERATED_SOURCES) $(CONFIGFILE) $(COPTS_FILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

#
# Targets
#
all : $(TARGET_LIB_FILES)

opp_run_executable: $(TARGET_EXE_FILES)

ifeq ($(SHARED_LIBS),yes)
# dynamically linked library (on all platforms except Windows)
$O/%$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

# dynamically linked library + import lib (on Windows)
$O/%$(DLL_LIB_SUFFIX) $O/%$(IMPLIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating DLL and import library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(IMPLIB_SUFFIX)
else
# statically linked library (on all platforms)
$O/%$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR_CR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
endif

# Build a static lib that contains the main() function used with executable or dynamically linked models
$O/$(MAINLIBNAME)$(A_LIB_SUFFIX) : $O/main.o
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR_CR) $(ARFLAG_OUT)$O/$(MAINLIBNAME)$(A_LIB_SUFFIX) $O/main.o
	$(Q)$(RANLIB) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX)

# build opp_run executable

$O/opp_run$(D)$(EXE_SUFFIX) : main.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	$(qecho) "Creating executable: $@"
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) $(INCL_FLAGS) main.cc -o "$@" $(LDFLAGS) $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS)

$O/opp_run_release$(EXE_SUFFIX) : $O/opp_run$(EXE_SUFFIX)
	$(Q)cp $< $@

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning envir
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES) $(TARGET_EXE_FILES)

# generated sources
%ventlogwriter.cc %ventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

#$O/envirbase.o: eventlogwriter.h

-include $(OBJS:%=%.d)

