#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppnedxml$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/nedxml
L=$(OMNETPP_LIB_DIR)

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

.PHONY: all clean doc build install install_nedtool
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
all : install install_nedtool

install: build $(CONFIGFILE)
	@mkdir -p $L
	-$(Q)cp $O/$(LIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(SO_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)$(SHLIB_POSTPROCESS) $L/$(LIBNAME)$(SO_LIB_SUFFIX) 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR) 2>/dev/null || :
ifeq ($(TOOLCHAIN_NAME),clangc2)
ifeq ($(MODE),debug)
	-$(Q)cp $O/$(LIBNAME).pdb $(OMNETPP_BIN_DIR) 2>/dev/null || :
endif
endif

build : $O/$(LIBNAME)$(LIB_SUFFIX)

install_nedtool: $O/nedtool$(EXE_SUFFIX)
	@mkdir -p $(OMNETPP_BIN_DIR)
	$(Q)cp $O/nedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	$(Q)cp opp_msgc $(OMNETPP_BIN_DIR) && chmod +x $(OMNETPP_BIN_DIR)/opp_msgc

$O/nedtool$(EXE_SUFFIX): nedtool.cc install
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) nedtool.cc -o $@ $(LDFLAGS) -loppnedxml$D $(IMPLIBS)

$O/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)

$O/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) $(XML_LIBS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

$O/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)

nedelements.cc nedelements.h neddtdvalidator.cc neddtdvalidator.h nedvalidator.cc nedvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/ned2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/etc/ned2.dtd

ned1.tab.hh ned1.tab.cc : ned1.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned1.tab.cc -p ned1yy -d ned1.y

lex.ned1yy.cc: ned1.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned1yy.cc -Pned1yy ned1.lex

ned2.tab.hh ned2.tab.cc : ned2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned2.tab.cc -p ned2yy -d ned2.y

lex.ned2yy.cc: ned2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned2yy.cc -Pned2yy ned2.lex

msg2.tab.hh msg2.tab.cc : msg2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o msg2.tab.cc -p msg2yy -d msg2.y

lex.msg2yy.cc: msg2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.msg2yy.cc -Pmsg2yy msg2.lex

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_BIN_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f nedtool$(EXE_SUFFIX) opp_msgc

-include $(OBJS:%.o=%.d)
