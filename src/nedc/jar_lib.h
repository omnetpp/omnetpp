/***************************************************/
/*                OMNeT++ JAR source               */
/*                                                 */
/*  File: jar_lib.h                                */
/*                                                 */
/*  Contents:                                      */
/*    data types, functions and global variables   */
/*    used during parsing                          */
/*                                                 */
/*  By: Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*  Edited: Andras Varga 1996-2001                 */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h> /* FILE */

/*------------------------------------------------------------------*/
/*--- String data types ---*/
typedef char name_type [128];
typedef char expr_type [3072];

/*--- Global variables used during parsing ---*/
extern name_type module_name;
extern name_type submodule_name;
extern name_type submodule_var;  /*submodule_name + "_mod" */
extern name_type submodule_type;

extern expr_type submodule_nr;

extern int hassimple;
extern int is_system;
extern int inside_nonvoid_function;

extern char indent[100];

/*------------------------------------------------------------------*/
/*--- name_list:
 *       linked list of namestr strings with two int parameters
 *---*/

typedef struct name_elem
{
        name_type namestr;
        int par1, par2;
        name_type parstr;
        struct name_elem *next;
}
name_elem;

typedef struct
{
        name_elem *names;
        int count;
}
name_list;

/*--- Functions to handle name_lists ---*/
void nl_init (name_list *);
void nl_empty (name_list *);
void nl_add (name_list *, char *);
void nl_add_par (name_list *nl, char *namestr, int par1, int par2);
void nl_add_str (name_list *nl, char *namestr, char *str);
name_elem * nl_retr_ith(name_list *, int i);
void nl_get (name_list, int, char *);
int nl_count (name_list);
int nl_find (name_list, char *);

/*--- Global name_list objects ---*/
extern name_list path_list;
extern name_list include_list;
extern name_list channel_list;
extern name_list for_list;

/*------------------------------------------------------------------*/
/*--- mod_def_list:
 *         structure to store a the list of modules
 *         w/ param and gate names
 *---*/
typedef struct mod_def_elem
{
        name_type namestr;

        name_list pars;
        name_list gates;

        struct mod_def_elem *next;
}
mod_def_elem;

typedef struct
{
        mod_def_elem *mods;
        int count;
}
mod_def_list;

/*--- Functions to handle mod_def_lists ---*/
void mdl_init (void);
void mdl_open (void);
void mdl_empty (void);

void mdl_add_mod (char *);
void mdl_add_par (char *, char *, char);
void mdl_add_gate (char *, char *, int, int);

int mdl_find_par (char *, char *);
int mdl_find_gate (char *, char *, int *, int *);
mod_def_elem *mdl_find_mod (char *);

/*--- Global module definition list ---*/
extern mod_def_list mdl;

/*------------------------------------------------------------------*/
/*--- current_mod_def:
 *        structure to store data about the current module:
 *        list of submodules w/ types, w/ param and gate names
 *---*/
typedef struct
{
        name_type namestr;
        int       is_simple;
        int       check_conns;

        name_list submods;
        name_list submods_typ;

        name_list submod_pars;
        name_list gates_mod;
        name_list gates;
}
current_mod_def;

/*--- Functions to handle current module definition ---*/
void cmd_init (void);
void cmd_new (char *mname, int is_simple);
void cmd_add_submod (char *, char *, int);
char cmd_get_submod_par_type (char *);
void cmd_add_submod_par (char *);
void cmd_add_gate (char *, int, char *, int, int);

void cmd_check_submod_pars (void);
void cmd_check_gates (void);
void cmd_check_submodpar_exists (char *modname, int modvec, char *parname);


/*--- Global current module definition ---*/
extern current_mod_def cmd;

/*------------------------------------------------------------------*/
/*--- utility funcs to write into generated file ---*/

/* void setindent(FILE *f, int indentlevel); */
/* void jarprintf(FILE *f, char *fmt, ... ); */

void print_main_remark (FILE *, char *);
void print_sub_remark (FILE *, char *);
void print_remark (FILE *, char *);
void print_temp_vars (FILE *);
