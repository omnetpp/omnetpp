#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppcommon$D
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

O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/common

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=-Wno-unused-function $(CFLAGS) $(LIBXML_CFLAGS) $(INCL_FLAGS)

IMPLIBS= $(LIBXML_LIBS)

OBJS= $O/lcgrandom.o $O/filelock.o $O/filereader.o $O/linetokenizer.o \
      $O/stringpool.o $O/pooledstring.o $O/stringtokenizer.o $O/fnamelisttokenizer.o \
      $O/expression.o $O/lex.expressionyy.o $O/expression.tab.o \
      $O/matchexpression.o $O/matchexpressionlexer.o $O/matchexpression.tab.o \
      $O/patternmatcher.o $O/unitconversion.o $O/displaystring.o $O/fileglobber.o \
      $O/fileutil.o $O/stringutil.o $O/commonutil.o $O/exception.o $O/bigdecimal.o \
      $O/enumstr.o $O/colorutil.o $O/statistics.o $O/sqlite3.o \
      $O/formattedprinter.o $O/csvwriter.o $O/jsonwriter.o $O/sqliteresultfileschema.o \
      $O/sqlitescalarfilewriter.o  $O/sqlitevectorfilewriter.o \
      $O/omnetppscalarfilewriter.o $O/omnetppvectorfilewriter.o \
      $O/exprnode.o $O/exprnodes.o $O/exprvalue.o $O/intutil.o $O/any_ptr.o \
      $O/saxparser_default.o $O/saxparser_libxml.o $O/saxparser_yxml.o $O/yxml.o

GENERATED_SOURCES= expression.tab.hh expression.tab.cc lex.expressionyy.cc \
                   matchexpression.tab.hh matchexpression.tab.cc

ifeq ("$(BUILDING_UILIBS)","yes")
OBJS+= $O/rwlock.o
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

# macro is used in $(EXPORT_DEFINES) with clang-msabi when building a shared lib
EXPORT_MACRO = -DCOMMON_EXPORT
ifeq ($(PLATFORM),win32)
    EXPORT_MACRO+= "-DSQLITE_API=__declspec(dllexport)"
else
    EXPORT_MACRO+= "-DSQLITE_API=__attribute__ ((visibility (\"default\")))"
endif

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
all : $(TARGET_LIB_FILES)

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

$O/sqlite3.o: sqlite3.c
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CC) -c $(COPTS) -Wno-deprecated-declarations $(EXPORT_DEFINES) -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -o $@ $<

$O/yxml.o: yxml.c
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CC) -c -I. $(COPTS) $(EXPORT_DEFINES) -o $@ $<

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning common
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES)

# generated source files
# NOTE: This is a trick: creating a muti-target pattern rule that prevents executing this target multiple times when running make in parallel
expressio%.tab.hh expressio%.tab.cc : expression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o expression.tab.cc -p expressionyy -d expression.y

lex.expressionyy.cc: expression.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.expressionyy.cc -Pexpressionyy expression.lex

matchexpressio%.tab.hh matchexpressio%.tab.cc : matchexpression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o matchexpression.tab.cc --no-lines -p matchexpressionyy -d matchexpression.y

-include $(OBJS:%=%.d)

