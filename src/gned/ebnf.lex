D                       [0-9]
L                       [a-zA-Z_]
E                       [Ee][+-]?{D}+

%{
/***************************************************/
/*       OMNeT++/OMNEST NEDC (JAR) source          */
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

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/*
 * IMPORTANT: This file is shared between NEDC and GNED.
 * The two copies must be kept identical!
 */


#if defined(__BORLANDC__) && !defined(__cplusplus)
#error Compile as C++ source!
#endif

#include <string.h>

#ifdef __BORLANDC__
#include <io.h>
#pragma warn -rch  /*turn off tons of 'Unreachable code' warnings --VA */
#endif

#ifdef _MSC_VER
#include <io.h>
#define isatty _isatty
#pragma warning(disable:4996) /* disable VC8.0 warnings on fileno(), etc */
#endif

#include "ebnf.h"
#include "ebnf.tab.h"
#include "ebnfcfg.h"  /* for NEDC() or GNED() */

#ifdef DOING_NEDC
#include "jar_func.h"
#endif
#ifdef DOING_GNED
#include "parsened.h"
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

void comment (void);
void count (void);

#define WARN(m) \
        sprintf (yyfailure, "! %s", m); \
        yyerror (""); \
        yyfailure [0] = 0;

/* Vars updated by count(): */
LineColumn pos,prevpos;

#define TEXTBUF_LEN 1024
char textbuf[TEXTBUF_LEN];

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
"xml"                   { count(); return (XMLTYPE); }
"anytype"               { count(); return (ANYTYPE); } /* --VA */


{L}({L}|{D})*           { count();
                                NEDC(yylval = strdup(yytext);)
                                GNED(yylval = 0;)
                                return (NAME); }
{D}+                    { count();
                                NEDC(yylval = strdup(yytext);)
                                GNED(yylval = 0;)
                                return (INTCONSTANT); }
{D}+{E}                 { count();
                                NEDC(yylval = strdup(yytext);)
                                GNED(yylval = 0;)
                                return (REALCONSTANT); }
{D}*"."{D}+({E})?       { count();
                                NEDC(yylval = strdup(yytext);)
                                GNED(yylval = 0;)
                                return (REALCONSTANT); }

\"[^\"]*\"              { count();
                                NEDC(yylval = strdup(yytext);)
                                GNED(yylval = 0;)
                                return (STRING); }

"++"                    { count(); return PLUSPLUS; }

";"                     { count(); return (';'); }
","                     { count(); return (','); }
":"                     { count(); return (':'); }
"="                     { count(); return ('='); }
"("                     { count(); return ('('); }
")"                     { count(); return (')'); }
"["                     { count(); return ('['); }
"]"                     { count(); return (']'); }
"."                     { count(); return ('.'); }
"?"                     { count(); return ('?'); } /* --VA */

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

"^"                     { count(); return (EXP); }
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

[ \t\v\n\r\f]           { count(); }
.                       { count(); return (INVALID_CHAR); }

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

/* the following #define is needed for broken flex versions */
#define yytext_ptr yytext

void comment(void)
{
        int c;
        while ((c = input())!='\n' && c!=0 && c!=EOF);
        if (c=='\n') unput(c);
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
                        textbuflen=0; textbuf[0]='\0';
                } else if (yytext[i] == '\t')
                        pos.co += 8 - (pos.co % 8);
                else {
                        pos.co++;
                }
                if (yytext[i] != '\n') {
                        if (textbuflen < TEXTBUF_LEN-5) {
                            textbuf[textbuflen++]=yytext[i]; textbuf[textbuflen]='\0';
                        }
                        else if (textbuflen == TEXTBUF_LEN-5) {
                            strcpy(textbuf+textbuflen, "...");
                            textbuflen++;
                        } else {
                            /* line too long -- ignore */
                        }
                }
        }
        /* printf("li=%d co=%d\n", pos.li, pos.co); good for debugging... */
        yylloc.first_line   = prevpos.li;
        yylloc.first_column = prevpos.co;
        yylloc.last_line    = pos.li;
        yylloc.last_column  = pos.co;

}

/***************************************************
* jar_yyrestart() -created by VA
***************************************************/
void jar_yyrestart(FILE *_yyin)
{
#ifdef FLEX_SCANNER
        yyrestart( _yyin );
#endif
}

