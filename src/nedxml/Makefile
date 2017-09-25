#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppnedxml$D
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
TARGET_EXE_FILES=$(OMNETPP_BIN_DIR)/nedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)/opp_msgc

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/nedxml

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CPPFLAGS) $(CFLAGS) $(XML_CFLAGS) $(INCL_FLAGS)

IMPLIBS= -loppcommon$D $(XML_LIBS)

OBJS= $O/nedelements.o $O/nedelement.o $O/nederror.o $O/nedexception.o \
      $O/nedvalidator.o $O/neddtdvalidator.o $O/neddtdvalidatorbase.o \
      $O/nedsyntaxvalidator.o $O/nedcrossvalidator.o \
      $O/nedfilebuffer.o $O/nedparser.o $O/nedyylib.o \
      $O/ned2.tab.o $O/lex.ned2yy.o $O/nedtools.o \
      $O/nedutil.o $O/ned2generator.o $O/ned1generator.o $O/xmlgenerator.o \
      $O/nedxmlparser.o $O/nedsaxhandler.o $O/saxparser_$(XMLPARSER).o \
      $O/ned1.tab.o $O/lex.ned1yy.o \
      $O/msg2.tab.o $O/lex.msg2yy.o \
      $O/msgcppgenerator.o \
      $O/nedresourcecache.o $O/nedtypeinfo.o

GENERATED_SOURCES=nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h \
                  neddtdvalidator.h neddtdvalidator.cc \
                  ned1.tab.hh ned1.tab.cc lex.ned1yy.cc \
                  ned2.tab.hh ned2.tab.cc lex.ned2yy.cc \
                  msg2.tab.hh msg2.tab.cc lex.msg2yy.cc

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DNEDXML_EXPORT

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

$O/nedtool$(EXE_SUFFIX): nedtool.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) nedtool.cc -o $@ $(LDFLAGS) -loppnedxml$D $(IMPLIBS)

$O/opp_msgc:
	@mkdir -p $O
	$(Q)cp opp_msgc $O && chmod +x $O/opp_msgc

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning nedxml
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES) $(TARGET_EXE_FILES)

# generated source files
# NOTE: This is a trick: creating a muti-target pattern rule that prevents executing this target multiple times when running make in parallel
%edelements.cc %edelements.h %eddtdvalidator.cc %eddtdvalidator.h %edvalidator.cc %edvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/ned2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/ned2.dtd

%ed1.tab.hh %ed1.tab.cc : ned1.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned1.tab.cc -p ned1yy -d ned1.y

%ed2.tab.hh %ed2.tab.cc : ned2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned2.tab.cc -p ned2yy -d ned2.y

%sg2.tab.hh %sg2.tab.cc : msg2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o msg2.tab.cc -p msg2yy -d msg2.y

lex.ned1yy.cc: ned1.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned1yy.cc -Pned1yy ned1.lex

lex.ned2yy.cc: ned2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned2yy.cc -Pned2yy ned2.lex

lex.msg2yy.cc: msg2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.msg2yy.cc -Pmsg2yy msg2.lex

-include $(OBJS:%.o=%.d)
