@rem *** just a dirty script for compiling on NT ***
bison -d -v ebnf.y
flex ebnf.lex

@rem Older versions of bison and flex generate filenames with underscores:
@rem copy ebnf_tab.c ebnf.tab.c
@rem copy ebnf_tab.h ebnf.tab.h
@rem copy lexyy.c lex.yy.c
