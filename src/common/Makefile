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
LIBNAME=liboppcommon$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/common
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) -I. -I$(OMNETPP_INCL_DIR)/platdep
IMPLIBS=

ifeq ("$(BUILDING_UILIBS)","yes")
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

OBJS= $O/lcgrandom.o $O/filereader.o $O/linetokenizer.o \
      $O/stringpool.o $O/stringtokenizer.o $O/fnamelisttokenizer.o \
      $O/expression.o $O/lex.expressionyy.o $O/expression.tab.o \
      $O/matchexpression.o $O/matchexpressionlexer.o $O/matchexpression.tab.o \
      $O/patternmatcher.o $O/unitconversion.o $O/displaystring.o $O/fileglobber.o \
      $O/fileutil.o $O/stringutil.o $O/commonutil.o $O/exception.o $O/bigdecimal.o \
      $O/enumstr.o $O/stringtokenizer2.o

ifeq ("$(BUILDING_UILIBS)","yes")
OBJS+= $O/rwlock.o
endif

GENERATED_SOURCES= expression.tab.hh expression.tab.cc lex.expressionyy.cc \
                   matchexpression.tab.hh matchexpression.tab.cc

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
all : $L/$(LIBNAME)$(LIB_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
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

expression.tab.hh expression.tab.cc : expression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o expression.tab.cc -p expressionyy -d expression.y

lex.expressionyy.cc: expression.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.expressionyy.cc -Pexpressionyy expression.lex

matchexpression.tab.hh matchexpression.tab.cc : matchexpression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o matchexpression.tab.cc --no-lines -p matchexpressionyy -d matchexpression.y

depend: $(GENERATED_SOURCES)
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -P\$$O/ -- *.cc

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_SOURCES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/bigdecimal.o: bigdecimal.cc \
  bigdecimal.h \
  commonutil.h \
  commondefs.h \
  opp_ctype.h \
  exception.h
$O/commonutil.o: commonutil.cc \
  commonutil.h \
  commondefs.h \
  exception.h
$O/displaystring.o: displaystring.cc \
  commonutil.h \
  commondefs.h \
  stringutil.h \
  displaystring.h \
  opp_ctype.h \
  exception.h
$O/enumstr.o: enumstr.cc \
  commondefs.h \
  enumstr.h
$O/exception.o: exception.cc \
  commonutil.h \
  commondefs.h \
  exception.h
$O/expression.o: expression.cc \
  commonutil.h \
  commondefs.h \
  stringutil.h \
  unitconversion.h \
  stringpool.h \
  expressionyydefs.h \
  exception.h \
  expression.h
$O/expression.tab.o: expression.tab.cc \
  commonutil.h \
  commondefs.h \
  unitconversion.h \
  stringutil.h \
  expressionyydefs.h \
  expression.h \
  stringpool.h \
  exception.h
$O/fileglobber.o: fileglobber.cc \
  commondefs.h \
  fileglobber.h
$O/filereader.o: filereader.cc \
  commonutil.h \
  commondefs.h \
  filereader.h \
  exception.h
$O/fileutil.o: fileutil.cc \
  commondefs.h \
  fileutil.h \
  opp_ctype.h \
  stringtokenizer.h \
  exception.h
$O/fnamelisttokenizer.o: fnamelisttokenizer.cc \
  commonutil.h \
  commondefs.h \
  fnamelisttokenizer.h \
  exception.h
$O/lcgrandom.o: lcgrandom.cc \
  commondefs.h \
  lcgrandom.h \
  exception.h
$O/lex.expressionyy.o: lex.expressionyy.cc \
  commonutil.h \
  commondefs.h \
  stringutil.h \
  expressionyydefs.h \
  expression.h \
  stringpool.h \
  expression.tab.hh \
  exception.h
$O/linetokenizer.o: linetokenizer.cc \
  commondefs.h \
  linetokenizer.h \
  exception.h
$O/matchexpression.o: matchexpression.cc \
  commonutil.h \
  commondefs.h \
  patternmatcher.h \
  matchexpression.h \
  exception.h
$O/matchexpression.tab.o: matchexpression.tab.cc \
  commondefs.h \
  patternmatcher.h \
  matchexpression.h \
  exception.h \
  matchexpressionlexer.h
$O/matchexpressionlexer.o: matchexpressionlexer.cc \
  commonutil.h \
  commondefs.h \
  stringutil.h \
  matchexpression.tab.hh \
  exception.h \
  matchexpressionlexer.h
$O/patternmatcher.o: patternmatcher.cc \
  commonutil.h \
  commondefs.h \
  stringutil.h \
  opp_ctype.h \
  patternmatcher.h \
  exception.h
$O/rwlock.o: rwlock.cc \
  rwlock.h \
  commondefs.h \
  exception.h
$O/stringpool.o: stringpool.cc \
  commondefs.h \
  stringpool.h
$O/stringtokenizer.o: stringtokenizer.cc \
  commondefs.h \
  stringtokenizer.h
$O/stringtokenizer2.o: stringtokenizer2.cc \
  commonutil.h \
  commondefs.h \
  stringtokenizer2.h \
  exception.h
$O/stringutil.o: stringutil.cc \
  commonutil.h \
  commondefs.h \
  stringutil.h \
  opp_ctype.h \
  stringtokenizer.h \
  exception.h
$O/unitconversion.o: unitconversion.cc \
  commonutil.h \
  commondefs.h \
  unitconversion.h \
  stringutil.h \
  opp_ctype.h \
  exception.h
