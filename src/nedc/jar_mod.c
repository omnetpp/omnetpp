/***************************************************/
/*            OMNeT++ NEDC (JAR) source            */
/*                                                 */
/*  File: jar_mod.c                                */
/*                                                 */
/*  Contents:                                      */
/*    some global variables                        */
/*    functions building modules                   */
/*                                                 */
/*  By: Jan Heijmans                               */
/*      Alex Paalvast                              */
/*      Robert van der Leij                        */
/*  Revised: Andras Varga 1996-2001                */
/*                                                 */
/***************************************************/

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <assert.h>
#include "jar_func.h"
#include "jar_lib.h"

/*--- Global variable defs ----------------------------*/

name_type physicalmach_name = "";

name_type module_name = "";
name_type submodule_name = "";
name_type submodule_var = "";
name_type submodule_type = "";
expr_type submodule_nr = "";

name_type mod_L = "";
name_type gate_L = "";
name_type mod_R = "";
name_type gate_R = "";

expr_type mod_nr_L = "";
expr_type gate_nr_L = "";
expr_type mod_nr_R = "";
expr_type gate_nr_R = "";

int hassimple = 0;
int is_system = 0;
int inside_nonvoid_function = 0;

int isref = 0;

char indent [100] = "\t";

/*--- Local function declarations ---------------------*/
void do_simp_or_comp (char *mname, int is_simple);

/*--- Functions ---------------------------------------*/

void do_module (char *mname)
{
        do_simp_or_comp (mname, 0);
}

void do_simple (char *mname)
{
        if (!hassimple && !firstpass)
        {
                /* print_sub_remark (yyout, "simple module inclusion:"); */
                /* fprintf (yyout, "#include \"%s.h\"\n\n", fname); */
                hassimple = 1;
        }

        do_simp_or_comp (mname, 1);
}

void do_simp_or_comp (char *mname, int is_simple)
{
        jar_strcpy (module_name, mname);

        if (firstpass)
        {
             mdl_add_mod (mname);
             jar_free (mname);
             return;
        }

        fprintf (yyout, "ModuleInterface( %s )\n", mname);

        if (!is_simple)
        {
             tmp_open ();

             fprintf (tmp,
                  "class %s : public cCompoundModule\n"
                  "{\n"
                  "  public:\n"
                  "    %s(const char *name, cModule *parentmod) :\n"
                  "      cCompoundModule(name, parentmod) {}\n",
                  mname,mname);
             fprintf (tmp,
                  "  protected:\n"
                  "    virtual void doBuildInside();\n"
                  "};\n\n"
                  );

             fprintf (tmp, "Define_Module( %s );\n\n", mname);

             fprintf (tmp, "void %s::doBuildInside()\n", mname);
             fprintf (tmp, "{\n");
             fprintf (tmp, "\tcModule *mod = this;\n");

             print_temp_vars (tmp);
        }

        cmd_new (mname, is_simple );
        nl_empty (&for_list);

        jar_free (mname);
}

void end_module (char *mname)
{
        int i, count;
        if (firstpass) {jar_free (mname);return;}

        if (cmd.check_conns)
        {
           cmd_check_gates ();
           fprintf (tmp, "%smod->checkInternalConnections();\n",indent);
        }

        /* generate code to free array of module pointers */
        count = nl_count(cmd.submods);
        for (i=1; i<=count; i++)
        {
            name_elem *subm = nl_retr_ith( &cmd.submods, i );
            if (subm->par1)
               fprintf(tmp, "\tdelete[] %s_mod;\n", subm->namestr);
        }

        /* finish module */
        if (mname)
            if (jar_strcmp (mname, module_name) != 0)
            {
                sprintf (errstr,
                         "Module definition \"%s\" ended with module name \"%s\"\n",
                         module_name, mname);
                adderr;
            }

        fprintf (yyout, "EndInterface\n\n"
                        "Register_ModuleInterface( %s )\n\n", module_name);

        fprintf (tmp, "\tcheck_error(); check_memory();\n");
        fprintf (tmp, "}\n\n");
        tmp_close ();

        jar_free (mname);
}

