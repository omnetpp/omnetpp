//==========================================================================
//  TKCMD.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
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
#include "cstruct.h"
#include "cinifile.h"
#include "tkapp.h"
#include "tklib.h"
#include "inspector.h"
#include "inspfactory.h"

// temporary place for visitor stuff -- after it boils down, should be
// moved to sim/


//-----------------------------------------------------------------------

/**
 * cVisitor base class
 */
class cVisitor
{
  public:
    /**
     * Virtual destructor.
     */
    virtual ~cVisitor() {}
    /**
     * Called on each immediate child object. Should be redefined by user.
     */
    virtual void visit(cObject *obj) = 0;

    /**
     * Emulate cObject::foreachChild() with the foreach() we have...
     */
    virtual void traverseChildrenOf(cObject *obj);
};


static bool do_foreach_child_call_visitor(cObject *obj, bool beg, cObject *_parent, cVisitor *_visitor)
{
    static cVisitor *visitor;
    static cObject *parent;
    if (!obj) {       // setup
         visitor = _visitor;
         parent = _parent;
         return false;
    }
    if (beg && obj==parent)
	     return true;
    if (beg && obj!=parent)
	     visitor->visit(obj);
    return false;
}

void cVisitor::traverseChildrenOf(cObject *obj)
{
    do_foreach_child_call_visitor(0,false,obj,this);
    obj->forEach((ForeachFunc)do_foreach_child_call_visitor);
}

//-----------------------------------------------------------------------
/**
 * Sample code:
 *   cCollectObjectsVisitor v;
 *   v->visit(object);
 *   cObject **objs = v->getArray();
 */
class cCollectObjectsVisitor : public cVisitor
{
  private:
    cObject **arr;
    int firstfree;
    int size;
  public:
    cCollectObjectsVisitor();
    ~cCollectObjectsVisitor();
    virtual void visit(cObject *obj);
    cObject **getArray()  {return arr;}
    int getArraySize()  {return firstfree;}
};

cCollectObjectsVisitor::cCollectObjectsVisitor()
{
    size=16;
    arr = new cObject *[size];
    firstfree=0;
}

cCollectObjectsVisitor::~cCollectObjectsVisitor()
{
    delete arr;
}

void cCollectObjectsVisitor::visit(cObject *obj)
{
    // if array is full, reallocate
    if (size==firstfree)
    {
        cObject **arr2 = new cObject *[2*size];
        for (int i=0; i<firstfree; i++) arr2[i] = arr[i];
        delete [] arr;
        arr = arr2;
        size = 2*size;
    }

    // add pointer to array
    arr[firstfree++] = obj;

    // go to children
    traverseChildrenOf(obj);
}

//----------------------------------------------------------------

// Command functions:
int newNetwork_cmd(ClientData, Tcl_Interp *, int, const char **);
int newRun_cmd(ClientData, Tcl_Interp *, int, const char **);
int getIniSectionNames_cmd(ClientData, Tcl_Interp *, int, const char **);
int createSnapshot_cmd(ClientData, Tcl_Interp *, int, const char **);
int exitOmnetpp_cmd(ClientData, Tcl_Interp *, int, const char **);
int oneStep_cmd(ClientData, Tcl_Interp *, int, const char **);
int slowExec_cmd(ClientData, Tcl_Interp *, int, const char **);
int run_cmd(ClientData, Tcl_Interp *, int, const char **);
int runExpress_cmd(ClientData, Tcl_Interp *, int, const char **);
int oneStepInModule_cmd(ClientData, Tcl_Interp *, int, const char **);
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
int getChildObjects_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSubObjects_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSimulationState_cmd(ClientData, Tcl_Interp *, int, const char **);
int fillListbox_cmd(ClientData, Tcl_Interp *, int, const char **);
int stopSimulation_cmd(ClientData, Tcl_Interp *, int, const char **);
int getSimOption_cmd(ClientData, Tcl_Interp *, int, const char **);
int setSimOption_cmd(ClientData, Tcl_Interp *, int, const char **);

