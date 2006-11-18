//==========================================================================
//  TKCMD.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cenvir.h"
#include "carray.h"
#include "csimul.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "cchannel.h"
#include "cstat.h"
#include "cwatch.h"
#include "ctypes.h"
#include "cclassdescriptor.h"
#include "cdispstr.h"
#include "cdispstr.h"
#include "cenum.h"

#include "tkapp.h"
#include "tklib.h"
#include "inspector.h"
#include "inspfactory.h"
#include "matchexpression.h"
#include "matchableobject.h"
#include "patternmatcher.h"
#include "visitor.h"
#include "fsutils.h"

using std::string;


//----------------------------------------------------------------
// Command functions:
int newNetwork_cmd(ClientData, Tcl_Interp *, int, const char **);
int newRun_cmd(ClientData, Tcl_Interp *, int, const char **);
int getIniSectionNames_cmd(ClientData, Tcl_Interp *, int, const char **);
int getIniEntryAsString_cmd(ClientData, Tcl_Interp *, int, const char **);
int createSnapshot_cmd(ClientData, Tcl_Interp *, int, const char **);
int exitOmnetpp_cmd(ClientData, Tcl_Interp *, int, const char **);

int oneStep_cmd(ClientData, Tcl_Interp *, int, const char **);
int run_cmd(ClientData, Tcl_Interp *, int, const char **);
int oneStepInModule_cmd(ClientData, Tcl_Interp *, int, const char **);
int setRunMode_cmd(ClientData, Tcl_Interp *, int, const char **);
int setRunUntil_cmd(ClientData, Tcl_Interp *, int, const char **);
int setRunUntilModule_cmd(ClientData, Tcl_Interp *, int, const char **);
int rebuild_cmd(ClientData, Tcl_Interp *, int, const char **);
int startAll_cmd(ClientData, Tcl_Interp *, int, const char **);
int finishSimulation_cmd(ClientData, Tcl_Interp *, int, const char **);
int loadLib_cmd(ClientData, Tcl_Interp *, int, const char **);

int getRunNumber_cmd(ClientData, Tcl_Interp *, int, const char **);
int getNetworkType_cmd(ClientData, Tcl_Interp *, int, const char **);
int getFileName_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectFullName_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectFullPath_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectClassName_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectInfoString_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectOwner_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectField_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectBaseClass_cmd(ClientData, Tcl_Interp *, int, const char **);
int getObjectId_cmd(ClientData, Tcl_Interp *, int, const char **);
int getChildObjects_cmd(ClientData, Tcl_Interp *, int, const char **);
int getNumChildObjects_cmd(ClientData, Tcl_Interp *, int, const char **);
int hasChildObjects_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSubObjects_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSubObjectsFilt_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSimulationState_cmd(ClientData, Tcl_Interp *, int, const char **);
int stopSimulation_cmd(ClientData, Tcl_Interp *, int, const char **);
int simulationIsStopping_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSimOption_cmd(ClientData, Tcl_Interp *, int, const char **);
int setSimOption_cmd(ClientData, Tcl_Interp *, int, const char **);
int getStringHashCode_cmd(ClientData, Tcl_Interp *, int, const char **);
int displayString_cmd(ClientData, Tcl_Interp *, int, const char **);
int hsbToRgb_cmd(ClientData, Tcl_Interp *, int, const char **);
int getModulePar_cmd(ClientData, Tcl_Interp *, int, const char **);
int setModulePar_cmd(ClientData, Tcl_Interp *, int, const char **);
int moduleByPath_cmd(ClientData, Tcl_Interp *, int, const char **);
int checkPattern_cmd(ClientData, Tcl_Interp *, int, const char **);
int fesMsgs_cmd(ClientData, Tcl_Interp *, int, const char **);
int sortFesAndGetRange_cmd(ClientData, Tcl_Interp *, int, const char **);
int msgArrTimeFromNow_cmd(ClientData, Tcl_Interp *, int, const char **);
int patmatch_cmd(ClientData, Tcl_Interp *, int, const char **);

int inspect_cmd(ClientData, Tcl_Interp *, int, const char **);
int supportedInspTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectByName_cmd(ClientData, Tcl_Interp *, int, const char **);
int updateInspector_cmd(ClientData, Tcl_Interp *, int, const char **);
int writeBackInspector_cmd(ClientData, Tcl_Interp *, int, const char **);
int deleteInspector_cmd(ClientData, Tcl_Interp *, int, const char **);
int markInspectorForDeletion_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspMarkedForDeletion_cmd(ClientData, Tcl_Interp *, int, const char **);
int updateInspectors_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectorType_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectorCommand_cmd(ClientData, Tcl_Interp *, int, const char **);
int getClassDescriptor_cmd(ClientData, Tcl_Interp *, int, const char **);
int getClassDescriptorFor_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv);
int classDescriptor_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv);
int getNameForEnum_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv);

int nullPointer_cmd(ClientData, Tcl_Interp *, int, const char **);
int isNullPointer_cmd(ClientData, Tcl_Interp *, int, const char **);
int isNotNullPointer_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectDefaultList_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectSimulation_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectSystemModule_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectMessageQueue_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectNetworks_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectModuleTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectChannelTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectFunctions_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectClasses_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectEnums_cmd(ClientData, Tcl_Interp *, int, const char **);

int loadNEDFile_cmd(ClientData, Tcl_Interp *, int, const char **);

