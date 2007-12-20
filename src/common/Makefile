#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

ifeq ("$(strip $(CONFIGNAME))",".")
O=.
L=$(OMNETPP_LIB_DIR)
else
O=$(OMNETPP_OBJ_DIR)/$(CONFIGNAME)/src/common
L=$(OMNETPP_LIB_DIR)/$(CONFIGNAME)
endif

#
# Local definitions
#
LIBNAME=libcommon

COPTS=$(CFLAGS) -I. -I$(OMNETPP_INCL_DIR)/platdep

IMPLIBS=

OBJS= $O/lcgrandom.o $O/filereader.o $O/linetokenizer.o \
      $O/stringpool.o $O/stringtokenizer.o $O/fnamelisttokenizer.o \
      $O/expression.o $O/lex.expressionyy.o $O/expression.tab.o \
      $O/matchexpression.o $O/lex.matchexpressionyy.o $O/matchexpression.tab.o \
      $O/patternmatcher.o $O/unitconversion.o $O/displaystring.o $O/fileglobber.o \
      $O/fileutil.o $O/stringutil.o $O/commonutil.o $O/exception.o $O/bigdecimal.o \
      $O/enumstr.o

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
all : $L/$(LIBNAME)$(LIB_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(CONFIGNAME)/$(LIBNAME)$(LIB_SUFFIX)* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

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
	$(MAKEDEPEND) -I. -P\$$O/ -- *.cc

clean:
	rm -f expression.tab.cc expression.tab.h lex.expressionyy.cc lex.matchexpressionyy.cc
	rm -f $O/*.o $O/$(LIBNAME)$(A_LIB_SUFFIX) $O/$(LIBNAME)$(SO_LIB_SUFFIX) $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/bigdecimal.o: bigdecimal.cc \
  opp_ctype.h \
  bigdecimal.h \
  commondefs.h \
  exception.h
$O/commonutil.o: commonutil.cc \
  commonutil.h \
  commondefs.h \
  exception.h
$O/displaystring.o: displaystring.cc \
  opp_ctype.h \
  displaystring.h \
  commondefs.h
$O/exception.o: exception.cc \
  exception.h \
  commondefs.h
$O/expression.o: expression.cc \
  expression.h \
  expressionyydefs.h \
  commondefs.h \
  commonutil.h \
  stringutil.h \
  exception.h
$O/fileglobber.o: fileglobber.cc \
  fileglobber.h \
  commondefs.h
$O/filereader.o: filereader.cc \
  commonutil.h \
  filereader.h \
  exception.h \
  commondefs.h
$O/fileutil.o: fileutil.cc \
  opp_ctype.h \
  fileutil.h \
  stringtokenizer.h \
  exception.h \
  commondefs.h
$O/fnamelisttokenizer.o: fnamelisttokenizer.cc \
  fnamelisttokenizer.h \
  commondefs.h \
  commonutil.h \
  exception.h
$O/lcgrandom.o: lcgrandom.cc \
  lcgrandom.h \
  commondefs.h \
  exception.h
$O/linetokenizer.o: linetokenizer.cc \
  exception.h \
  linetokenizer.h \
  commondefs.h
$O/matchexpression.o: matchexpression.cc \
  matchexpression.h \
  patternmatcher.h \
  commonutil.h \
  commondefs.h \
  exception.h
$O/matchexpression.tab.o: matchexpression.tab.cc \
  matchexpression.h \
  patternmatcher.h \
  exception.h \
  commondefs.h
$O/patternmatcher.o: patternmatcher.cc \
  opp_ctype.h \
  patternmatcher.h \
  stringutil.h \
  exception.h \
  commondefs.h \
  commonutil.h
$O/stringpool.o: stringpool.cc \
  stringpool.h \
  commondefs.h
$O/stringtokenizer.o: stringtokenizer.cc \
  stringtokenizer.h \
  commondefs.h
$O/stringutil.o: stringutil.cc \
  commonutil.h \
  opp_ctype.h \
  stringutil.h \
  stringtokenizer.h \
  commondefs.h \
  exception.h
$O/unitconversion.o: unitconversion.cc \
  opp_ctype.h \
  unitconversion.h \
  commondefs.h \
  exception.h