void end_simple (char *mname)
{
        if (firstpass) {jar_free (mname);return;}

        fprintf (yyout, "EndInterface\n\n"
                        "Register_ModuleInterface( %s )\n\n",cmd.namestr);
        fprintf (yyout, "// class %s : public cSimpleModule\n"
                        "// {\n"
                        "//     Module_Class_Members(%s,cSimpleModule,8192)\n"
                        "//     virtual void activity();\n"
                        "//     // Add you own member functions here!\n"
                        "// };\n"
                        "//\n"
                        "// Define_Module( %s )\n"
                        "//\n",
                        cmd.namestr, cmd.namestr, cmd.namestr );
        fprintf (yyout, "// void %s::activity()\n"
                        "// {\n"
                        "//     // Put code for simple module activity here!\n"
                        "// }\n"
                        "//\n\n",
                        cmd.namestr );
        jar_free (mname);
}

void do_machine (char *maname)  /* --LG */
{
        if (firstpass)
        {
            mdl_add_mach (module_name, maname);
            jar_free (maname);
            return;
        }

        fprintf (yyout, "\tMachine( %s )\n", maname);
        jar_free (maname);

}

void do_parameters (void)
{
        if (firstpass)
                return;

        if (!cmd.is_simple)
                print_remark (tmp, "const parameters:");
}

void do_parameter (char *pname, char ptype)
{
        char *s;

        if (firstpass)
        {
            mdl_add_par (module_name, pname, ptype );
            jar_free (pname);
            return;
        }

        switch(ptype) {
           case TYPE_NUMERIC:   s = "ParType_Numeric"; break;
           case TYPE_CONST_NUM: s = "ParType_Numeric ParType_Const"; break;
           case TYPE_STRING:    s = "ParType_String"; break;
           case TYPE_BOOL:      s = "ParType_Bool"; break;
           case TYPE_ANYTYPE:   s = "ParType_Any"; break;
        }
        fprintf (yyout, "\tParameter( %s, %s )\n", pname, s );
        jar_free (pname);
}

void do_gates (void)
{
}

void do_gatedecl (char *gname, int is_in, int is_vector )
{
        if (firstpass)
        {
            mdl_add_gate( module_name, gname, is_in, is_vector );
            jar_free( gname );
            return;
        }

        fprintf (yyout, "\tGate( %s%s, %s )\n", gname,
                        is_vector ? "[]" : "",
                        is_in ? "GateDir_Input" : "GateDir_Output");
        jar_free( gname );
}

void do_gatesizes (void)
{
        if (firstpass)
                return;

        print_remark (tmp, "'gatesizes:' section");
}

void do_gatesize (char *gname, char *gnr_expr)
{
        char count[32];
        expr_type gnr_val;

        if (firstpass)
        {
                jar_free (gname);
                jar_free (gnr_expr);
                return;
        }

        if (gnr_expr==NULL)
                jar_strcpy (count, "1");
        else
        {
                get_expression(gnr_expr, tmp, gnr_val);
                fprintf(tmp, "%sn = %s;\n",indent, gnr_val);
                fprintf (tmp, "%scheck_gate_count( n, \"%s\",\"%s\",\"%s\");\n",
                              indent, submodule_name, gname, cmd.namestr);
                jar_strcpy (count, "n");
        }
        fprintf (tmp, "%s%s->setGateSize(\"%s\", %s );\n\n",
                      indent, submodule_var, gname, count );
        jar_free (gname);
        jar_free (gnr_expr);
}

void do_submodules (void)
{
        if (firstpass)
                return;

        print_remark (tmp, "Submodules:");
        fprintf (tmp, "\tcModuleType *modtype = NULL;\n");
        fprintf (tmp, "\tlong sub_nr = 0;\n");
        fprintf (tmp, "\tlong sub_i = 0;\n\n");
}

void do_iointerfaces (void)
{
        print_remark (tmp, "IO-interfaces:");
}

