/***************************************************/
/*            OMNeT++ NEDC (JAR) source            */
/*                                                 */
/*  File: jar_expr.c                               */
/*                                                 */
/*  Contents:                                      */
/*    expression parsing functions                 */
/*                                                 */
/*  By: Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*  Revised: Andras Varga 1996                     */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "jar_func.h"
#include "jar_lib.h"

static struct { char *fname; int args; } known_funcs[] =
{
   /* <math.h> */
   {"fabs", 1},    {"fmod", 2},
   {"acos", 1},    {"asin", 1},    {"atan", 1},   {"atan2", 1},
   {"sin", 1},     {"cos", 1},     {"tan", 1},    {"hypot", 2},
   {"ceil", 1},    {"floor", 1},
   {"exp", 1},     {"pow", 2},     {"sqrt", 1},
   {"log",  1},    {"log10", 1},

   /* OMNeT++ */
   {"uniform", 2},      {"intuniform", 2},       {"exponential", 1},
   {"normal", 2},       {"truncnormal", 2},
   {"genk_uniform", 3}, {"genk_intuniform",  3}, {"genk_exponential", 2},
   {"genk_normal", 3},  {"genk_truncnormal", 3},
   {"min", 2},  {"max", 2},

   /* OMNeT++, to support expressions */
   {"bool_and",2}, {"bool_or",2}, {"bool_xor",2}, {"bool_not",1},
   {"bin_and",2},  {"bin_or",2},  {"bin_xor",2},  {"bin_compl",1},
   {"shift_left",2}, {"shift_right",2},
   {NULL,0}
};

/* a global var */
expr_type temp_res;


void add_to_exprtab(char *exprtab,char *par)
{
    if (EXPR_USE(par)==USE_TABLE)
    {
       strcat(exprtab,EXPR_STR(par));
    }
    else if (EXPR_USE(par)==USE_VALUE)
    {
       strcat(exprtab,EXPR_STR(par));
       sprintf(exprtab+strlen(exprtab),
               "%sexpr_tab[k++] = value;\n", indent);
    }
    else /* USE_LITERAL */
    {
       sprintf(exprtab+strlen(exprtab),
               "%sexpr_tab[k++] = %s;\n", indent, EXPR_STR(par));
    }
}