int colorizeImage_cmd(ClientData, Tcl_Interp *, int, const char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // Commands invoked from the menu
   { "opp_newnetwork",       newNetwork_cmd     }, // args: <netname>
   { "opp_newrun",           newRun_cmd         }, // args: <run#>
   { "opp_createsnapshot",   createSnapshot_cmd }, // args: <label>
   { "opp_exitomnetpp",      exitOmnetpp_cmd    }, // args: -
   { "opp_onestep",          oneStep_cmd        }, // args: -
   { "opp_run",              run_cmd            }, // args: ?fast? ?timelimit?
   { "opp_onestepinmodule",  oneStepInModule_cmd}, // args: <inspectorwindow>
   { "opp_set_run_mode",     setRunMode_cmd     }, // args: fast|normal|slow|express
   { "opp_set_run_until",    setRunUntil_cmd    }, // args: <timelimit> <eventlimit>
   { "opp_set_run_until_module",setRunUntilModule_cmd}, // args: <inspectorwindow>
   { "opp_rebuild",          rebuild_cmd        }, // args: -
   { "opp_start_all",        startAll_cmd       }, // args: -
   { "opp_finish_simulation",finishSimulation_cmd}, // args: -
   { "opp_loadlib",          loadLib_cmd        }, // args: <fname>
   // Utility commands
   { "opp_getrunnumber",     getRunNumber_cmd         }, // args: -  ret: current run
   { "opp_getnetworktype",   getNetworkType_cmd       }, // args: -  ret: type of current network
   { "opp_getinisectionnames",getIniSectionNames_cmd  }, // args: -
   { "opp_getinientryasstring",getIniEntryAsString_cmd}, // args: <section> <key>  ret: value
   { "opp_getfilename",      getFileName_cmd          }, // args: <filetype>  ret: filename
   { "opp_getobjectfullname",getObjectFullName_cmd    }, // args: <pointer>  ret: fullName()
   { "opp_getobjectfullpath",getObjectFullPath_cmd    }, // args: <pointer>  ret: fullPath()
   { "opp_getobjectclassname",getObjectClassName_cmd  }, // args: <pointer>  ret: className()
   { "opp_getobjectbaseclass",getObjectBaseClass_cmd  }, // args: <pointer>  ret: a base class
   { "opp_getobjectid",      getObjectId_cmd          }, // args: <pointer>  ret: object ID (if object has one) or ""
   { "opp_getobjectowner",   getObjectOwner_cmd       }, // args: <pointer>  ret: <ownerptr>
   { "opp_getobjectinfostring",getObjectInfoString_cmd}, // args: <pointer>  ret: info()
   { "opp_getobjectfield",   getObjectField_cmd       }, // args: <pointer> <field>  ret: value of object field (if supported)
   { "opp_getchildobjects",  getChildObjects_cmd      }, // args: <pointer> ret: list with its child object ptrs
   { "opp_getnumchildobjects",getNumChildObjects_cmd  }, // args: <pointer> ret: length of child objects list
   { "opp_haschildobjects",  hasChildObjects_cmd      }, // args: <pointer> ret: 0 or 1
   { "opp_getsubobjects",    getSubObjects_cmd        }, // args: <pointer> ret: list with all object ptrs in subtree
   { "opp_getsubobjectsfilt",getSubObjectsFilt_cmd    }, // args: <pointer> <args> ret: filtered list of object ptrs in subtree
   { "opp_getsimulationstate", getSimulationState_cmd }, // args: -  ret: NONET,READY,RUNNING,ERROR,TERMINATED,etc.
   { "opp_stopsimulation",   stopSimulation_cmd       }, // args: -
   { "opp_simulationisstopping", simulationIsStopping_cmd}, // args: -
   { "opp_getsimoption",     getSimOption_cmd         }, // args: <option-namestr>
   { "opp_setsimoption",     setSimOption_cmd         }, // args: <option-namestr> <value>
   { "opp_getstringhashcode",getStringHashCode_cmd    }, // args: <string> ret: numeric hash code
   { "opp_displaystring",    displayString_cmd        }, // args: <displaystring> <command> <args>
   { "opp_hsb_to_rgb",       hsbToRgb_cmd             }, // args: <@hhssbb> ret: <#rrggbb>
   { "opp_modulebypath",     moduleByPath_cmd         }, // args: <fullpath> ret: modptr
   { "opp_getmodulepar",     getModulePar_cmd         }, // args: <modptr> <parname> ret: value
   { "opp_setmodulepar",     setModulePar_cmd         }, // args: <modptr> <parname> <value>
   { "opp_checkpattern",     checkPattern_cmd         }, // args: <pattern>
   { "opp_fesmsgs",          fesMsgs_cmd              }, // args: <maxnum> <wantSelfMsgs> <wantNonSelfMsgs> <namePattern> <classNamePattern>
   { "opp_sortfesandgetrange",sortFesAndGetRange_cmd  }, // args: -  ret: {minDeltaT maxDeltaT}
   { "opp_msgarrtimefromnow",msgArrTimeFromNow_cmd    }, // args: <modptr>
   { "opp_patmatch",         patmatch_cmd             }, // args: <string> <pattern>

   // Inspector stuff
   { "opp_inspect",           inspect_cmd           }, // args: <ptr> <type> <opt> ret: window
   { "opp_supported_insp_types",supportedInspTypes_cmd}, // args: <ptr>  ret: insp type list
   { "opp_inspectbyname",     inspectByName_cmd     }, // args: <objfullpath> <classname> <insptype> <geom>
   { "opp_updateinspector",   updateInspector_cmd   }, // args: <window>
   { "opp_writebackinspector",writeBackInspector_cmd}, // args: <window>
   { "opp_deleteinspector",   deleteInspector_cmd   }, // args: <window>
   { "opp_markinspectorfordeletion", markInspectorForDeletion_cmd}, // args: <window>
   { "opp_inspmarkedfordeletion", inspMarkedForDeletion_cmd}, // args: <window>
   { "opp_updateinspectors",  updateInspectors_cmd  }, // args: -
   { "opp_inspectortype",     inspectorType_cmd     }, // translates inspector type code to namestr and v.v.
   { "opp_inspectorcommand",  inspectorCommand_cmd  }, // args: <window> <args-to-be-passed-to-inspectorCommand>
   { "opp_getclassdescriptor",getClassDescriptor_cmd}, // args: <window>
   { "opp_getclassdescriptorfor", getClassDescriptorFor_cmd}, // args: <objptr>
   { "opp_classdescriptor",   classDescriptor_cmd   }, // args: <descrptr> <objptr> ...
   { "opp_getnameforenum",    getNameForEnum_cmd    }, // args: <enumname> <number>

   // Functions that return object pointers
   { "opp_null",                nullPointer_cmd        },
   { "opp_isnull",              isNullPointer_cmd      }, // args: <ptr>
   { "opp_isnotnull",           isNotNullPointer_cmd   }, // args: <ptr>
   { "opp_object_defaultlist",  objectDefaultList_cmd  },
   { "opp_object_simulation",   objectSimulation_cmd   },
   { "opp_object_systemmodule", objectSystemModule_cmd },
   { "opp_object_messagequeue", objectMessageQueue_cmd },
   { "opp_object_networks",     objectNetworks_cmd     },
   { "opp_object_moduletypes",  objectModuleTypes_cmd  },
   { "opp_object_channeltypes", objectChannelTypes_cmd },
   { "opp_object_classes",      objectClasses_cmd      },
   { "opp_object_functions",    objectFunctions_cmd    },
   { "opp_object_enums",        objectEnums_cmd        },
   // NEDXML
   { "opp_loadnedfile",         loadNEDFile_cmd        },   // args: <ptr> ret: <xml>
   // experimental
   { "opp_colorizeimage",       colorizeImage_cmd      },   // args: <image> ... ret: -
   // end of list
   { NULL, },
};


//=================================================================

int exitOmnetpp_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   exit_omnetpp = 1;
   return TCL_OK;
}

int newNetwork_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->newNetwork( argv[1] );
   return TCL_OK;
}

int newRun_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   int runnr = atoi( argv[1] );

   app->newRun( runnr );
   return TCL_OK;
}

int getIniSectionNames_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cConfiguration *cfg = app->getConfig();
   int n = cfg->getNumSections();
   const char **sections = new const char *[n];
   for (int i=0; i<n; i++)
       sections[i] = cfg->getSectionName(i);
   char *buf = Tcl_Merge(n,const_cast<char **>(sections));
   delete [] sections;
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   return TCL_OK;
}

int getIniEntryAsString_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   const char *section = argv[1];
   const char *key = argv[2];

   cConfiguration *cfg = app->getConfig();
   const char *value = cfg->getAsString(section, key, "");
   Tcl_SetResult(interp, TCLCONST(value), TCL_VOLATILE);
   return TCL_OK;
}

int createSnapshot_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   TRY( app->createSnapshot(argv[1]) );
   return TCL_OK;
}

int oneStep_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->doOneStep();
   return TCL_OK;
}


