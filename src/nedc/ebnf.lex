D                       [0-9]
L                       [a-zA-Z_]
E                       [Ee][+-]?{D}+

%{
/***************************************************/
/*            OMNeT++ NEDC (JAR) source            */
/*                                                 */
/*  File: ebnf.lex                                 */
/*                                                 */
/*  Contents:                                      */
/*    lexical analyzer source                      */
/*                                                 */
/*  By: Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*  Edited: Andras Varga 1996-98                   */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#if defined(__BORLANDC__) && !defined(__cplusplus)
#error Compile as C++ source!
/*-- You'll also need to remove the stupid '#include <osfcn.h>' line from
     the generated lexyy.c --VA */
#endif

#include <string.h>

#ifdef __BORLANDC__
#include <io.h>
#pragma warn -rch  /*turn off tons of 'Unreachable code' warnings --VA */
#endif

/* define YYSTYPE to 'char *'; must be consistent in ebnf.lex/ebnf.y --VA */
#define YYSTYPE   char *

#ifdef __MSDOS__
#include "ebnf_tab.h"
#else
#include "ebnf.tab.h"
#endif

#ifdef DOING_NEDC
#include "jar_func.h"
#endif

extern YYSTYPE yylval;

void comment (void);
void count (void);

#define WARN(m) \
        sprintf (yyfailure, "! %s", m); \
        yyerror (""); \
        yyfailure [0] = 0;

/* beware: this typedef is replicated in ebnf.y */
typedef struct {int li; int co;} LineColumn;

/* Vars updated by count(): */
LineColumn pos,prevpos;
char textbuf[256], lasttextbuf[256] = "";

%}

%%
"//"                    { comment(); }
"--"                    { comment(); }

"include"               { count(); return (INCLUDE); }
"import"                { count(); return (INCLUDE); }
"network"               { count(); return (NETWORK); }
"module"                { count(); return (MODULE); }
"simple"                { count(); return (SIMPLE); }
"channel"               { count(); return (CHANNEL); }
"delay"                 { count(); return (DELAY); }
"error"                 { count(); return (ERROR); }
"datarate"              { count(); return (DATARATE); }
"for"                   { count(); return (FOR); }
"do"                    { count(); return (DO); }
"true"                  { count(); return (_TRUE); }
"false"                 { count(); return (_FALSE); }
"ref"                   { count(); return (REF); }
 /*"byvalue"               { count(); return (BYVALUE); }  --VA*/
"ancestor"              { count(); return (ANCESTOR); }
"input"                 { count(); return (INPUT); }
"const"                 { count(); return (NED_CONST); }
"sizeof"                { count(); return (SIZEOF); }  /* --VA*/
"endsimple"             { count(); return (ENDSIMPLE); }
"endmodule"             { count(); return (ENDMODULE); }
"endchannel"            { count(); return (ENDCHANNEL); }
  /*"endgates"              { count(); return (ENDGATES); } --VA*/
"endnetwork"            { count(); return (ENDNETWORK); }
"endfor"                { count(); return (ENDFOR); }
  /*"endsubmodules"         { count(); return (ENDSUBMODULES); } --VA*/
  /*"endconnections"        { count(); return (ENDCONNECTIONS); } --VA*/
"parameters"            { count(); return (PARAMETERS); }
"gates"                 { count(); return (GATES); }
"gatesizes"             { count(); return (GATESIZES); }  /* --VA*/
"in:"                   { count(); return (IN); }
"out:"                  { count(); return (OUT); }
"submodules"            { count(); return (SUBMODULES); }
"connections"           { count(); return (CONNECTIONS); }
"display"               { count(); return (DISPLAY); }
"on"                    { count(); return (ON); }          /* --LG */
"like"                  { count(); return (LIKE); }
"machines"              { count(); return (MACHINES); }         /* --LG */
"io-interfaces"         { count(); return (IO_INTERFACES); }    /* --LG */
"ifpair"                { count(); return (IFPAIR); }    /* --LG */
"-->"                   { count(); return (RIGHT_ARROW); }
"<--"                   { count(); return (LEFT_ARROW); } /* --VA */
".."                    { count(); return (TO); }
"..."                   { count(); return (TO); }
"to"                    { count(); return (TO); }  /* --VA */
"if"                    { count(); return (IF); }  /* --VA */
"index"                 { count(); return (SUBMODINDEX); }  /* --VA */
"nocheck"               { count(); return (NOCHECK); }  /* --VA */

