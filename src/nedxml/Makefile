#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CPPFLAGS) $(CFLAGS) $(XML_CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep -DBUILDING_NEDXML

#OBJS= nedelements.o nedelement.o nederror.o \
#      nedvalidator.o neddtdvalidator.o neddtdvalidatorbase.o \
#      #      nedsyntaxvalidator.o nedsemanticvalidator.o \
#      nedfilebuffer.o nedparser.o nedyylib.o \
#      #      ned2.tab.o lex.ned2yy.o \
#      msg2.tab.o lex.msg2yy.o \
#      #      ned1.tab.o lex.ned1yy.o \
#      nedutil.o nedgenerator.o xmlgenerator.o nedtools.o \
#      #      nedxmlparser.o nedsaxhandler.o saxparser_$(XMLPARSER).o \
#      nedcompiler.o cppgenerator.o cppmsggenerator.o cppexprgenerator.o
#
#      # FIXME begin temporary def
OBJS= nedelements.o nedelement.o nederror.o \
      nedvalidator.o neddtdvalidator.o neddtdvalidatorbase.o \
      nedsyntaxvalidator.o nedsemanticvalidator.o \
      nedfilebuffer.o nedparser.o nedyylib.o \
      ned2.tab.o lex.ned2yy.o nedtools.o \
      nedutil.o ned2generator.o ned1generator.o xmlgenerator.o \
      nedxmlparser.o nedsaxhandler.o saxparser_$(XMLPARSER).o \
      ned1.tab.o lex.ned1yy.o \
      msg2.tab.o lex.msg2yy.o \
      nedresourcecache.o nedcomponent.o \
      nedcompiler.o
# FIXME end temporary def

#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all : nedxmllib nedtool
	chmod +x nedc
	cp nedc $(OMNETPP_BIN_DIR)

nedtool: nedtool.o
	$(CXX) $(LDFLAGS) -o nedtool nedtool.o -L$(OMNETPP_LIB_DIR) -lnedxml -lcommon $(XML_LIBS)
	../utils/install-prog nedtool $(OMNETPP_BIN_DIR)

nedtool.o: nedtool.cc
	$(CXX) -c $(COPTS) nedtool.cc

nedxmllib : libnedxml$(LIB_SUFFIX)

libnedxml$(A_LIB_SUFFIX): $(OBJS)
	$(AR) libnedxml$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libnedxml$(A_LIB_SUFFIX)
	../utils/install-lib libnedxml$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libnedxml$(A_LIB_SUFFIX)

libnedxml$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) $(XML_LIBS) -o libnedxml$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libnedxml$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libnedxml$(SO_LIB_SUFFIX)

nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h : dtdclassgen.pl ned2.dtd
	perl dtdclassgen.pl ned2.dtd

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
	$(MAKEDEPEND) -I../../include -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	rm -f *.o
	rm -f libnedxml$(A_LIB_SUFFIX) libnedxml$(SO_LIB_SUFFIX)
#	rm -f nedelements.cc nedelements.h nedvalidator.cc nedvalidator.h

# DO NOT DELETE THIS LINE -- make depend depends on it.
cppexprgenerator.o: cppexprgenerator.cc \
  nedcompiler.h \
  cppexprgenerator.h \
  nederror.h \
  nedelements.h \
  nedresourcecache.h \
  nedelement.h \
  nedxmldefs.h \
  nedcomponent.h
cppgenerator.o: cppgenerator.cc \
  nedelements.h \
  cppgenerator.h \
  nederror.h \
  nedelement.h \
  nedxmldefs.h \
  cppexprgenerator.h \
  nedcompiler.h \
  nedresourcecache.h \
  nedcomponent.h
cppmsggenerator.o: cppmsggenerator.cc \
  nedelements.h \
  cppgenerator.h \
  nederror.h \
  nedelement.h \
  nedxmldefs.h \
  cppexprgenerator.h \
  nedcompiler.h \
  nedresourcecache.h \
  nedcomponent.h
