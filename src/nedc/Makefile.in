#*************************************************#
#	   OMNET++ JAR				  #
#						  #
#    Makefile to build NEDC (formerly JAR)	  #
#						  #
#    By: Jan Heijmans				  #
#	 Alex Paalvast				  #
#	 Robert van der Leij			  #
#    Edited: Andras Varga 1996-98		  #
#*************************************************#

# Definitions to be customized by configure
CCC=gcc -c -g
LD=gcc
YACC=bison
LEX=flex

# Local definitions

# Platform-dependent filenames, adjust if necessary.
#   UNIX, Win95/NT (Cygnus gcc):  ebnf.tab.*, lex.yy.*
#   MS-DOS (Djgpp):		  ebnf_tab.*, lexyy.*

# ebnf_tab=ebnf_tab
# lexyy=lexyy
ebnf_tab=ebnf.tab
lexyy=lex.yy

nedc: $(ebnf_tab).o $(lexyy).o jar.o jar_lib.o jar_mod.o jar_expr.o jar_mem.o
	$(LD) $(lexyy).o $(ebnf_tab).o jar.o jar_lib.o jar_mod.o jar_expr.o jar_mem.o -o nedc

$(lexyy).o: jar_func.h $(lexyy).c
	$(CCC) $(lexyy).c

$(ebnf_tab).o: jar_func.h $(ebnf_tab).h $(ebnf_tab).c
	$(CCC) $(ebnf_tab).c

$(ebnf_tab).h: ebnf.y
	$(YACC) -b ebnf -d ebnf.y

$(ebnf_tab).c: ebnf.y
	$(YACC) -b ebnf -d ebnf.y

$(lexyy).c: ebnf.lex
	$(LEX) ebnf.lex

jar.o: jar_lib.h jar_func.h jar.c
	$(CCC) jar.c

jar_lib.o: jar_lib.h jar_func.h jar_lib.c
	$(CCC) jar_lib.c

jar_mod.o: jar_lib.h jar_func.h jar_mod.c
	$(CCC) jar_mod.c

jar_expr.o: jar_lib.h jar_func.h jar_expr.c
	$(CCC) jar_expr.c

jar_mem.o: jar_func.h jar_mem.c
	$(CCC) jar_mem.c

