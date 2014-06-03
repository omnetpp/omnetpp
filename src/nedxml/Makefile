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

nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd

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
  msg2.tab.hh \
  nedxmldefs.h \
  nedyydefs.h
$O/lex.ned1yy.o: lex.ned1yy.cc \
  nedexception.h \
  ned1.tab.hh \
  nedyydefs.h \
  nedxmldefs.h
$O/lex.ned2yy.o: lex.ned2yy.cc \
  nedyydefs.h \
  nedxmldefs.h \
  ned2.tab.hh \
  nedexception.h
$O/msg2.tab.o: msg2.tab.cc \
  nedyylib.h \
  nedutil.h \
  nedelement.h \
  msg2.tab.hh \
  nedfilebuffer.h \
  nedexception.h \
  nederror.h \
  nedparser.h \
  nedyydefs.h \
  nedxmldefs.h \
  nedelements.h
$O/ned1.tab.o: ned1.tab.cc \
  nedyydefs.h \
  nedxmldefs.h \
  nedparser.h \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedyylib.h \
  nedutil.h \
  nedexception.h \
  nedfilebuffer.h
$O/ned1generator.o: ned1generator.cc \
  nedelement.h \
  nedutil.h \
  nedelements.h \
  nedxmldefs.h \
  nederror.h \
  ned1generator.h
$O/ned2.tab.o: ned2.tab.cc \
  nedyylib.h \
  nedutil.h \
  ned2.tab.hh \
  nedelement.h \
  nedexception.h \
  nedfilebuffer.h \
  nedparser.h \
  nedxmldefs.h \
  nedyydefs.h \
  nederror.h \
  nedelements.h
$O/ned2generator.o: ned2generator.cc \
  nedxmldefs.h \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedutil.h \
  ned2generator.h
$O/nedcrossvalidator.o: nedcrossvalidator.cc \
  nederror.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h \
  nedresourcecache.h \
  nedelement.h \
  nedtypeinfo.h \
  nedcrossvalidator.h \
  nedexception.h
$O/neddtdvalidator.o: neddtdvalidator.cc \
  nedelements.h \
  nederror.h \
  neddtdvalidatorbase.h \
  nedvalidator.h \
  nedxmldefs.h \
  neddtdvalidator.h \
  nedexception.h \
  nedelement.h
$O/neddtdvalidatorbase.o: neddtdvalidatorbase.cc \
  nedxmldefs.h \
  nedvalidator.h \
  neddtdvalidatorbase.h \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedexception.h
$O/nedelement.o: nedelement.cc \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nederror.h
$O/nedelements.o: nedelements.cc \
  nedelements.h \
  nederror.h \
  nedxmldefs.h \
  nedexception.h \
  nedelement.h
$O/nederror.o: nederror.cc \
  nederror.h \
  nedxmldefs.h \
  nedelement.h
$O/nedexception.o: nedexception.cc \
  nedxmldefs.h \
  nedexception.h \
  nedelement.h
$O/nedfilebuffer.o: nedfilebuffer.cc \
  nedfilebuffer.h \
  nedyylib.h \
  nedelement.h \
  nedelements.h \
  nedparser.h \
  nedxmldefs.h \
  nedyydefs.h \
  nederror.h
$O/nedparser.o: nedparser.cc \
  nedelements.h \
  nedparser.h \
  nedxmldefs.h \
  nedyydefs.h \
  nederror.h \
  nedfilebuffer.h \
  nedelement.h
$O/nedresourcecache.o: nedresourcecache.cc \
  nedcrossvalidator.h \
  nedtypeinfo.h \
  nedelement.h \
  nedxmlparser.h \
  neddtdvalidator.h \
  nedexception.h \
  nederror.h \
  neddtdvalidatorbase.h \
  nedvalidator.h \
  nedparser.h \
  nedxmldefs.h \
  nedyydefs.h \
  nedelements.h \
  nedsyntaxvalidator.h \
  nedresourcecache.h
$O/nedsaxhandler.o: nedsaxhandler.cc \
  nedxmldefs.h \
  nederror.h \
  nedelements.h \
  nedsaxhandler.h \
  saxparser.h \
  nedelement.h \
  nedexception.h
$O/nedsyntaxvalidator.o: nedsyntaxvalidator.cc \
  nedsyntaxvalidator.h \
  nedelements.h \
  nedxmldefs.h \
  nedvalidator.h \
  nederror.h \
  nedexception.h \
  nedelement.h
$O/nedtool.o: nedtool.cc \
  nedxmldefs.h \
  neddtdvalidatorbase.h \
  nederror.h \
  nedsyntaxvalidator.h \
  nedelements.h \
  nedxmlparser.h \
  nedtools.h \
  nedelement.h \
  xmlgenerator.h \
  nedcrossvalidator.h \
  ned2generator.h \
  neddtdvalidator.h \
  nedyydefs.h \
  nedparser.h \
  nedvalidator.h \
  ned1generator.h \
  nedexception.h \
  ../common/ver.h
$O/nedtools.o: nedtools.cc \
  nedexception.h \
  nedvalidator.h \
  nedelement.h \
  nedtools.h \
  neddtdvalidator.h \
  neddtdvalidatorbase.h \
  nederror.h \
  nedxmldefs.h \
  nedelements.h
$O/nedtypeinfo.o: nedtypeinfo.cc \
  ned2generator.h \
  nedutil.h \
  nedelement.h \
  nedresourcecache.h \
  nedelements.h \
  nedxmldefs.h \
  nederror.h \
  nedexception.h \
  nedtypeinfo.h
$O/nedutil.o: nedutil.cc \
  nedelement.h \
  nedutil.h \
  nedelements.h \
  nedxmldefs.h \
  nedexception.h
$O/nedvalidator.o: nedvalidator.cc \
  nedvalidator.h \
  nedexception.h \
  nedelements.h \
  nedxmldefs.h \
  nederror.h \
  nedelement.h
$O/nedxmlparser.o: nedxmlparser.cc \
  nedxmldefs.h \
  nederror.h \
  nedsaxhandler.h \
  nedxmlparser.h \
  saxparser.h \
  nedelement.h
$O/nedyylib.o: nedyylib.cc \
  nedelement.h \
  nedfilebuffer.h \
  nederror.h \
  nedxmldefs.h \
  nedelements.h \
  nedyylib.h \
  nedparser.h \
  nedyydefs.h
$O/saxparser_expat.o: saxparser_expat.cc \
  saxparser.h \
  nedxmldefs.h
$O/saxparser_libxml.o: saxparser_libxml.cc \
  saxparser.h \
  nedxmldefs.h
$O/saxparser_none.o: saxparser_none.cc \
  nederror.h \
  nedxmldefs.h \
  saxparser.h
$O/xmlgenerator.o: xmlgenerator.cc \
  nedelements.h \
  nedxmldefs.h \
  nedelement.h \
  xmlgenerator.h
