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

#ifndef YY_NEDYY_NED_TAB_H_INCLUDED
# define YY_NEDYY_NED_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int nedyydebug;
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
    MODULE = 261,                  /* MODULE  */
    PARAMETERS = 262,              /* PARAMETERS  */
    GATES = 263,                   /* GATES  */
    GATESIZES = 264,               /* GATESIZES  */
    SUBMODULES = 265,              /* SUBMODULES  */
    CONNECTIONS = 266,             /* CONNECTIONS  */
    DISPLAY = 267,                 /* DISPLAY  */
    IN = 268,                      /* IN  */
    OUT = 269,                     /* OUT  */
    NOCHECK = 270,                 /* NOCHECK  */
    LEFT_ARROW = 271,              /* LEFT_ARROW  */
    RIGHT_ARROW = 272,             /* RIGHT_ARROW  */
    FOR = 273,                     /* FOR  */
    TO = 274,                      /* TO  */
    DO = 275,                      /* DO  */
    IF = 276,                      /* IF  */
    LIKE = 277,                    /* LIKE  */
    NETWORK = 278,                 /* NETWORK  */
    ENDSIMPLE = 279,               /* ENDSIMPLE  */
    ENDMODULE = 280,               /* ENDMODULE  */
    ENDCHANNEL = 281,              /* ENDCHANNEL  */
    ENDNETWORK = 282,              /* ENDNETWORK  */
    ENDFOR = 283,                  /* ENDFOR  */
    MACHINES = 284,                /* MACHINES  */
    ON = 285,                      /* ON  */
    CHANATTRNAME = 286,            /* CHANATTRNAME  */
    INTCONSTANT = 287,             /* INTCONSTANT  */
    REALCONSTANT = 288,            /* REALCONSTANT  */
    NAME = 289,                    /* NAME  */
    STRINGCONSTANT = 290,          /* STRINGCONSTANT  */
    CHARCONSTANT = 291,            /* CHARCONSTANT  */
    TRUE_ = 292,                   /* TRUE_  */
    FALSE_ = 293,                  /* FALSE_  */
    INPUT_ = 294,                  /* INPUT_  */
    XMLDOC = 295,                  /* XMLDOC  */
    REF = 296,                     /* REF  */
    ANCESTOR = 297,                /* ANCESTOR  */
    CONSTDECL = 298,               /* CONSTDECL  */
    NUMERICTYPE = 299,             /* NUMERICTYPE  */
    STRINGTYPE = 300,              /* STRINGTYPE  */
    BOOLTYPE = 301,                /* BOOLTYPE  */
    XMLTYPE = 302,                 /* XMLTYPE  */
    ANYTYPE = 303,                 /* ANYTYPE  */
    CPLUSPLUS = 304,               /* CPLUSPLUS  */
    CPLUSPLUSBODY = 305,           /* CPLUSPLUSBODY  */
    MESSAGE = 306,                 /* MESSAGE  */
    CLASS = 307,                   /* CLASS  */
    STRUCT = 308,                  /* STRUCT  */
    ENUM = 309,                    /* ENUM  */
    NONCOBJECT = 310,              /* NONCOBJECT  */
    EXTENDS = 311,                 /* EXTENDS  */
    FIELDS = 312,                  /* FIELDS  */
    PROPERTIES = 313,              /* PROPERTIES  */
    ABSTRACT = 314,                /* ABSTRACT  */
    READONLY = 315,                /* READONLY  */
    CHARTYPE = 316,                /* CHARTYPE  */
    SHORTTYPE = 317,               /* SHORTTYPE  */
    INTTYPE = 318,                 /* INTTYPE  */
    LONGTYPE = 319,                /* LONGTYPE  */
    DOUBLETYPE = 320,              /* DOUBLETYPE  */
    UNSIGNED_ = 321,               /* UNSIGNED_  */
    SIZEOF = 322,                  /* SIZEOF  */
    SUBMODINDEX = 323,             /* SUBMODINDEX  */
    PLUSPLUS = 324,                /* PLUSPLUS  */
    EQ = 325,                      /* EQ  */
    NE = 326,                      /* NE  */
    GT = 327,                      /* GT  */
    GE = 328,                      /* GE  */
    LS = 329,                      /* LS  */
    LE = 330,                      /* LE  */
    AND = 331,                     /* AND  */
    OR = 332,                      /* OR  */
    XOR = 333,                     /* XOR  */
    NOT = 334,                     /* NOT  */
    BIN_AND = 335,                 /* BIN_AND  */
    BIN_OR = 336,                  /* BIN_OR  */
    BIN_XOR = 337,                 /* BIN_XOR  */
    BIN_COMPL = 338,               /* BIN_COMPL  */
    SHIFT_LEFT = 339,              /* SHIFT_LEFT  */
    SHIFT_RIGHT = 340,             /* SHIFT_RIGHT  */
    INVALID_CHAR = 341,            /* INVALID_CHAR  */
    UMIN = 342                     /* UMIN  */
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


extern YYSTYPE nedyylval;
extern YYLTYPE nedyylloc;

int nedyyparse (void);


#endif /* !YY_NEDYY_NED_TAB_H_INCLUDED  */
