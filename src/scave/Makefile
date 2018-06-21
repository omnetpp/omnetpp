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
TARGET_EXE_FILES=$(OMNETPP_BIN_DIR)/scavetool$(EXE_SUFFIX) \
                 $(OMNETPP_BIN_DIR)/opp_scavetool

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/scave

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(XML_CFLAGS) $(INCL_FLAGS)
IMPLIBS= -loppcommon$D

ifeq ("$(BUILDING_UILIBS)","yes")
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

OBJS= $O/arraybuilder.o $O/channel.o $O/commonnodes.o $O/compoundfilter.o \
      $O/dataflowmanager.o $O/datasorter.o $O/diffquot.o \
      $O/filewriter.o $O/filternodes.o $O/customfilter.o $O/stddev.o \
      $O/idlist.o $O/mergernodes.o $O/nodetype.o $O/nodetyperegistry.o \
      $O/omnetppresultfileloader.o $O/sqliteresultfileloader.o \
      $O/resultfilemanager.o $O/slidingwinavg.o \
      $O/vectorfilereader.o $O/vectorfilewriter.o $O/windowavg.o \
      $O/xyplotnode.o $O/indexedvectorfile.o \
      $O/vectorfileindexer.o $O/indexfile.o $O/scaveutils.o \
      $O/scaveexception.o $O/enumtype.o $O/teenode.o \
      $O/indexedvectorfilereader.o $O/xyarray.o $O/fields.o \
      $O/sqlitevectorreader.o $O/vectorreaderbyfiletype.o \
      $O/sqliteresultfileutils.o \
      $O/datatable.o $O/exporter.o $O/exportutils.o \
      $O/csvrecexporter.o $O/csvspreadexporter.o $O/jsonexporter.o \
      $O/omnetppscalarfileexporter.o $O/sqlitescalarfileexporter.o \
      $O/omnetppvectorfileexporter.o $O/sqlitevectorfileexporter.o

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DSCAVE_EXPORT

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

$O/scavetool$(EXE_SUFFIX) : scavetool.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) scavetool.cc -o $@ $(LDFLAGS) -loppscave$D $(IMPLIBS)

$O/opp_scavetool:
	@mkdir -p $O
	$(Q)cp opp_scavetool $O && chmod +x $O/opp_scavetool

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

-include $(OBJS:%.o=%.d)
