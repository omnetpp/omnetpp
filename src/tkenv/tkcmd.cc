//==========================================================================
//  TKCMD.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cenvir.h"
#include "carray.h"
#include "csimul.h"
#include "cmodule.h"
#include "ctypes.h"
#include "tkapp.h"
#include "tklib.h"
#include "tkinsp.h"

// Command functions:
int newNetwork_cmd(ClientData, Tcl_Interp *, int, char **);
int newRun_cmd(ClientData, Tcl_Interp *, int, char **);
int createSnapshot_cmd(ClientData, Tcl_Interp *, int, char **);
int exitOmnetpp_cmd(ClientData, Tcl_Interp *, int, char **);
int oneStep_cmd(ClientData, Tcl_Interp *, int, char **);
int slowExec_cmd(ClientData, Tcl_Interp *, int, char **);
int run_cmd(ClientData, Tcl_Interp *, int, char **);
int runNoTracing_cmd(ClientData, Tcl_Interp *, int, char **);
int oneStepInModule_cmd(ClientData, Tcl_Interp *, int, char **);
int rebuild_cmd(ClientData, Tcl_Interp *, int, char **);
int startAll_cmd(ClientData, Tcl_Interp *, int, char **);
int callFinish_cmd(ClientData, Tcl_Interp *, int, char **);
int loadLib_cmd(ClientData, Tcl_Interp *, int, char **);

int getNetworkType_cmd(ClientData, Tcl_Interp *, int, char **);
int getFileName_cmd(ClientData, Tcl_Interp *, int, char **);
int getObjectFullpath_cmd(ClientData, Tcl_Interp *, int, char **);
int simulationOk_cmd(ClientData, Tcl_Interp *, int, char **);
int isRunning_cmd(ClientData, Tcl_Interp *, int, char **);
int fillListbox_cmd(ClientData, Tcl_Interp *, int, char **);
int stopSimulation_cmd(ClientData, Tcl_Interp *, int, char **);
int getSimOption_cmd(ClientData, Tcl_Interp *, int, char **);
int setSimOption_cmd(ClientData, Tcl_Interp *, int, char **);

int inspect_cmd(ClientData, Tcl_Interp *, int, char **);
int supportedInspTypes_cmd(ClientData, Tcl_Interp *, int, char **);
int inspectMatching_cmd(ClientData, Tcl_Interp *, int, char **);
int updateInspector_cmd(ClientData, Tcl_Interp *, int, char **);
int writeBackInspector_cmd(ClientData, Tcl_Interp *, int, char **);
int deleteInspector_cmd(ClientData, Tcl_Interp *, int, char **);
int updateInspectors_cmd(ClientData, Tcl_Interp *, int, char **);
int inspectorType_cmd(ClientData, Tcl_Interp *, int, char **);
int inspectorCommand_cmd(ClientData, Tcl_Interp *, int, char **);