int inspect_cmd(ClientData, Tcl_Interp *, int, const char **);
int supportedInspTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectMatching_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectByName_cmd(ClientData, Tcl_Interp *, int, const char **);
int updateInspector_cmd(ClientData, Tcl_Interp *, int, const char **);
int writeBackInspector_cmd(ClientData, Tcl_Interp *, int, const char **);
int deleteInspector_cmd(ClientData, Tcl_Interp *, int, const char **);
int updateInspectors_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectorType_cmd(ClientData, Tcl_Interp *, int, const char **);
int inspectorCommand_cmd(ClientData, Tcl_Interp *, int, const char **);
int hasDescriptor_cmd(ClientData, Tcl_Interp *, int, const char **);

int objectNullPointer_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectSimulation_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectSystemModule_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectMessageQueue_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectModuleLocals_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectModuleMembers_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectNetworks_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectModuleTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectChannelTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int objectFunctions_cmd(ClientData, Tcl_Interp *, int, const char **);

// command table
OmnetTclCommand tcl_commands[] = {
   // Commands invoked from the menu
   { "opp_newnetwork",       newNetwork_cmd     }, // args: <netname>
   { "opp_newrun",           newRun_cmd         }, // args: <run#>
   { "opp_createsnapshot",   createSnapshot_cmd }, // args: <label>
   { "opp_exitomnetpp",      exitOmnetpp_cmd    }, // args: -
   { "opp_onestep",          oneStep_cmd        }, // args: -
   { "opp_slowexec",         slowExec_cmd       }, // args: -
   { "opp_run",              run_cmd            }, // args: ?fast? ?timelimit?
   { "opp_run_express",      runExpress_cmd     }, // args: ?timelimit?
   { "opp_onestepinmodule",  oneStepInModule_cmd}, // args: <window>
   { "opp_rebuild",          rebuild_cmd        }, // args: -
   { "opp_start_all",        startAll_cmd       }, // args: -
   { "opp_finish_simulation",finishSimulation_cmd}, // args: -
   { "opp_loadlib",          loadLib_cmd        }, // args: <fname>
   // Utility commands
   { "opp_getrunnumber",     getRunNumber_cmd   }, // args: -  ret: current run
   { "opp_getnetworktype",   getNetworkType_cmd   }, // args: -  ret: type of current network
   { "opp_getinisectionnames",getIniSectionNames_cmd}, // args: -
   { "opp_getfilename",      getFileName_cmd      }, // args: <filetype>  ret: filename
   { "opp_getobjectfullname",getObjectFullName_cmd}, // args: <pointer>  ret: fullName()
   { "opp_getobjectfullpath",getObjectFullPath_cmd}, // args: <pointer>  ret: fullPath()
   { "opp_getobjectclassname",getObjectClassName_cmd}, // args: <pointer>  ret: className()
   { "opp_getchildobjects",  getChildObjects_cmd    }, // args: <pointer> ret: list with its child object ptrs
   { "opp_getsubobjects",    getSubObjects_cmd    }, // args: <pointer> ret: list with all object ptrs in subtree
   { "opp_getsimulationstate", getSimulationState_cmd }, // args: -  ret: NONET,READY,RUNNING,ERROR,TERMINATED,etc.
   { "opp_fill_listbox",     fillListbox_cmd    }, // args: <listbox> <ptr> <options>
   { "opp_stopsimulation",   stopSimulation_cmd }, // args: -
   { "opp_getsimoption",     getSimOption_cmd   }, // args: <option-namestr>
   { "opp_setsimoption",     setSimOption_cmd   }, // args: <option-namestr> <value>
   // Inspector stuff
   { "opp_inspect",           inspect_cmd           }, // args: <ptr> <type> <opt> ret: window
   { "opp_supported_insp_types",supportedInspTypes_cmd}, // args: <ptr>  ret: insp type list
   { "opp_inspect_matching",  inspectMatching_cmd   }, // args: <pattern>
   { "opp_inspectbyname",     inspectByName_cmd     }, // args: <objfullpath> <classname> <insptype> <geom>
   { "opp_updateinspector",   updateInspector_cmd   }, // args: <window>
   { "opp_writebackinspector",writeBackInspector_cmd}, // args: <window>
   { "opp_deleteinspector",   deleteInspector_cmd   }, // args: <window>
   { "opp_updateinspectors",  updateInspectors_cmd  }, // args: -
   { "opp_inspectortype",     inspectorType_cmd     }, // translates inspector type code to namestr and v.v.
   { "opp_inspectorcommand",  inspectorCommand_cmd  }, // args: <window> <args-to-be-passed-to-inspectorCommand>
   { "opp_hasdescriptor",     hasDescriptor_cmd     }, // args: <window>
   // Functions that return object pointers
   { "opp_object_nullpointer",  objectNullPointer_cmd  },
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

void splitInspectorName(const char *namestr, cObject *&object, int& type)
{
   // namestr is the window path name, sth like ".ptr80005a31-2"
   // split it into pointer string ("ptr80005a31") and inspector type ("2")
   assert(namestr!=0); // must exist
   assert(namestr[0]=='.');  // must begin with a '.'

   // find '-' and temporarily replace it with EOS
   char *s;
   for (s=const_cast<char *>(namestr); *s!='-' && *s!='\0'; s++);
   assert(*s=='-');  // there must be a '-' in the string
   *s = '\0';

   object = (cObject *)strToPtr(namestr+1);
   type = atoi(s+1);
   *s = '-';  // restore '-'
}

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
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->newNetwork( argv[1] );
   return TCL_OK;
}

