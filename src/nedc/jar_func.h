/***************************************************/
/*                OMNeT++ JAR source               */
/*                                                 */
/*  File: jar_func.h                               */
/*                                                 */
/*  Contents:                                      */
/*    Functions called by the parser               */
/*                                                 */
/*  By: Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*  Edited: Andras Varga 1996-2001                 */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>  /* FILE */

/*--- for internal handling of expressions -------------------------*/
#define EXPR_USE(s)    (s[0])
#define EXPR_TYPE(s)   (s[1])
#define EXPR_STR(s)    (s+2)
#define EXPR_PREFIX    "LN"

/* to EXPR_USE: */
#define USE_LITERAL    'L'    /* like "1", "i_loop" */
#define USE_VALUE      'V'    /* result is in "value" cPar object */
#define USE_TABLE      'T'    /* expr_tab[] style */

/* to EXPR_TYPE: */
#define TYPE_NUMERIC   'N'
#define TYPE_CONST_NUM 'C'
#define TYPE_STRING    'S'
#define TYPE_BOOL      'B'
#define TYPE_ANYTYPE   'A'


/*--- Functions called by the parser -------------------------*/

/*--- include ---*/
void do_include (char *fname);

/*--- channels ---*/
void do_channel (char *cname, char *delay, char *error, char *datarate );

/*--- machines ---*/
/*void do_physicalmachine (char *pm);*/
/*void do_addvirtualmachine (char *vm);*/

/*--- modules, gates, parameters ---*/

/* begin a simple or compound module */
void do_module (char *mname);
void do_simple (char *sname);

/* machine declaration */
void do_machine (char *maname);

/* parameter declarations */
void do_parameters (void);
void do_parameter (char *pname, char type);

/* gate declarations */
void do_gates (void); /* --VA */
void do_gatedecl (char *gname, int is_in, int is_vector ); /* --VA */

/* submodules: begin a submodule */
void do_submodules (void);
void do_submodule1 (char *smname, char *smcount, char *smtype, char *smlike);
void do_submodule2 (char *smname, char *smcount, char *smtype, char *smlike);

/* submodules: on list */
void do_onlist();
void do_on_mach(char* maname);
void do_empty_onlist(void);

/* submodules: defining parameter values */
void do_substparams (void);
void do_substparameter (char *pname, char *exp);
void do_readallparameters(void);

/* submodules: defining gate vector sizes */
void do_gatesizes (void);
void do_gatesize(char *gname, char *gnr);

/* submodules: end (contains all kind of checks) */
void end_submodule (void);

/* io-interfaces */
void do_iointerfaces (void);

/*--- connections ---*/
void do_connections (void);
void set_checkconns(int a);

void do_for (void);
void do_index (char *iname, char *ibegin, char *iend);
void end_for (void);

void do_mod_L (char *gname, char *gi);
void do_mod_R (char *gname, char *gi);
void do_gate_L (char *gname, char *gi);
void do_gate_R (char *gname, char *gi);

char *do_channeldescr(char *chan,
                      char *delay_expr, char *error_expr, char *datarate_expr);
void end_connection (char *cname, char dir, char *dispstr);

/*--- system module ---*/
void do_system (char *stname);
void do_systemmodule (char *stname, char *sttype, char *stlike);
void end_system (void);

/*--- ending a module */
void end_module (char *mname);
void end_simple (char *mname);

/*--- expressions ---*/
void get_expression(const char *expr, FILE *f, char *destbuf ); /* call this to use an expression! */

char *do_op(int args, char op, char *p1,char *p2,char *p3);
char *do_func(int args,char *fname, char *p1,char *p2,char *p3,char *p4);

char *do_parname(char *submodname, char *submodindex, char *namestr, int isanc, int isref); /* --VA*/
char *do_sizeof(char *gname); /* --VA*/
char *do_inputvalue(char *val, char *promptstr); /* --VA*/
char *do_addminus(char *numconst);
char *do_timeconstant(char *num, char *unit, char *rest);

char *make_literal_expr(char type, char *str);

char *end_expr(char *expr);

/*--- IF clause ---*/
void open_if(char *cond);
void close_if(void);

void do_condition(char *cond);

/*--- display strings ---*/
void do_displaystr_enclosing(char *str);
void do_displaystr_submod(char *str);


/*--- Other misc stuff ---------------------------------------*/

#define TMP_FNAME "jar_tmp.$$$"
extern FILE *tmp;

void tmp_open (void);
void tmp_close (void);

extern char fname[];
extern FILE *yyin;
extern FILE *yyout;
extern int firstpass;

extern int no_err;
extern char errstr[];

extern char yyfailure[];
void yyerror (char *);

#define adderr \
        no_err++; \
        yyerror (errstr); \
        jar_strcpy (yyfailure, "");


int jar_strcmp (char *a, char *b);
int jar_strlen (const char *s);
char *jar_strcpy(char *dest, const char *src);
char *jar_strcat(char *dest, const char *src);
char *jar_strdup (const char *s);

void *jar_malloc (unsigned size);
void jar_free (void *ptr);

int runparse (void);
