
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
MAINLIBNAME=$(LIB_PREFIX)oppmain$D
LIBNAME=$(LIB_PREFIX)oppenvir$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/envir
L=$(OMNETPP_LIB_DIR)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

ifeq ($(PLATFORM),win32.x86_64)
  PSAPI_LIB=-lpsapi
endif

ifeq ($(PREFER_SQLITE_RESULT_FILES),yes)
  DEFINES += -DPREFER_SQLITE_RESULT_FILES
endif

COPTS=$(CFLAGS) $(AKAROA_CFLAGS) $(INCL_FLAGS) -DHAVE_DLOPEN=$(HAVE_DLOPEN) -DSHARED_LIB_SUFFIX="\"$(SHARED_LIB_SUFFIX)\"" -DOMNETPP_IMAGE_PATH="\"$(OMNETPP_IMAGE_PATH)\""

IMPLIBS= -loppsim$D -loppnedxml$D -loppcommon$D $(XML_LIBS)

OBJS= $O/appreg.o $O/args.o $O/startup.o $O/evmain.o $O/logformatter.o $O/envirbase.o $O/fsutils.o \
      $O/sectionbasedconfig.o $O/inifilereader.o $O/scenario.o $O/valueiterator.o \
      $O/fileoutvectormgr.o $O/fileoutscalarmgr.o $O/filesnapshotmgr.o \
      $O/indexedfileoutvectormgr.o $O/akoutvectormgr.o \
      $O/speedometer.o $O/matchableobject.o $O/matchablefield.o \
      $O/akaroarng.o $O/cxmldoccache.o $O/eventlogwriter.o $O/objectprinter.o \
      $O/eventlogfilemgr.o $O/runattributes.o $O/intervals.o \
      $O/sqliteoutscalarmgr.o $O/sqliteoutvectormgr.o \
      $O/visitor.o $O/envirutils.o

GENERATED_SOURCES= eventlogwriter.cc eventlogwriter.h

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DENVIR_EXPORT

.PHONY: all clean envir-lib opp_run_executable build install
#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc $(CONFIGFILE) | $(GENERATED_SOURCES)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

#
# Targets
#
all : install

install: build $(CONFIGFILE)
	@mkdir -p $L
	-$(Q)cp $O/$(MAINLIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(SO_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)$(SHLIB_POSTPROCESS) $L/$(LIBNAME)$(SO_LIB_SUFFIX) 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR) 2>/dev/null || :

build : $O/$(LIBNAME)$(LIB_SUFFIX) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX)

# Build a static lib that contains the main() function used with executable or dynamically linked models
$O/$(MAINLIBNAME)$(A_LIB_SUFFIX) : $O/main.o
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(MAINLIBNAME)$(A_LIB_SUFFIX) $O/main.o
	$(Q)$(RANLIB) $O/$(MAINLIBNAME)$(A_LIB_SUFFIX)

$O/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)

$O/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

$O/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)

eventlogwriter.cc eventlogwriter.h : eventlogwriter.pl ../eventlog/eventlogentries.txt
	$(PERL) eventlogwriter.pl

$O/envirbase.o: eventlogwriter.h

# default opp_run target - called only at the end once tkenv and cmdenv is done
# opp_run is useless with static libs
ifeq ("$(LIB_SUFFIX)","$(SHARED_LIB_SUFFIX)")
ifeq ("$(MODE)","debug")
opp_run_executable: $O/opp_run$(EXE_SUFFIX)
$O/opp_run$(EXE_SUFFIX) : install
	$(qecho) "Creating executable: $@"
	$(Q)$(CXX) $(COPTS) $(IMPORT_DEFINES) $(INCL_FLAGS) opp_run.cc -o "$@" $(LDFLAGS) $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS) $(PSAPI_LIB)
	$(Q)cp $O/opp_run$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
else
opp_run_executable: $O/opp_run_release$(EXE_SUFFIX)
$O/opp_run_release$(EXE_SUFFIX) : install
	$(qecho) "Creating executable: $@"
	$(Q)$(CXX) $(COPTS) $(IMPORT_DEFINES) $(INCL_FLAGS) main.cc -o "$@" $(LDFLAGS) $(ALL_ENV_LIBS) $(IMPLIBS) $(SYS_LIBS)
	$(Q)cp $O/opp_run_release$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	$(Q)cp $O/opp_run_release$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)/opp_run$(EXE_SUFFIX)
endif

# TODO remove these
$O/opp_run.o : opp_run.cc
	@# Note: not in OBJS because opp_run.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) $(COPTS) $(IMPORT_DEFINES) -c $(INCL_FLAGS) -o$O/opp_run.o opp_run.cc

$O/main.o : main.cc
	@# Note: not in OBJS because main.cc must be compiled *without* -DENVIR_EXPORT
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) $(COPTS) $(IMPORT_DEFINES) -c $(INCL_FLAGS) -o$O/main.o main.cc

else
opp_run_executable:
	@echo "Note: We don't create opp_run.exe when building static libs"
endif

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_BIN_DIR)/$(LIBNAME).*
	$(Q)rm -f $(OMNETPP_BIN_DIR)/opp_run$(EXE_SUFFIX)
	$(Q)rm -f $(OMNETPP_BIN_DIR)/opp_run_release$(EXE_SUFFIX)

-include $(OBJS:%.o=%.d)
