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
LIBNAME=libnedxml$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/nedxml
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CPPFLAGS) $(CFLAGS) $(XML_CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep -DBUILDING_NEDXML

IMPLIBS= -L$L -lcommon$D $(XML_LIBS)

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
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all : $L/$(LIBNAME)$(LIB_SUFFIX) $O/nedtool$(EXE_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .
	chmod +x opp_msgc
	cp opp_msgc $(OMNETPP_BIN_DIR)

$O/nedtool$(EXE_SUFFIX): $O/nedtool.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o $O/nedtool$(EXE_SUFFIX) $O/nedtool.o -L$L -lnedxml$D -lcommon$D $(XML_LIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog nedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

$L/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(SHLIB_LD) $(XML_LIBS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX) $(LDFLAGS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd

ned1.tab.hh ned1.tab.cc : ned1.y
	$(YACC) -o ned1.tab.cc -p ned1yy -d ned1.y

lex.ned1yy.cc: ned1.lex
	$(LEX) -o lex.ned1yy.cc -Pned1yy ned1.lex

ned2.tab.hh ned2.tab.cc : ned2.y
	$(YACC) -o ned2.tab.cc -p ned2yy -d ned2.y

lex.ned2yy.cc: ned2.lex
	$(LEX) -o lex.ned2yy.cc -Pned2yy ned2.lex

msg2.tab.hh msg2.tab.cc : msg2.y
	$(YACC) -o msg2.tab.cc -p msg2yy -d msg2.y

lex.msg2yy.cc: msg2.lex
	$(LEX) -o lex.msg2yy.cc -Pmsg2yy msg2.lex

depend: $(GENERATED_SOURCES)
	$(MAKEDEPEND) -I../../include -P\$$O/ -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	rm -f $(GENERATED_SOURCES)
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	cd $(OMNETPP_BIN_DIR) && rm -f nedtool$(EXE_SUFFIX) opp_msgc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/cppexprgenerator.o: cppexprgenerator.cc \
  cppexprgenerator.h \
  nedelement.h \
  nederror.h \
  nedelements.h \
  nedxmldefs.h
$O/cppgenerator.o: cppgenerator.cc \
  cppexprgenerator.h \
  cppgenerator.h \
  nedelement.h \
  nederror.h \
  nedelements.h \
  nedxmldefs.h
$O/cppmsggenerator.o: cppmsggenerator.cc \
  cppexprgenerator.h \
  cppgenerator.h \
  nedelement.h \
  nederror.h \
  nedelements.h \
  nedxmldefs.h
$O/lex.msg2yy.o: lex.msg2yy.cc \
  msg2.tab.hh \
  nedyydefs.h \
  nedexception.h \
  nedxmldefs.h
$O/lex.ned1yy.o: lex.ned1yy.cc \
  nedyydefs.h \
  ned1.tab.hh \
  nedexception.h \
  nedxmldefs.h
$O/lex.ned2yy.o: lex.ned2yy.cc \
  nederror.h \
  nedfilebuffer.h \
  nedelements.h \
  nedutil.h \
  nedparser.h \
  ned2.tab.hh \
  nedyydefs.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedyylib.h
$O/msg2.tab.o: msg2.tab.cc \
  nedfilebuffer.h \
  nederror.h \
  nedelements.h \
  nedutil.h \
  nedparser.h \
  nedyydefs.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedyylib.h
$O/ned1.tab.o: ned1.tab.cc \
  nedfilebuffer.h \
  nederror.h \
  nedelements.h \
  nedutil.h \
  nedparser.h \
  nedyydefs.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedyylib.h
$O/ned1generator.o: ned1generator.cc \
  ned1generator.h \
  nederror.h \
  nedelements.h \
  nedutil.h \
  nedelement.h \
  nedxmldefs.h
$O/ned2.tab.o: ned2.tab.cc \
  nederror.h \
  nedfilebuffer.h \
  nedelements.h \
  nedutil.h \
  nedparser.h \
  ned2.tab.hh \
  nedyydefs.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedyylib.h
$O/ned2generator.o: ned2generator.cc \
  nederror.h \
  nedelements.h \
  nedutil.h \
  nedelement.h \
  nedxmldefs.h \
  ned2generator.h
$O/nedcrossvalidator.o: nedcrossvalidator.cc \
  nederror.h \
  nedelements.h \
  nedresourcecache.h \
  nedcrossvalidator.h \
  nedtypeinfo.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedvalidator.h
$O/neddtdvalidator.o: neddtdvalidator.cc \
  nederror.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  neddtdvalidator.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedvalidator.h
$O/neddtdvalidatorbase.o: neddtdvalidatorbase.cc \
  nederror.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedvalidator.h
$O/nedelement.o: nedelement.cc \
  nederror.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h
$O/nedelements.o: nedelements.cc \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h
$O/nederror.o: nederror.cc \
  nederror.h \
  nedelement.h \
  nedxmldefs.h
$O/nedexception.o: nedexception.cc \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h
$O/nedfilebuffer.o: nedfilebuffer.cc \
  nederror.h \
  nedfilebuffer.h \
  nedelements.h \
  nedparser.h \
  nedyydefs.h \
  nedelement.h \
  nedxmldefs.h \
  nedyylib.h
$O/nedparser.o: nedparser.cc \
  nederror.h \
  nedfilebuffer.h \
  nedelements.h \
  nedparser.h \
  nedyydefs.h \
  nedelement.h \
  nedxmldefs.h
$O/nedresourcecache.o: nedresourcecache.cc \
  nedsyntaxvalidator.h \
  nederror.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  nedresourcecache.h \
  nedparser.h \
  nedcrossvalidator.h \
  neddtdvalidator.h \
  nedyydefs.h \
  nedtypeinfo.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedxmlparser.h
$O/nedsaxhandler.o: nedsaxhandler.cc \
  nederror.h \
  nedelements.h \
  nedelement.h \
  saxparser.h \
  nedexception.h \
  nedxmldefs.h \
  nedsaxhandler.h
$O/nedsyntaxvalidator.o: nedsyntaxvalidator.cc \
  nedsyntaxvalidator.h \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedexception.h \
  nedxmldefs.h \
  nedvalidator.h
$O/nedtool.o: nedtool.cc \
  ned1generator.h \
  nedparser.h \
  nedyydefs.h \
  nedelement.h \
  nedexception.h \
  nedtools.h \
  ned2generator.h \
  nedsyntaxvalidator.h \
  nederror.h \
  nedelements.h \
  xmlgenerator.h \
  neddtdvalidatorbase.h \
  ../utils/ver.h \
  neddtdvalidator.h \
  nedcrossvalidator.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedxmlparser.h
$O/nedtools.o: nedtools.cc \
  nedelement.h \
  nedexception.h \
  nedtools.h \
  nederror.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  neddtdvalidator.h \
  nedxmldefs.h \
  nedvalidator.h
$O/nedtypeinfo.o: nedtypeinfo.cc \
  nedresourcecache.h \
  nedelement.h \
  nedexception.h \
  ned2generator.h \
  nederror.h \
  nedelements.h \
  nedutil.h \
  nedtypeinfo.h \
  nedxmldefs.h
$O/nedtypeinfo2.o: nedtypeinfo2.cc \
  nedresourcecache.h \
  nedelement.h \
  nedexception.h \
  nedtypeinfo2.h \
  ned2generator.h \
  nederror.h \
  nedelements.h \
  nedutil.h \
  nedtypeinfo.h \
  nedxmldefs.h
$O/nedutil.o: nedutil.cc \
  nedelement.h \
  nedexception.h \
  nedelements.h \
  nedutil.h \
  nedxmldefs.h
$O/nedvalidator.o: nedvalidator.cc \
  nedelement.h \
  nedexception.h \
  nederror.h \
  nedelements.h \
  nedxmldefs.h \
  nedvalidator.h
$O/nedxmlparser.o: nedxmlparser.cc \
  nedelement.h \
  nedsaxhandler.h \
  nederror.h \
  saxparser.h \
  nedxmldefs.h \
  nedxmlparser.h
$O/nedyylib.o: nedyylib.cc \
  nedfilebuffer.h \
  nedparser.h \
  nedyydefs.h \
  nedelement.h \
  nederror.h \
  nedelements.h \
  nedxmldefs.h \
  nedyylib.h
$O/saxparser_expat.o: saxparser_expat.cc \
  saxparser.h \
  nedxmldefs.h
$O/saxparser_libxml.o: saxparser_libxml.cc \
  saxparser.h \
  nedxmldefs.h
$O/saxparser_none.o: saxparser_none.cc \
  nederror.h \
  saxparser.h \
  nedxmldefs.h
$O/xmlgenerator.o: xmlgenerator.cc \
  nedelement.h \
  nedelements.h \
  xmlgenerator.h \
  nedxmldefs.h