int newRun_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   int runnr = atoi( argv[1] );

   app->newRun( runnr );
   return TCL_OK;
}

int getIniSectionNames_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cIniFile *inifile = app->getIniFile();
   int n = inifile->getNumSections();
   const char **sections = new const char *[n];
   for (int i=0; i<n; i++)
       sections[i] = inifile->getSectionName(i);
   char *buf = Tcl_Merge(n,sections);
   delete [] sections;
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   return TCL_OK;
}

int createSnapshot_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->createSnapshot( argv[1] );
   return TCL_OK;
}

int oneStep_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->doOneStep();
   return TCL_OK;
}

int slowExec_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->runSimulation(0, 0, true, false);
   return TCL_OK;
}

int run_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2 && argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
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

int runExpress_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc>3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   simtime_t until_time=0;
   long until_event=0;
   if (argc==3)
   {
      until_time = strToSimtime(argv[1]);
      sscanf(argv[2],"%ld",&until_event);
   }
   app->runSimulationExpress( until_time, until_event );
   return TCL_OK;
}

int oneStepInModule_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   // fast run until we get to that module
   app->runSimulation( 0, 0, false, false, (cSimpleModule *)object );

   return TCL_OK;
}

int rebuild_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->rebuildSim();
   return TCL_OK;
}

int startAll_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->startAll();
   return TCL_OK;
}

int finishSimulation_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->finishSimulation();
   return TCL_OK;
}

int loadLib_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   return opp_loadlibrary(argv[1]) ? TCL_OK : TCL_ERROR;
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
   Tcl_SetResult(interp, const_cast<char*>(!n ? "" : n->name()), TCL_VOLATILE);
   return TCL_OK;
}

int getFileName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

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
   Tcl_SetResult(interp, const_cast<char*>(!s ? "" : s), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectFullName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, const_cast<char*>(object->fullName()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectFullPath_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, const_cast<char*>(object->fullPath()), TCL_VOLATILE);
   return TCL_OK;
}

int getObjectClassName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, const_cast<char*>(object->className()), TCL_VOLATILE);
   return TCL_OK;
}

int getChildObjects_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   Tcl_SetResult(interp, "not implemented", TCL_STATIC); // TBD
   return TCL_ERROR;
}

