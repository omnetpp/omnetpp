/***************************************************/
/*            OMNeT++ NEDC (JAR) source            */
/*                                                 */
/*  File: jar_lib.c                                */
/*                                                 */
/*  Contents:                                      */
/*    string functions                             */
/*    name_list handling functions                 */
/*    module list handling functions               */
/*    current module definition handling functions */
/*    printing comments into output file           */
/*                                                 */
/*  By: Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*  Revised: Andras Varga 1996-2001                */
/*                                                 */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "jar_func.h"
#include "jar_lib.h"

/*-------------------------------------------------------*/
/* Global variables: */

int no_err = 0;
char errstr[256];

name_list path_list;
name_list include_list;
name_list channel_list;
name_list for_list;
name_list machine_list;

mod_def_list mdl;

current_mod_def cmd;

FILE *tmp;

/*-------------------------------------------------------*/
/* String functions: */

int jar_strcmp (char *a, char *b)
{
        if (!a && !b)
                return 0;
        if (!a)
                return -1;
        if (!b)
                return 1;

        while (*a && *b && *a==*b)
        {
                a++;
                b++;
        }
        if (*a==*b)
                return 0;
        if (*a>*b)
                return 1;
        return -1;
}

int jar_strlen(const char *s)
{
        return s==NULL ? 0 : strlen(s);
}

char *jar_strcpy(char *dest, const char *src)
{
        return src==NULL ? (dest[0]=0,dest) : strcpy(dest,src);
}

char *jar_strcat(char *dest, const char *src)
{
        return src==NULL ? dest : strcat(dest,src);
}

char *jar_strdup(const char *s)
{
        if (!s)
            return NULL;
        else
        {
            char *s1 = (char *)jar_malloc( strlen(s)+1 );
            strcpy( s1, s);
            return s1;
        }
}

/*-----------------------------------------------------------*/
/* name_list handling functions: */

void nl_init (name_list *nl)
{
        nl -> names = NULL;
        nl -> count = 0;
}

void nl_empty (name_list *nl)
{
        name_elem *next;
        name_elem *cur;

        next = nl -> names;
        while (next)
        {
                cur = next;
                next = cur -> next;
                jar_free (cur);
        }
        nl -> count = 0;
        nl -> names = NULL;
}

name_elem * nl_retr_ith(name_list *nl, int i)
{    /* retrieves the ith name string from name_list pointed by nl */
     /* numbering goes from 1, as in nl_get() */
        int j;
        name_elem *cur;

        assert(i<=nl->count);
        cur = nl->names;
        for (j = nl->count; j > i; j--)
                cur = cur -> next;
        return cur;
}

void nl_add_par (name_list *nl, char *namestr, int par1, int par2)
{
        name_elem *nw;

        nw = (name_elem *)jar_malloc (sizeof (name_elem));

        jar_strcpy (nw -> namestr, namestr);
        nw -> par1 = par1;
        nw -> par2 = par2;
        nw -> next = nl -> names;
        nl -> names = nw;

        nl -> count ++;
}

void nl_add (name_list *nl, char *namestr)
{
        nl_add_par (nl, namestr, 0, 0);
}

void nl_add_str (name_list *nl, char *namestr, char *str)
{
        name_elem *nw;

        nw = (name_elem *)jar_malloc (sizeof (name_elem));

        jar_strcpy (nw -> namestr, namestr);
        jar_strcpy (nw -> parstr, str);
        nw -> next = nl -> names;
        nl -> names = nw;

        nl -> count ++;
}

void nl_get_par (name_list nl, int i, char *namestr, int *par1, int *par2)
{
        int j;
        name_elem *cur;

        cur = nl.names;
        for (j = nl.count; j > i; j--)
                cur = cur -> next;

        if (namestr)
                jar_strcpy (namestr, cur -> namestr);
        if (par1)
                *par1 = cur -> par1;
        if (par2)
                *par2 = cur -> par2;
}


void nl_get (name_list nl, int i, char *namestr)
{
        nl_get_par (nl, i, namestr, NULL, NULL);
}

int nl_count (name_list nl)
{
        return nl.count;
}