"numeric"               { count(); return (NUMERICTYPE); } /* --VA */
"string"                { count(); return (STRINGTYPE); } /* --VA */
"bool"                  { count(); return (BOOLTYPE); } /* --VA */
"anytype"               { count(); return (ANYTYPE); } /* --VA */


{L}({L}|{D})*           { count();
                                yylval = strdup (yytext);
                                return (NAME); }
{D}+                    { count();
                                yylval = strdup (yytext);
                                return (INTCONSTANT); }
{D}+{E}                 { count();
                                yylval = strdup (yytext);
                                return (REALCONSTANT); }
{D}*"."{D}+({E})?       { count();
                                yylval = strdup (yytext);
                                return (REALCONSTANT); }

\"[^\"]*\"              { count();
                                yylval = strdup (yytext);
                                return (STRING); }

";"                     { count(); return (';'); }
","                     { count(); return (','); }
":"                     { count(); return (':'); }
"="                     { count(); return ('='); }
"("                     { count(); return ('('); }
")"                     { count(); return (')'); }
"["                     { count(); return ('['); }
"]"                     { count(); return (']'); }
"."                     { count(); return ('.'); }

"||"                    { count(); return (OR); }  /* --VA */
"&&"                    { count(); return (AND); } /* --VA */
"##"                    { count(); return (XOR); } /* --VA */
"!"                     { count(); return (NOT); } /* --VA */

"|"                     { count(); return (BIN_OR); }  /* --VA */
"&"                     { count(); return (BIN_AND); } /* --VA */
"#"                     { count(); return (BIN_XOR); } /* --VA */
"~"                     { count(); return (BIN_COMPL); } /* --VA */
"<<"                    { count(); return (SHIFT_LEFT); } /* --VA */
">>"                    { count(); return (SHIFT_RIGHT); } /* --VA */

"^"                     { count(); return (EXP); }  /* --VA */
"+"                     { count(); return (PLUS); }
"-"                     { count(); return (MIN); }
"*"                     { count(); return (MUL); }
"/"                     { count(); return (DIV); }
"%"                     { count(); return (MOD); }

"=="                    { count(); return (EQ); }  /* --VA */
"!="                    { count(); return (NE); }  /* --VA */
"<"                     { count(); return (LS); }  /* --VA */
"<="                    { count(); return (LE); }  /* --VA */
">"                     { count(); return (GT); }  /* --VA */
">="                    { count(); return (GE); }  /* --VA */

[ \t\v\n\f]             { count(); }
.                       { count(); /* ignore bad characters */ }

%%

/***************************************************
* yywrap() -created 11 july 1995 alex paalvast
*           used to signal end of inputstream
* - should be: just return(1)
***************************************************/
#ifndef yywrap
int yywrap(void)
{
         return 1;
}            /* commented out for use with flex --VA */
#endif

/***************************************************
* comment() -created 6 july 1995 alex paalvast
* - discards all remaining characters of a line of
*   text from the inputstream.
* - the characters are read with the input() and
*   unput() functions.
* - input() is sometimes called yyinput()...
***************************************************/
#ifdef __cplusplus
#define input  yyinput
#endif
void comment(void)
{
        char c;
        while ((c = input()) != '\n' && c != 0);
        unput(c);
}

/***************************************************
* count() -created 6 july 1995 alex paalvast; edited by VA
* - counts the line and column number of the current token in `pos'
* - keeps a record of the complete current line in `textbuf[]'
* - yytext[] is the current token passed by (f)lex
***************************************************/

void count(void)
{
        static int textbuflen;
        int i;

        /* init textbuf */
        if (pos.li==1 && pos.co==0) {textbuf[0]='\0'; textbuflen=0;}

        prevpos = pos;
        for (i = 0; yytext[i] != '\0'; i++) {
                if (yytext[i] == '\n') {
                        pos.co = 0;
                        pos.li++;
                        strcpy (lasttextbuf, textbuf);
                        textbuflen=0; textbuf[0]='\0';
                } else if (yytext[i] == '\t')
                        pos.co += 8 - (pos.co % 8);
                else
                        pos.co++;
                if (yytext[i] != '\n') {
                        textbuf[textbuflen++]=yytext[i]; textbuf[textbuflen]='\0';
                }
        }
        /* printf("li=%d co=%d\n", pos.li, pos.co); good for debugging... */
}

/***************************************************
* jar_yyrestart() -created by VA
***************************************************/
void jar_yyrestart(FILE *_yyin)
{
#ifdef FLEX_SCANNER
        if (yy_current_buffer)
              yyrestart( _yyin );
#endif
}