void do_sub_or_sys1 (char *smname, char *smcount, int issys, char *smtype, char *smlike )
{
        if (firstpass) return;

        jar_strcpy (submodule_name, smname);
        sprintf    (submodule_var, "%s_mod", smname);
        jar_strcpy (submodule_type, smlike==NULL ? smtype : smlike);

        jar_strcpy (submodule_nr, smcount);

        /*
        *if (issys)
        *{
        *       print_remark (tmp, "starting the program on other machines");
        *       fprintf (tmp, "\tif (simulation.netInterface()!=NULL &&\n"
        *                     "\t    simulation.netInterface()->isConsole())\n"
        *                     "\t\tsimulation.netInterface()->start_segments( "
        *                         "machines, ev.argc, ev.argv );\n");
        *}
        */

        if (!issys)
        {
                print_remark (tmp, smname);
        }
        else
        {
                print_remark (tmp, "system module creation");
                fprintf (tmp, "\tcModuleType *modtype;\n");
        }
        if (smlike==NULL)
        {
                fprintf (tmp, "\tmodtype = findModuleType( \"%s\" );\n", smtype);
                fprintf (tmp, "\tcheck_modtype( modtype, \"%s\" );\n", smtype);
        }
        else
        {
                fprintf (tmp, "\tpar = &mod->par(\"%s\");\n", smtype);
                fprintf (tmp, "\tcheck_param(par,\"%s\");\n", smtype);
                fprintf (tmp, "\ttype_name = (const char *)par;\n");
                fprintf (tmp, "\ttype_name = mod->par(\"%s\");\n", smtype);
                fprintf (tmp, "\tmodtype = findModuleType( type_name );\n");
                fprintf (tmp, "\tcheck_modtype( modtype, type_name );\n");
        }

        jar_strcpy (indent, "\t");

        if (!smcount)
                fprintf (tmp, "\tcModule *%s;\n\n", submodule_var);
        else
        {
                expr_type smnr_val;
                get_expression(smcount,tmp,smnr_val);
                fprintf (tmp, "\tsub_nr = %s;\n",smnr_val);

                /*NL checking the array variable*/
                fprintf (tmp, "\tcheck_module_count(sub_nr,\"%s\",\"%s\");\n",
                              smname, cmd.namestr);
                fprintf (tmp, "\tcModule **%s = new cModule *[sub_nr];\n\n",
                              submodule_var);
                /*NL*/
                fprintf (tmp, "\tfor (sub_i=0; sub_i<sub_nr; sub_i++)\n");
                fprintf (tmp, "\t{\n");

                sprintf (submodule_var,
                        "%s_mod[sub_i]", smname);

                jar_strcpy (indent, "\t\t");
        }
        /* no jar_free(), it'll be called from do_sub_or_sys2() */
}

