#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppcommon$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/common
L=$(OMNETPP_LIB_DIR)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=-Wno-unused-function $(CFLAGS) $(INCL_FLAGS)
IMPLIBS=

OBJS= $O/lcgrandom.o $O/filereader.o $O/linetokenizer.o \
      $O/stringpool.o $O/stringtokenizer.o $O/fnamelisttokenizer.o \
      $O/expression.o $O/lex.expressionyy.o $O/expression.tab.o \
      $O/matchexpression.o $O/matchexpressionlexer.o $O/matchexpression.tab.o \
      $O/patternmatcher.o $O/unitconversion.o $O/displaystring.o $O/fileglobber.o \
      $O/fileutil.o $O/stringutil.o $O/commonutil.o $O/exception.o $O/bigdecimal.o \
      $O/enumstr.o $O/stringtokenizer2.o $O/colorutil.o $O/sqlite3.o

GENERATED_SOURCES= expression.tab.hh expression.tab.cc lex.expressionyy.cc \
                   matchexpression.tab.hh matchexpression.tab.cc

ifeq ("$(BUILDING_UILIBS)","yes")
OBJS+= $O/rwlock.o
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DCOMMON_EXPORT

.PHONY: all clean build install
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
	-$(Q)cp $O/$(LIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(SO_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)$(SHLIB_POSTPROCESS) $L/$(LIBNAME)$(SO_LIB_SUFFIX) 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR) 2>/dev/null || :

build : $O/$(LIBNAME)$(LIB_SUFFIX)

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

expression.tab.hh expression.tab.cc : expression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o expression.tab.cc -p expressionyy -d expression.y

lex.expressionyy.cc: expression.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.expressionyy.cc -Pexpressionyy expression.lex

matchexpression.tab.hh matchexpression.tab.cc : matchexpression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o matchexpression.tab.cc --no-lines -p matchexpressionyy -d matchexpression.y

$O/sqlite3.o: sqlite3.c
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CC) -c $(COPTS) -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_DEFAULT_FOREIGN_KEYS=1  -o $@ $<

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_BIN_DIR)/$(LIBNAME).*

-include $(OBJS:%.o=%.d)
