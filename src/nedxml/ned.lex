D                       [0-9]
L                       [a-zA-Z_]
E                       [Ee][+-]?{D}+

%{
/*==================================================
 * File: ned.lex
 *
 *  Lexical analyser for OMNeT++ NED.
 *
 *  Author: Andras Varga
 *
 *  Based on code from nedc.
 *
 ==================================================*/

/*--------------------------------------------------------------*
  Copyright (C) 1992,2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>

#if defined(__BORLANDC__)
# include <io.h>
# pragma warn -rch  /*turn off tons of 'Unreachable code' warnings --VA */
#elseif defined(_MSC_VER)
# include <io.h>
# define isatty _isatty
#else
extern "C" { int isatty(int); }
#endif

#include "nedgrammar.h"
#include "ned.tab.h"

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
char textbuf[256], lasttextbuf[256] = "";

%}

%%
"//"                    { comment(); }
"--"                    { comment(); }

"include"               { count(); return INCLUDE; }
"import"                { count(); return INCLUDE; }
"network"               { count(); return NETWORK; }
"module"                { count(); return MODULE; }
"simple"                { count(); return SIMPLE; }
"channel"               { count(); return CHANNEL; }
"for"                   { count(); return FOR; }
"do"                    { count(); return DO; }
"true"                  { count(); return TRUE_; }
"false"                 { count(); return FALSE_; }
"ref"                   { count(); return REF; }
"ancestor"              { count(); return ANCESTOR; }
"input"                 { count(); return INPUT_; }
"const"                 { count(); return CONSTDECL; }
"sizeof"                { count(); return SIZEOF; }
"endsimple"             { count(); return ENDSIMPLE; }
"endmodule"             { count(); return ENDMODULE; }
"endchannel"            { count(); return ENDCHANNEL; }
"endnetwork"            { count(); return ENDNETWORK; }
"endfor"                { count(); return ENDFOR; }
"parameters"            { count(); return PARAMETERS; }
"gates"                 { count(); return GATES; }
"gatesizes"             { count(); return GATESIZES; }
"in:"                   { count(); return IN; }
"out:"                  { count(); return OUT; }
"submodules"            { count(); return SUBMODULES; }
"connections"           { count(); return CONNECTIONS; }
"display"               { count(); return DISPLAY; }
"on"                    { count(); return ON; }          /* --LG */
"like"                  { count(); return LIKE; }
"machines"              { count(); return MACHINES; }         /* --LG */
"io-interfaces"         { count(); return IO_INTERFACES; }    /* --LG */
"ifpair"                { count(); return IFPAIR; }    /* --LG */
"-->"                   { count(); return RIGHT_ARROW; }
"<--"                   { count(); return LEFT_ARROW; }
".."                    { count(); return TO; }
"..."                   { count(); return TO; }
"to"                    { count(); return TO; }
"if"                    { count(); return IF; }
"index"                 { count(); return SUBMODINDEX; }
"nocheck"               { count(); return NOCHECK; }

"numeric"               { count(); return NUMERICTYPE; }
"string"                { count(); return STRINGTYPE; }
"bool"                  { count(); return BOOLTYPE; }
"anytype"               { count(); return ANYTYPE; }

"cppinclude"            { count(); return CPPINCLUDE; }
"struct"                { count(); return STRUCT; }
"cobject"               { count(); return COBJECT; }
"noncobject"            { count(); return NONCOBJECT; }
"enum"                  { count(); return ENUM; }
"extends"               { count(); return EXTENDS; }
"message"               { count(); return MESSAGE; }
"class"                 { count(); return CLASS; }
"fields"                { count(); return FIELDS; }
"properties"            { count(); return PROPERTIES; }
"abstract"              { count(); return ABSTRACT; }

"char"                  { count(); return CHARTYPE; }
"short"                 { count(); return SHORTTYPE; }
"int"                   { count(); return INTTYPE; }
"long"                  { count(); return LONGTYPE; }
"double"                { count(); return DOUBLETYPE; }

{L}({L}|{D})*           { count(); return NAME; }
{D}+                    { count(); return INTCONSTANT; }
{D}+{E}                 { count(); return REALCONSTANT; }
{D}*"."{D}+({E})?       { count(); return REALCONSTANT; }

\"[^\"]*\"              { count(); return STRINGCONSTANT; }
\'[^\']\'               { count(); return CHARCONSTANT; }
"<"[^ \t\n\>]*">"       { count(); return SYSINCFILENAME; }

";"                     { count(); return (';'); }
","                     { count(); return (','); }
":"                     { count(); return (':'); }
"="                     { count(); return ('='); }
"("                     { count(); return ('('); }
")"                     { count(); return (')'); }
"["                     { count(); return ('['); }
"]"                     { count(); return (']'); }
"{"                     { count(); return ('{'); }
"}"                     { count(); return ('}'); }
"."                     { count(); return ('.'); }
"?"                     { count(); return ('?'); }

"||"                    { count(); return OR; }
"&&"                    { count(); return AND; }
"##"                    { count(); return XOR; }
"!"                     { count(); return NOT; }

"|"                     { count(); return BIN_OR; }
"&"                     { count(); return BIN_AND; }
"#"                     { count(); return BIN_XOR; }
"~"                     { count(); return BIN_COMPL; }
"<<"                    { count(); return SHIFT_LEFT; }
">>"                    { count(); return SHIFT_RIGHT; }

"^"                     { count(); return '^'; }
"+"                     { count(); return '+'; }
"-"                     { count(); return '-'; }
"*"                     { count(); return '*'; }
"/"                     { count(); return '/'; }
"%"                     { count(); return '%'; }

"=="                    { count(); return EQ; }
"!="                    { count(); return NE; }
"<"                     { count(); return LS; }
"<="                    { count(); return LE; }
">"                     { count(); return GT; }
">="                    { count(); return GE; }

[ \t\v\n\r\f]           { count(); }
.                       { count(); return INVALID_CHAR; }

%%

/***************************************************
* yywrap() -created 11 july 1995 alex paalvast
*    used to signal end of inputstream
*    not used with flex (only with lex) --VA
***************************************************/
#ifndef yywrap
int yywrap(void)
{
         return 1;
}
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
        if (yy_current_buffer)
              yyrestart( _yyin );
#endif
}