static int resolveRunMode(const char *mode)
{
   if (!strcmp(mode,"slow"))
       return TOmnetTkApp::RUNMODE_SLOW;
   else if (!strcmp(mode,"normal"))
       return TOmnetTkApp::RUNMODE_NORMAL;
   else if (!strcmp(mode,"fast"))
       return TOmnetTkApp::RUNMODE_FAST;
   else if (!strcmp(mode,"express"))
       return TOmnetTkApp::RUNMODE_EXPRESS;
   else
       return -1;
}

int run_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2 && argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   int mode = resolveRunMode(argv[1]);
   if (mode==-1) {Tcl_SetResult(interp, "wrong mode argument, should be slow, normal, fast or express", TCL_STATIC);return TCL_ERROR;}

   simtime_t until_time=0;
   long until_event=0;
   if (argc==4)
   {
       until_time = strToSimtime(argv[2]);
       sscanf(argv[3],"%ld",&until_event);
   }

   app->runSimulation(mode, until_time, until_event);
   return TCL_OK;
}

int setRunMode_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   int mode = resolveRunMode(argv[1]);
   if (mode==-1) {Tcl_SetResult(interp, "wrong mode argument, should be slow, normal, fast or express", TCL_STATIC);return TCL_ERROR;}

   app->setSimulationRunMode(mode);
   return TCL_OK;
}

int setRunUntil_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=1 && argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   if (argc==1)
   {
       app->setSimulationRunUntil(0,0);
   }
   else
   {
       simtime_t until_time = strToSimtime(argv[1]);
       long until_event = atol(argv[2]);

       app->setSimulationRunUntil(until_time, until_event);
   }
   return TCL_OK;
}

int setRunUntilModule_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc>2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   if (argc==1)
   {
       app->setSimulationRunUntilModule(NULL);
   }
   else
   {
       cObject *object; int type;
       splitInspectorName( argv[1], object, type);
       if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}
       cModule *mod = dynamic_cast<cModule *>(object);
       if (!mod) {Tcl_SetResult(interp, "object is not a module", TCL_STATIC); return TCL_ERROR;}

       app->setSimulationRunUntilModule(mod);
   }
   return TCL_OK;
}

int oneStepInModule_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2 && argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName(argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}
   cModule *mod = dynamic_cast<cModule *>(object);
   if (!mod) {Tcl_SetResult(interp, "object is not a module", TCL_STATIC); return TCL_ERROR;}

   int mode = TOmnetTkApp::RUNMODE_NORMAL;
   if (argc==3)
   {
       mode = resolveRunMode(argv[2]);
       if (mode==-1) {Tcl_SetResult(interp, "wrong mode argument, should be slow, normal, fast or express", TCL_STATIC);return TCL_ERROR;}
   }

   // fast run until we get to that module
   app->runSimulation(mode, 0, 0, mod);

   return TCL_OK;
}

int rebuild_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->rebuildSim();
   return TCL_OK;
}

int startAll_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->startAll();
   return TCL_OK;
}

int finishSimulation_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->finishSimulation();
   return TCL_OK;
}

int loadLib_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TRY(loadExtensionLibrary(argv[1]));
   return TCL_OK;
}

//--------------

int getRunNumber_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   char buf[16];
   sprintf(buf, "%d", simulation.runNumber());
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int getNetworkType_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cNetworkType *n = simulation.networkType();
   Tcl_SetResult(interp, TCLCONST(!n ? "" : n->name()), TCL_VOLATILE);
   return TCL_OK;
}