int nl_find (name_list nl, char *namestr)
{
        int i;
        name_elem *cur;

        cur = nl.names;
        for (i = nl.count; i > 0; i--)
        {
                if (jar_strcmp (cur -> namestr, namestr) == 0)
                        return i;
                cur = cur -> next;
        }
        return 0;
}

void nl_print (name_list nl)
{
        int i;
        name_type namestr;
        int par1;
        int par2;
        name_elem *cur;

        printf ("#: %d\n", nl_count (nl));

        for (i = 1; i <= nl_count (nl); i++)
        {
                nl_get_par (nl, i, namestr, &par1, &par2);
                printf ("%d\t%s\t%d\t%d\n", i, namestr, par1, par2);
        }
        printf ("--------------------\n\n");

        cur = nl.names;
        for (i = 1; i <= 2 * nl_count (nl) && cur; i++)
        {
                printf ("%d\t%s\t%d\t%d\n", i,
                        cur -> namestr, cur -> par1, cur -> par2);
                cur = cur -> next;
        }
        printf ("--------------------\n\n");
        printf ("--------------------\n\n");
}

/*-----------------------------------------------------------*/
/* module list handling functions: */

void mdl_init (void)
{
        mdl.count = 0;
        mdl.mods = NULL;
}

void mdl_empty (void)
{
        mod_def_elem *cur;
        mod_def_elem *next;

        next = mdl.mods;
        while (next)
        {
                cur = next;
                next = cur -> next;

                nl_empty (&cur -> machs); /* --LG */
                nl_empty (&cur -> pars);
                nl_empty (&cur -> gates);
                jar_free (cur);
        }
        mdl.count = 0;
        mdl.mods = NULL;
}

void mdl_add_mod (char *m_name)
{
        mod_def_elem *nw;

        if (mdl_find_mod (m_name))
        {
                sprintf (errstr,
                        "Duplicate module definition name \"%s\"\n",
                        m_name);
                adderr;
        }
        nw = (mod_def_elem *)jar_malloc (sizeof (mod_def_elem));

        nw -> next = mdl.mods;
        jar_strcpy (nw -> namestr, m_name);
        nl_init (&nw -> machs);
        nl_init (&nw -> pars);
        nl_init (&nw -> gates);

        mdl.mods = nw;
        mdl.count ++;
}

void mdl_add_par (char *m_name, char *p_name, char p_type)
{
        mod_def_elem *me;
        char t[2];

        if (mdl_find_par (m_name, p_name))
                {
                        sprintf (errstr,
                                "Duplicate parameter name \"%s\" in module defintion \"%s\"\n",
                                p_name, m_name);
                        adderr;
                }

        me = mdl_find_mod (m_name);
        assert( me!=NULL );

        t[0] = p_type;
        t[1] = '\0';
        nl_add_str (&me->pars, p_name, t);
}

void mdl_add_gate (char *m_name, char *g_name, int isin, int isvec)
{
        mod_def_elem *me;

        if (mdl_find_gate (m_name, g_name, NULL, NULL))
        {
                sprintf (errstr,
                        "Duplicate gate name \"%s\" in module definition \"%s\"\n",
                        g_name, m_name);
                adderr;
        }

        me = mdl_find_mod (m_name);
        assert( me!=NULL );
        nl_add_par (&me -> gates, g_name, isin, isvec);
}

void mdl_add_mach (char *m_name, char *ma_name) /* --LG */
{
        mod_def_elem *me;

        if (mdl_find_mach (m_name, ma_name))
                {
                        sprintf (errstr,
                                "Duplicate machine name \"%s\" in module defintion \"%s\"\n",
                                ma_name, m_name);
                        adderr;
                }

        me = mdl_find_mod (m_name);
        assert( me!=NULL );
        nl_add (&me -> machs, ma_name);
}

mod_def_elem *mdl_find_mod (char *m_name)
{
        mod_def_elem *me;

        me = mdl.mods;
        while (me)
        {
                if (jar_strcmp (me -> namestr, m_name) == 0)
                        return me;
                me = me -> next;
        }
        return 0;
}

