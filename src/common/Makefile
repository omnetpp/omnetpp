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

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/common

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=-Wno-unused-function $(CFLAGS) $(INCL_FLAGS)

IMPLIBS=

OBJS= $O/lcgrandom.o $O/filereader.o $O/linetokenizer.o \
      $O/stringpool.o $O/stringtokenizer.o $O/fnamelisttokenizer.o \
      $O/expression.o $O/lex.expressionyy.o $O/expression.tab.o \
      $O/matchexpression.o $O/matchexpressionlexer.o $O/matchexpression.tab.o \
      $O/patternmatcher.o $O/unitconversion.o $O/displaystring.o $O/fileglobber.o \
      $O/fileutil.o $O/stringutil.o $O/commonutil.o $O/exception.o $O/bigdecimal.o \
      $O/enumstr.o $O/stringtokenizer2.o $O/colorutil.o $O/statistics.o $O/sqlite3.o \
      $O/formattedprinter.o $O/csvwriter.o $O/jsonwriter.o $O/sqliteresultfileschema.o \
      $O/sqlitescalarfilewriter.o  $O/sqlitevectorfilewriter.o \
      $O/omnetppscalarfilewriter.o $O/omnetppvectorfilewriter.o

GENERATED_SOURCES= expression.tab.hh expression.tab.cc lex.expressionyy.cc \
                   matchexpression.tab.hh matchexpression.tab.cc

ifeq ("$(BUILDING_UILIBS)","yes")
OBJS+= $O/rwlock.o
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DCOMMON_EXPORT

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
all : $(TARGET_LIB_FILES)

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

$O/sqlite3.o: sqlite3.c
	@mkdir -p $O
	$(qecho) "$<"
ifeq ($(TOOLCHAIN_NAME),clangc2)
# On Windows Clang/C2 compiler we use cl for this C file because Clang/C2 crashes on the file (too big file :)
	$(Q)cl -nologo -c -O2 "-DSQLITE_API=__declspec(dllexport)" -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -Fo: $@ $<
else
	$(Q)$(CC) -c $(COPTS) -Wno-deprecated-declarations -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_DEFAULT_FOREIGN_KEYS=1 -o $@ $<
endif

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

-include $(OBJS:%.o=%.d)
