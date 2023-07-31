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

%xpression.tab.hh %xpression.tab.cc : expression.y
	$(qecho) YACC: $<
	$(Q)$(YACC) -o expression.tab.cc -p expressionyy -d expression.y

lex.expressionyy.cc: expression.lex
	$(qecho) LEX: $<
	$(Q)$(LEX) -olex.expressionyy.cc -Pexpressionyy expression.lex

%atchexpression.tab.hh %atchexpression.tab.cc : matchexpression.y
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
  commonutil.h \
  bigdecimal.h \
  commondefs.h \
  exception.h \
  opp_ctype.h
$O/commonutil.o: commonutil.cc \
  exception.h \
  commondefs.h \
  commonutil.h
$O/displaystring.o: displaystring.cc \
  opp_ctype.h \
  stringutil.h \
  commondefs.h \
  exception.h \
  commonutil.h \
  displaystring.h
$O/enumstr.o: enumstr.cc \
  commondefs.h \
  enumstr.h
$O/exception.o: exception.cc \
  commondefs.h \
  exception.h \
  commonutil.h
$O/expression.o: expression.cc \
  stringpool.h \
  unitconversion.h \
  expression.h \
  expressionyydefs.h \
  commonutil.h \
  stringutil.h \
  commondefs.h \
  exception.h
$O/expression.tab.o: expression.tab.cc \
  commonutil.h \
  expressionyydefs.h \
  commondefs.h \
  exception.h \
  stringutil.h \
  unitconversion.h \
  expression.h \
  stringpool.h
$O/fileglobber.o: fileglobber.cc \
  fileglobber.h \
  commondefs.h
$O/filereader.o: filereader.cc \
  filereader.h \
  exception.h \
  commondefs.h \
  commonutil.h
$O/fileutil.o: fileutil.cc \
  opp_ctype.h \
  stringtokenizer.h \
  fileutil.h \
  exception.h \
  commondefs.h
$O/fnamelisttokenizer.o: fnamelisttokenizer.cc \
  fnamelisttokenizer.h \
  commonutil.h \
  exception.h \
  commondefs.h
$O/lcgrandom.o: lcgrandom.cc \
  exception.h \
  commondefs.h \
  lcgrandom.h
$O/lex.expressionyy.o: lex.expressionyy.cc \
  commonutil.h \
  expressionyydefs.h \
  exception.h \
  commondefs.h \
  stringutil.h \
  expression.tab.hh \
  expression.h \
  stringpool.h
$O/linetokenizer.o: linetokenizer.cc \
  commondefs.h \
  exception.h \
  linetokenizer.h
$O/matchexpression.o: matchexpression.cc \
  patternmatcher.h \
  commondefs.h \
  exception.h \
  matchexpression.h \
  commonutil.h
$O/matchexpression.tab.o: matchexpression.tab.cc \
  matchexpressionlexer.h \
  commondefs.h \
  exception.h \
  patternmatcher.h \
  matchexpression.h
$O/matchexpressionlexer.o: matchexpressionlexer.cc \
  matchexpressionlexer.h \
  commondefs.h \
  exception.h \
  stringutil.h \
  commonutil.h \
  matchexpression.tab.hh
$O/patternmatcher.o: patternmatcher.cc \
  opp_ctype.h \
  commonutil.h \
  exception.h \
  commondefs.h \
  stringutil.h \
  patternmatcher.h
$O/rwlock.o: rwlock.cc \
  rwlock.h \
  exception.h \
  commondefs.h
$O/stringpool.o: stringpool.cc \
  stringpool.h \
  commondefs.h
$O/stringtokenizer.o: stringtokenizer.cc \
  commondefs.h \
  stringtokenizer.h
$O/stringtokenizer2.o: stringtokenizer2.cc \
  commonutil.h \
  exception.h \
  commondefs.h \
  stringtokenizer2.h
$O/stringutil.o: stringutil.cc \
  stringtokenizer.h \
  opp_ctype.h \
  commonutil.h \
  exception.h \
  commondefs.h \
  stringutil.h
$O/unitconversion.o: unitconversion.cc \
  unitconversion.h \
  opp_ctype.h \
  commonutil.h \
  exception.h \
  commondefs.h \
  stringutil.h
