#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=scave_bindings$D
O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/scave/python
SCAVE_PYTHON_DIR=$(OMNETPP_PYTHON_DIR)/omnetpp/scave
TARGET_LIB_FILES=$(SCAVE_PYTHON_DIR)/$(LIBNAME)$(PYTHON_EXT_SUFFIX)

CFLAGS += -DOMNETPP_MODE_SUFFIX=$D -DNB_SHARED -DNB_BUILD $(PYTHON_BINDINGS_CFLAGS)
LDFLAGS += -static-libstdc++ $(PYTHON_BINDINGS_LDFLAGS)

NANOBIND_DIR=3rdparty/nanobind
NBO=$(O)/3rdparty/nanobind
NANOBIND_OBJS=$(NBO)/common.o $(NBO)/implicit.o $(NBO)/nb_func.o $(NBO)/nb_ndarray.o \
    $(NBO)/nb_static_property.o $(NBO)/trampoline.o $(NBO)/error.o $(NBO)/nb_enum.o \
    $(NBO)/nb_internals.o $(NBO)/nb_type.o


INCL_FLAGS=-I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)" \
    -I"$(NANOBIND_DIR)/ext/robin_map/include" -I"$(NANOBIND_DIR)/include"

COPTS=$(CFLAGS) $(LIBXML_CFLAGS) $(INCL_FLAGS)

# Do not link with the omnetpp libs dynamically, instead use the generated object files directly
# because we want to minimize the dependencies on shared libs so re-distribution of the python module
# will be simpler.
IMPLIBS= $(LIBXML_LIBS)
# Also disable import stub generation (by not passing -DOMNETPPLIBS_IMPORT) avoiding LNK4217 warnings. 
# On Windows, we also explicitly disable LNK4217 warnings for the linker, because references to symbols
# in common lib have already an import stub in scave object files that were generated when the scave
# shared library was built. The presence of import stubs just prevent a bit of optimization
# that we are willing to trade to avoid building the scave object files twice.
ifeq ($(PLATFORM),win32)
  IMPORT_DEFINES=
  LDFLAGS += -Wl,/ignore:4217
endif

# We repeat the object file list of the common and scave library here, which is redundant.
# It would be possible to build both shared and static versions from all OMNeT++ libraries
# at the same time and then use the static version of the library here instead of the
# explicit object file list.
C=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/common
COMMON_OBJS= $C/lcgrandom.o $C/filelock.o $C/filereader.o $C/linetokenizer.o \
      $C/stringpool.o $C/pooledstring.o $C/stringtokenizer.o $C/fnamelisttokenizer.o \
      $C/expression.o $C/expression.lex.o $C/expression.tab.o $C/quantityformatter.o \
      $C/matchexpression.o $C/matchexpressionlexer.o $C/matchexpression.tab.o \
      $C/patternmatcher.o $C/unitconversion.o $C/fileglobber.o \
      $C/fileutil.o $C/stringutil.o $C/commonutil.o $C/exception.o $C/bigdecimal.o \
      $C/enumstr.o $C/colorutil.o $C/statistics.o $C/sqlite3.o \
      $C/formattedprinter.o $C/csvwriter.o $C/jsonwriter.o $C/sqliteresultfileschema.o \
      $C/sqlitescalarfilewriter.o  $C/sqlitevectorfilewriter.o \
      $C/omnetppscalarfilewriter.o $C/omnetppvectorfilewriter.o \
      $C/exprnode.o $C/exprnodes.o $C/exprvalue.o $C/intutil.o $C/any_ptr.o \
      $C/saxparser_default.o $C/saxparser_libxml.o $C/saxparser_yxml.o $C/yxml.o

S=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/scave
SCAVE_OBJS= $S/idlist.o \
      $S/omnetppresultfileloader.o $S/sqliteresultfileloader.o \
      $S/resultfilemanager.o $S/resultitems.o $S/indexedvectorfilereader.o \
      $S/vectorfileindexer.o $S/vectorfileindex.o $S/indexfileutils.o \
      $S/indexfilereader.o  $S/indexfilewriter.o $S/filefingerprint.o \
      $S/scaveutils.o $S/scaveexception.o $S/enumtype.o \
      $S/xyarray.o $S/fields.o $S/vectorutils.o $S/memoryutils.o $S/sqliteresultfileutils.o \
      $S/sqlitevectordatareader.o $S/exporter.o $S/exportutils.o \
      $S/csvrecexporter.o $S/csvspreadexporter.o $S/jsonexporter.o \
      $S/omnetppscalarfileexporter.o $S/sqlitescalarfileexporter.o \
      $S/omnetppvectorfileexporter.o $S/sqlitevectorfileexporter.o

OBJS=$O/$(LIBNAME).o

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE=$O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

.PHONY: all clean
.SECONDARY: $(OBJS) $(NANOBIND_OBJS) $O/$(LIBNAME)$(PYTHON_EXT_SUFFIX)

# disabling all implicit rules
.SUFFIXES:

#
# Targets
#
all: $(TARGET_LIB_FILES)

#
# Automatic rules
#

$O/%$D.o: %.cc $(CONFIGFILE) $(COPTS_FILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

$(NBO)/%.o: $(NANOBIND_DIR)/src/%.cpp $(CONFIGFILE) $(COPTS_FILE)
	@mkdir -p $(NBO)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

# dynamically linked library (on all platforms)
$O/%$(PYTHON_EXT_SUFFIX) : $(OBJS) $(NANOBIND_OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(PYTHON_EXT_SUFFIX) \
	    $(OBJS) $(NANOBIND_OBJS) $(SCAVE_OBJS) $(COMMON_OBJS) $(IMPLIBS) $(PYTHON_LDFLAGS)

# copy the module to the python directory from the out directory
$(SCAVE_PYTHON_DIR)/%: $O/% $(CONFIGFILE)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@
	$(Q)cp $(SCAVE_PYTHON_DIR)/scave_bindings.pyi.dist $(SCAVE_PYTHON_DIR)/scave_bindings$D.pyi

clean:
	$(qecho) Cleaning scave/python
	$(Q)rm -rf $O $(NBO) $(SCAVE_PYTHON_DIR)/$(LIBNAME)*$(SHARED_LIB_SUFFIX) $(SCAVE_PYTHON_DIR)/*.pyi

-include $(OBJS:%=%.d)
-include $(NANOBIND_OBJS:%=%.d)
