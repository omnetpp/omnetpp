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
TARGET_EXE_FILES= $(OMNETPP_BIN_DIR)/opp_nedtool$(EXE_SUFFIX) \
                  $(OMNETPP_BIN_DIR)/opp_msgtool$(EXE_SUFFIX) \
                  $(OMNETPP_BIN_DIR)/opp_msgc

O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/nedxml

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(LIBXML_CFLAGS) $(INCL_FLAGS)

IMPLIBS= -loppcommon$D $(LIBXML_LIBS)

OBJS= $O/astnode.o $O/sourcedocument.o $O/errorstore.o $O/exception.o \
      $O/nedelements.o $O/nedvalidator.o $O/neddtdvalidator.o $O/dtdvalidationutils.o \
      $O/msgelements.o $O/msgvalidator.o $O/msgdtdvalidator.o \
      $O/nedsyntaxvalidator.o $O/nedcrossvalidator.o \
      $O/nedparser.o $O/nedyyutil.o $O/msgparser.o $O/msgyyutil.o $O/yyutil.o\
      $O/ned2.tab.o $O/lex.ned2yy.o $O/nedtools.o $O/nedutil.o \
      $O/nedgenerator.o $O/msggenerator.o $O/xmlgenerator.o \
      $O/xmlastparser.o $O/astbuilder.o \
      $O/msg2.tab.o $O/lex.msg2yy.o \
      $O/msgcompiler.o $O/msgtypetable.o $O/msganalyzer.o $O/msgcodegenerator.o \
      $O/sim_std_msg.o $O/nedresourcecache.o $O/nedtypeinfo.o

GENERATED_SOURCES=nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h \
                  neddtdvalidator.h neddtdvalidator.cc \
                  msgelements.cc msgelements.h msgvalidator.cc msgvalidator.h \
                  msgdtdvalidator.h msgdtdvalidator.cc \
                  ned2.tab.hh ned2.tab.cc lex.ned2yy.cc \
                  msg2.tab.hh msg2.tab.cc lex.msg2yy.cc \
                  sim_std_msg.cc

# macro is used in $(EXPORT_DEFINES) with clang-msabi when building a shared lib
EXPORT_MACRO = -DNEDXML_EXPORT

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

$O/opp_nedtool$(EXE_SUFFIX): opp_nedtool.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) opp_nedtool.cc -o $@ $(LDFLAGS) -loppnedxml$D $(IMPLIBS)

$O/opp_msgtool$(EXE_SUFFIX): opp_msgtool.cc $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) opp_msgtool.cc -o $@ $(LDFLAGS) -loppnedxml$D $(IMPLIBS)

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
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/ned2.dtd ned

%sgelements.cc %sgelements.h %sgdtdvalidator.cc %sgdtdvalidator.h %sgvalidator.cc %sgvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/msg2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/msg2.dtd msg

%ed2.tab.hh %ed2.tab.cc : ned2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned2.tab.cc -p ned2yy -d ned2.y

%sg2.tab.hh %sg2.tab.cc : msg2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o msg2.tab.cc -p msg2yy -d msg2.y

lex.ned2yy.cc: ned2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned2yy.cc -Pned2yy ned2.lex

lex.msg2yy.cc: msg2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.msg2yy.cc -Pmsg2yy msg2.lex

sim_std_msg.cc : ../sim/sim_std.msg
	$(qecho) STRINGIFY: $<
	$(Q)echo '//' >$@
	$(Q)echo '// THIS IS A GENERATED FILE, DO NOT EDIT!' >>$@
	$(Q)echo '//' >>$@
	$(Q)echo '' >>$@
	$(Q)echo 'namespace omnetpp { namespace nedxml { const char *SIM_STD_DEFINITIONS = R"ENDMARK(' >>$@
	$(Q)cat $< >>$@
	$(Q)echo ')ENDMARK"; } }' >>$@

-include $(OBJS:%=%.d)