int getFileName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   const char *s = NULL;
   if (0==strcmp(argv[1],"ini"))
        s = app->getIniFileName();
   else if (0==strcmp(argv[1],"outvector"))
        s = app->getOutVectorFileName();
   else if (0==strcmp(argv[1],"outscalar"))
        s = app->getOutScalarFileName();
   else if (0==strcmp(argv[1],"snapshot"))
        s = app->getSnapshotFileName();
   else
        return TCL_ERROR;
   Tcl_SetResult(interp, TCLCONST(!s ? "" : s), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectFullName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, TCLCONST(object->fullName()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectFullPath_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, TCLCONST(object->fullPath().c_str()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectClassName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, TCLCONST(object->className()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectOwner_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   Tcl_SetResult(interp, ptrToStr(object->owner()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectInfoString_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   Tcl_SetResult(interp, TCLCONST(object->info().c_str()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectField_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   const char *field = argv[2];

   if (!strcmp(field,"fullName")) {
       Tcl_SetResult(interp, TCLCONST(object->fullName()), TCL_VOLATILE);
   } else if (!strcmp(field,"fullPath")) {
       Tcl_SetResult(interp, TCLCONST(object->fullPath().c_str()), TCL_VOLATILE);
   } else if (!strcmp(field,"className")) {
       Tcl_SetResult(interp, TCLCONST(object->className()), TCL_VOLATILE);
   } else if (!strcmp(field,"info")) {
       Tcl_SetResult(interp, TCLCONST(object->info().c_str()), TCL_VOLATILE);
   } else if (!strcmp(field,"detailedInfo")) {
       Tcl_SetResult(interp, TCLCONST(object->detailedInfo().c_str()), TCL_VOLATILE);
   } else if (!strcmp(field,"displayString")) {
       if (dynamic_cast<cModule *>(object)) {
           cModule *mod = dynamic_cast<cModule *>(object);
           const char *str = mod->hasDisplayString() ? mod->displayString().getString() : "";
           Tcl_SetResult(interp, TCLCONST(str), TCL_VOLATILE);
       } else if (dynamic_cast<cMessage *>(object)) {
           Tcl_SetResult(interp, TCLCONST(dynamic_cast<cMessage *>(object)->displayString()), TCL_VOLATILE);
       } else if (dynamic_cast<cGate *>(object)) {
           cGate *g = dynamic_cast<cGate *>(object);
           const char *str = g->hasDisplayString() ? g->displayString().getString() : "";
           Tcl_SetResult(interp, TCLCONST(str), TCL_VOLATILE);
       } else {
           Tcl_SetResult(interp, "no such field in this object", TCL_STATIC); return TCL_ERROR;
       }
   } else if (!strcmp(field,"backgroundDisplayString")) {
       if (dynamic_cast<cModule *>(object)) {
           cModule *mod = dynamic_cast<cModule *>(object);
           const char *str = mod->hasBackgroundDisplayString() ? mod->backgroundDisplayString().getString() : "";
           Tcl_SetResult(interp, TCLCONST(str), TCL_VOLATILE);
       } else {
           Tcl_SetResult(interp, "no such field in this object", TCL_STATIC); return TCL_ERROR;
       }
   } else if (!strcmp(field,"kind")) {
       if (dynamic_cast<cMessage *>(object)) {
           char buf[20];
           sprintf(buf,"%d", dynamic_cast<cMessage *>(object)->kind());
           Tcl_SetResult(interp, buf, TCL_VOLATILE);
       } else {
           Tcl_SetResult(interp, "no such field in this object", TCL_STATIC); return TCL_ERROR;
       }
   } else if (!strcmp(field,"length")) {
       if (dynamic_cast<cMessage *>(object)) {
           char buf[20];
           sprintf(buf,"%ld", dynamic_cast<cMessage *>(object)->length());
           Tcl_SetResult(interp, buf, TCL_VOLATILE);
       } else if (dynamic_cast<cQueue *>(object)) {
           char buf[20];
           sprintf(buf,"%d", dynamic_cast<cQueue *>(object)->length());
           Tcl_SetResult(interp, buf, TCL_VOLATILE);
       } else {
           Tcl_SetResult(interp, "no such field in this object", TCL_STATIC); return TCL_ERROR;
       }
   } else {
       Tcl_SetResult(interp, "no such field in this object", TCL_STATIC); return TCL_ERROR;
   }
   return TCL_OK;
}

int getObjectBaseClass_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   if (dynamic_cast<cSimpleModule *>(object))
       Tcl_SetResult(interp, "cSimpleModule", TCL_STATIC);
   else if (dynamic_cast<cCompoundModule *>(object))
       Tcl_SetResult(interp, "cCompoundModule", TCL_STATIC);
   else if (dynamic_cast<cMessage *>(object))
       Tcl_SetResult(interp, "cMessage", TCL_STATIC);
   else if (dynamic_cast<cArray *>(object))
       Tcl_SetResult(interp, "cArray", TCL_STATIC);
   else if (dynamic_cast<cQueue *>(object))
       Tcl_SetResult(interp, "cQueue", TCL_STATIC);
   else if (dynamic_cast<cGate *>(object))
       Tcl_SetResult(interp, "cGate", TCL_STATIC);
   else if (dynamic_cast<cPar *>(object))
       Tcl_SetResult(interp, "cPar", TCL_STATIC);
   else if (dynamic_cast<cChannel *>(object))
       Tcl_SetResult(interp, "cChannel", TCL_STATIC);
   else if (dynamic_cast<cOutVector *>(object))
       Tcl_SetResult(interp, "cOutVector", TCL_STATIC);
   else if (dynamic_cast<cStatistic *>(object))
       Tcl_SetResult(interp, "cStatistic", TCL_STATIC);
   else if (dynamic_cast<cMessageHeap *>(object))
       Tcl_SetResult(interp, "cMessageHeap", TCL_STATIC);
   else if (dynamic_cast<cWatchBase *>(object))
       Tcl_SetResult(interp, "cWatchBase", TCL_STATIC);
   else
       Tcl_SetResult(interp, TCLCONST(object->className()), TCL_STATIC);
   return TCL_OK;
}

int getObjectId_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   if (dynamic_cast<cModule *>(object))
   {
       char buf[32];
       sprintf(buf, "%d", dynamic_cast<cModule *>(object)->id());
       Tcl_SetResult(interp, buf, TCL_VOLATILE);
   }
   else
   {
       Tcl_SetResult(interp, "", TCL_STATIC);
   }
   return TCL_OK;
}

int getChildObjects_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   cCollectChildrenVisitor visitor(object);
   visitor.process(object);

   setObjectListResult(interp, &visitor);
   return TCL_OK;
}

int getNumChildObjects_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   cCountChildrenVisitor visitor(object);
   visitor.process(object);
   int count = visitor.getCount();

   char buf[20];
   sprintf(buf, "%d", count);
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int hasChildObjects_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   // TBD may be optimized, by exiting foreach on 1st child
   cCountChildrenVisitor visitor(object);
   visitor.process(object);
   int count = visitor.getCount();

   Tcl_SetResult(interp, TCLCONST(count==0 ? "0" : "1"), TCL_STATIC);
   return TCL_OK;
}

int getSubObjects_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   // args: <ptr> <maxcount>
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   int maxcount = atoi(argv[2]);
   if (!maxcount) {Tcl_SetResult(interp, "maxcount must be a nonzero integer", TCL_STATIC); return TCL_ERROR;}

   cCollectObjectsVisitor visitor;
   visitor.setSizeLimit(maxcount);
   visitor.process(object);

   setObjectListResult(interp, &visitor);
   return TCL_OK;
}

int getSubObjectsFilt_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   E_TRY
   // args: <ptr> <category> <class> <fullpath> <maxcount> <orderby>, where
   //    <category> consists of letters m,q,s,g,v,o;
   //    <class> and <fullpath> may contain wildcards
   //    <order> is one of "Name", "Full Name", "Class" or empty string
   if (argc!=7) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   const char *catstr = argv[2];
   unsigned int category = 0;
   if (strchr(catstr,'a'))
   {
       category = ~0U;  // 'a' = all
   }
   else
   {
       if (strchr(catstr,'m')) category |= CATEGORY_MODULES;
       if (strchr(catstr,'q')) category |= CATEGORY_QUEUES;
       if (strchr(catstr,'s')) category |= CATEGORY_STATISTICS;
       if (strchr(catstr,'g')) category |= CATEGORY_MESSAGES;
       if (strchr(catstr,'v')) category |= CATEGORY_VARIABLES;
       if (strchr(catstr,'p')) category |= CATEGORY_MODPARAMS;
       if (strchr(catstr,'c')) category |= CATEGORY_CHANSGATES;
       if (strchr(catstr,'o')) category |= CATEGORY_OTHERS;
   }
   const char *classnamepattern = argv[3];
   const char *objfullpathpattern = argv[4];
   int maxcount = atoi(argv[5]);
   if (!maxcount) {Tcl_SetResult(interp, "maxcount must be a nonzero integer", TCL_STATIC); return TCL_ERROR;}
   const char *orderby = argv[6];

   // get filtered list
   cFilteredCollectObjectsVisitor visitor;
   visitor.setSizeLimit(maxcount);
   TRY(visitor.setFilterPars(category, classnamepattern, objfullpathpattern));

   visitor.process(object);

   // order it
   if (!strcmp(orderby,""))
       ; // nothing
   else if (!strcmp(orderby,"Name"))
       sortObjectsByName(visitor.getArray(), visitor.getArraySize());
   else if (!strcmp(orderby,"Full name"))
       sortObjectsByFullPath(visitor.getArray(), visitor.getArraySize());
   else if (!strcmp(orderby,"Class"))
       sortObjectsByClassName(visitor.getArray(), visitor.getArraySize());
   else
       {Tcl_SetResult(interp, "wrong sort criteria", TCL_STATIC); return TCL_ERROR;}

   // return list
   setObjectListResult(interp, &visitor);
   return TCL_OK;
   E_CATCH
}

int getSimulationState_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   char *statename;
   switch (app->getSimulationState())
   {
       case TOmnetTkApp::SIM_NONET:       statename = "SIM_NONET"; break;
       case TOmnetTkApp::SIM_NEW:         statename = "SIM_NEW"; break;
       case TOmnetTkApp::SIM_RUNNING:     statename = "SIM_RUNNING"; break;
       case TOmnetTkApp::SIM_READY:       statename = "SIM_READY"; break;
       case TOmnetTkApp::SIM_TERMINATED:  statename = "SIM_TERMINATED"; break;
       case TOmnetTkApp::SIM_ERROR:       statename = "SIM_ERROR"; break;
       case TOmnetTkApp::SIM_FINISHCALLED:statename = "SIM_FINISHCALLED"; break;
       case TOmnetTkApp::SIM_BUSY:        statename = "SIM_BUSY"; break;
       default: Tcl_SetResult(interp, "invalid simulation state", TCL_STATIC); return TCL_ERROR;
   }
   Tcl_SetResult(interp, statename, TCL_STATIC);
   return TCL_OK;
}

int stopSimulation_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->setStopSimulationFlag();
   return TCL_OK;
}

int simulationIsStopping_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   bool stopping = app->getStopSimulationFlag();
   Tcl_SetResult(interp, TCLCONST(stopping ? "1" : "0"), TCL_STATIC);
   return TCL_OK;
}