char *do_func (int args, char *fname, char *p1,char *p2,char *p3)
{
    char *func;
    int i;
    char buf[32];
    char findfunc_code[128];

    if (firstpass)
        {jar_free(fname);jar_free(p1);jar_free(p2);jar_free(p3);return NULL;}

    /* do we know this function? */
    for (i=0; known_funcs[i].fname!=NULL;i++)
        if (0==strcmp(fname,known_funcs[i].fname))
            break;

    if (known_funcs[i].fname!=NULL) /* found */
    {
        if (args!=known_funcs[i].args)
        {
                sprintf (errstr,
                        "Function \"%s\" expects %d arguments and not %d\n",
                        fname,known_funcs[i].args,args);
                adderr;
        }
        func = known_funcs[i].fname;
        findfunc_code[0] = '\0';
    }
    else /* unknown */
    {
        sprintf(findfunc_code,
                "%sfunc = findFunction(\"%s\")->f;\n"
                "%scheck_function( func, \"%s\");\n",
                indent, fname,
                indent, fname);
        func = "func";
    }
    switch (args)
    {
       case 0: sprintf( temp_res,
                        EXPR_PREFIX "%s"
                        "%svalue.cancelRedirection();\n" /* I-bug */
                        "%svalue.setDoubleValue((MathFuncNoArg)%s);\n",
                        findfunc_code,
                        indent,
                        indent,func);
               EXPR_USE(temp_res) = USE_VALUE;
               EXPR_TYPE(temp_res) = TYPE_NUMERIC;
               break;

       case 1: if (EXPR_TYPE(p1)==TYPE_STRING)
               {
                  sprintf (errstr, "Function \"%s\" expects numeric argument\n",fname);
                  adderr;
               }
               if (EXPR_USE(p1)==USE_LITERAL)
               {
                  sprintf( temp_res,
                           EXPR_PREFIX "%s"
                           "%svalue.cancelRedirection();\n" /* I-bug */
                           "%svalue.setDoubleValue((MathFunc1Arg)%s,%s);\n",
                           findfunc_code,
                           indent,
                           indent,func,
                           EXPR_STR(p1));
                  EXPR_USE(temp_res) = USE_VALUE;
                  EXPR_TYPE(temp_res) = TYPE_NUMERIC;
               }
               else
               {
                  strcpy(temp_res, EXPR_PREFIX);
                  EXPR_USE(temp_res) = USE_TABLE;
                  EXPR_TYPE(temp_res) = TYPE_NUMERIC;

                  add_to_exprtab(temp_res, p1);
                  strcat(temp_res,findfunc_code);

                  sprintf(buf, EXPR_PREFIX "(MathFunc1Arg)%s", func);
                  EXPR_USE(buf) = USE_LITERAL;
                  EXPR_TYPE(buf) = TYPE_NUMERIC;
                  add_to_exprtab(temp_res,buf);
               }
               break;

       case 2: if (EXPR_TYPE(p1)==TYPE_STRING || EXPR_TYPE(p2)==TYPE_STRING)
               {
                  sprintf (errstr, "Function \"%s\" expects numeric arguments\n",fname);
                  adderr;
               }
               if (EXPR_USE(p1)==USE_LITERAL &&
                   EXPR_USE(p2)==USE_LITERAL)
               {
                  sprintf( temp_res,
                           EXPR_PREFIX "%s"
                           "%svalue.cancelRedirection();\n" /* I-bug */
                           "%svalue.setDoubleValue((MathFunc2Args)%s,%s,%s);\n",
                           findfunc_code,
                           indent,
                           indent,func,
                           EXPR_STR(p1),EXPR_STR(p2));
                  EXPR_USE(temp_res) = USE_VALUE;
                  EXPR_TYPE(temp_res) = TYPE_NUMERIC;
               }
               else
               {
                  strcpy(temp_res, EXPR_PREFIX);
                  EXPR_USE(temp_res) = USE_TABLE;
                  EXPR_TYPE(temp_res) = TYPE_NUMERIC;

                  add_to_exprtab(temp_res, p1);
                  add_to_exprtab(temp_res, p2);
                  strcat(temp_res,findfunc_code);

                  sprintf(buf, EXPR_PREFIX "(MathFunc2Args)%s", func);
                  EXPR_USE(buf) = USE_LITERAL;
                  EXPR_TYPE(buf) = TYPE_NUMERIC;
                  add_to_exprtab(temp_res,buf);
               }
               break;
       case 3: if (EXPR_TYPE(p1)==TYPE_STRING ||
                   EXPR_TYPE(p2)==TYPE_STRING ||
                   EXPR_TYPE(p3)==TYPE_STRING)
               {
                  sprintf (errstr, "Function \"%s\" expects numeric arguments\n",fname);
                  adderr;
               }

               if (EXPR_USE(p1)==USE_LITERAL &&
                   EXPR_USE(p2)==USE_LITERAL &&
                   EXPR_USE(p3)==USE_LITERAL)
               {
                  sprintf( temp_res,
                           EXPR_PREFIX "%s"
                           "%svalue.cancelRedirection();\n" /* I-bug */
                           "%svalue.setDoubleValue((MathFunc3Args)%s,%s,%s,%s);\n",
                           findfunc_code,
                           indent,
                           indent,func,
                           EXPR_STR(p1),EXPR_STR(p2),EXPR_STR(p3));
                  EXPR_USE(temp_res) = USE_VALUE;
                  EXPR_TYPE(temp_res) = TYPE_NUMERIC;
               }
               else
               {
                  strcpy(temp_res, EXPR_PREFIX);
                  EXPR_USE(temp_res) = USE_TABLE;
                  EXPR_TYPE(temp_res) = TYPE_NUMERIC;

                  add_to_exprtab(temp_res, p1);
                  add_to_exprtab(temp_res, p2);
                  add_to_exprtab(temp_res, p3);
                  strcat(temp_res,findfunc_code);

                  sprintf(buf, EXPR_PREFIX "(MathFunc3Args)%s", func);
                  EXPR_USE(buf) = USE_LITERAL;
                  EXPR_TYPE(buf) = TYPE_NUMERIC;
                  add_to_exprtab(temp_res,buf);
               }
               break;
    }
    jar_free(fname);
    jar_free(p1);
    jar_free(p2);
    jar_free(p3);
    return jar_strdup(temp_res);
}