int mdl_find_par (char *m_name, char *p_name)
{
        mod_def_elem *me;
        int i;

        me = mdl_find_mod (m_name);
        if (!me)
                return 0;

        i = nl_find (me -> pars, p_name);
        return i;
}

int mdl_find_gate (char *m_name, char *g_name, int *isin, int *isvec)
{
        mod_def_elem *me;
        int i;

        me = mdl_find_mod (m_name);
        if (!me)
                return 0;

        i = nl_find (me -> gates, g_name);
        if (i)
                nl_get_par (me -> gates, i, NULL, isin, isvec);
        return i;
}

int mdl_find_mach (char *m_name, char *p_name)
{
        mod_def_elem *me;
        int i;

        me = mdl_find_mod (m_name);
        if (!me)
                return 0;

        i = nl_find (me -> machs, p_name);
        return i;
}

void mdl_print (void)
{
        mod_def_elem *me;
        me = mdl.mods;
        while (me)
        {
                printf ("-------------------\n\n");
                printf ("%s\n", me -> namestr);
                printf ("-------------------\n\n");
                printf ("pars:\n");
                nl_print (me -> pars);
                printf ("gates:\n");
                nl_print (me -> gates);
                me = me -> next;
        }
}

/*-----------------------------------------------------------*/
/* current module definition handling functions: */

void cmd_init (void)
{
        nl_init (&cmd.submods);
        nl_init (&cmd.submods_typ);

        nl_init (&cmd.submod_machs);

        nl_init (&cmd.submod_pars);

        nl_init (&cmd.gates_mod);
        nl_init (&cmd.gates);
}

void cmd_new (char *namestr, int is_simple)
{
        jar_strcpy (cmd.namestr, namestr);
        cmd.is_simple = is_simple;

        nl_empty (&cmd.submods);
        nl_empty (&cmd.submods_typ);

        nl_empty (&cmd.submod_machs);

        nl_empty (&cmd.submod_pars);

        nl_empty (&cmd.gates_mod);
        nl_empty (&cmd.gates);
}

void cmd_add_submod (char *namestr, char *typ, int modvec)
{
        if (nl_find (cmd.submods, namestr))
        {
                sprintf (errstr,
                        "Duplicate submodule name \"%s\"\n",
                        namestr);
                adderr;
        }
        if (!mdl_find_mod (typ))
        {
                sprintf (errstr,
                        "Module definition \"%s\" unknown\n",
                        typ);
                adderr;
        }

        nl_add_par (&cmd.submods, namestr, modvec, 0);
        nl_add (&cmd.submods_typ, typ);

        nl_empty (&cmd.submod_pars);
}

char cmd_get_submod_par_type (char *par)
{
        int n;

        mod_def_elem *me;
        name_type typ;
        name_elem *ne;

        n = nl_count (cmd.submods);
        nl_get (cmd.submods_typ, n, typ);

        me = mdl_find_mod (typ);
        if ( me!=NULL ) {
            int k = nl_find( me->pars, par );
            if (k>0) {
               ne = nl_retr_ith (&me->pars, k);
               assert( ne!=NULL );
               return ne->parstr[0];
            }
        }
        return 0;
}

void cmd_add_submod_par (char *par)
{
        int n;

        name_type namestr;
        name_type typ;

        n = nl_count (cmd.submods);
        nl_get (cmd.submods, n, namestr);
        nl_get (cmd.submods_typ, n, typ);

        /* Commented out because with IF, multiple PARAMETERS sections can be present
        * if (nl_find (cmd.submod_pars, par))
        * {
        *       sprintf (errstr,
        *               "Duplicate parameter \"%s\" substitution in submodule \"%s\" declaration\n",
        *               par, namestr);
        *       adderr;
        * }
        */

        if (mdl_find_mod (typ))
                if (!mdl_find_par (typ, par))
                {
                        sprintf (errstr,
                                "module type \"%s\" has no parameter called \"%s\"\n",
                                typ, par);
                        adderr;
                }

        nl_add (&cmd.submod_pars, par);
}