void do_sub_or_sys2 (char *smname, char *smcount, int issys, char *smtype, char *smlike )
{
        mod_def_elem *me;

        if (firstpass)
        {
                jar_free (smname);
                jar_free (smcount);
                jar_free (smtype);
                jar_free (smlike);
                return;
        }

        print_remark(tmp, "module creation:");

        fprintf (tmp, "%sislocal = simulation.netInterface()==NULL ||\n"
                      "%s          simulation.netInterface()->isLocalMachineIn( machines );\n",
                      indent, indent );
        if (issys)
        {
          fprintf (tmp, "%sif (!islocal)\n"
                        "%s    throw new cException(\"Local machine `%%s' is not among machines specified for this network\",\n"
                        "%s              simulation.netInterface()->localhost());\n",
                        indent, indent, indent);
        }

        fprintf (tmp, "%s%s = modtype->create( \"%s\", %s, islocal);\n",
                      indent, submodule_var, smname, issys?"NULL":"mod" );

        fprintf (tmp, "%scheck_error(); check_memory();\n\n", indent);

        if (smcount)
                fprintf (tmp, "%s%s->setIndex(sub_i, sub_nr);\n\n",
                              indent, submodule_var);

        cmd_add_submod (smname, smlike==NULL?smtype:smlike, smcount!=NULL );

        /* check the number of formal and actual machines --LG */
        me = mdl_find_mod(submodule_type);
        if (me==NULL)
        {
          /* "module type was not found" msg was printed earlier, ignore */
        }
        else if (nl_count(me->machs) != cmd.submod_machs.count)
        {
          name_list *machs;
          machs = &(me->machs);

          /* they differ: give an appropriate error message --LG */
          if ( issys )
          {
            sprintf (errstr,
              "System Module \"%s\" has %i actual machine(s) to substitute "
              "%i formal machine(s)\n",
              submodule_name, cmd.submod_machs.count, nl_count(*machs) );
            adderr;
          }
          else
          {
            sprintf (errstr,
              "Module \"%s\" has %i actual machine(s) to substitute "
              "%i formal machine(s) in compound module \"%s\"\n",
              submodule_name, cmd.submod_machs.count,
              nl_count(*machs), cmd.namestr );
            adderr;
          }
        }
        else
        {
          name_list *machs;
          machs = &(me->machs);

          /* they are equal: set machine list if machines are given --LG */
          if ( cmd.submod_machs.count ) /* actual machine list is not epmty */
          {
            int i;

            print_remark(tmp, "set machine list:");
            for (i=0; i<cmd.submod_machs.count; i++)
            {
              if (issys)
                fprintf(tmp,
                  "%s%s->setMachinePar( \"%s\", ev.getPhysicalMachineFor(\"%s\") );\n",
                  indent, submodule_var, nl_retr_ith(machs,i+1)->namestr,
                  nl_retr_ith( /*machs*/ &cmd.submod_machs,i+1)->namestr );
              else
                fprintf(tmp,
                  "%s%s->setMachinePar( \"%s\", ((cPar *)machines[%i])->stringValue() );\n",
                  indent, submodule_var, nl_retr_ith(machs,i+1)->namestr, i );
            }
            fprintf (tmp, "%scheck_error(); check_memory();\n\n", indent);
            nl_empty(&cmd.submod_machs);
          }
        }
        jar_free (smname);
        jar_free (smcount);
        jar_free (smtype);
        jar_free (smlike);
}

void do_submodule1 (char *smname, char *smcount, char *smtype, char *smlike)
{
        do_sub_or_sys1 (smname, smcount, 0, smtype, smlike);
}

void do_submodule2 (char *smname, char *smcount, char *smtype, char *smlike)
{
        do_sub_or_sys2 (smname, smcount, 0, smtype, smlike);
}

void do_systemmodule (char *stname, char *sttype, char *stlike) /* created --VA */
{
        do_sub_or_sys1 ( stname, NULL, 1, sttype, stlike);
        do_sub_or_sys2 ( stname, NULL, 1, sttype, stlike);
}

void do_substparams (void)
{
        if (firstpass)
                return;

        print_remark (tmp, "'parameters:' section");
}

void do_onlist()        /* --LG */
{
        if (firstpass)
                return;

        print_remark (tmp, "'on:' section");

        nl_empty(&cmd.submod_machs);
}

void do_on_mach(char * maname)  /* --LG */
{
        if (firstpass)
        {
            jar_free (maname);
            return;
        }
        if (!is_system)
        {
            fprintf (tmp,  "%spar = new cPar();\n"
                           "%s*par = mod->machinePar(\"%s\");\n"
                           "%smachines.add( par );\n",
                           indent, indent, maname, indent );
        }
        else
        {
            int idx;
            char *phys_mach;
            idx = nl_find (machine_list, maname);
            if (idx==0)
               phys_mach = maname; /* no substitution! */
            else
               phys_mach = nl_retr_ith( &machine_list, idx )->parstr;

            fprintf (tmp,  "%spar = new cPar();\n"
                           "%s*par = ev.getPhysicalMachineFor(\"%s\");\n"
                           "%smachines.add( par );\n",
                           indent, indent, phys_mach, indent );
        }
        fprintf (tmp, "%scheck_error(); check_memory();\n\n",
                      indent);
        cmd_add_submod_mach (maname);
        jar_free (maname);
}