char *do_op(int args, char ch, char *p1,char *p2,char *p3)
{
    if (firstpass)
        {jar_free(p1);jar_free(p2);jar_free(p3);return NULL;}

    strcpy(temp_res, EXPR_PREFIX );
    EXPR_USE(temp_res) = USE_TABLE;
    EXPR_TYPE(temp_res) = TYPE_NUMERIC;

    if (EXPR_TYPE(p1)==TYPE_STRING ||
        (args>=2 && EXPR_TYPE(p2)==TYPE_STRING) ||
        (args>=3 && EXPR_TYPE(p3)==TYPE_STRING))
    {
        printf (errstr, "Function \"%s\" expects numeric arguments\n",fname);
        adderr;
    }

    switch (args)
    {
       case 1: add_to_exprtab(temp_res,p1);
               break;
       case 2: add_to_exprtab(temp_res,p1);
               add_to_exprtab(temp_res,p2);
               break;
       case 3: add_to_exprtab(temp_res,p1);
               add_to_exprtab(temp_res,p2);
               add_to_exprtab(temp_res,p3);
               break;
    }
    if (ch=='%')
        sprintf(temp_res+strlen(temp_res),
                "%sexpr_tab[k++] = '%%';\n", indent);
    else
        sprintf(temp_res+strlen(temp_res),
               "%sexpr_tab[k++] = '%c';\n", indent, ch);

    jar_free(p1);
    jar_free(p2);
    jar_free(p3);
    return jar_strdup(temp_res);
}

char *do_addminus(char *expr)
{
    if (firstpass)
        {jar_free(expr);return NULL;}

    if (EXPR_TYPE(expr)==TYPE_STRING) /* numeric,bool,anytype are OK */
    {
        sprintf(errstr,"Type mismatch in module definition \"%s\"\n", module_name);
        adderr;
    }

    if (EXPR_USE(expr)==USE_TABLE || EXPR_USE(expr)==USE_VALUE)
    {
        EXPR_USE(expr)=USE_VALUE;
        add_to_exprtab(temp_res,EXPR_STR(expr));
        add_to_exprtab(temp_res,"-1");
        add_to_exprtab(temp_res,"'*'");
    }
    else
    {
        sprintf(temp_res, EXPR_PREFIX "-%s", EXPR_STR(expr));
    }
    jar_free(expr);
    return jar_strdup(temp_res);
}