int getSubObjects_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   cCollectObjectsVisitor visitor;
   visitor.visit(object);

   const int ptrsize = 21; // one ptr should be max 20 chars (good for even 64bit-ptrs)
   int n = visitor.getArraySize();
   cObject **objs = visitor.getArray();
   char *buf = Tcl_Alloc(ptrsize*n);
   char *s=buf;
   for (int i=0; i<n; i++)
   {
       ptrToStr(objs[i],s);
       s+=strlen(s);
       assert(strlen(s)<=20);
       *s++ = ' ';
   }
   *s='\0';
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   return TCL_OK;
}

int getSimulationState_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

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
       default: Tcl_SetResult(interp, "invalid simulation state", TCL_STATIC); return TCL_ERROR;
   }
   Tcl_SetResult(interp, statename, TCL_STATIC);
   return TCL_OK;
}

int fillListbox_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   // Example:
   //   opp_fill_listbox .dialog.frame.listbox objects ptr80004a72 fullpath
   // 1st arg: listbox variable
   // 2nd arg: "modules" or "objects"
   // 3rd arg: pointer ("ptr80004ab1")
   // rest:    options: deep, nameonly etc.

   if (argc<3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   // listbox
   const char *listbox = argv[1];

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
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

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
        {Tcl_SetResult(interp, "unrecognized option", TCL_STATIC);return TCL_ERROR;}
   }

   // do the job
   if (modulelist)
        fillListboxWithChildModules( (cModule *)object, interp, listbox, f, simpleonly, deep );
   else
        fillListboxWithChildObjects( object, interp, listbox, f, deep);
   return TCL_OK;
}

int stopSimulation_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
   app->setStopSimulationFlag();
   return TCL_OK;
}

int getSimOption_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   char buf[32];
   if (0==strcmp(argv[1], "stepdelay"))
      sprintf(buf,"%lu ms", app->opt_stepdelay);
   else if (0==strcmp(argv[1], "default-run"))
      sprintf(buf,"%d", app->opt_default_run);
   else if (0==strcmp(argv[1], "bkpts_enabled"))
      sprintf(buf,"%d", app->opt_bkpts_enabled);
   else if (0==strcmp(argv[1], "animation_enabled"))
      sprintf(buf,"%d", app->opt_animation_enabled);
   else if (0==strcmp(argv[1], "animation_msgnames"))
      sprintf(buf,"%d", app->opt_animation_msgnames);
   else if (0==strcmp(argv[1], "animation_msgcolors"))
      sprintf(buf,"%d", app->opt_animation_msgcolors);
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
   else
      return TCL_ERROR;
   Tcl_SetResult(interp, buf, TCL_VOLATILE);
   return TCL_OK;
}

int setSimOption_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
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

int inspect_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3 && argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   int type;
   if (argv[2][0]>='0' && argv[2][0]<='9')
        type = atoi( argv[2] );
   else if ((type=insptype_code_from_name(argv[2])) < 0)
        {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}

   void *dat = (argc==4) ? (void *)argv[3] : NULL;
   TInspector *insp = app->inspect(object, type, "", dat);
   Tcl_SetResult(interp, const_cast<char*>(insp ? insp->windowName() : ""), TCL_VOLATILE);
   return TCL_OK;
}

int supportedInspTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   // expected arg: pointer
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cObject *object = (cObject *)strToPtr( argv[1] );
   if (!object) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}

   // collect supported inspector types
   int insp_types[20];
   int n=0;
   for (cIterator i(inspectorfactories); !i.end(); i++)
   {
      cInspectorFactory *ifc = (cInspectorFactory *) i();
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
      strcat(buf, insptype_name_from_code(insp_types[j]) );
      strcat(buf, "} " );
   }
   Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   return TCL_OK;
}