int objectNullPointer_cmd(ClientData, Tcl_Interp *, int, char **);
int objectRoot_cmd(ClientData, Tcl_Interp *, int, char **);
int objectSimulation_cmd(ClientData, Tcl_Interp *, int, char **);
int objectSystemModule_cmd(ClientData, Tcl_Interp *, int, char **);
int objectMessageQueue_cmd(ClientData, Tcl_Interp *, int, char **);
int objectModuleLocals_cmd(ClientData, Tcl_Interp *, int, char **);
int objectModuleMembers_cmd(ClientData, Tcl_Interp *, int, char **);
int objectNetworks_cmd(ClientData, Tcl_Interp *, int, char **);
int objectModuleTypes_cmd(ClientData, Tcl_Interp *, int, char **);
int objectChannelTypes_cmd(ClientData, Tcl_Interp *, int, char **);
int objectFunctions_cmd(ClientData, Tcl_Interp *, int, char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // Commands invoked from the menu
   { "opp_newnetwork",     newNetwork_cmd     }, // args: <netname>
   { "opp_newrun",         newRun_cmd         }, // args: <run#>
   { "opp_createsnapshot", createSnapshot_cmd }, // args: <label>
   { "opp_exitomnetpp",    exitOmnetpp_cmd    }, // args: -
   { "opp_onestep",        oneStep_cmd        }, // args: -
   { "opp_slowexec",       slowExec_cmd       }, // args: -
   { "opp_run",            run_cmd            }, // args: ?fast? ?timelimit?
   { "opp_run_notracing",  runNoTracing_cmd   }, // args: ?timelimit?
   { "opp_onestepinmodule",oneStepInModule_cmd}, // args: <window>
   { "opp_rebuild",        rebuild_cmd        }, // args: -
   { "opp_start_all",      startAll_cmd       }, // args: -
   { "opp_call_finish",    callFinish_cmd     }, // args: -
   { "opp_loadlib",        loadLib_cmd        }, // args: <fname>
   // Utility commands
   { "opp_getnetworktype",   getNetworkType_cmd   }, // args: -  ret: type of current network
   { "opp_getfilename",      getFileName_cmd      }, // args: <filetype>  ret: filename
   { "opp_getobjectfullpath",getObjectFullpath_cmd}, // args: <pointer>  ret: fullPath()
   { "opp_simulation_ok",    simulationOk_cmd   }, // args: -  ret: simulation.ok()
   { "opp_is_running",       isRunning_cmd      }, // args: -  ret: is_running
   { "opp_fill_listbox",     fillListbox_cmd    }, // args: <listbox> <ptr> <options>
   { "opp_stopsimulation",   stopSimulation_cmd }, // args: -
   { "opp_getsimoption",     getSimOption_cmd   }, // args: <option-namestr>
   { "opp_setsimoption",     setSimOption_cmd   }, // args: <option-namestr> <value>
   // Inspector stuff
   { "opp_inspect",           inspect_cmd           }, // args: <ptr> <type> <opt> ret: window
   { "opp_supported_insp_types",supportedInspTypes_cmd}, // args: <ptr>  ret: insp type list
   { "opp_inspect_matching",  inspectMatching_cmd   }, // args: <pattern>
   { "opp_updateinspector",   updateInspector_cmd   }, // args: <window>
   { "opp_writebackinspector",writeBackInspector_cmd}, // args: <window>
   { "opp_deleteinspector",   deleteInspector_cmd   }, // args: <window>
   { "opp_updateinspectors",  updateInspectors_cmd  }, // args: -
   { "opp_inspectortype",     inspectorType_cmd     }, // translates inspector type code to namestr and v.v.
   { "opp_inspectorcommand",  inspectorCommand_cmd  }, // args: <window> <args-to-be-passed-to-inspectorCommand>
   // Functions that return object pointers
   { "opp_object_nullpointer",  objectNullPointer_cmd  },
   { "opp_object_root",         objectRoot_cmd         },
   { "opp_object_simulation",   objectSimulation_cmd   },
   { "opp_object_systemmodule", objectSystemModule_cmd },
   { "opp_object_messagequeue", objectMessageQueue_cmd },
   { "opp_object_modulelocals", objectModuleLocals_cmd },
   { "opp_object_modulemembers",objectModuleMembers_cmd},
   { "opp_object_networks",     objectNetworks_cmd     },
   { "opp_object_moduletypes",  objectModuleTypes_cmd  },
   { "opp_object_channeltypes", objectChannelTypes_cmd },
   { "opp_object_functions",    objectFunctions_cmd    },
   { NULL, },
};

//=================================================================
// a utility function:

void splitInspectorName(char *namestr, cObject *&object,int& type)
{
   // namestr is the window path name, sth like ".ptr80005a31-2"
   // split it into pointer string ("ptr80005a31") and inspector type ("2")
   assert(namestr!=0); // must exist
   char *s;
   for (s=namestr; *s!='-' && *s!='\0'; s++);
   assert(*s=='-'); // there must be a '-' in the string
   *s = '\0';
   s++;

   object = (cObject *)strToPtr( ptrbuf );
   type = atoi( s );
}

//=================================================================

int exitOmnetpp_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   exit_omnetpp = 1;
   return TCL_OK;
}

int newNetwork_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->newNetwork( argv[1] );
   return TCL_OK;
}

int newRun_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   int runnr = atoi( argv[1] );

   app->newRun( runnr );
   return TCL_OK;
}

