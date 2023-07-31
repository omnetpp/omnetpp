#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

# Make sure that output locations are set
ifeq ("$(strip $(OMNETPP_BIN_DIR))","")
$(error OMNETPP_BIN_DIR must be correctly set)
endif
ifeq ("$(strip $(OMNETPP_OUT_DIR))","")
$(error OMNETPP_OUT_DIR must be correctly set)
endif
ifeq ("$(strip $(OMNETPP_LIB_DIR))","")
$(error OMNETPP_LIB_DIR must be correctly set)
endif

#
# Local definitions
#
LIBNAME=liboppnedxml$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/nedxml
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

COPTS=$(CPPFLAGS) $(CFLAGS) $(XML_CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep -DBUILDING_NEDXML

IMPLIBS= -L$L -loppcommon$D $(XML_LIBS)

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
                  ned1.tab.hh ned1.tab.cc lex.ned1yy.cc \
                  ned2.tab.hh ned2.tab.cc lex.ned2yy.cc \
                  msg2.tab.hh msg2.tab.cc lex.msg2yy.cc

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

#
# Targets
#
all : $L/$(LIBNAME)$(LIB_SUFFIX) $O/nedtool$(EXE_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .
	$(Q)cp opp_msgc $(OMNETPP_BIN_DIR)
	$(Q)chmod +x $(OMNETPP_BIN_DIR)/opp_msgc

$O/nedtool$(EXE_SUFFIX): $O/nedtool.o $L/$(LIBNAME)$(LIB_SUFFIX)
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/nedtool$(EXE_SUFFIX) $O/nedtool.o -L$L -loppnedxml$D -loppcommon$D $(XML_LIBS)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-prog nedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

$L/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) $(XML_LIBS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(Q)$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L
	$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

%edelements.cc %edelements.h %edvalidator.cc %edvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd

%ed1.tab.hh %ed1.tab.cc : ned1.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned1.tab.cc -p ned1yy -d ned1.y

lex.ned1yy.cc: ned1.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned1yy.cc -Pned1yy ned1.lex

%ed2.tab.hh %ed2.tab.cc : ned2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o ned2.tab.cc -p ned2yy -d ned2.y

lex.ned2yy.cc: ned2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.ned2yy.cc -Pned2yy ned2.lex

%sg2.tab.hh %sg2.tab.cc : msg2.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o msg2.tab.cc -p msg2yy -d msg2.y

lex.msg2yy.cc: msg2.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.msg2yy.cc -Pmsg2yy msg2.lex

depend: $(GENERATED_SOURCES)
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I../../include -P\$$O/ -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f nedtool$(EXE_SUFFIX) opp_msgc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/lex.msg2yy.o: lex.msg2yy.cc \
  nedexception.h \
  nedxmldefs.h \
  nedyydefs.h \
  msg2.tab.hh
$O/lex.ned1yy.o: lex.ned1yy.cc \
  nedexception.h \
  nedxmldefs.h \
  ned1.tab.hh \
  nedyydefs.h
$O/lex.ned2yy.o: lex.ned2yy.cc \
  ned2.tab.hh \
  nedxmldefs.h \
  nedexception.h \
  nedyydefs.h
$O/msg2.tab.o: msg2.tab.cc \
  nedfilebuffer.h \
  nedxmldefs.h \
  nedelement.h \
  msg2.tab.hh \
  nedparser.h \
  nedyylib.h \
  nedutil.h \
  nederror.h \
  nedelements.h \
  nedexception.h \
  nedyydefs.h
$O/msgcppgenerator.o: msgcppgenerator.cc \
  ../../include/platdep/platdefs.h \
  nedelement.h \
  nedxmldefs.h \
  ../../include/simkerneldefs.h \
  ../../include/platdep/intxtypes.h \
  nedexception.h \
  nedelements.h \
  ned2generator.h \
  msgcppgenerator.h \
  nederror.h \
  nedutil.h
$O/ned1.tab.o: ned1.tab.cc \
  nedfilebuffer.h \
  nedxmldefs.h \
  nedelement.h \
  nedparser.h \
  nedyylib.h \
  nederror.h \
  nedutil.h \
  nedelements.h \
  nedexception.h \
  nedyydefs.h
$O/ned1generator.o: ned1generator.cc \
  nedxmldefs.h \
  nedelement.h \
  nedutil.h \
  nederror.h \
  ned1generator.h \
  nedelements.h
$O/ned2.tab.o: ned2.tab.cc \
  nedelement.h \
  nedxmldefs.h \
  nedfilebuffer.h \
  nedutil.h \
  nederror.h \
  nedyylib.h \
  nedparser.h \
  ned2.tab.hh \
  nedyydefs.h \
  nedexception.h \
  nedelements.h
$O/ned2generator.o: ned2generator.cc \
  nedelement.h \
  nedxmldefs.h \
  nederror.h \
  nedutil.h \
  ned2generator.h \
  nedelements.h
$O/nedcrossvalidator.o: nedcrossvalidator.cc \
  nederror.h \
  nedcrossvalidator.h \
  nedexception.h \
  nedelements.h \
  nedvalidator.h \
  nedxmldefs.h \
  nedelement.h \
  nedresourcecache.h \
  nedtypeinfo.h
$O/neddtdvalidator.o: neddtdvalidator.cc \
  nedelements.h \
  nedexception.h \
  neddtdvalidatorbase.h \
  nederror.h \
  nedelement.h \
  neddtdvalidator.h \
  nedxmldefs.h \
  nedvalidator.h
$O/neddtdvalidatorbase.o: neddtdvalidatorbase.cc \
  nedvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  neddtdvalidatorbase.h \
  nederror.h \
  nedelements.h \
  nedexception.h
$O/nedelement.o: nedelement.cc \
  nedelement.h \
  nedxmldefs.h \
  nedexception.h \
  nederror.h
$O/nedelements.o: nedelements.cc \
  nedxmldefs.h \
  nedelement.h \
  nederror.h \
  nedelements.h \
  nedexception.h
$O/nederror.o: nederror.cc \
  nedxmldefs.h \
  nedelement.h \
  nederror.h
$O/nedexception.o: nedexception.cc \
  nedexception.h \
  nedxmldefs.h \
  nedelement.h
$O/nedfilebuffer.o: nedfilebuffer.cc \
  nedelement.h \
  nedxmldefs.h \
  nedfilebuffer.h \
  nederror.h \
  nedyylib.h \
  nedparser.h \
  nedyydefs.h \
  nedelements.h
$O/nedparser.o: nedparser.cc \
  nedelements.h \
  nedyydefs.h \
  nedparser.h \
  nederror.h \
  nedfilebuffer.h \
  nedelement.h \
  nedxmldefs.h
$O/nedresourcecache.o: nedresourcecache.cc \
  nedsyntaxvalidator.h \
  nedresourcecache.h \
  nedtypeinfo.h \
  neddtdvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedxmlparser.h \
  nedvalidator.h \
  nedelements.h \
  nedexception.h \
  nedyydefs.h \
  nedparser.h \
  neddtdvalidatorbase.h \
  nederror.h \
  nedcrossvalidator.h
$O/nedsaxhandler.o: nedsaxhandler.cc \
  nedsaxhandler.h \
  nedelement.h \
  nedxmldefs.h \
  saxparser.h \
  nederror.h \
  nedelements.h \
  nedexception.h
$O/nedsyntaxvalidator.o: nedsyntaxvalidator.cc \
  nederror.h \
  nedexception.h \
  nedelements.h \
  nedvalidator.h \
  nedxmldefs.h \
  nedelement.h \
  nedsyntaxvalidator.h
$O/nedtool.o: nedtool.cc \
  nedtools.h \
  nedvalidator.h \
  nedsyntaxvalidator.h \
  nedelement.h \
  neddtdvalidator.h \
  neddtdvalidatorbase.h \
  ned1generator.h \
  msgcppgenerator.h \
  nederror.h \
  ned2generator.h \
  nedexception.h \
  nedxmlparser.h \
  ../common/ver.h \
  nedxmldefs.h \
  nedparser.h \
  xmlgenerator.h \
  nedcrossvalidator.h \
  nedelements.h \
  nedyydefs.h
$O/nedtools.o: nedtools.cc \
  nedelements.h \
  nedxmldefs.h \
  nederror.h \
  neddtdvalidatorbase.h \
  nedexception.h \
  nedvalidator.h \
  nedtools.h \
  neddtdvalidator.h \
  nedelement.h
$O/nedtypeinfo.o: nedtypeinfo.cc \
  nedelement.h \
  nederror.h \
  ned2generator.h \
  nedexception.h \
  nedxmldefs.h \
  nedresourcecache.h \
  nedtypeinfo.h \
  nedutil.h \
  nedelements.h
$O/nedutil.o: nedutil.cc \
  nedxmldefs.h \
  nedutil.h \
  nedelements.h \
  nedelement.h \
  nedexception.h
$O/nedvalidator.o: nedvalidator.cc \
  nedxmldefs.h \
  nedelements.h \
  nedelement.h \
  nedvalidator.h \
  nedexception.h \
  nederror.h
$O/nedxmlparser.o: nedxmlparser.cc \
  saxparser.h \
  nedxmlparser.h \
  nedxmldefs.h \
  nederror.h \
  nedsaxhandler.h \
  nedelement.h
$O/nedyylib.o: nedyylib.cc \
  nedyydefs.h \
  nedelements.h \
  nedparser.h \
  nedxmldefs.h \
  nedfilebuffer.h \
  nederror.h \
  nedyylib.h \
  nedelement.h
$O/saxparser_expat.o: saxparser_expat.cc \
  nedxmldefs.h \
  saxparser.h
$O/saxparser_libxml.o: saxparser_libxml.cc \
  nedxmldefs.h \
  saxparser.h
$O/saxparser_none.o: saxparser_none.cc \
  saxparser.h \
  nedxmldefs.h \
  nederror.h
$O/xmlgenerator.o: xmlgenerator.cc \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h \
  xmlgenerator.h