ned1generator.o: ned1generator.cc \
  ned1generator.h \
  nedutil.h \
  nedelements.h \
  nederror.h \
  nedelement.h \
  nedxmldefs.h
ned2generator.o: ned2generator.cc \
  ned2generator.h \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedxmldefs.h
nedsyntaxvalidator.o: nedsyntaxvalidator.cc \
  nederror.h \
  nedsyntaxvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h
nedcompiler.o: nedcompiler.cc \
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
nedcomponent.o: nedcomponent.cc \
  nederror.h \
  nedcomponent.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h
neddtdvalidatorbase.o: neddtdvalidatorbase.cc \
  nederror.h \
  neddtdvalidatorbase.h \
  nedelement.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h
neddtdvalidator.o: neddtdvalidator.cc \
  nederror.h \
  neddtdvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h \
  neddtdvalidatorbase.h \
  nedvalidator.h
nedelement.o: nedelement.cc \
  nedelement.h \
  nederror.h \
  nedxmldefs.h
nedelements.o: nedelements.cc \
  nederror.h \
  nedelements.h \
  nedelement.h \
  nedxmldefs.h
nederror.o: nederror.cc \
  nederror.h \
  nedelement.h \
  nedxmldefs.h
nedfilebuffer.o: nedfilebuffer.cc \
  nedfilebuffer.h \
  nedyylib.h \
  nedparser.h \
  nedelement.h \
  nedyydefs.h \
  nederror.h \
  nedxmldefs.h \
  nedelements.h
nedparser.o: nedparser.cc \
  nedparser.h \
  nedfilebuffer.h \
  nedelements.h \
  nederror.h \
  nedyydefs.h \
  nedelement.h \
  nedxmldefs.h
nedresourcecache.o: nedresourcecache.cc \
  nederror.h \
  nedresourcecache.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h \
  nedcomponent.h
nedsaxhandler.o: nedsaxhandler.cc \
  nedsaxhandler.h \
  nedelements.h \
  nederror.h \
  nedelement.h \
  saxparser.h \
  nedxmldefs.h
nedsemanticvalidator.o: nedsemanticvalidator.cc \
  nederror.h \
  nedsemanticvalidator.h \
  nedresourcecache.h \
  nedelement.h \
  nedxmldefs.h \
  nedvalidator.h \
  nedelements.h \
  nedcomponent.h
nedtool.o: nedtool.cc \
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
nedtools.o: nedtools.cc \
  nedtools.h \
  nederror.h \
  neddtdvalidator.h \
  nedelements.h \
  nedelement.h \
  nedxmldefs.h \
  neddtdvalidatorbase.h \
  nedvalidator.h
nedutil.o: nedutil.cc \
  nederror.h \
  nedutil.h \
  nedelement.h \
  nedxmldefs.h
nedvalidator.o: nedvalidator.cc \
  nederror.h \
  nedvalidator.h \
  nedelement.h \
  nedxmldefs.h \
  nedelements.h
nedxmlparser.o: nedxmlparser.cc \
  saxparser.h \
  nedsaxhandler.h \
  nederror.h \
  nedxmlparser.h \
  nedxmldefs.h \
  nedelement.h
nedyylib.o: nedyylib.cc \
  nedyylib.h \
  nedfilebuffer.h \
  nedyydefs.h \
  nedelements.h \
  nedparser.h \
  nedelement.h \
  nedxmldefs.h \
  nederror.h
saxparser_expat.o: saxparser_expat.cc \
  saxparser.h \
  nedxmldefs.h
saxparser_libxml.o: saxparser_libxml.cc \
  saxparser.h \
  nedxmldefs.h
saxparser_none.o: saxparser_none.cc \
  nederror.h \
  saxparser.h \
  nedelement.h \
  nedxmldefs.h
xmlgenerator.o: xmlgenerator.cc \
  nedelements.h \
  xmlgenerator.h \
  nedelement.h \
  nedxmldefs.h