int createSnapshot_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->createSnapshot( argv[1] );
   return TCL_OK;
}

int oneStep_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->doOneStep();
   return TCL_OK;
}

int slowExec_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->runSimulation(0, 0, true, false);
   return TCL_OK;
}

int run_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2 && argc!=4) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   bool fast = (strcmp(argv[1],"fast")==0);
   simtime_t until_time=0;
   long until_event=0;
   if (argc==4)
   {
       until_time = strToSimtime(argv[2]);
       sscanf(argv[3],"%ld",&until_event);
   }
   app->runSimulation( until_time, until_event, 0, fast);
   return TCL_OK;
}

int runNoTracing_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc>3) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   simtime_t until_time=0;
   long until_event=0;
   if (argc==3)
   {
      until_time = strToSimtime(argv[1]);
      sscanf(argv[2],"%ld",&until_event);
   }
   app->runSimulationNoTracing( until_time, until_event );
   return TCL_OK;
}

int oneStepInModule_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);

   // fast run until we get to that module
   app->runSimulation( 0, 0, false, false, (cSimpleModule *)object );

   return TCL_OK;
}

int rebuild_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->rebuildSim();
   return TCL_OK;
}

int startAll_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->startAll();
   return TCL_OK;
}

int callFinish_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->callFinish();
   return TCL_OK;
}

int loadLib_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   return opp_loadlibrary(argv[1]) ? TCL_OK : TCL_ERROR;
}

//--------------

int getNetworkType_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   cNetworkType *n = simulation.networkType();
   interp->result = CONST_CAST(!n ? "" : n->name());
   return TCL_OK;
}

int getFileName_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   if (0==strcmp(argv[1],"ini"))
        interp->result = CONST_CAST(app->getIniFileName());
   else if (0==strcmp(argv[1],"snapshot"))
        interp->result = CONST_CAST(app->getSnapshotFileName());
   else if (0==strcmp(argv[1],"outvector"))
        interp->result = CONST_CAST(app->getOutVectorFileName());
   else if (0==strcmp(argv[1],"parchanges"))
        interp->result = CONST_CAST(app->getParChangeFileName());
   else
        return TCL_ERROR;
   return TCL_OK;
}

int getObjectFullpath_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   cObject *object = (cObject *)strToPtr( argv[1] );
   interp->result = CONST_CAST(object->fullPath());
   return TCL_OK;
}

int simulationOk_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = simulation.ok() ? "1" : "0";
   return TCL_OK;
}

int isRunning_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   interp->result = app->is_running ? "1" : "0";
   return TCL_OK;
}

int fillListbox_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   // Example:
   //   opp_fill_listbox .dialog.frame.listbox objects ptr80004a72 fullpath
   // 1st arg: listbox variable
   // 2nd arg: "modules" or "objects"
   // 3rd arg: pointer ("ptr80004ab1")
   // rest:    options: deep, nameonly etc.

   if (argc<3) return TCL_ERROR;

   // listbox
   char *listbox = argv[1];

   // argv[2]: "objects" or "modules"
   bool modulelist;
   if (strcmp(argv[2],"modules")==0)
       modulelist=true;
   else if (strcmp(argv[2],"objects")==0)
       modulelist=false;
   else
       return TCL_ERROR;

   // argv[3]: pointer
   cObject *object = (cObject *)strToPtr( argv[3] );

   // set defaults & process options
   InfoFunc f = modulelist ? infofunc_module : infofunc_infotext;
   bool deep = false;
   bool simpleonly = false;
   for(int i=4; i<argc; i++)
   {
     if (0==strcmp(argv[i],"deep"))
        deep = true;
     else if (0==strcmp(argv[i],"notdeep"))
        deep = false;
     else if (0==strcmp(argv[i],"allmodules"))
        simpleonly = false;
     else if (0==strcmp(argv[i],"simpleonly"))
        simpleonly = true;
     else if (0==strcmp(argv[i],"nameonly"))
        f = infofunc_nameonly;
     else if (0==strcmp(argv[i],"infotext"))
        f = infofunc_infotext;
     else if (0==strcmp(argv[i],"fullpath"))
        f = infofunc_fullpath;
     else if (0==strcmp(argv[i],"typeandfullpath"))
        f = infofunc_typeandfullpath;
     else
        {interp->result="unrecognized option";return TCL_ERROR;}
   }

   // do the job
   if (modulelist)
        modcollection( (cModule *)object, interp, listbox, f, simpleonly, deep );
   else
        collection( object, interp, listbox, f, deep);
   return TCL_OK;
}

