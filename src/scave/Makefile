#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppscave$D
TARGET_LIB_FILES=$(OMNETPP_LIB_DIR)/$(LIBNAME)$(LIB_SUFFIX)
# On Windows, the targets are different (DLLs go into the bin dir and separate export libs are created)
ifeq ($(PLATFORM),win32)
  ifeq ($(SHARED_LIBS),yes)
    TARGET_LIB_FILES=$(OMNETPP_BIN_DIR)/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(IMPLIB_SUFFIX)
    ifeq ($(TOOLCHAIN_NAME),clang-msabi)
      ifeq ($(MODE),debug)
        TARGET_LIB_FILES+=$(OMNETPP_BIN_DIR)/$(LIBNAME).pdb
      endif
    endif
  endif
endif
TARGET_EXE_FILES=$(OMNETPP_BIN_DIR)/opp_scavetool$(EXE_SUFFIX)

O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/scave

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS)
IMPLIBS= -loppcommon$D

ifeq ("$(BUILDING_UILIBS)","yes")
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

OBJS= $O/idlist.o \
      $O/omnetppresultfileloader.o $O/sqliteresultfileloader.o \
      $O/resultfilemanager.o $O/resultitems.o $O/indexedvectorfilereader.o \
      $O/vectorfileindexer.o $O/vectorfileindex.o $O/indexfileutils.o \
      $O/indexfilereader.o  $O/indexfilewriter.o $O/filefingerprint.o \
      $O/scaveutils.o $O/scaveexception.o $O/enumtype.o \
      $O/xyarray.o $O/fields.o $O/vectorutils.o $O/memoryutils.o $O/sqliteresultfileutils.o \
      $O/sqlitevectordatareader.o $O/exporter.o $O/exportutils.o \
      $O/csvrecexporter.o $O/csvspreadexporter.o $O/jsonexporter.o \
      $O/omnetppscalarfileexporter.o $O/sqlitescalarfileexporter.o \
      $O/omnetppvectorfileexporter.o $O/sqlitevectorfileexporter.o

# macro is used in $(EXPORT_DEFINES) with clang-msabi when building a shared lib
EXPORT_MACRO = -DSCAVE_EXPORT

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

.PHONY: all clean
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
all : $(TARGET_LIB_FILES) $(TARGET_EXE_FILES)

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

$O/opp_scavetool$(EXE_SUFFIX) : opp_scavetool.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) opp_scavetool.cc -o $@ $(LDFLAGS) -loppscave$D $(IMPLIBS)

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning scave
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES) $(TARGET_EXE_FILES)

-include $(OBJS:%=%.d)
