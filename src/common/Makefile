#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) -I. -I$(OMNETPP_INCL_DIR)/platdep

OBJS= lcgrandom.o filereader.o linetokenizer.o \
      stringtokenizer.o fnamelisttokenizer.o \
      expression.o lex.expressionyy.o expression.tab.o \
      matchexpression.o lex.matchexpressionyy.o matchexpression.tab.o \
      patternmatcher.o unitconversion.o displaystring.o fileglobber.o \
      fileutil.o stringutil.o commonutil.o exception.o bigdecimal.o

#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all : libcommon$(LIB_SUFFIX)

libcommon$(A_LIB_SUFFIX) : $(OBJS)
	$(AR) libcommon$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libcommon$(A_LIB_SUFFIX)
	../utils/install-lib libcommon$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libcommon$(A_LIB_SUFFIX)

libcommon$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o libcommon$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libcommon$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libcommon$(SO_LIB_SUFFIX)

expression.tab.h expression.tab.cc : expression.y
	$(YACC) -p expressionyy -b expression -d expression.y
	mv expression.tab.c expression.tab.cc

lex.expressionyy.cc: expression.lex expression.tab.h
	$(LEX) -Pexpressionyy expression.lex
	mv lex.expressionyy.c lex.expressionyy.cc

matchexpression.tab.h matchexpression.tab.cc : matchexpression.y
	$(YACC) -p matchexpressionyy -b matchexpression -d matchexpression.y
	mv matchexpression.tab.c matchexpression.tab.cc

lex.matchexpressionyy.cc: matchexpression.lex matchexpression.tab.h
	$(LEX) -Pmatchexpressionyy matchexpression.lex
	mv lex.matchexpressionyy.c lex.matchexpressionyy.cc

depend:
	$(MAKEDEPEND) -I.  -- *.cc

clean:
	rm -f *.o libcommon$(A_LIB_SUFFIX) libcommon$(SO_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
commonutil.o: commonutil.cc \
  commonutil.h \
  commondefs.h \
  exception.h
displaystring.o: displaystring.cc \
  displaystring.h \
  commondefs.h
exception.o: exception.cc \
  exception.h \
  commondefs.h
fileglobber.o: fileglobber.cc \
  fileglobber.h \
  commondefs.h
filereader.o: filereader.cc \
  commonutil.h \
  filereader.h \
  exception.h \
  commondefs.h
fileutil.o: fileutil.cc \
  fileutil.h \
  commondefs.h
fnamelisttokenizer.o: fnamelisttokenizer.cc \
  fnamelisttokenizer.h \
  commondefs.h \
  commonutil.h \
  exception.h
lcgrandom.o: lcgrandom.cc \
  lcgrandom.h \
  commondefs.h
linetokenizer.o: linetokenizer.cc \
  exception.h \
  linetokenizer.h \
  commondefs.h
matchexpression.o: matchexpression.cc \
  matchexpression.h \
  patternmatcher.h \
  commondefs.h \
  exception.h
patternmatcher.o: patternmatcher.cc \
  patternmatcher.h \
  stringutil.h \
  exception.h \
  commondefs.h \
  commonutil.h
stringtokenizer.o: stringtokenizer.cc \
  stringtokenizer.h \
  commondefs.h
stringutil.o: stringutil.cc \
  commonutil.h \
  stringutil.h \
  commondefs.h \
  exception.h
unitconversion.o: unitconversion.cc \
  unitconversion.h \
  commondefs.h \
  exception.h