char *do_parname (char *namestr, int isanc, int byvalue)
{
    expr_type temp_res;

    int isfor, /* index in loop connection */
        ispar; /* parameter of current compound module */

    if (firstpass)
         {jar_free (namestr);return NULL;}

    isfor = nl_find (for_list, namestr);
    ispar = mdl_find_par (module_name, namestr);

    if (isfor)
    {
         /* we could give a warning if any modifier (ancestor,ref) was used */
         sprintf(temp_res, EXPR_PREFIX "%s_index",namestr);
         EXPR_USE(temp_res) = USE_LITERAL;
         EXPR_TYPE(temp_res) = TYPE_NUMERIC;
    }
    else if (!isanc)
    {
        if (!ispar) /* check if exists */
        {
             sprintf(errstr,
                     "Parameter \"%s\" unknown in module definition \"%s\"\n",
                     namestr, module_name);
             adderr;
        }

        if (byvalue)
        {
             mod_def_elem *modp;
             name_elem *parp;
             char partype;

             modp = mdl_find_mod(module_name);
             parp = nl_retr_ith(&modp->pars, nl_find(modp->pars, namestr));
             partype = parp ? parp->parstr[0] : TYPE_ANYTYPE;
             if (partype==TYPE_CONST_NUM) partype=TYPE_NUMERIC;

             sprintf(temp_res, EXPR_PREFIX "mod->par( \"%s\" )", namestr);
             EXPR_USE(temp_res) = USE_LITERAL;
             EXPR_TYPE(temp_res) = partype;
        }
        else
        { /* by ref */
             mod_def_elem *modp;
             name_elem *parp;
             char partype;

             modp = mdl_find_mod(module_name);
             parp = nl_retr_ith(&modp->pars, nl_find(modp->pars, namestr));
             partype = parp ? parp->parstr[0] : TYPE_ANYTYPE;
             if (partype==TYPE_CONST_NUM) partype=TYPE_NUMERIC;

             sprintf(temp_res, EXPR_PREFIX
                               "%svalue.cancelRedirection();\n" /* I-bug */
                               "%svalue.setRedirection(&mod->par( \"%s\"));\n",
                               indent,
                               indent, namestr);
             EXPR_USE(temp_res) = USE_VALUE;
             EXPR_TYPE(temp_res) = partype;
        }
    }
    else /* ancestor */
    {
        if (byvalue)
        {
             sprintf(temp_res,EXPR_PREFIX
                              "%spar = &mod->ancestorPar( \"%s\" );\n"
                              "%scheck_anc_param(par,\"%s\",\"%s\");\n"
                              "%svalue.cancelRedirection();\n" /* I-bug */
                              "%svalue = *par;\n",
                              indent,namestr,
                              indent,namestr,module_name,
                              indent,
                              indent);
             EXPR_USE(temp_res) = USE_VALUE;
             EXPR_TYPE(temp_res) = TYPE_ANYTYPE; /* can't make better guess */
        }
        else /* by ref */
        {
             sprintf(temp_res,EXPR_PREFIX
                              "%spar = &mod->ancestorPar( \"%s\" );\n"
                              "%scheck_anc_param(par,\"%s\",\"%s\");\n"
                              "%svalue.cancelRedirection();\n" /* I-bug */
                              "%svalue.setRedirection( par );\n",
                              indent,namestr,
                              indent,namestr,module_name,
                              indent,
                              indent);
             EXPR_USE(temp_res) = USE_VALUE;
             EXPR_TYPE(temp_res) = TYPE_ANYTYPE; /* can't make better guess */
        }
    }

    jar_free (namestr);
    return jar_strdup(temp_res);
}

char *end_expr (char *expr)
{
    if (firstpass)
        {jar_free(expr);return NULL;}

    if (EXPR_USE(expr)!=USE_TABLE)
    {
        return expr;
    }
    else
    {
        /* convert USE_TABLE to USE_VALUE */
        /* count how big expr_tab[] we need */
        int k=0;
        char *s=expr;
        while ((s=strstr(s,"expr_tab[k++]"))!=NULL)
        {
           k++;
           s++;
        }
        sprintf(temp_res,EXPR_PREFIX
                         "%sexpr_tab = new sXElem[%d]; k=0;\n"
                         "%s"
                         "%svalue.cancelRedirection();\n" /* I-bug */
                         "%svalue.setDoubleValue(expr_tab,k);\n",
                         indent,k,
                         EXPR_STR(expr),
                         indent,
                         indent );
        EXPR_USE(temp_res) = USE_VALUE;
        EXPR_TYPE(temp_res) = EXPR_TYPE(expr);
        jar_free(expr);
        return jar_strdup(temp_res);
    }
}