int getSimOption_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   char buf[32];
   if (0==strcmp(argv[1], "stepdelay"))
      sprintf(buf,"%lu ms", app->opt_stepdelay);
   else if (0==strcmp(argv[1], "default-run"))
      sprintf(buf,"%d", app->opt_default_run);
   else if (0==strcmp(argv[1], "bkpts_enabled"))
      sprintf(buf,"%d", app->opt_bkpts_enabled);
   else if (0==strcmp(argv[1], "animation_enabled"))
      sprintf(buf,"%d", app->opt_animation_enabled);
   else if (0==strcmp(argv[1], "nexteventmarkers"))
      sprintf(buf,"%d", app->opt_nexteventmarkers);
   else if (0==strcmp(argv[1], "senddirect_arrows"))
      sprintf(buf,"%d", app->opt_senddirect_arrows);
   else if (0==strcmp(argv[1], "anim_methodcalls"))
      sprintf(buf,"%d", app->opt_anim_methodcalls);
   else if (0==strcmp(argv[1], "methodcalls_delay"))
      sprintf(buf,"%d", app->opt_methodcalls_delay);
   else if (0==strcmp(argv[1], "animation_msgnames"))
      sprintf(buf,"%d", app->opt_animation_msgnames);
   else if (0==strcmp(argv[1], "animation_msgclassnames"))
      sprintf(buf,"%d", app->opt_animation_msgclassnames);
   else if (0==strcmp(argv[1], "animation_msgcolors"))
      sprintf(buf,"%d", app->opt_animation_msgcolors);
   else if (0==strcmp(argv[1], "penguin_mode"))
      sprintf(buf,"%d", app->opt_penguin_mode);
   else if (0==strcmp(argv[1], "showlayouting"))
      sprintf(buf,"%d", app->opt_showlayouting);
   else if (0==strcmp(argv[1], "bubbles"))
      sprintf(buf,"%d", app->opt_bubbles);
   else if (0==strcmp(argv[1], "animation_speed"))
      sprintf(buf,"%g", app->opt_animation_speed);
   else if (0==strcmp(argv[1], "print_banners"))
      sprintf(buf,"%d", app->opt_print_banners);
   else if (0==strcmp(argv[1], "use_mainwindow"))
      sprintf(buf,"%d", app->opt_use_mainwindow);
   else if (0==strcmp(argv[1], "updatefreq_fast"))
      sprintf(buf,"%u", app->opt_updatefreq_fast);
   else if (0==strcmp(argv[1], "updatefreq_express"))
      sprintf(buf,"%u", app->opt_updatefreq_express);
   else if (0==strcmp(argv[1], "expressmode_autoupdate"))
      sprintf(buf,"%d", app->opt_expressmode_autoupdate);
   else
      return TCL_ERROR;
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int setSimOption_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   if (0==strcmp(argv[1], "stepdelay"))
      app->opt_stepdelay = long(1000*strToSimtime(argv[2])+.5);
   else if (0==strcmp(argv[1], "bkpts_enabled"))
      app->opt_bkpts_enabled = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_enabled"))
      app->opt_animation_enabled = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "nexteventmarkers"))
      app->opt_nexteventmarkers = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "senddirect_arrows"))
      app->opt_senddirect_arrows = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "anim_methodcalls"))
      app->opt_anim_methodcalls = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "methodcalls_delay"))
      app->opt_methodcalls_delay = atoi(argv[2]);
   else if (0==strcmp(argv[1], "animation_msgnames"))
      app->opt_animation_msgnames = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_msgclassnames"))
      app->opt_animation_msgclassnames = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "animation_msgcolors"))
      app->opt_animation_msgcolors = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "penguin_mode"))
      app->opt_penguin_mode = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "showlayouting"))
      app->opt_showlayouting = (argv[2][0]!='0');
   else if (0==strcmp(argv[1], "bubbles"))
      app->opt_bubbles = (argv[2][0]!='0');
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
   else if (0==strcmp(argv[1], "expressmode_autoupdate"))
      app->opt_expressmode_autoupdate = (argv[2][0]!='0');
   else
      return TCL_ERROR;
   return TCL_OK;
}

int getStringHashCode_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   const char *s = argv[1];
   int i = 0;
   long hash = 0;
   while (*s) {
       hash += (i++) * ((*s++ * 173) & 0xff);
   }
   char buf[32];
   sprintf(buf, "%ld", hash);
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int displayString_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   const char *dispstr = argv[1];
   if (0==strcmp(argv[2], "ptrparse"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong argcount for ptrparse", TCL_STATIC); return TCL_ERROR;}
       cDisplayString *dp = (cDisplayString *)strToVoidPtr(argv[1]);
       const char *array = argv[3];
       for (int k=0; k<dp->getNumTags(); k++)
       {
           Tcl_Obj *arglist = Tcl_NewListObj(0, NULL);
           for (int i=0; i<dp->getNumArgs(k); i++)
           {
               const char *s = dp->getTagArg(k,i);
               Tcl_ListObjAppendElement(interp, arglist, Tcl_NewStringObj(TCLCONST(s),-1));
           }
           Tcl_SetVar2Ex(interp, TCLCONST(array), TCLCONST(dp->getTagName(k)), arglist, 0);
       }
   }
   else if (0==strcmp(argv[2], "parse"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong argcount for parse", TCL_STATIC); return TCL_ERROR;}
       const char *array = argv[3];
       cDisplayString dp(dispstr);
       for (int k=0; k<dp.getNumTags(); k++)
       {
           Tcl_Obj *arglist = Tcl_NewListObj(0, NULL);
           for (int i=0; i<dp.getNumArgs(k); i++)
           {
               const char *s = dp.getTagArg(k,i);
               Tcl_ListObjAppendElement(interp, arglist, Tcl_NewStringObj(TCLCONST(s),-1));
           }
           Tcl_SetVar2Ex(interp, TCLCONST(array), TCLCONST(dp.getTagName(k)), arglist, 0);
       }
   }
   else if (0==strcmp(argv[2], "getTagArg"))
   {
       // gettag <tag> <k> -- get kth component of given tag
       if (argc!=5) {Tcl_SetResult(interp, "wrong argcount for getTagArg", TCL_STATIC); return TCL_ERROR;}
       const char *tag = argv[3];
       int k = atoi(argv[4]);
       cDisplayString dp(dispstr);
       const char *val = dp.getTagArg(tag,k);
       Tcl_SetResult(interp, TCLCONST(val), TCL_VOLATILE);
   }
   else
   {
       Tcl_SetResult(interp, "bad command", TCL_STATIC); return TCL_ERROR;
   }
   return TCL_OK;
}


//
// HSB-to-RGB conversion
// source: http://nuttybar.drama.uga.edu/pipermail/dirgames-l/2001-March/006061.html
// Input:   hue, saturation, and brightness as floats scaled from 0.0 to 1.0
// Output:  red, green, and blue as floats scaled from 0.0 to 1.0
//
static void hsbToRgb(double hue, double saturation, double brightness,
                     double& red, double& green, double &blue)
{
   if (brightness == 0.0)
   {   // safety short circuit again
       red   = 0.0;
       green = 0.0;
       blue  = 0.0;
       return;
   }

   if (saturation == 0.0)
   {   // grey
       red   = brightness;
       green = brightness;
       blue  = brightness;
       return;
   }

   float domainOffset;         // hue mod 1/6
   if (hue < 1.0/6)
   {   // red domain; green ascends
       domainOffset = hue;
       red   = brightness;
       blue  = brightness * (1.0 - saturation);
       green = blue + (brightness - blue) * domainOffset * 6;
   }
     else if (hue < 2.0/6)
     { // yellow domain; red descends
       domainOffset = hue - 1.0/6;
       green = brightness;
       blue  = brightness * (1.0 - saturation);
       red   = green - (brightness - blue) * domainOffset * 6;
     }
     else if (hue < 3.0/6)
     { // green domain; blue ascends
       domainOffset = hue - 2.0/6;
       green = brightness;
       red   = brightness * (1.0 - saturation);
       blue  = red + (brightness - red) * domainOffset * 6;
     }
     else if (hue < 4.0/6)
     { // cyan domain; green descends
       domainOffset = hue - 3.0/6;
       blue  = brightness;
       red   = brightness * (1.0 - saturation);
       green = blue - (brightness - red) * domainOffset * 6;
     }
     else if (hue < 5.0/6)
     { // blue domain; red ascends
       domainOffset = hue - 4.0/6;
       blue  = brightness;
       green = brightness * (1.0 - saturation);
       red   = green + (brightness - green) * domainOffset * 6;
     }
     else
     { // magenta domain; blue descends
       domainOffset = hue - 5.0/6;
       red   = brightness;
       green = brightness * (1.0 - saturation);
       blue  = red - (brightness - green) * domainOffset * 6;
     }
}

inline int h2d(char c)
{
    if (c>='0' && c<='9') return c-'0';
    if (c>='A' && c<='F') return c-'A'+10;
    if (c>='a' && c<='f') return c-'a'+10;
    return 0;
}

int hsbToRgb_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *hsb = argv[1];
   if (hsb[0]!='@' || strlen(hsb)!=7) {Tcl_SetResult(interp, "malformed HSB color, format is @hhssbb where h,s,b are hex digits", TCL_STATIC); return TCL_ERROR;}

   // parse hsb
   double hue =        (h2d(hsb[1])*16+h2d(hsb[2]))/256.0;
   double saturation = (h2d(hsb[3])*16+h2d(hsb[4]))/256.0;
   double brightness = (h2d(hsb[5])*16+h2d(hsb[6]))/256.0;

   // convert
   double red, green, blue;
   hsbToRgb(hue, saturation, brightness, red, green, blue);

   // produce rgb
   char rgb[10];
   sprintf(rgb,"#%2.2x%2.2x%2.2x",
                int(min(red*256,255)),
                int(min(green*256,255)),
                int(min(blue*256,255))
          );
   Tcl_SetResult(interp, rgb, TCL_VOLATILE);
   return TCL_OK;
}