void do_empty_onlist(void)
{
        if (firstpass)
                return;

        print_remark (tmp, "an empty actual machine list for the next module");

        nl_empty(&cmd.submod_machs);

        do_on_mach( jar_strdup("default"));
}

void do_substparameter(char *pname, char *expr)
{
        expr_type value;
        char decl_type,expr_type;

        if (firstpass)
                {jar_free (pname);jar_free (expr);return;}

        cmd_add_submod_par (pname);

        /* prepare for type check */
        decl_type = cmd_get_submod_par_type (pname);
        if (!decl_type) /*not found*/
                {jar_free (pname);jar_free (expr);return;}
        expr_type = EXPR_TYPE(expr);

        /* bool will also be treated as numeric */
        if (decl_type==TYPE_CONST_NUM || decl_type==TYPE_BOOL)
            decl_type = TYPE_NUMERIC;
        if (expr_type==TYPE_CONST_NUM || expr_type==TYPE_BOOL)
            expr_type = TYPE_NUMERIC;

        if (decl_type!=expr_type &&
            decl_type!=TYPE_ANYTYPE &&
            expr_type!=TYPE_ANYTYPE)
        {
            sprintf (errstr,"Type mismatch for parameter \"%s\"\n",pname);
            adderr;
        }

        /* set parameter value */
        get_expression(expr,tmp,value);
        /*
         * fprintf(tmp, "%s%s->par( \"%s\" ) = %s;\n\n",
         *            indent, submodule_var, pname, value);
         */
        fprintf(tmp, "%spar = &(%s->par(\"%s\"));\n",
                     indent, submodule_var, pname);
        fprintf(tmp, "%scheck_param(par, \"%s\");\n",
                     indent, pname);
        fprintf(tmp, "%s*par = %s;\n\n",
                     indent, value);

        jar_free (pname);
        jar_free (expr);
}

void do_readallparameters()
{
        if (firstpass) return;

        fprintf(tmp, "%sn = %s->params();\n"
                     "%sfor(k=0;k<n;k++)\n"
                     "%s   if(%s->par(k).isInput())\n"
                     "%s      {%s->par(k).read();check_error();}\n\n",
                     indent, submodule_var,
                     indent,
                     indent, submodule_var,
                     indent, submodule_var
                     );
}

void end_submodule (void)
{
        if (firstpass)
                return;

        /*
        * This call would check if all parameters have been set.
        * It has been removed to make unset parameters be Input by default.
        * --VA
        *
        * cmd_check_submod_pars ();
        */

        print_remark (tmp, "build function call:");
        fprintf (tmp, "%s%s->buildInside();\n",
                      indent, submodule_var);

        if (jar_strlen (submodule_nr) != 0)
                fprintf (tmp, "\t}\n");

        jar_strcpy (indent, "\t");
        fprintf (tmp, "%smachines.clear();\n\n", indent);

}

void do_connections (void)
{
        if (firstpass)
                return;

        print_remark (tmp, "Connections:");
        /*print_remark (tmp, "temp vars");*/
        fprintf (tmp, "\tcLinkType *link_p;\n");
        fprintf (tmp, "\tcPar *delay_p, *error_p, *datarate_p;\n");
        fprintf (tmp, "\tint gateL, gateR;\n");
        fprintf (tmp, "\tlong mod_nr_L, mod_nr_R, gate_nr_L, gate_nr_R;\n");
}

void do_for (void)
{
        if (firstpass)
                return;

        /*print_remark (tmp, "do_for");*/
        fprintf (tmp, "\t{\n");

        jar_strcpy (indent, "\t\t");
}

