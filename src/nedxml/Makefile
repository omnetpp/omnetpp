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

OBJS= $O/nedelements.o $O/nedelement.o $O/nederror.o \
      $O/nedvalidator.o $O/neddtdvalidator.o $O/neddtdvalidatorbase.o \
      $O/nedsyntaxvalidator.o $O/nedsemanticvalidator.o \
      $O/nedfilebuffer.o $O/nedparser.o $O/nedyylib.o \
      $O/ned2.tab.o $O/lex.ned2yy.o $O/nedtools.o \
      $O/nedutil.o $O/ned2generator.o $O/ned1generator.o $O/xmlgenerator.o \
      $O/nedxmlparser.o $O/nedsaxhandler.o $O/saxparser_$(XMLPARSER).o \
      $O/ned1.tab.o $O/lex.ned1yy.o \
      $O/msg2.tab.o $O/lex.msg2yy.o \
      $O/nedresourcecache.o $O/nedcomponent.o \
      $O/nedcompiler.o

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
all : $L/$(LIBNAME)$(LIB_SUFFIX) $O/nedtool
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME)* .
	chmod +x nedc opp_msgc
	cp nedc opp_msgc $(OMNETPP_BIN_DIR)

$O/nedtool: $O/nedtool.o $L/$(LIBNAME)$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o $O/nedtool $O/nedtool.o -L$L -lnedxml$D -lcommon$D $(XML_LIBS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog nedtool $(OMNETPP_BIN_DIR)

$L/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(SHLIB_LD) $(XML_LIBS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h : dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd
	perl dtdclassgen.pl $(OMNETPP_ROOT)/doc/ned2/ned2.dtd

lex.yy.o: lex.yy.cc
	$(CXX) -c $(COPTS) lex.yy.cc

ned1.tab.h ned1.tab.cc : ned1.y
	$(YACC) -p ned1yy -b ned1 -d ned1.y
	mv ned1.tab.c ned1.tab.cc

lex.ned1yy.cc: ned1.lex
	$(LEX) -Pned1yy ned1.lex
	mv lex.ned1yy.c lex.ned1yy.cc

ned2.tab.h ned2.tab.cc : ned2.y
	$(YACC) --verbose --debug -p ned2yy -b ned2 -d ned2.y
	mv ned2.tab.c ned2.tab.cc

lex.ned2yy.cc: ned2.lex
	$(LEX) -Pned2yy ned2.lex
	mv lex.ned2yy.c lex.ned2yy.cc

msg2.tab.h msg2.tab.cc : msg2.y
	$(YACC) -p msg2yy -b msg2 -d msg2.y
	mv msg2.tab.c msg2.tab.cc

lex.msg2yy.cc: msg2.lex
	$(LEX) -Pmsg2yy msg2.lex
	mv lex.msg2yy.c lex.msg2yy.cc

depend:
	$(MAKEDEPEND) -I../../include -P\$$O/ -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	rm -f *.tab.* lex.*yy.*
	rm -f nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	cd $(OMNETPP_BIN_DIR) && rm -f nedtool$(EXE_SUFFIX) nedc opp_msgc

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/cppexprgenerator.o: cppexprgenerator.cc \
  nedcompiler.h \
  cppexprgenerator.h \
  nederror.h \
  nedelements.h \
  nedresourcecache.h \
  nedelement.h \
  nedxmldefs.h \
  nedcomponent.h
$O/cppgenerator.o: cppgenerator.cc \
  nedelements.h \
  cppgenerator.h \
  nederror.h \
  nedelement.h \
  nedxmldefs.h \
  cppexprgenerator.h \
  nedcompiler.h \
  nedresourcecache.h \
  nedcomponent.h
$O/cppmsggenerator.o: cppmsggenerator.cc \
  nedelements.h \
  cppgenerator.h \
  nederror.h \
  nedelement.h \
  nedxmldefs.h \
  cppexprgenerator.h \
  nedcompiler.h \
  nedresourcecache.h \
  nedcomponent.h
$O/lex.msg2yy.o: lex.msg2yy.cc \
  nedyydefs.h \
  nederror.h \
  msg2.tab.h \
  nedxmldefs.h \
  nedelement.h
$O/lex.ned1yy.o: lex.ned1yy.cc \
  nedyydefs.h \
  nederror.h \
  ned1.tab.h \
  nedxmldefs.h \
  nedelement.h
$O/lex.ned2yy.o: lex.ned2yy.cc \
  nedyydefs.h \
  nederror.h \
  ned2.tab.h \
  nedxmldefs.h \
  nedelement.h \
  nedparser.h \
  nedfilebuffer.h \
  nedelements.h \
  nedutil.h \
  nedyylib.h
$O/msg2.tab.o: msg2.tab.cc \
  nedyydefs.h \
  nederror.h \
  nedparser.h \
  nedfilebuffer.h \
  nedelements.h \
  nedutil.h \
  nedyylib.h \
  nedxmldefs.h \
  nedelement.h
$O/ned1.tab.o: ned1.tab.cc \
  nedyydefs.h \
  nedutil.h \
  nederror.h \
  nedparser.h \
  nedfilebuffer.h \
  nedelements.h \
  nedyylib.h \
  nedxmldefs.h \
  nedelement.h
$O/ned1generator.o: ned1generator.cc \
  ned1generator.h \
  nedutil.h \
  nedelements.h \
  nederror.h \
  nedelement.h \
  nedxmldefs.h
$O/ned2.tab.o: ned2.tab.cc \
  ned2.tab.h \
  nedyydefs.h \
  nederror.h \
  nedparser.h \
  nedfilebuffer.h \
  nedelements.h \
  nedutil.h \
  nedyylib.h \
  nedxmldefs.h \
  nedelement.h
$O/ned2generator.o: ned2generator.cc \
  ned2generator.h \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedxmldefs.h
$O/nedcompiler.o: nedcompiler.cc \
  nederror.h \
  nedcompiler.h \
  nedparser.h \
  neddtdvalidator.h \
  nedsyntaxvalidator.h \
  nedsemanticvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h \
  nedresourcecache.h \
  nedcomponent.h \
  nedyydefs.h \
  neddtdvalidatorbase.h \
  nedvalidator.h
$O/nedcomponent.o: nedcomponent.cc \
  nederror.h \
  nedcomponent.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h
$O/neddtdvalidator.o: neddtdvalidator.cc \
  nederror.h \
  neddtdvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  nedvalidator.h
$O/neddtdvalidatorbase.o: neddtdvalidatorbase.cc \
  nederror.h \
  neddtdvalidatorbase.h \
  nedelement.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h
$O/nedelement.o: nedelement.cc \
  nedelement.h \
  nederror.h \
  nedxmldefs.h
$O/nedelements.o: nedelements.cc \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedxmldefs.h
$O/nederror.o: nederror.cc \
  nederror.h \
  nedelement.h \
  nedxmldefs.h
$O/nedfilebuffer.o: nedfilebuffer.cc \
  nedfilebuffer.h \
  nedyylib.h \
  nedparser.h \
  nedelement.h \
  nedyydefs.h \
  nederror.h \
  nedxmldefs.h \
  nedelements.h
$O/nedparser.o: nedparser.cc \
  nedparser.h \
  nedfilebuffer.h \
  nedelements.h \
  nederror.h \
  nedyydefs.h \
  nedelement.h \
  nedxmldefs.h
$O/nedresourcecache.o: nedresourcecache.cc \
  nederror.h \
  nedresourcecache.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h \
  nedcomponent.h
$O/nedsaxhandler.o: nedsaxhandler.cc \
  nedsaxhandler.h \
  nedelements.h \
  nederror.h \
  nedelement.h \
  saxparser.h \
  nedxmldefs.h
$O/nedsemanticvalidator.o: nedsemanticvalidator.cc \
  nederror.h \
  nedsemanticvalidator.h \
  nedresourcecache.h \
  nedelement.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h \
  nedcomponent.h
$O/nedsyntaxvalidator.o: nedsyntaxvalidator.cc \
  nederror.h \
  nedsyntaxvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h
$O/nedtool.o: nedtool.cc \
  nedparser.h \
  nederror.h \
  nedxmlparser.h \
  neddtdvalidator.h \
  nedsyntaxvalidator.h \
  nedsemanticvalidator.h \
  ned2generator.h \
  ned1generator.h \
  xmlgenerator.h \
  nedtools.h \
  ../utils/ver.h \
  nedelement.h \
  nedyydefs.h \
  nedxmldefs.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  nedvalidator.h
$O/nedtools.o: nedtools.cc \
  nedtools.h \
  nederror.h \
  neddtdvalidator.h \
  nedelements.h \
  nedelement.h \
  nedxmldefs.h \
  neddtdvalidatorbase.h \
  nedvalidator.h
$O/nedutil.o: nedutil.cc \
  nederror.h \
  nedutil.h \
  nedelement.h \
  nedxmldefs.h
$O/nedvalidator.o: nedvalidator.cc \
  nederror.h \
  nedvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h
$O/nedxmlparser.o: nedxmlparser.cc \
  saxparser.h \
  nedsaxhandler.h \
  nederror.h \
  nedxmlparser.h \
  nedxmldefs.h \
  nedelement.h
$O/nedyylib.o: nedyylib.cc \
  nedyylib.h \
  nedfilebuffer.h \
  nedyydefs.h \
  nedelements.h \
  nedparser.h \
  nedelement.h \
  nedxmldefs.h \
  nederror.h
$O/saxparser_expat.o: saxparser_expat.cc \
  saxparser.h \
  nedxmldefs.h
$O/saxparser_libxml.o: saxparser_libxml.cc \
  saxparser.h \
  nedxmldefs.h
$O/saxparser_none.o: saxparser_none.cc \
  nederror.h \
  saxparser.h \
  nedelement.h \
  nedxmldefs.h
$O/xmlgenerator.o: xmlgenerator.cc \
  nedelements.h \
  xmlgenerator.h \
  nedelement.h \
  nedxmldefs.h