int getModulePar_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
   if (!mod) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   const char *parname = argv[2];
   string result;

   TRY( result = mod->par(parname).getAsText() );

   Tcl_SetResult(interp, TCLCONST(result.c_str()), TCL_VOLATILE);
   return TCL_OK;
}

int setModulePar_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cModule *mod = dynamic_cast<cModule *>(strToPtr(argv[1]));
   if (!mod) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   const char *parname = argv[2];
   const char *value = argv[3];

   TRY( mod->par(parname).setFromText(value,'?') );

   return TCL_OK;
}

int moduleByPath_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *path = argv[1];
   cModule *mod;
   TRY(mod = simulation.moduleByPath(path));
   Tcl_SetResult(interp, ptrToStr(mod), TCL_VOLATILE);
   return TCL_OK;
}

int checkPattern_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   E_TRY
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *pattern = argv[1];
   // try parse pattern
   MatchExpression matcher(pattern, false, true, true);
   // let's see if MatchableObjectAdapter can parse field names inside
   // (unmatched "[", etc.), by trying to match some random object
   MatchableObjectAdapter objectAdapter(MatchableObjectAdapter::FULLNAME, &simulation);
   matcher.matches(&objectAdapter);
   return TCL_OK;
   E_CATCH
}

int fesMsgs_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   E_TRY
   if (argc!=6) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   int maxNum = atoi(argv[1]);
   bool wantSelfMsgs = atoi(argv[2])!=0;
   bool wantNonSelfMsgs = atoi(argv[3])!=0;
   const char *namePattern = argv[4];
   const char *classNamePattern = argv[5];

   if (!*namePattern) namePattern = "*";
   MatchExpression nameMatcher(namePattern, false, true, true);

   if (!*classNamePattern) classNamePattern = "*";
   MatchExpression classNameMatcher(classNamePattern, false, true, true);

   Tcl_Obj *listobj = Tcl_NewListObj(0, NULL);
   for (cMessageHeap::Iterator it(simulation.msgQueue); maxNum>0 && !it.end(); it++, maxNum--)
   {
       cMessage *msg = it();
       if (msg->isSelfMessage() ? !wantSelfMsgs : !wantNonSelfMsgs)
           continue;

       MatchableObjectAdapter msgAdapter(MatchableObjectAdapter::FULLNAME, msg);
       if (namePattern[0] && !nameMatcher.matches(&msgAdapter))
           continue;

       msgAdapter.setDefaultAttribute(MatchableObjectAdapter::CLASSNAME);
       if (classNamePattern[0] && !classNameMatcher.matches(&msgAdapter))
           continue;

       Tcl_ListObjAppendElement(interp, listobj, Tcl_NewStringObj(ptrToStr(msg), -1));
   }
   Tcl_SetObjResult(interp, listobj);
   return TCL_OK;
   E_CATCH
}

int sortFesAndGetRange_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   // find smallest t!=simTime() element, and greatest element.
   simulation.msgQueue.sort();
   int len = simulation.msgQueue.length();
   if (len==0) {Tcl_SetResult(interp, "0 0", TCL_STATIC); return TCL_OK;}
   simtime_t now = simulation.simTime();
   simtime_t tmin = now;
   for (int i=0; i<len; i++)
       if (simulation.msgQueue.peek(i)->arrivalTime()!=now)
           {tmin = simulation.msgQueue.peek(i)->arrivalTime(); break;}
   simtime_t tmax = simulation.msgQueue.peek(len-1)->arrivalTime();

   // return result
   Tcl_Obj *listobj = Tcl_NewListObj(0, NULL);
   Tcl_ListObjAppendElement(interp, listobj, Tcl_NewDoubleObj(tmin-now));
   Tcl_ListObjAppendElement(interp, listobj, Tcl_NewDoubleObj(tmax-now));
   Tcl_SetObjResult(interp, listobj);
   return TCL_OK;
}

int msgArrTimeFromNow_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cMessage *msg = dynamic_cast<cMessage *>(strToPtr( argv[1] ));
   if (!msg) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetObjResult(interp, Tcl_NewDoubleObj(msg->arrivalTime()-simulation.simTime()));
   return TCL_OK;
}

int patmatch_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *s = argv[1];
   const char *p = argv[2];
   PatternMatcher pat;
   TRY(pat.setPattern(p, true, true, true));
   Tcl_SetResult(interp, TCLCONST(pat.matches(s) ? "1" : "0"), TCL_STATIC);
   return TCL_OK;
}

//--------------