void do_index (char *iname, char *ibegin_expr, char *iend_expr)
{
        name_type title;
        name_type ibegin_val, iend_val;
        if (firstpass)
        {
                jar_free (iname);
                jar_free (ibegin_expr);
                jar_free (iend_expr);
                return;
        }

        sprintf (title, "for %s loop:", iname);
        print_remark (tmp, title);

        fprintf (tmp, "%slong %s_index;\n\n", indent, iname);
        get_expression(ibegin_expr,tmp,ibegin_val);
        fprintf (tmp, "%slong %s_begin = %s;\n", indent, iname, ibegin_val);
        get_expression(iend_expr,tmp,iend_val);
        fprintf (tmp, "%slong %s_end = %s;\n", indent, iname, iend_val);
        /*NL index check*/
        fprintf (tmp, "%scheck_loop_bounds(%s_begin, %s_end, \"%s\");\n",
                      indent, iname, iname, cmd.namestr);

        fprintf (tmp, "%sfor (%s_index=%s_begin; %s_index<=%s_end; %s_index++)\n",
                      indent, iname, iname, iname, iname, iname);
        fprintf (tmp, "%s{\n", indent);

        nl_add (&for_list, iname);
        jar_strcat (indent, "\t");

        jar_free (iname);
        jar_free (ibegin_expr);
        jar_free (iend_expr);
}

void end_for (void)
{
        int i;
        name_type namestr;

        if (firstpass)
                return;

        for (i = nl_count (for_list); i >= 1; i--)
        {
                nl_get (for_list, i, namestr);

                indent [i + 1] = 0;
                fprintf (tmp, "%s} // end for %s\n\n", indent, namestr);
        }
        fprintf (tmp, "\t}\n\n");

        jar_strcpy (indent, "\t");
        nl_empty (&for_list);
}

void do_mod_L (char *gname, char *gi)
{
        if (firstpass)
        {
                jar_free (gname);
                jar_free (gi);
                return;
        }

        jar_strcpy (mod_L,    gname ? gname : "");
        jar_strcpy (mod_nr_L, gi    ? gi    : "");

        jar_free (gname);
        jar_free (gi);
}

void do_gate_L (char *gname, char *gi)
{
        if (firstpass)
        {
                jar_free (gname);
                jar_free (gi);
                return;
        }

        jar_strcpy (gate_L, gname);
        jar_strcpy (gate_nr_L, gi ? gi : "");

        jar_free (gname);
        jar_free (gi);
}

void do_mod_R (char *gname, char *gi)
{
        if (firstpass)
        {
                jar_free (gname);
                jar_free (gi);
                return;
        }

        jar_strcpy (mod_R,    gname ? gname : "");
        jar_strcpy (mod_nr_R, gi    ? gi    : "");

        jar_free (gname);
        jar_free (gi);
}

void do_gate_R (char *gname, char *gi)
{
        if (firstpass)
        {
                jar_free (gname);
                jar_free (gi);
                return;
        }

        jar_strcpy (gate_R, gname);
        jar_strcpy (gate_nr_R, gi ? gi : "");

        jar_free (gname);
        jar_free (gi);
}

expr_type modL = "";
expr_type modR = "";

void do_con_gate_L (char arrowdir)
{
        expr_type value;

        if (jar_strlen (mod_L) == 0)
                jar_strcpy (modL, "mod");
        else if (jar_strlen (mod_nr_L) == 0)
                sprintf (modL, "%s_mod", mod_L);
        else
        {
                get_expression(mod_nr_L,tmp,value);
                fprintf (tmp, "%smod_nr_L = %s;\n",indent,value);
                fprintf (tmp, "%scheck_module_index(mod_nr_L,%s_mod,\"%s\", \"%s\");\n",
                        indent, mod_L, mod_L, cmd.namestr );
                sprintf (modL, "%s_mod[mod_nr_L]",
                        mod_L);
        }

        if (jar_strlen (gate_nr_L) == 0)
        {
           fprintf (tmp, "%sgateL = %s->findGate( \"%s\" );\n",
                      indent, modL, gate_L );
           fprintf (tmp, "%scheck_gate( gateL, \"%s\", \"%s\" );\n\n",
                         indent, mod_L, gate_L );
        }
        else
        {
           get_expression(gate_nr_L,tmp,value);
           fprintf (tmp, "%sgate_nr_L = %s;\n", indent, value);
           fprintf (tmp, "%sgateL = %s->findGate( \"%s\", gate_nr_L );\n",
                         indent, modL, gate_L );
           fprintf (tmp, "%scheck_gate( gateL, \"%s\", "
                         "indexedname(b1,\"%s\",gate_nr_L) );\n\n",
                         indent, mod_L, gate_L );
        }
        cmd_add_gate (mod_L, jar_strlen (mod_nr_L) != 0,
                      gate_L, arrowdir=='L', jar_strlen (gate_nr_L) != 0);
}