void cmd_add_submod_mach (char *maname)
{
        /* Commented out because with IF, multiple PARAMETERS sections can be present
        *  commented out by           +--LG
        * n = nl_count (cmd.submods);
        * nl_get (cmd.submods, n, namestr);
        * nl_get (cmd.submods_typ, n, typ);
        *
        * if (nl_find (cmd.submod_machs, maname))
        * {
        *       sprintf (errstr,
        *               "Duplicate machine \"%s\" substitution in submodule \"%s\" declaration\n",
        *               maname, namestr);
        *       adderr;
        * }
        */

        nl_add (&cmd.submod_machs, maname);
}

void cmd_add_gate (char *namestr, int modvec, char *gate, int isin, int isvec)
{
        int i;
        name_type typ;
        name_type name2;
        int risin;
        int risvec;
        int rmodvec;

        if (jar_strlen (namestr) == 0)
        {
                isin = !isin;
                i = -1;
                sprintf (name2, "module definition \"%s\"", cmd.namestr);
        }
        else
        {
                i = nl_find (cmd.submods, namestr);
                sprintf (name2, "submodule \"%s\"", namestr);
        }
        if (!i)
        {
                sprintf (errstr,
                        "\"%s\" is not a submodule\n",
                        namestr);
                adderr;
        }
        else
        {
                if (i < 0)
                        jar_strcpy (typ, cmd.namestr);
                else
                {
                        nl_get (cmd.submods_typ, i, typ);
                        nl_get_par (cmd.submods, i, NULL, &rmodvec, NULL);
                        if (rmodvec != modvec)
                        {
                                if (rmodvec)
                                        sprintf (errstr,
                                                "\"%s\" is a vector submodule\n",
                                                namestr);
                                else
                                        sprintf (errstr,
                                                "\"%s\" is not a vector submodule\n",
                                                namestr);
                                adderr;
                        }
                }
                if (mdl_find_mod (typ))
                {
                        if (!mdl_find_gate (typ, gate, &risin, &risvec))
                        {
                                sprintf (errstr,
                                        "%s has no gate named \"%s\"\n",
                                        name2, gate);
                                adderr;
                        }
                        else if (risin != isin)
                        {
                                sprintf (errstr,
                                         "direction of gate \"%s\" of %s does not match usage\n",
                                         gate, name2);
                                adderr;
                        }
                        else if (risvec != isvec)
                        {
                                if (risvec)
                                        sprintf (errstr,
                                                "Gate \"%s\" of %s is a vector gate\n",
                                                gate, name2);
                                else
                                        sprintf (errstr,
                                                "Gate \"%s\" of %s is not a vector gate\n",
                                                gate, name2);
                                adderr;
                        }
                }
        }

        nl_add (&cmd.gates_mod, namestr);
        nl_add (&cmd.gates, gate);
}

void cmd_check_submod_pars (void)
{
        int n;
        int j;
        mod_def_elem *me;

        name_type par;
        name_type namestr;
        name_type typ;

        n = nl_count (cmd.submods);
        nl_get (cmd.submods, n, namestr);
        nl_get (cmd.submods_typ, n, typ);

        me = mdl_find_mod (typ);
        if (!me)
                return;

        for (j = 1; j <= nl_count (me -> pars); j++)
        {
                nl_get (me -> pars, j, par);
                if (!nl_find (cmd.submod_pars, par))
                {
                        sprintf (errstr,
                                "Parameter \"%s\" not substituted in submodule \"%s\"\n",
                                par, namestr);
                        adderr;
                }
        }
}


void cmd_check_submodpar_exists (char *modname, int modvec, char *parname)
{
        int i;
        int rmodvec;
        name_type modtype;

        /* find submodule name within current compound module */
        i = nl_find (cmd.submods, modname);
        if (!i)
        {
                sprintf (errstr,
                        "\"%s\" is not a submodule\n",
                        modname);
                adderr;
                return;
        }

        /* vector submodule needs index when referencing */
        nl_get_par (cmd.submods, i, NULL, &rmodvec, NULL);
        if (rmodvec != modvec)
        {
                if (rmodvec)
                        sprintf (errstr,
                                "\"%s\" is a vector submodule\n",
                                modname);
                else
                        sprintf (errstr,
                                "\"%s\" is not a vector submodule\n",
                                modname);
                adderr;
        }

        /* get submodule type and check it really has this parameter */
        nl_get (cmd.submods_typ, i, modtype);
        if (mdl_find_mod (modtype))
        {
                if (!mdl_find_par (modtype, parname))
                {
                        sprintf (errstr,
                                "submodule \"%s\" (type \"%s\") has no parameter called \"%s\"\n",
                                modname, modtype, parname);
                        adderr;
                }
        }
}


