/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_EBNF_TAB_H_INCLUDED
# define YY_YY_EBNF_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INCLUDE = 258,                 /* INCLUDE  */
    SIMPLE = 259,                  /* SIMPLE  */
    CHANNEL = 260,                 /* CHANNEL  */
    DELAY = 261,                   /* DELAY  */
    ERROR = 262,                   /* ERROR  */
    DATARATE = 263,                /* DATARATE  */
    MODULE = 264,                  /* MODULE  */
    PARAMETERS = 265,              /* PARAMETERS  */
    GATES = 266,                   /* GATES  */
    GATESIZES = 267,               /* GATESIZES  */
    SUBMODULES = 268,              /* SUBMODULES  */
    CONNECTIONS = 269,             /* CONNECTIONS  */
    DISPLAY = 270,                 /* DISPLAY  */
    IN = 271,                      /* IN  */
    OUT = 272,                     /* OUT  */
    NOCHECK = 273,                 /* NOCHECK  */
    LEFT_ARROW = 274,              /* LEFT_ARROW  */
    RIGHT_ARROW = 275,             /* RIGHT_ARROW  */
    FOR = 276,                     /* FOR  */
    TO = 277,                      /* TO  */
    DO = 278,                      /* DO  */
    IF = 279,                      /* IF  */
    LIKE = 280,                    /* LIKE  */
    NETWORK = 281,                 /* NETWORK  */
    ENDSIMPLE = 282,               /* ENDSIMPLE  */
    ENDMODULE = 283,               /* ENDMODULE  */
    ENDCHANNEL = 284,              /* ENDCHANNEL  */
    ENDNETWORK = 285,              /* ENDNETWORK  */
    ENDFOR = 286,                  /* ENDFOR  */
    MACHINES = 287,                /* MACHINES  */
    ON = 288,                      /* ON  */
    IO_INTERFACES = 289,           /* IO_INTERFACES  */
    IFPAIR = 290,                  /* IFPAIR  */
    INTCONSTANT = 291,             /* INTCONSTANT  */
    REALCONSTANT = 292,            /* REALCONSTANT  */
    NAME = 293,                    /* NAME  */
    STRING = 294,                  /* STRING  */
    _TRUE = 295,                   /* _TRUE  */
    _FALSE = 296,                  /* _FALSE  */
    INPUT = 297,                   /* INPUT  */
    REF = 298,                     /* REF  */
    ANCESTOR = 299,                /* ANCESTOR  */
    NED_CONST = 300,               /* NED_CONST  */
    NUMERICTYPE = 301,             /* NUMERICTYPE  */
    STRINGTYPE = 302,              /* STRINGTYPE  */
    BOOLTYPE = 303,                /* BOOLTYPE  */
    XMLTYPE = 304,                 /* XMLTYPE  */
    ANYTYPE = 305,                 /* ANYTYPE  */
    PLUS = 306,                    /* PLUS  */
    MIN = 307,                     /* MIN  */
    MUL = 308,                     /* MUL  */
    DIV = 309,                     /* DIV  */
    MOD = 310,                     /* MOD  */
    EXP = 311,                     /* EXP  */
    SIZEOF = 312,                  /* SIZEOF  */
    SUBMODINDEX = 313,             /* SUBMODINDEX  */
    PLUSPLUS = 314,                /* PLUSPLUS  */
    EQ = 315,                      /* EQ  */
    NE = 316,                      /* NE  */
    GT = 317,                      /* GT  */
    GE = 318,                      /* GE  */
    LS = 319,                      /* LS  */
    LE = 320,                      /* LE  */
    AND = 321,                     /* AND  */
    OR = 322,                      /* OR  */
    XOR = 323,                     /* XOR  */
    NOT = 324,                     /* NOT  */
    BIN_AND = 325,                 /* BIN_AND  */
    BIN_OR = 326,                  /* BIN_OR  */
    BIN_XOR = 327,                 /* BIN_XOR  */
    BIN_COMPL = 328,               /* BIN_COMPL  */
    SHIFT_LEFT = 329,              /* SHIFT_LEFT  */
    SHIFT_RIGHT = 330,             /* SHIFT_RIGHT  */
    INVALID_CHAR = 331,            /* INVALID_CHAR  */
    UMIN = 332                     /* UMIN  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
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
#define XMLTYPE 304
#define ANYTYPE 305
#define PLUS 306
#define MIN 307
#define MUL 308
#define DIV 309
#define MOD 310
#define EXP 311
#define SIZEOF 312
#define SUBMODINDEX 313
#define PLUSPLUS 314
#define EQ 315
#define NE 316
#define GT 317
#define GE 318
#define LS 319
#define LE 320
#define AND 321
#define OR 322
#define XOR 323
#define NOT 324
#define BIN_AND 325
#define BIN_OR 326
#define BIN_XOR 327
#define BIN_COMPL 328
#define SHIFT_LEFT 329
#define SHIFT_RIGHT 330
#define INVALID_CHAR 331
#define UMIN 332

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_EBNF_TAB_H_INCLUDED  */
