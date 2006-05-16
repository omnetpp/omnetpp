/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INCLUDE = 258,
     SIMPLE = 259,
     CHANNEL = 260,
     MODULE = 261,
     PARAMETERS = 262,
     GATES = 263,
     GATESIZES = 264,
     SUBMODULES = 265,
     CONNECTIONS = 266,
     DISPLAY = 267,
     IN = 268,
     OUT = 269,
     NOCHECK = 270,
     LEFT_ARROW = 271,
     RIGHT_ARROW = 272,
     FOR = 273,
     TO = 274,
     DO = 275,
     IF = 276,
     LIKE = 277,
     NETWORK = 278,
     ENDSIMPLE = 279,
     ENDMODULE = 280,
     ENDCHANNEL = 281,
     ENDNETWORK = 282,
     ENDFOR = 283,
     MACHINES = 284,
     ON = 285,
     CHANATTRNAME = 286,
     INTCONSTANT = 287,
     REALCONSTANT = 288,
     NAME = 289,
     STRINGCONSTANT = 290,
     CHARCONSTANT = 291,
     TRUE_ = 292,
     FALSE_ = 293,
     INPUT_ = 294,
     XMLDOC = 295,
     REF = 296,
     ANCESTOR = 297,
     CONSTDECL = 298,
     NUMERICTYPE = 299,
     STRINGTYPE = 300,
     BOOLTYPE = 301,
     XMLTYPE = 302,
     ANYTYPE = 303,
     CPLUSPLUS = 304,
     CPLUSPLUSBODY = 305,
     MESSAGE = 306,
     CLASS = 307,
     STRUCT = 308,
     ENUM = 309,
     NONCOBJECT = 310,
     EXTENDS = 311,
     FIELDS = 312,
     PROPERTIES = 313,
     ABSTRACT = 314,
     READONLY = 315,
     CHARTYPE = 316,
     SHORTTYPE = 317,
     INTTYPE = 318,
     LONGTYPE = 319,
     DOUBLETYPE = 320,
     UNSIGNED_ = 321,
     SIZEOF = 322,
     SUBMODINDEX = 323,
     PLUSPLUS = 324,
     EQ = 325,
     NE = 326,
     GT = 327,
     GE = 328,
     LS = 329,
     LE = 330,
     AND = 331,
     OR = 332,
     XOR = 333,
     NOT = 334,
     BIN_AND = 335,
     BIN_OR = 336,
     BIN_XOR = 337,
     BIN_COMPL = 338,
     SHIFT_LEFT = 339,
     SHIFT_RIGHT = 340,
     INVALID_CHAR = 341,
     UMIN = 342
   };
#endif
/* Tokens.  */
#define INCLUDE 258
#define SIMPLE 259
#define CHANNEL 260
#define MODULE 261
#define PARAMETERS 262
#define GATES 263
#define GATESIZES 264
#define SUBMODULES 265
#define CONNECTIONS 266
#define DISPLAY 267
#define IN 268
#define OUT 269
#define NOCHECK 270
#define LEFT_ARROW 271
#define RIGHT_ARROW 272
#define FOR 273
#define TO 274
#define DO 275
#define IF 276
#define LIKE 277
#define NETWORK 278
#define ENDSIMPLE 279
#define ENDMODULE 280
#define ENDCHANNEL 281
#define ENDNETWORK 282
#define ENDFOR 283
#define MACHINES 284
#define ON 285
#define CHANATTRNAME 286
#define INTCONSTANT 287
#define REALCONSTANT 288
#define NAME 289
#define STRINGCONSTANT 290
#define CHARCONSTANT 291
#define TRUE_ 292
#define FALSE_ 293
#define INPUT_ 294
#define XMLDOC 295
#define REF 296
#define ANCESTOR 297
#define CONSTDECL 298
#define NUMERICTYPE 299
#define STRINGTYPE 300
#define BOOLTYPE 301
#define XMLTYPE 302
#define ANYTYPE 303
#define CPLUSPLUS 304
#define CPLUSPLUSBODY 305
#define MESSAGE 306
#define CLASS 307
#define STRUCT 308
#define ENUM 309
#define NONCOBJECT 310
#define EXTENDS 311
#define FIELDS 312
#define PROPERTIES 313
#define ABSTRACT 314
#define READONLY 315
#define CHARTYPE 316
#define SHORTTYPE 317
#define INTTYPE 318
#define LONGTYPE 319
#define DOUBLETYPE 320
#define UNSIGNED_ 321
#define SIZEOF 322
#define SUBMODINDEX 323
#define PLUSPLUS 324
#define EQ 325
#define NE 326
#define GT 327
#define GE 328
#define LS 329
#define LE 330
#define AND 331
#define OR 332
#define XOR 333
#define NOT 334
#define BIN_AND 335
#define BIN_OR 336
#define BIN_XOR 337
#define BIN_COMPL 338
#define SHIFT_LEFT 339
#define SHIFT_RIGHT 340
#define INVALID_CHAR 341
#define UMIN 342




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE nedyylval;

#if ! defined (YYLTYPE) && ! defined (YYLTYPE_IS_DECLARED)
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE nedyylloc;