int inspect_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3 && argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   int type;
   if (argv[2][0]>='0' && argv[2][0]<='9')
        type = atoi( argv[2] );
   else if ((type=insptypeCodeFromName(argv[2])) < 0)
        {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}

   void *dat = (argc==4) ? (void *)argv[3] : NULL;
   TInspector *insp = app->inspect(object, type, "", dat);
   Tcl_SetResult(interp, TCLCONST(insp ? insp->windowName() : ""), TCL_VOLATILE);
   return TCL_OK;
}

int supportedInspTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   // expected arg: pointer
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   // collect supported inspector types
   int insp_types[20];
   int n=0;
   for (cArray::Iterator it(*inspectorfactories.instance()); !it.end(); it++)
   {
      cInspectorFactory *ifc = static_cast<cInspectorFactory *>(it());
      if (ifc->supportsObject(object))
      {
          int k;
          for (k=0; k<n; k++)
              if (insp_types[k] == ifc->inspectorType())
                 break;
          if (k==n) // not yet in array, insert
              insp_types[n++] = ifc->inspectorType();
          assert(n<20);  // fixed-size array :(
      }
   }

   char *buf = Tcl_Alloc(1024);
   buf[0] = '\0';
   for (int j=0; j<n; j++)
   {
      strcat(buf, "{" );
      strcat(buf, insptypeNameFromCode(insp_types[j]) );
      strcat(buf, "} " );
   }
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   return TCL_OK;
}


int inspectByName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   // args: <objfullpath> <classname> <insptype> ?geom?
   if (argc!=4 && argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   const char *fullpath = argv[1];
   const char *classname = argv[2];

   int insptype;
   if (argv[3][0]>='0' && argv[3][0]<='9')
        insptype = atoi( argv[3] );
   else if ((insptype=insptypeCodeFromName(argv[3])) < 0)
        {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}

   const char *geometry = (argc==5) ? argv[4] : NULL;

   inspectObjectByName(fullpath, classname, insptype, geometry);
   return TCL_OK;
}


int updateInspector_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   insp->update();

   return TCL_OK;
}

int writeBackInspector_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   insp->writeBack();
   insp->update();  // show what writeBack() did

   return TCL_OK;
}

int deleteInspector_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   app->deleteInspector(insp);
   return TCL_OK;
}

int markInspectorForDeletion_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   insp->markForDeletion();
   return TCL_OK;
}

int inspMarkedForDeletion_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   Tcl_SetResult(interp, TCLCONST(insp->isMarkedForDeletion() ? "1" : "0"), TCL_STATIC);
   return TCL_OK;
}

int updateInspectors_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();
   app->updateInspectors();
   return TCL_OK;
}

int inspectorType_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   if (strcmp(argv[1],"all")==0)
   {
        char *buf = Tcl_Alloc(1024);
        buf[0] = '\0';
        for (int i=0; i<NUM_INSPECTORTYPES; i++)
        {
            strcat(buf, "{");
            strcat(buf, insptypeNameFromCode(i));
            strcat(buf, "} ");
        }
        Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   }
   else if (argv[1][0]>='0' && argv[1][0]<='9')
   {
        int type = atoi( argv[1] );
        const char *namestr = insptypeNameFromCode( type );
        if (namestr==NULL)
           {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}
        Tcl_SetResult(interp, TCLCONST(namestr), TCL_STATIC);
   }
   else
   {
        int type = insptypeCodeFromName( argv[1] );
        if (type<0)
           {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}
        char buf[20];
        sprintf(buf, "%d", type);
        Tcl_SetResult(interp, buf, TCL_VOLATILE);
   }
   return TCL_OK;
}

int inspectorCommand_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = getTkApplication();

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   return insp->inspectorCommand(interp, argc-2, argv+2);
}

int getClassDescriptor_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   cObject *object = dynamic_cast<cObject *>(strToPtr(argv[1]));
   cClassDescriptor *sd = object ? object->getDescriptor() : NULL;
   Tcl_SetResult(interp, ptrToStr(sd), TCL_VOLATILE);
   return TCL_OK;
}

//--------------
int nullPointer_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr(NULL), TCL_VOLATILE);
   return TCL_OK;
}

int isNullPointer_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *ptr = argv[1];
   Tcl_SetResult(interp, TCLCONST(strcmp(ptr,ptrToStr(NULL))==0 ? "1" : "0"), TCL_STATIC);
   return TCL_OK;
}

int isNotNullPointer_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *ptr = argv[1];
   Tcl_SetResult(interp, TCLCONST(strcmp(ptr,ptrToStr(NULL))==0 ? "0" : "1"), TCL_STATIC);
   return TCL_OK;
}

int objectDefaultList_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &defaultList ), TCL_VOLATILE);
   return TCL_OK;
}

int objectSimulation_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &simulation ), TCL_VOLATILE);
   return TCL_OK;
}

int objectSystemModule_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( simulation.systemModule() ), TCL_VOLATILE);
   return TCL_OK;
}

int objectMessageQueue_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &simulation.msgQueue ), TCL_VOLATILE);
   return TCL_OK;
}

int objectNetworks_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( networks.instance() ), TCL_VOLATILE);
   return TCL_OK;
}

int objectModuleTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( modtypes.instance() ), TCL_VOLATILE);
   return TCL_OK;
}

int objectChannelTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( channeltypes.instance() ), TCL_VOLATILE);
   return TCL_OK;
}

int objectFunctions_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( functions.instance() ), TCL_VOLATILE);
   return TCL_OK;
}

int objectClasses_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( classes.instance() ), TCL_VOLATILE);
   return TCL_OK;
}

int objectEnums_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( enums.instance() ), TCL_VOLATILE);
   return TCL_OK;
}

int loadNEDFile_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "1 arg expected", TCL_STATIC); return TCL_ERROR;}
   const char *fname = argv[1];
   TOmnetTkApp *app = getTkApplication();
   app->loadNedFile(fname);
   return TCL_OK;
}

int colorizeImage_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=4) {Tcl_SetResult(interp, "3 args expected", TCL_STATIC); return TCL_ERROR;}
   const char *imgname = argv[1];
   const char *targetcolorname = argv[2];
   const char *weightstr = argv[3]; // 0-100

   Tk_PhotoHandle imghandle = Tk_FindPhoto(interp, TCLCONST(imgname));
   if (!imghandle)
   {
       Tcl_SetResult(interp, "image doesn't exist or is not a photo image", TCL_STATIC);
       return TCL_ERROR;
   }
   Tk_PhotoImageBlock imgblock;
   Tk_PhotoGetImage(imghandle, &imgblock);

   if (imgblock.pixelSize!=4)
   {
       Tcl_SetResult(interp, "unsupported pixelsize in photo image", TCL_STATIC);
       return TCL_ERROR;
   }

   XColor *targetcolor = Tk_GetColor(interp, Tk_MainWindow(interp), TCLCONST(targetcolorname));
   if (!targetcolor)
   {
       Tcl_SetResult(interp, "invalid color", TCL_STATIC);
       return TCL_ERROR;
   }
   int rdest = targetcolor->red / 256;  // scale down to 8 bits
   int gdest = targetcolor->green / 256;
   int bdest = targetcolor->blue / 256;
   Tk_FreeColor(targetcolor);

   double weight = atol(weightstr)/100.0;
   if (weight<0 || weight>1.0)
   {
       Tcl_SetResult(interp, "colorizing weight is out of range, should be between 0 and 100", TCL_STATIC);
       return TCL_ERROR;
   }

   int redoffset = imgblock.offset[0];
   int greenoffset = imgblock.offset[1];
   int blueoffset = imgblock.offset[2];
   for (int y=0; y<imgblock.height; y++)
   {
       unsigned char *pixel = imgblock.pixelPtr + y*imgblock.pitch;
       for (int x=0; x<imgblock.width; x++, pixel+=imgblock.pixelSize)
       {
           // extract
           int r = pixel[redoffset];
           int g = pixel[greenoffset];
           int b = pixel[blueoffset];

           // transform
           int lum = (int)(0.2126*r + 0.7152*g + 0.0722*b);
           r = (int)((1-weight)*r + weight*lum*rdest/128.0);
           g = (int)((1-weight)*g + weight*lum*gdest/128.0);
           b = (int)((1-weight)*b + weight*lum*bdest/128.0);

           // fix range
           r = r<0 ? 0 : r>255 ? 255 : r;
           g = g<0 ? 0 : g>255 ? 255 : g;
           b = b<0 ? 0 : b>255 ? 255 : b;

           // and put back
           pixel[redoffset] = r;
           pixel[greenoffset] = g;
           pixel[blueoffset] = b;
       }
   }
   return TCL_OK;
}

