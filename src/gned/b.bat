rem *** just a dirty script for compiling on NT ***
bison -d -v ebnf.y
copy ebnf_tab.c ebnf.tab.c
copy ebnf_tab.h ebnf.tab.h
flex ebnf.lex
copy lexyy.c lex.yy.c
