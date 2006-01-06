D  [0-9]
L  [a-zA-Z_]
X  [0-9a-fA-F]
E  [Ee][+-]?{D}+
S  [ \t\v\n\r\f]

%x cplusplusbody


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

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>

#if defined(__BORLANDC__)
# include <io.h>
# pragma warn -rch  /*turn off tons of 'Unreachable code' warnings --VA */
#elif defined(_MSC_VER)
# include <io.h>
# define isatty _isatty
# pragma warning(disable:4996) /* disable VC8.0 warnings on fileno(), etc */
#else
//extern "C" { int isatty(int); }
# include <unistd.h>
#endif

#include "nedgrammar.h"
#include "ned.tab.h"

extern YYSTYPE yylval;
extern YYLTYPE yylloc;

void comment(void);
void count(void);
void extendCount(void);

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

"import"                { count(); use_chanattrname_token = 0; return INCLUDE; }
"network"               { count(); use_chanattrname_token = 0; return NETWORK; }
"module"                { count(); use_chanattrname_token = 0; return MODULE; }
"simple"                { count(); use_chanattrname_token = 0; return SIMPLE; }
"channel"               { count(); use_chanattrname_token = 1; return CHANNEL; }
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
"on"                    { count(); return ON; }
"like"                  { count(); return LIKE; }
"-->"                   { count(); return RIGHT_ARROW; }
"<--"                   { count(); return LEFT_ARROW; }
".."                    { count(); return TO; }
"..."                   { count(); return TO; }
"to"                    { count(); return TO; }
"if"                    { count(); return IF; }
"index"                 { count(); return SUBMODINDEX; }
"nocheck"               { count(); return NOCHECK; }
"xmldoc"                { count(); return XMLDOC; }

"numeric"               { count(); return NUMERICTYPE; }
"string"                { count(); return STRINGTYPE; }
"bool"                  { count(); return BOOLTYPE; }
"xml"                   { count(); return XMLTYPE; }
"anytype"               { count(); return ANYTYPE; }

"cplusplus"             { count(); use_chanattrname_token = 0; return CPLUSPLUS; }
"struct"                { count(); use_chanattrname_token = 0; return STRUCT; }
"message"               { count(); use_chanattrname_token = 0; return MESSAGE; }
"class"                 { count(); use_chanattrname_token = 0; return CLASS; }
"noncobject"            { count(); use_chanattrname_token = 0; return NONCOBJECT; }
"enum"                  { count(); use_chanattrname_token = 0; return ENUM; }
"extends"               { count(); return EXTENDS; }
"fields"                { count(); return FIELDS; }
"properties"            { count(); return PROPERTIES; }
"abstract"              { count(); return ABSTRACT; }
"readonly"              { count(); return READONLY; }

"char"                  { count(); return CHARTYPE; }
"short"                 { count(); return SHORTTYPE; }
"int"                   { count(); return INTTYPE; }
"long"                  { count(); return LONGTYPE; }
"double"                { count(); return DOUBLETYPE; }
"unsigned"              { count(); return UNSIGNED_; }

"delay"                 { count(); return use_chanattrname_token ? CHANATTRNAME : NAME; }
"error"                 { count(); return use_chanattrname_token ? CHANATTRNAME : NAME; }
"datarate"              { count(); return use_chanattrname_token ? CHANATTRNAME : NAME; }

{L}({L}|{D})*           { count(); return NAME; }
{D}+                    { count(); return INTCONSTANT; }
0[xX]{X}+               { count(); return INTCONSTANT; }
{D}+{E}                 { count(); return REALCONSTANT; }
{D}*"."{D}+({E})?       { count(); return REALCONSTANT; }

\"[^\"]*\"              { count(); return STRINGCONSTANT; }
\'[^\']\'               { count(); return CHARCONSTANT; }

"{{"                    { count(); BEGIN(cplusplusbody); }
<cplusplusbody>"}}"     { extendCount(); BEGIN(INITIAL); return CPLUSPLUSBODY; }
<cplusplusbody>{S}      { extendCount(); }
<cplusplusbody>.        { extendCount(); }

"++"                    { count(); return PLUSPLUS; }

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

{S}                     { count(); }
.                       { count(); return INVALID_CHAR; }

%%

/***************************************************
* yywrap() -created 11 july 1995 alex paalvast
*    used to signal end of inputstream
*    not used with flex (only with lex) --VA
***************************************************/
//#ifndef yywrap
int yywrap(void)
{
         return 1;
}
//#endif

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

void _count(int updateprevpos)
{
        static int textbuflen;
        int i;

        /* printf("DBG: count(): prev=%d,%d  pos=%d,%d yytext=>>%s<<\n",
               prevpos.li, prevpos.co, pos.li, pos.co, yytext);
        */

        /* init textbuf */
        if (pos.li==1 && pos.co==0) {
                textbuf[0]='\0'; textbuflen=0;
        }

        if (updateprevpos) {
                prevpos = pos;
        }
        for (i = 0; yytext[i] != '\0'; i++) {
                if (yytext[i] == '\n') {
                        pos.co = 0;
                        pos.li++;
                        textbuflen=0; textbuf[0]='\0';
                } else if (yytext[i] == '\t')
                        pos.co += 8 - (pos.co % 8);
                else
                        pos.co++;
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

void count(void)
{
    _count(1);
}

void extendCount(void)
{
    _count(0);
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