void do_con_gate_R (char arrowdir)
{
        expr_type value;

        if (jar_strlen (mod_R) == 0)
                jar_strcpy (modR, "mod");
        else if (jar_strlen (mod_nr_R) == 0)
                sprintf (modR, "%s_mod", mod_R);
        else
        {
                get_expression(mod_nr_R,tmp,value);
                fprintf (tmp, "%smod_nr_R = %s;\n",indent,value);
                fprintf (tmp, "%scheck_module_index(mod_nr_R,%s_mod,\"%s\", \"%s\");\n",
                        indent, mod_R, mod_R, cmd.namestr );
                sprintf (modR, "%s_mod[mod_nr_R]",
                        mod_R);
        }

        if (jar_strlen (gate_nr_R) == 0)
        {
           fprintf (tmp, "%sgateR = %s->findGate( \"%s\" );\n",
                      indent, modR, gate_R );
           fprintf (tmp, "%scheck_gate( gateR, \"%s\", \"%s\" );\n\n",
                         indent, mod_R, gate_R );
        }
        else
        {
           get_expression(gate_nr_R,tmp,value);
           fprintf (tmp, "%sgate_nr_R = %s;\n", indent,value);
           fprintf (tmp, "%sgateR = %s->findGate( \"%s\", gate_nr_R );\n",
                         indent, modR, gate_R );
           fprintf (tmp, "%scheck_gate( gateR, \"%s\", "
                         "indexedname(b1,\"%s\",gate_nr_R) );\n\n",
                         indent, mod_R, gate_R );
        }
        cmd_add_gate (mod_R, jar_strlen (mod_nr_R) != 0,
                      gate_R, arrowdir=='R', jar_strlen (gate_nr_R) != 0);
}

char *do_channeldescr(char *link_name, char *delay_expr, char *error_expr, char *datarate_expr)
{
        /* returns a string that can be used as args to a connect() call */
        char buf[64];
        expr_type value;

        if (firstpass)
        {
           jar_free (link_name);
           jar_free (delay_expr);
           jar_free (error_expr);
           jar_free (datarate_expr);
           return NULL;
        }

        /*
         * printf("do_channeldescr: name[%s] delay[%s] error[%s] datarate[%s]\n",
         *       link_name, delay_expr, error_expr, datarate_expr);
         */

        /* channel type */
        if (link_name!=NULL)
        {
            /* we must have seen the declaration */
            if (!nl_find (channel_list, link_name))
            {
                sprintf (errstr, "Channel \"%s\" unknown\n", link_name);
                adderr;
            }
            fprintf (tmp, "%slink_p = findLink( \"%s\" );\n",
                     indent, link_name);

            jar_free(link_name);
            /* no delay, error, datarate was given, no more jar_free() */
            return jar_strdup("link_p");
        }

        /* delay, error and data rate expressions */
        if (delay_expr!=NULL)
        {
            get_expression(delay_expr,tmp,value);
            fprintf(tmp, "%sdelay_p = new cPar(); *delay_p = %s;\n",
                     indent,value);
        }
        if (error_expr!=NULL)
        {
            get_expression(error_expr,tmp,value);
            fprintf (tmp, "%serror_p = new cPar(); *error_p = %s;\n",
                     indent,value);
        }
        if (datarate_expr!=NULL)
        {
            get_expression(datarate_expr,tmp,value);
            fprintf (tmp, "%sdatarate_p = new cPar(); *datarate_p = %s;\n",
                     indent,value);
        }

        sprintf(buf, "%s, %s, %s",
                  (delay_expr!=NULL) ?    "delay_p" :    "NULL",
                  (error_expr!=NULL) ?    "error_p" :    "NULL",
                  (datarate_expr!=NULL) ? "datarate_p" : "NULL"
               );

        /*jar_free (link_name);*/
        jar_free (delay_expr);
        jar_free (error_expr);
        jar_free (datarate_expr);
        return jar_strdup( buf );
}

