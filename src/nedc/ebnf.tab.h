/* A Bison parser, made from ebnf.y, by GNU bison 1.75.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef BISON_EBNF_TAB_H
# define BISON_EBNF_TAB_H

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INCLUDE = 258,
     SIMPLE = 259,
     CHANNEL = 260,
     DELAY = 261,
     ERROR = 262,
     DATARATE = 263,
     MODULE = 264,
     PARAMETERS = 265,
     GATES = 266,
     GATESIZES = 267,
     SUBMODULES = 268,
     CONNECTIONS = 269,
     DISPLAY = 270,
     IN = 271,
     OUT = 272,
     NOCHECK = 273,
     LEFT_ARROW = 274,
     RIGHT_ARROW = 275,
     FOR = 276,
     TO = 277,
     DO = 278,
     IF = 279,
     LIKE = 280,
     NETWORK = 281,
     ENDSIMPLE = 282,
     ENDMODULE = 283,
     ENDCHANNEL = 284,
     ENDNETWORK = 285,
     ENDFOR = 286,
     MACHINES = 287,
     ON = 288,
     IO_INTERFACES = 289,
     IFPAIR = 290,
     INTCONSTANT = 291,
     REALCONSTANT = 292,
     NAME = 293,
     STRING = 294,
     _TRUE = 295,
     _FALSE = 296,
     INPUT = 297,
     REF = 298,
     ANCESTOR = 299,
     NED_CONST = 300,
     NUMERICTYPE = 301,
     STRINGTYPE = 302,
     BOOLTYPE = 303,
     ANYTYPE = 304,
     PLUS = 305,
     MIN = 306,
     MUL = 307,
     DIV = 308,
     MOD = 309,
     EXP = 310,
     SIZEOF = 311,
     SUBMODINDEX = 312,
     EQ = 313,
     NE = 314,
     GT = 315,
     GE = 316,
     LS = 317,
     LE = 318,
     AND = 319,
     OR = 320,
     XOR = 321,
     NOT = 322,
     BIN_AND = 323,
     BIN_OR = 324,
     BIN_XOR = 325,
     BIN_COMPL = 326,
     SHIFT_LEFT = 327,
     SHIFT_RIGHT = 328,
     INVALID_CHAR = 329,
     UMIN = 330
   };
#endif
#define INCLUDE 258
#define SIMPLE 259
#define CHANNEL 260
#define DELAY 261
#define ERROR 262
#define DATARATE 263
#define MODULE 264
#define PARAMETERS 265
#define GATES 266
#define GATESIZES 267
#define SUBMODULES 268
#define CONNECTIONS 269
#define DISPLAY 270
#define IN 271
#define OUT 272
#define NOCHECK 273
#define LEFT_ARROW 274
#define RIGHT_ARROW 275
#define FOR 276
#define TO 277
#define DO 278
#define IF 279
#define LIKE 280
#define NETWORK 281
#define ENDSIMPLE 282
#define ENDMODULE 283
#define ENDCHANNEL 284
#define ENDNETWORK 285
#define ENDFOR 286
#define MACHINES 287
#define ON 288
#define IO_INTERFACES 289
#define IFPAIR 290
#define INTCONSTANT 291
#define REALCONSTANT 292
#define NAME 293
#define STRING 294
#define _TRUE 295
#define _FALSE 296
#define INPUT 297
#define REF 298
#define ANCESTOR 299
#define NED_CONST 300
#define NUMERICTYPE 301
#define STRINGTYPE 302
#define BOOLTYPE 303
#define ANYTYPE 304
#define PLUS 305
#define MIN 306
#define MUL 307
#define DIV 308
#define MOD 309
#define EXP 310
#define SIZEOF 311
#define SUBMODINDEX 312
#define EQ 313
#define NE 314
#define GT 315
#define GE 316
#define LS 317
#define LE 318
#define AND 319
#define OR 320
#define XOR 321
#define NOT 322
#define BIN_AND 323
#define BIN_OR 324
#define BIN_XOR 325
#define BIN_COMPL 326
#define SHIFT_LEFT 327
#define SHIFT_RIGHT 328
#define INVALID_CHAR 329
#define UMIN 330




#ifndef YYSTYPE
typedef int yystype;
# define YYSTYPE yystype
#endif

extern YYSTYPE yylval;

#ifndef YYLTYPE
typedef struct yyltype
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} yyltype;
# define YYLTYPE yyltype
#endif

extern YYLTYPE yylloc;

#endif /* not BISON_EBNF_TAB_H */