int getClassDescriptorFor_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cPolymorphic *object = strToPtr(argv[1]);
   cClassDescriptor *sd = object ? object->getDescriptor() : NULL;
   Tcl_SetResult(interp, ptrToStr(sd), TCL_VOLATILE);
   return TCL_OK;
}

int getNameForEnum_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   const char *enumName = argv[1];
   int value = atoi(argv[2]);
   cEnum *e = findEnum(enumName);
   Tcl_SetResult(interp, TCLCONST(e ? e->stringFor(value) : ""), TCL_VOLATILE);
   return TCL_OK;
}

static char tmpbuf[16384];  //XXX

int classDescriptor_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   E_TRY
   if (argc<4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   void *object = strToPtr(argv[1]);
   cClassDescriptor *sd = dynamic_cast<cClassDescriptor *>(strToPtr(argv[2]));
   if (!object) {Tcl_SetResult(interp, "object is null", TCL_STATIC); return TCL_ERROR;}
   if (!sd) {Tcl_SetResult(interp, "classdescriptor is null", TCL_STATIC); return TCL_ERROR;}
   const char *cmd = argv[3];

   // 'opp_classdescriptor <object> <classdescr> name'
   if (strcmp(cmd,"name")==0)
   {
      if (argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      Tcl_SetResult(interp, TCLCONST(sd->name()), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> baseclassdesc'
   if (strcmp(cmd,"baseclassdesc")==0)
   {
      if (argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      Tcl_SetResult(interp, ptrToStr(sd->getBaseClassDescriptor()), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> property <propertyname>'
   if (strcmp(cmd,"property")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      const char *propertyname = argv[4];

      // "object" is unused, but what the hell
      Tcl_SetResult(interp, TCLCONST(sd->getProperty(propertyname)), TCL_VOLATILE);
      return TCL_OK;
   }

   if (strcmp(cmd,"fieldcount")==0)
   {
      if (argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      char buf[20];
      sprintf(buf, "%d", sd->getFieldCount(object));
      Tcl_SetResult(interp, buf, TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldisarray <fieldindex>'
   if (strcmp(cmd,"fieldisarray")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldIsArray(object, fld) ? "1" : "0"), TCL_STATIC);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldiscompound <fieldindex>'
   if (strcmp(cmd,"fieldiscompound")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldIsCompound(object, fld) ? "1" : "0"), TCL_STATIC);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldispointer <fieldindex>'
   if (strcmp(cmd,"fieldispointer")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldIsPointer(object, fld) ? "1" : "0"), TCL_STATIC);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldiscpolymorphic <fieldindex>'
   if (strcmp(cmd,"fieldiscpolymorphic")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldIsCPolymorphic(object, fld) ? "1" : "0"), TCL_STATIC);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldiscobject <fieldindex>'
   if (strcmp(cmd,"fieldiscobject")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldIsCObject(object, fld) ? "1" : "0"), TCL_STATIC);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldiseditable <fieldindex>'
   if (strcmp(cmd,"fieldiseditable")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldIsEditable(object, fld) ? "1" : "0"), TCL_STATIC);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldname <fieldindex>'
   if (strcmp(cmd,"fieldname")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldName(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldtypename <fieldindex>'
   if (strcmp(cmd,"fieldtypename")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldTypeString(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldarraysize <fieldindex>'
   if (strcmp(cmd,"fieldarraysize")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      char buf[20];
      sprintf(buf, "%d", sd->getArraySize(object, fld));
      Tcl_SetResult(interp, buf, TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldvalue <fieldindex> ?index?'
   if (strcmp(cmd,"fieldvalue")==0)
   {
      if (argc!=5 && argc!=6) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      int i = (argc==6) ? atoi(argv[5]) : 0;
      if (!sd->getFieldAsString(object, fld, i, tmpbuf, sizeof(tmpbuf)-1))
      {
         Tcl_SetResult(interp, "error in getFieldAsString()", TCL_STATIC);
         return TCL_ERROR;
      }
      Tcl_SetResult(interp, tmpbuf, TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldsetvalue <fieldindex> <index> <value>'
   if (strcmp(cmd,"fieldsetvalue")==0)
   {
      if (argc!=7) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      int i = atoi(argv[5]);  // 0 if unspec (empty string)
      const char *value = argv[6];
      if (!sd->setFieldAsString(object, fld, i, value))
      {
         Tcl_SetResult(interp, "Syntax error", TCL_STATIC);
         return TCL_ERROR;
      }
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldproperty <fieldindex> <propertyname>'
   if (strcmp(cmd,"fieldproperty")==0)
   {
      if (argc!=6) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      const char *propertyname = argv[5];

      Tcl_SetResult(interp, TCLCONST(sd->getFieldProperty(object, fld, propertyname)), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldstructname <fieldindex>'
   if (strcmp(cmd,"fieldstructname")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      Tcl_SetResult(interp, TCLCONST(sd->getFieldStructName(object, fld)), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldstructdesc <fieldindex>'
   if (strcmp(cmd,"fieldstructdesc")==0)
   {
      if (argc!=5) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      const char *fieldStructName = sd->getFieldStructName(object, fld);
      Tcl_SetResult(interp, ptrToStr(cClassDescriptor::getDescriptorFor(fieldStructName)), TCL_VOLATILE);
      return TCL_OK;
   }

   // 'opp_classdescriptor <object> <classdescr> fieldstructpointer <fieldindex> ?index?'
   if (strcmp(cmd,"fieldstructpointer")==0)
   {
      if (argc!=5 && argc!=6) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
      int fld = atoi(argv[4]);
      int i = (argc==6) ? atoi(argv[5]) : 0;
      Tcl_SetResult(interp, voidPtrToStr(sd->getFieldStructPointer(object, fld, i)), TCL_VOLATILE);
      return TCL_OK;
   }
   Tcl_SetResult(interp, "first arg is not a valid option", TCL_STATIC);
   return TCL_ERROR;
   E_CATCH;
}