void end_connection (char *channel, char dir, char *dispstr)
{
        if (firstpass) {jar_free (channel); jar_free(dispstr); return;}

        fprintf (tmp, "%sif (do_this_block) // \"if\" in NED\n"
                      "%s{\n", indent, indent);

        do_con_gate_L( dir );
        do_con_gate_R( dir );

        if (channel==NULL)
            channel = jar_strdup( "NULL");

        if (dir=='R')   /* right arrow --> */
        {
            fprintf (tmp, "%sconnect (%s, gateL,\n", indent, modL);
            fprintf (tmp, "%s         %s,\n", indent, channel );
            fprintf (tmp, "%s         %s, gateR);\n\n", indent, modR);

            if (dispstr)
                fprintf (tmp, "%s%s->gate(gateL)->setDisplayString( %s );\n", indent, modL,dispstr);
        }
        else            /* left arrow <-- */
        {
            fprintf (tmp, "%sconnect (%s, gateR,\n", indent, modR);
            fprintf (tmp, "%s         %s,\n", indent, channel );
            fprintf (tmp, "%s         %s, gateL);\n\n", indent, modL);

            if (dispstr)
                fprintf (tmp, "%s%s->gate(gateR)->setDisplayString( %s );\n", indent, modR,dispstr);
        }
        fprintf (tmp, "%scheck_error(); check_memory();\n", indent);

        fprintf (tmp, "%s} // end-if\n\n",
                      indent);

        jar_free (channel);
        jar_free (dispstr);
}

/* void do_physicalmachine (char *pm)
{
        if (firstpass)
        {
            -- pm string contains the quotes too which must be chopped off --
            jar_strcpy( physicalmach_name, pm+1);
            physicalmach_name[ strlen(physicalmach_name)-1 ] = '\0';
        }
        jar_free (pm);
}
*/

/* void do_addvirtualmachine (char *vm)
{
        if (firstpass)
        {
            nl_add_str( &machine_list, vm, physicalmach_name);
        }
        jar_free (vm);
}
*/

void do_condition(char *cond_expr)
{
        if (firstpass) {jar_free(cond_expr); return;}

        if (cond_expr==NULL)
        {
            fprintf(tmp, "\n%sdo_this_block = true; // no \"if\" condition\n", indent);
        }
        else
        {
            expr_type cond;
            fprintf(tmp, "\n%s// if condition for next block:\n", indent);
            get_expression(cond_expr,tmp,cond);
            fprintf(tmp, "%sdo_this_block = (bool) %s;\n", indent,cond);
        }
        jar_free(cond_expr);
}

void open_if(char *cond_expr)
{
        expr_type cond;
        if (firstpass) {jar_free(cond_expr);return;}

        get_expression(cond_expr,tmp,cond);
        fprintf(tmp, "%sif ((long) %s)\n"
                     "%s{\n",
                     indent,cond,indent);
        strcat(indent,"\t");
}

void close_if(void)
{
        if (firstpass) {return;}

        indent[strlen(indent)-1] = '\0';
        fprintf(tmp, "%s}\n\n", indent);
}

void set_checkconns(int a)
{
        cmd.check_conns = a;
}

void do_displaystr_enclosing(char *str)
{
        if (firstpass) {jar_free (str);return;}

        /* note: quotes are included in str */
        fprintf (tmp, "%smod->setDisplayString(dispENCLOSINGMOD, %s );\n\n",
                 indent, str);
        jar_free (str);
}

void do_displaystr_submod(char *str)
{
        if (firstpass) {jar_free (str);return;}

        /* note: quotes are included in str */
        fprintf (tmp, "%s%s->setDisplayString(dispSUBMOD, %s );\n\n",
                 indent, submodule_var, str);
        jar_free (str);

}