int stopSimulation_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->stop_simulation = true;
   return TCL_OK;
}

int getSimOption_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   if (0==strcmp(argv[1], "stepdelay"))
      sprintf(interp->result,"%lu ms", app->opt_stepdelay);
   else if (0==strcmp(argv[1], "default-run"))
      sprintf(interp->result,"%d", app->opt_default_run);
   else if (0==strcmp(argv[1], "bkpts_enabled"))
      sprintf(interp->result,"%d", app->opt_bkpts_enabled);
   else if (0==strcmp(argv[1], "animation_enabled"))
      sprintf(interp->result,"%d", app->opt_animation_enabled);
   else if (0==strcmp(argv[1], "animation_msgnames"))
      sprintf(interp->result,"%d", app->opt_animation_msgnames);
   else if (0==strcmp(argv[1], "animation_msgcolors"))
      sprintf(interp->result,"%d", app->opt_animation_msgcolors);
   else if (0==strcmp(argv[1], "animation_speed"))
      sprintf(interp->result,"%g", app->opt_animation_speed);
   else if (0==strcmp(argv[1], "print_banners"))
      sprintf(interp->result,"%d", app->opt_print_banners);
   else if (0==strcmp(argv[1], "use_mainwindow"))
      sprintf(interp->result,"%d", app->opt_use_mainwindow);
   else if (0==strcmp(argv[1], "updatefreq_fast"))
      sprintf(interp->result,"%u", app->opt_updatefreq_fast);
   else if (0==strcmp(argv[1], "updatefreq_express"))
      sprintf(interp->result,"%u", app->opt_updatefreq_express);
   else
      return TCL_ERROR;
   return TCL_OK;
}

int setSimOption_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=3) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   if (0==strcmp(argv[1], "stepdelay"))
      app->opt_stepdelay = long(1000*strToSimtime(argv[2])+.5);
   else if (0==strcmp(argv[1], "bkpts_enabled"))
      app->opt_bkpts_enabled = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_enabled"))
      app->opt_animation_enabled = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_msgnames"))
      app->opt_animation_msgnames = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_msgcolors"))
      app->opt_animation_msgcolors = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_speed"))
   {
      sscanf(argv[2],"%lg",&app->opt_animation_speed);
      if (app->opt_animation_speed<0) app->opt_animation_speed=0;
      if (app->opt_animation_speed>3) app->opt_animation_speed=3;
   }
   else if (0==strcmp(argv[1], "print_banners"))
      app->opt_print_banners = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "use_mainwindow"))
      app->opt_use_mainwindow = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "updatefreq_fast"))
      app->opt_updatefreq_fast = atoi(argv[2]);
   else if (0==strcmp(argv[1], "updatefreq_express"))
      app->opt_updatefreq_express = atoi(argv[2]);
   else
      return TCL_ERROR;
   return TCL_OK;
}

//--------------

int inspect_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=3 && argc!=4) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object = (cObject *)strToPtr( argv[1] );
   if (object==NULL) {interp->result="NULL pointer"; return TCL_ERROR;}

   int type;
   if (argv[2][0]>='0' && argv[2][0]<='9')
        type = atoi( argv[2] );
   else if ((type=insptype_code_from_name(argv[2])) < 0)
        {interp->result="unrecognized inspector type";return TCL_ERROR;}

   void *dat = (argc==4) ? (void *)argv[3] : NULL;
   TInspector *insp = app->inspect( object, type, dat );
   interp->result = insp ? insp->windowname : CONST_CAST("");
   return TCL_OK;
}

int supportedInspTypes_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   // expected arg: pointer
   if (argc!=2) return TCL_ERROR;
   cObject *obj = (cObject *)strToPtr( argv[1] );

   int insp_types[32];
   obj->inspector(INSP_GETTYPES, insp_types);

   interp->result[0] = 0;
   for (int i=0; insp_types[i]>=0; i++)
   {
      strcat( interp->result, "{" );
      strcat( interp->result, insptype_name_from_code(insp_types[i]) );
      strcat( interp->result, "} " );
   }
   return TCL_OK;
}

