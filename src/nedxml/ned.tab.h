/* A Bison parser, made from ned.y, by GNU bison 1.75.  */

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

#ifndef BISON_NED_TAB_H
# define BISON_NED_TAB_H

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
     IO_INTERFACES = 286,
     IFPAIR = 287,
     INTCONSTANT = 288,
     REALCONSTANT = 289,
     NAME = 290,
     STRINGCONSTANT = 291,
     CHARCONSTANT = 292,
     TRUE_ = 293,
     FALSE_ = 294,
     INPUT_ = 295,
     REF = 296,
     ANCESTOR = 297,
     CONSTDECL = 298,
     NUMERICTYPE = 299,
     STRINGTYPE = 300,
     BOOLTYPE = 301,
     ANYTYPE = 302,
     CPPINCLUDE = 303,
     SYSINCFILENAME = 304,
     STRUCT = 305,
     COBJECT = 306,
     NONCOBJECT = 307,
     ENUM = 308,
     EXTENDS = 309,
     MESSAGE = 310,
     CLASS = 311,
     FIELDS = 312,
     PROPERTIES = 313,
     ABSTRACT = 314,
     CHARTYPE = 315,
     SHORTTYPE = 316,
     INTTYPE = 317,
     LONGTYPE = 318,
     DOUBLETYPE = 319,
     SIZEOF = 320,
     SUBMODINDEX = 321,
     EQ = 322,
     NE = 323,
     GT = 324,
     GE = 325,
     LS = 326,
     LE = 327,
     AND = 328,
     OR = 329,
     XOR = 330,
     NOT = 331,
     BIN_AND = 332,
     BIN_OR = 333,
     BIN_XOR = 334,
     BIN_COMPL = 335,
     SHIFT_LEFT = 336,
     SHIFT_RIGHT = 337,
     INVALID_CHAR = 338,
     UMIN = 339
   };
#endif
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
#define IO_INTERFACES 286
#define IFPAIR 287
#define INTCONSTANT 288
#define REALCONSTANT 289
#define NAME 290
#define STRINGCONSTANT 291
#define CHARCONSTANT 292
#define TRUE_ 293
#define FALSE_ 294
#define INPUT_ 295
#define REF 296
#define ANCESTOR 297
#define CONSTDECL 298
#define NUMERICTYPE 299
#define STRINGTYPE 300
#define BOOLTYPE 301
#define ANYTYPE 302
#define CPPINCLUDE 303
#define SYSINCFILENAME 304
#define STRUCT 305
#define COBJECT 306
#define NONCOBJECT 307
#define ENUM 308
#define EXTENDS 309
#define MESSAGE 310
#define CLASS 311
#define FIELDS 312
#define PROPERTIES 313
#define ABSTRACT 314
#define CHARTYPE 315
#define SHORTTYPE 316
#define INTTYPE 317
#define LONGTYPE 318
#define DOUBLETYPE 319
#define SIZEOF 320
#define SUBMODINDEX 321
#define EQ 322
#define NE 323
#define GT 324
#define GE 325
#define LS 326
#define LE 327
#define AND 328
#define OR 329
#define XOR 330
#define NOT 331
#define BIN_AND 332
#define BIN_OR 333
#define BIN_XOR 334
#define BIN_COMPL 335
#define SHIFT_LEFT 336
#define SHIFT_RIGHT 337
#define INVALID_CHAR 338
#define UMIN 339




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

#endif /* not BISON_NED_TAB_H */