char *do_sizeof(char *gname)
{
    if (firstpass)
    {
        jar_free(gname);
        return NULL;
    }

    /* check if gate exists */
    if (mdl_find_gate(module_name, gname, NULL, NULL)==0)
    {
        sprintf (errstr,
                "Gate \"%s\" unknown in module definition \"%s\"\n",
                gname, module_name);
        adderr;
    }

    sprintf (temp_res, EXPR_PREFIX "mod->gate( \"%s\" )->size();\n",gname);
    EXPR_USE(temp_res) = USE_LITERAL;
    EXPR_TYPE(temp_res) = TYPE_NUMERIC;

    jar_free( gname );
    return jar_strdup( temp_res );
}

char *do_inputvalue (char *val_expr, char *promptstr)
{
    if (firstpass)
    {
        jar_free (val_expr);
        jar_free (promptstr);
        return NULL;
    }

    assert(!val_expr || EXPR_USE(val_expr)!=USE_TABLE);

    if (promptstr && EXPR_USE(promptstr)!=USE_LITERAL)
    {
        sprintf (errstr,"Prompt string cannot be an expression\n");
        adderr;
    }

    /* default value will be 0L */
    if (!val_expr)
        val_expr = make_literal_expr(TYPE_NUMERIC,jar_strdup("0L"));

    if (EXPR_USE(val_expr)==USE_VALUE)
    {
        strcpy( temp_res, val_expr);
    }
    else
    {
        sprintf( temp_res, EXPR_PREFIX
                           "%svalue.cancelRedirection();\n" /* I-bug */
                           "%svalue = %s;\n",
                           indent,
                           indent, EXPR_STR(val_expr));
        EXPR_USE(temp_res) = USE_VALUE;
        EXPR_TYPE(temp_res) = EXPR_TYPE(val_expr);
    }

    sprintf( temp_res+strlen(temp_res),
                       "%svalue.setPrompt( %s );\n"
                       "%svalue.setInput( true );\n",
                       indent, promptstr?EXPR_STR(promptstr):"NULL", indent );
    jar_free (val_expr);
    jar_free (promptstr);
    return jar_strdup(temp_res);
}

void get_expression( char *expr, FILE *f, char *destbuf)
{
    assert(EXPR_USE(expr)!=USE_TABLE);

    if (EXPR_USE(expr)==USE_LITERAL)
    {
        strcpy(destbuf, EXPR_STR(expr));
    }
    else /* USE_VALUE */
    {
        fputs(EXPR_STR(expr),f);
        strcpy( destbuf, "value");
    }
}

char *make_literal_expr(char type, char *str)
{
    char *s;

    if (firstpass)
        {jar_free(str);return NULL;}

    s = (char *) jar_malloc( strlen(EXPR_PREFIX)+strlen(str)+1 );
    EXPR_USE(s) = USE_LITERAL;
    EXPR_TYPE(s) = type;
    strcpy( EXPR_STR(s), str );

    return s;
}

char *do_timeconstant(char *num, char *unit, char *rest)
{
    double mult = 1;
    if (firstpass)
        {jar_free(num);jar_free(unit);jar_free(rest);return NULL;}

    if (strcmp(unit,"ns")==0)       mult = 1e-9;
    else if (strcmp(unit,"us")==0)  mult = 1e-6;
    else if (strcmp(unit,"ms")==0)  mult = 1e-3;
    else if (strcmp(unit,"s")==0)   mult = 1;
    else if (strcmp(unit,"m")==0)   mult = 60;
    else if (strcmp(unit,"h")==0)   mult = 3600;
    else if (strcmp(unit,"d")==0)   mult = 24*3600;
    else
    {
        sprintf (errstr, "Invalid time unit \"%s\"\n",unit);
        adderr;
    }
    if (rest)
          sprintf(temp_res, "%lg", atof(num)*mult + atof(rest) );
    else
          sprintf(temp_res, "%lg", atof(num)*mult );

    jar_free(num);
    jar_free(unit);
    jar_free(rest);
    return jar_strdup(temp_res);
}