int inspectMatching_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   // expected args: pattern type [countonly]
   if (argc!=3 && argc!=4) return TCL_ERROR;

   char pattern[64];
   strncpy(pattern,argv[1],64); pattern[63]='\0';

   int type;
   if (argv[2][0]>='0' && argv[2][0]<='9')
        type = atoi( argv[2] );
   else if ((type=insptype_code_from_name(argv[2])) < 0)
        {interp->result="unrecognized inspector type";return TCL_ERROR;}

   bool countonly=false;
   if (argc==4)
   {
      if (strcmp(argv[3],"countonly")==0)
         countonly=true;
      else
         return TCL_ERROR;
   }
   int count = inspect_matching( &superhead, interp, pattern, type, countonly);
   sprintf(interp->result,"%d", count);
   return TCL_OK;
}

int updateInspector_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   insp->update();

   return TCL_OK;
}

int writeBackInspector_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   insp->writeBack();
   insp->update();  // show what writeBack() did

   return TCL_OK;
}

int deleteInspector_cmd(ClientData, Tcl_Interp *, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   delete insp; // this will also delete the window

   return TCL_OK;
}

int updateInspectors_cmd(ClientData, Tcl_Interp *, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->updateInspectors();
   return TCL_OK;
}

int inspectorType_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;

   if (strcmp(argv[1],"all")==0)
   {
        *interp->result = '\0';
        for (int i=0; i<NUM_INSPECTORTYPES; i++)
        {
            strcat( interp->result, "{" );
            strcat( interp->result, insptype_name_from_code(i) );
            strcat( interp->result, "} " );
        }
   }
   else if (argv[1][0]>='0' && argv[1][0]<='9')
   {
        int type = atoi( argv[1] );
        char *namestr = insptype_name_from_code( type );
        if (namestr==NULL)
           {interp->result="unrecognized inspector type";return TCL_ERROR;}
        interp->result = namestr;
   }
   else
   {
        int type = insptype_code_from_name( argv[1] );
        if (type<0)
           {interp->result="unrecognized inspector type";return TCL_ERROR;}
        sprintf( interp->result, "%d", type);
   }
   return TCL_OK;
}

int inspectorCommand_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc<2) return TCL_ERROR;
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   return insp->inspectorCommand(interp, argc-2, argv+2);
}

//--------------
int objectNullPointer_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( NO(cObject) );
   return TCL_OK;
}

int objectRoot_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &superhead );
   return TCL_OK;
}

int objectSimulation_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &simulation );
   return TCL_OK;
}

int objectSystemModule_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( simulation.systemModule() );
   return TCL_OK;
}

int objectMessageQueue_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &simulation.msgQueue );
   return TCL_OK;
}

int objectModuleLocals_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   cModule *mod = (cModule *)strToPtr( argv[1] );
   if (!mod || !mod->isSimple()) return TCL_ERROR;
   cSimpleModule *simplemod = (cSimpleModule *)mod;
   interp->result = ptrToStr( &(simplemod->locals) );
   return TCL_OK;
}

int objectModuleMembers_cmd(ClientData, Tcl_Interp *interp, int argc, char **argv)
{
   if (argc!=2) return TCL_ERROR;
   cModule *mod = (cModule *)strToPtr( argv[1] );
   if (!mod || !mod->isSimple()) return TCL_ERROR;
   cSimpleModule *simplemod = (cSimpleModule *)mod;
   interp->result = ptrToStr( &(simplemod->members) );
   return TCL_OK;
}

int objectNetworks_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &networks );
   return TCL_OK;
}

int objectModuleTypes_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &modtypes );
   return TCL_OK;
}

int objectChannelTypes_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &linktypes );
   return TCL_OK;
}

int objectFunctions_cmd(ClientData, Tcl_Interp *interp, int argc, char **)
{
   if (argc!=1) return TCL_ERROR;
   interp->result = ptrToStr( &functions );
   return TCL_OK;
}