int inspectMatching_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   // expected args: pattern type [countonly]
   if (argc!=3 && argc!=4) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   char pattern[64];
   strncpy(pattern,argv[1],64); pattern[63]='\0';

   int type;
   if (argv[2][0]>='0' && argv[2][0]<='9')
        type = atoi( argv[2] );
   else if ((type=insptype_code_from_name(argv[2])) < 0)
        {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}

   bool countonly=false;
   if (argc==4)
   {
      if (strcmp(argv[3],"countonly")==0)
         countonly=true;
      else
         return TCL_ERROR;
   }
   int count = inspectMatchingObjects(&simulation, interp, pattern, type, countonly); // FIXME was 'superhead'!
   sprintf(interp->result,"%d", count); // FIXME use Tcl_SetResult()
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
   else if ((insptype=insptype_code_from_name(argv[3])) < 0)
        {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}

   const char *geometry = (argc==5) ? argv[4] : NULL;

   inspectObjectByName(fullpath, classname, insptype, geometry);
   return TCL_OK;
}


int updateInspector_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

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
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

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
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   insp->setOwner(NULL); // to avoid trouble, cause insp's dtor calls objectDeleted() too!
   delete insp; // this will also delete the window
   return TCL_OK;
}

int updateInspectors_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;
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
            strcat(buf, insptype_name_from_code(i));
            strcat(buf, "} ");
        }
        Tcl_SetResult(interp, buf, TCL_DYNAMIC);
   }
   else if (argv[1][0]>='0' && argv[1][0]<='9')
   {
        int type = atoi( argv[1] );
        const char *namestr = insptype_name_from_code( type );
        if (namestr==NULL)
           {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}
        Tcl_SetResult(interp, const_cast<char *>(namestr), TCL_STATIC);
   }
   else
   {
        int type = insptype_code_from_name( argv[1] );
        if (type<0)
           {Tcl_SetResult(interp, "unrecognized inspector type", TCL_STATIC);return TCL_ERROR;}
        sprintf( interp->result, "%d", type); // FIXME use Tcl_SetResult()
   }
   return TCL_OK;
}

int inspectorCommand_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   TOmnetTkApp *app = (TOmnetTkApp *)ev.app;

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   TInspector *insp = app->findInspector( object, type );
   assert(insp!=NULL);

   return insp->inspectorCommand(interp, argc-2, argv+2);
}

int hasDescriptor_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}

   cObject *object; int type;
   splitInspectorName( argv[1], object, type);
   if (!object) {Tcl_SetResult(interp, "wrong inspectorname string", TCL_STATIC); return TCL_ERROR;}

   Tcl_SetResult(interp, const_cast<char *>(cStructDescriptor::hasDescriptor(object->className()) ? "1" : "0"), TCL_VOLATILE);
   return TCL_OK;
}

//--------------
int objectNullPointer_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( NULL ), TCL_VOLATILE);
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

int objectModuleLocals_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cModule *mod = (cModule *)strToPtr( argv[1] );
   if (!mod) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   if (!mod || !mod->isSimple()) return TCL_ERROR;
   cSimpleModule *simplemod = (cSimpleModule *)mod;
   Tcl_SetResult(interp, ptrToStr( &(simplemod->locals) ), TCL_VOLATILE);
   return TCL_OK;
}

int objectModuleMembers_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   cModule *mod = (cModule *)strToPtr( argv[1] );
   if (!mod) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   if (!mod || !mod->isSimple()) return TCL_ERROR;
   cSimpleModule *simplemod = (cSimpleModule *)mod;
   Tcl_SetResult(interp, ptrToStr( &(simplemod->members) ), TCL_VOLATILE);
   return TCL_OK;
}

int objectNetworks_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &networks ), TCL_VOLATILE);
   return TCL_OK;
}

int objectModuleTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &modtypes ), TCL_VOLATILE);
   return TCL_OK;
}

int objectChannelTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &linktypes ), TCL_VOLATILE);
   return TCL_OK;
}

int objectFunctions_cmd(ClientData, Tcl_Interp *interp, int argc, const char **)
{
   if (argc!=1) {Tcl_SetResult(interp, "wrong argcount", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr( &functions ), TCL_VOLATILE);
   return TCL_OK;
}