void cmd_check_gates (void)
{
        int i;
        int k;
        int l;
        mod_def_elem *me;

        name_type gate;
        name_type namestr;
        name_type typ;

        name_type gm;
        name_type g;

        int gate_found;
        /*int help; --VA */

        for (i = 0; i <= nl_count(cmd.submods); i++)
        {
            /*help = 0; --VA */
            if (i == 0) /* i==0: check current module */
            {
                jar_strcpy (namestr, "");
                jar_strcpy (typ, cmd.namestr);
                /*help = 1; --VA */
            }
            else /* i==1..count: check a submodule */
            {
                nl_get (cmd.submods, i, namestr);
                nl_get (cmd.submods_typ, i, typ);
            }
            me = mdl_find_mod (typ);
            if (me==NULL)
                return;

            for (k = 1; k <= nl_count(me->gates); k++)
            {
                /* checking gate 'gate' of module type 'typ' (name 'namestr')*/
                nl_get (me->gates, k, gate);

                /* try to find gate among connected gates 'cmd.gates' */
                gate_found = 0;
                for (l = 1; l <= nl_count(cmd.gates); l++)
                {
                    nl_get (cmd.gates_mod, l, gm);
                    nl_get (cmd.gates, l, g);
                    if (jar_strcmp(namestr, gm)==0 && jar_strcmp(gate, g)==0)
                            gate_found += 1;
                }
                if (!gate_found)
                {
                    if (jar_strlen(namestr)==0) /* current module */
                        sprintf (errstr,
                                 "Gate \"%s\" of module definition \"%s\" not connected\n",
                                 gate, typ);
                    else /* submodule */
                        sprintf (errstr,
                                 "Gate \"%s\" of submodule \"%s\" not connected\n",
                                 gate, namestr);
                    adderr;
                }
            }
        }
}

/*-----------------------------------------------------------*/
/* Temporary file handling functions: */

void tmp_open (void)
{
        tmp = fopen (TMP_FNAME, "w");
}

void tmp_close (void)
{
        int ch;

        fclose (tmp);

        tmp = fopen (TMP_FNAME, "r");
        ch = fgetc (tmp);
        while (ch != EOF)
        {
                fputc (ch, yyout);
                ch = fgetc (tmp);
        }
        fclose (tmp);

        remove (TMP_FNAME);
}

/*-----------------------------------------------------------*/
/* Print comments into output file: */

void print_main_remark (FILE *f, char *s)
{
        fprintf (f, "//=========================================\n");
        fprintf (f, "// %s\n", s);
        fprintf (f, "//=========================================\n\n\n");
        /*fflush (f);*/
}

void print_sub_remark (FILE *f, char *s)
{
        fprintf (f, "//-------------------------------------\n");
        fprintf (f, "// %s\n", s);
        fprintf (f, "//-------------------------------------\n\n");
        /*fflush (f);*/
}

void print_remark (FILE *f, char *s)
{
        fprintf (f, "%s// %s\n", indent, s);
        /*fflush (f);*/
}

void print_temp_vars (FILE *f)
{
        print_remark (f, "temporary variables:");
        fprintf (f, "\tcPar value, *par;\n");
        fprintf (f, "\tcPar::ExprElem *expr_tab; int k;\n");
        fprintf (f, "\tMathFunc func;\n");
        fprintf (f, "\tconst char *type_name;\n");
        fprintf (f, "\tchar b1[64], b2[64];\n");
        fprintf (f, "\tcArray machines;\n"); /* --LG */
        fprintf (f, "\tbool islocal, do_this_block;\n");
        fprintf (f, "\tint n;\n");
        fprintf (f, "\n");
}

