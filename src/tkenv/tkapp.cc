//==========================================================================
//  TKAPP.CC -
//            for the Tcl/Tk environment of
//                            OMNeT++
//
//  contains:  OmnetTkApp member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "appreg.h"
#include "cinifile.h"
#include "cenvir.h"
#include "cmodule.h"
#include "cmessage.h"
#include "cstk.h"
#include "ctypes.h"
#include "cnetmod.h"

#include "tkdefs.h"
#include "tkapp.h"
#include "tklib.h"
#include "tkinsp.h"


//
// Register the Tkenv user interface
//
Register_OmnetApp("Tkenv",TOmnetTkApp,false,20,"Tkenv (Tk-based graphical user interface)");

// some functions that can/should be called from Envir in order to force the
// linker to include the Tkenv library into the executable:
void tkenvDummy() {}
void envirDummy() {}


//=========================================================================
// TOmnetTkApp
//        is the application class that runs the Tcl/Tk environment
//        of the OMNeT++ simulator. An instance of this class is created
//        by the envir object (typed cEnvir) which provides an interface
//        between the simulator and its run environment.
//        Class declaration and member function definitions for TOmnetTkApp
//        follow here.
//=========================================================================

TOmnetTkApp::TOmnetTkApp(ArgList *args, cIniFile *inifile) :
  TOmnetApp(args, inifile),
  inspectors("inspectors", NULL)
{
    interp = 0;  // Tcl/Tk not set up yet
    is_running = false;

    // The opt_* vars will be set by readOptions()
}

TOmnetTkApp::~TOmnetTkApp()
{
}

void TOmnetTkApp::setup()
{
    // initialize base class
    TOmnetApp::setup();  // includes readOptions()
    if (!simulation.ok())
        return;

    // path for the Tcl user interface files
#ifdef OMNETPP_TKENV_DIR
    tkenv_dir = getenv("OMNETPP_TKENV_DIR");
    if (!tkenv_dir)
        tkenv_dir = OMNETPP_TKENV_DIR;
#endif

    // path for icon directory
    bitmap_dir = getenv("OMNETPP_BITMAP_PATH");
    if (!bitmap_dir)
        bitmap_dir = OMNETPP_BITMAP_PATH;

    // set up Tcl/Tk
    interp = initTk( args->argCount(), args->argVector() );
    if (!interp)
    {
        interp = 0;
        simulation.setErrorCode(eUISTARTUP);
        return;
    }

    // add OMNeT++'s commands to Tcl
    createTkCommands( interp, tcl_commands );

    Tcl_SetVar(interp, "OMNETPP_BITMAP_PATH", CONST_CAST((const char *)bitmap_dir), TCL_GLOBAL_ONLY);

    // eval Tcl sources: either from .tcl files or from compiler-in string
    // literal (tclcode.cc)...

#ifdef OMNETPP_TKENV_DIR
    //
    // Case A: TCL code in separate .tcl files
    //
    Tcl_SetVar(interp, "OMNETPP_TKENV_DIR",  CONST_CAST((const char *)tkenv_dir), TCL_GLOBAL_ONLY);
    if (Tcl_EvalFile(interp,fastconcat(tkenv_dir,"/tkenv.tcl"))==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s. "
                        "Is the OMNETPP_TKENV_DIR environment variable set correctly? "
                        "When not set, it defaults to " OMNETPP_TKENV_DIR ".\n",
                        interp->result);
        interp = 0;
        simulation.setErrorCode(eUISTARTUP);
        return;
    }
#else
    //
    // Case B: compiled-in TCL code
    //
    // The tclcode.cc file must be generated from the TCL scripts
    // with the tcl2c program (to be compiled from tcl2c.c).
    // The Unix makefile automatically does so; on Win95/NT, you have
    // to do it by hand: bcc tcl2c.c; tcl2c tclcode.cc *.tcl
    //
#   include "tclcode.cc"
    if (Tcl_Eval(interp,(char *)tcl_code)==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s\n", interp->result);
        interp = 0;
        simulation.setErrorCode(eUISTARTUP);
        return;
    }
#endif

    // evaluate main script and build user interface
    if (Tcl_Eval(interp,"start_tkenv")==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s\n", interp->result);
        interp = 0;
        simulation.setErrorCode(eUISTARTUP);
        return;
    }

}

void TOmnetTkApp::run()
{
    CHK(Tcl_Eval(interp,"startup_commands"));
    runTk( interp );
}

void TOmnetTkApp::rebuildSim()
{
    // finish previous run if we haven't done so yet
    if (simulation.ok()) simulation.endRun();

    if (simulation.runNumber()>0)
         newRun( simulation.runNumber() );
    else if (simulation.networkType()!=NULL)
         newNetwork( simulation.networkType()->name() );
    else
         CHK(Tcl_VarEval(interp,"messagebox {Confirm} {",
                                "Choose File|New Network or File|New Run.",
                                "} info ok",NULL));
    if (simulation.systemModule())
         inspect( simulation.systemModule(),INSP_DEFAULT,NULL );

    // Old code: try to really restart simulation
    //           (cannot be done decently because object members of
    //            simple modules cannot be restarted)
    //
    //makeOptionsEffective();
    //simulation.startRun();

    //updateSimtimeDisplay();
    //updateNextModuleDisplay();
    //updateInspectors();
}

void TOmnetTkApp::doOneStep()
{
    clearNextModuleDisplay();
    updateSimtimeDisplay();

    bkpt_hit = false;
    animation_ok = true;

    is_running = true;
    simulation.startClock();
    cSimpleModule *mod = simulation.selectNextModule();
    if (mod != NULL)
    {
       if (opt_print_banners)
          printEventBanner(mod);

       simulation.doOneEvent( mod );
       simulation.incEventNumber();

       updateSimtimeDisplay();
       updateNextModuleDisplay();
       updateInspectors();
    }
    simulation.stopClock();
    is_running = false;

    if (!simulation.ok())
    {
       if (simulation.normalTermination())
          callFinish();  // includes endRun()
       else
          simulation.endRun();
    }
}

void TOmnetTkApp::runSimulation( simtime_t until_time, long until_event,
                                 bool slowexec, bool fast,
                                 cSimpleModule *stopinmod )
{
    bkpt_hit = false;
    stop_simulation = false;
    animation_ok = !fast;

    clearNextModuleDisplay();
    updateSimtimeDisplay();
    Tcl_Eval(interp, "update");

    is_running = true;
    simulation.startClock();
    while(1)
    {
        // query which module will execute the next event
        cSimpleModule *mod = simulation.selectNextModule();
        if (mod==NULL) break;

        // do a simulation step
        if (opt_print_banners)  printEventBanner(mod);
        simulation.doOneEvent( mod );
        simulation.incEventNumber();

        // exit conditions:
        if (stopinmod && mod==stopinmod) break;
        if (!simulation.ok() || bkpt_hit || stop_simulation) break;
        if (until_time>0 && simulation.simTime()>=until_time) break;
        if (until_event>0 && simulation.eventNumber()>=until_event) break;

        // if we single-step inside a module, we want rare display updates
        if (stopinmod)
        {
            if (simulation.eventNumber()%opt_updatefreq_fast==0)
            {
                updateSimtimeDisplay();
                updateInspectors();
                Tcl_Eval(interp, "update");
            }
        }
        else
        {
            if (!fast || simulation.eventNumber()%opt_updatefreq_fast==0)
            {
                updateSimtimeDisplay();
                if (!stopinmod) updateInspectors();
                Tcl_Eval(interp, "update");
            }

            if (slowexec)
            {
                clock_t start = clock();
                double dclk = opt_stepdelay / 1000.0 * CLOCKS_PER_SEC;
                while (clock()-start<dclk && !stop_simulation)
                    Tcl_Eval(interp, "update");
            }
        }
    }
    simulation.stopClock();
    is_running = false;

    if (!simulation.ok())
    {
       if (simulation.normalTermination())
          callFinish();  // includes endRun()
       else
          simulation.endRun();
    }

    updateNextModuleDisplay();
    if (fast || stopinmod) updateSimtimeDisplay();
    if (fast || stopinmod) updateInspectors();
}

void TOmnetTkApp::runSimulationNoTracing(simtime_t until_time,long until_event)
{
    // implements 'express run'

    ev.disable_tracing = true;
    bkpt_hit = false;
    stop_simulation = false;
    animation_ok = false;

    clearNextModuleDisplay();
    updateSimtimeDisplay();
    Tcl_Eval(interp, "update");

    is_running = true;
    simulation.startClock();
    do {
         cSimpleModule *mod = simulation.selectNextModule();
         if (mod==NULL) break;
         simulation.doOneEvent( mod );
         simulation.incEventNumber();

         if (simulation.eventNumber()%opt_updatefreq_express==0)
         {
            updateSimtimeDisplay();
            Tcl_Eval(interp, "update");
         }

    } while(  simulation.ok() && !bkpt_hit && !stop_simulation &&
              (until_time<=0 || simulation.simTime()<until_time) &&
              (until_event<=0 || simulation.eventNumber()<until_event)
           );
    simulation.stopClock();
    is_running = false;

    if (!simulation.ok())
    {
       if (simulation.normalTermination())
          callFinish();  // includes endRun()
       else
          simulation.endRun();
    }

    ev.disable_tracing = false;

    updateSimtimeDisplay();
    updateNextModuleDisplay();
    updateInspectors();
}

void TOmnetTkApp::startAll()
{
    opp_error("Not implemented");
}

void TOmnetTkApp::callFinish()
{
    // print banner into main window
    if (opt_use_mainwindow)
         CHK(Tcl_VarEval(interp,
              ".main.text insert end {** Calling finish() for modules\n} event\n"
              ".main.text see end", NULL));

    // should print banner into per module windows too!
    // TO BE IMPLEMENTED

    // now really call finish()
    simulation.callFinish();
    simulation.endRun();
    opp_terminate(eFINISH);

    updateSimtimeDisplay();
    updateNextModuleDisplay();
    updateInspectors();
}

void TOmnetTkApp::newNetwork( const char *network_name )
{
    cNetworkType *network = findNetwork( network_name );

    if( network )
    {
       // finish previous run if we haven't done so yet
       if (simulation.ok()) simulation.endRun();
       simulation.deleteNetwork();

       // the rest of the code will set up and start this run

       // run number will be 0 which means that only the [All runs]
       // section of the ini file will be searched for parameter values
       readPerRunOptions( 0 );
       opt_network_name = network->name();

       simulation.setupNetwork( network, 0 );
       if (!simulation.ok())
           simulation.deleteNetwork();
       else
       {
           makeOptionsEffective();
           if (simulation.ok())
           {
               simulation.startRun();
               if (!simulation.ok())
                   simulation.deleteNetwork();
           }
       }
       updateNetworkRunDisplay();
       updateNextModuleDisplay();
       updateSimtimeDisplay();
       updateInspectors();
    }
}

void TOmnetTkApp::newRun( int run )
{
    run_nr = run;

    // finish previous run if we haven't done so yet
    if (simulation.ok()) simulation.endRun();
    simulation.deleteNetwork();

    readPerRunOptions( run_nr );

    cNetworkType *network = findNetwork( opt_network_name );
    if (network==NULL)
    {
         opp_error("Network `%s' not found",(const char *)opt_network_name);
         return;
    }

    simulation.setupNetwork( network, run_nr );
    if (!simulation.ok())
        simulation.deleteNetwork();
    else
    {
        makeOptionsEffective();
        if (simulation.ok())
        {
           simulation.startRun();
           if (!simulation.ok())
             simulation.deleteNetwork();
        }
    }
    updateNetworkRunDisplay();
    updateNextModuleDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

bool TOmnetTkApp::isBreakpointActive(const char *, cSimpleModule *)
{
    // args: label, module

    if (opt_bkpts_enabled)
    {
         return true;    // To be implemented!
                         // Should be able to stop depending on
                         // all/selected labels, all/selected modules.
    }
    else
         return false;
}

void TOmnetTkApp::stopAtBreakpoint(const char *label, cSimpleModule *mod)
{
    updateSimtimeDisplay();

    char buf[256];
    sprintf(buf, "Breakpoint \"%s\" hit in module %s (#%d)!",
                 label, mod->fullPath(), mod->id() );
    CHK(Tcl_VarEval(interp,"messagebox {Confirm} {",buf,"} info ok",NULL));
    bkpt_hit = true;

    mod->pause();
}

TInspector *TOmnetTkApp::inspect(cObject *obj, int type, void *dat)
{
    // create inspector object & window or display existing one
    TInspector *existing_insp = findInspector( obj, type );
    if (existing_insp && existing_insp->windowExists())
    {
        existing_insp->showWindow();
        return existing_insp;
    }

    if (existing_insp)  // there is an inspector, but without window: error!
    {
           fprintf(stderr, "deleting inspector without window\n");
           delete existing_insp;
    }

    // create inspector
    TInspector *insp = obj->inspector(type,dat);
    if (!insp)
    {
            // message: object has no such inspector
            CHK(Tcl_VarEval(interp,"messagebox {Confirm}"
                                   " {Class `",obj->className(),"' has no `",
                                   insptype_name_from_code(type),
                                   "' inspector.} info ok",NULL));
            return (TInspector *)0;
    }

    // An inspector was created. Now, its actual type can be different
    // from the 'type' parameter we passed. For example,
    // INSP_NATIVE becomes INSP_OBJECT, INSP_CONTAINER etc.
    // Moreover, its 'object' pointer can be different, too; for example,
    // with INSP_LOCALVARS or INSP_CLASSMEMBERS.
    // So we have to check again if an inspector with the
    // actual parameters (type & object) already exists.
    cObject *actual_obj = insp->object;
    int actual_type = insp->type;
    if (actual_obj!=obj || actual_type!=type)
    {
        existing_insp = findInspector( actual_obj, actual_type );
        if (existing_insp)
        {
             delete insp;
             existing_insp->showWindow();
             return existing_insp;
        }
    }

    // everything ok, finish inspector
    insp->setOwner( &inspectors ); // insert into inspector list
    insp->createWindow();
    insp->update();

    return insp;
}

TInspector *TOmnetTkApp::findInspector(cObject *obj, int type)
{
    for (cIterator i(inspectors); !i.end(); i++)
    {
         TInspector *insp = (TInspector *) i();
         if (insp->object==obj && insp->type==type)
             return insp;
    }
    return NULL;
}

void TOmnetTkApp::updateInspectors()
{
    // update inspectors
    for (cIterator i(inspectors); !i.end(); i++)
    {
         TInspector *insp = (TInspector *) i();
         insp->update();
    }
}

void TOmnetTkApp::createSnapshot( const char *label )
{
    simulation.snapshot(&simulation, label );
}

void TOmnetTkApp::updateNetworkRunDisplay()
{
    char runnr[10];
    const char *networkname;

    if (simulation.runNumber()<=0)
         sprintf(runnr, "?");
    else
         sprintf(runnr, "%d",simulation.runNumber());

    if (simulation.networkType()==NULL)
         networkname = "(no network)";
    else
         networkname = simulation.networkType()->name();

    CHK(Tcl_VarEval(interp, ".statusbar.networklabel config -text {",
                        "Run #",runnr,": ",networkname,
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, "wm title . {OMNeT++/Tkenv - ",networkname,"}",NULL));
}

void TOmnetTkApp::updateSimtimeDisplay()
{
    char buf[16];
    sprintf(buf, "%lu", simulation.eventNumber());
    CHK(Tcl_VarEval(interp, ".statusbar.eventlabel config -text {"
                        "Event #", buf,
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, ".statusbar.timelabel config -text {"
                        "T=", simtimeToStr( simulation.simTime() ),
                        "}", NULL ));
}

void TOmnetTkApp::updateNextModuleDisplay()
{
    char subsc[16];
    const char *modulename;
    cSimpleModule *mod;

    if (!simulation.ok())
    {
      modulename = "n/a";
      subsc[0]=0;
    }
    else if ((mod=simulation.selectNextModule())==NULL)
    {
      modulename = "n/a";
      subsc[0]=0;
    } else {
      modulename = mod->fullPath();
      sprintf(subsc,"#%lu ",mod->id());
    }
    CHK(Tcl_VarEval(interp, ".statusbar.nextlabel config -text {"
                        "Next: ", subsc, modulename,
                        "}", NULL ));
}

void TOmnetTkApp::clearNextModuleDisplay()
{
    CHK(Tcl_VarEval(interp, ".statusbar.nextlabel config -text {"
                        "Running..."
                        "}", NULL ));
}

void TOmnetTkApp::printEventBanner(cSimpleModule *mod)
{
    char banner[256];
    if (mod->phase()[0]==0)
    {
        sprintf(banner,"** Event #%ld.  T=%s.  Module #%d `%s'\n",
                simulation.eventNumber(),
                simtimeToStr( simulation.simTime() ),
                mod->id(),
                mod->fullPath()
              );
    }
    else
    {
        sprintf(banner,"** Event #%ld.  T=%s.  Module #%d `%s' phase `%s'.\n",
                simulation.eventNumber(),
                simtimeToStr( simulation.simTime() ),
                mod->id(),
                mod->fullPath(),
                mod->phase()
              );
    }

    // insert into main window
    if (opt_use_mainwindow)
         CHK(Tcl_VarEval(interp,
              ".main.text insert end {",banner,"} event\n"
              ".main.text see end", NULL));

    // and into the message window
    CHK(Tcl_VarEval(interp,
            "catch {\n"
            " .messagewindow.main.text insert end {",banner,"} event\n"
            " .messagewindow.main.text see end\n"
            "}\n", NULL));

    // print banner into module window if one exists
    TModuleWindow *insp = (TModuleWindow *)findInspector(mod,INSP_MODULEOUTPUT);
    if (insp)
    {
       if (mod->phase()[0]==0)
           sprintf(banner,"** Event #%ld.  T=%s\n",
                          simulation.eventNumber(),
                          simtimeToStr(simulation.simTime()));
       else
           sprintf(banner,"** Event #%ld.  T=%s.  Phase `%s'\n",
                          simulation.eventNumber(),
                          simtimeToStr(simulation.simTime()),
                          mod->phase());
       // insert into window
       CHK(Tcl_VarEval(interp,
           insp->windowname,".main.text insert end {",banner,"} event\n",
           insp->windowname,".main.text see end",
           NULL));
    }
}

//=========================================================================
void TOmnetTkApp::readOptions()
{
    TOmnetApp::readOptions();

    opt_default_run = ini_file->getAsInt( "Tkenv", "default-run", 0);
    opt_stepdelay = long(1000*ini_file->getAsTime( "Tkenv", "slowexec-delay", 0.3 )+.5);
    opt_updatefreq_fast = ini_file->getAsInt( "Tkenv", "update-freq-fast", 10 );
    opt_updatefreq_express = ini_file->getAsInt( "Tkenv", "update-freq-express", 500 );
    opt_bkpts_enabled = ini_file->getAsBool( "Tkenv", "breakpoints-enabled", true );
    opt_animation_enabled = ini_file->getAsBool( "Tkenv", "animation-enabled", true );
    opt_animation_msgnames = ini_file->getAsBool( "Tkenv", "animation-msgnames", true );
    opt_animation_msgcolors = ini_file->getAsBool( "Tkenv", "animation-msgcolors", true );
    opt_animation_speed = ini_file->getAsDouble( "Tkenv", "animation-speed", 1);
    if (opt_animation_speed<0) opt_animation_speed=0;
    if (opt_animation_speed>2) opt_animation_speed=3;
    opt_print_banners = ini_file->getAsBool( "Tkenv", "print-banners", true );
    opt_use_mainwindow = ini_file->getAsBool( "Tkenv", "use-mainwindow", true );
}

void TOmnetTkApp::readPerRunOptions(int run_nr)
{
    TOmnetApp::readPerRunOptions( run_nr );
}

void TOmnetTkApp::objectDeleted( cObject *object )
{
    cIterator i(inspectors);
    while (!i.end())
    {
         TInspector *insp = (TInspector *) i();
         if (insp->object == object)
         {
              delete insp;
              // i is no longer valid, must restart it...
              i.init(inspectors);
         }
         else
         {
              i++;
         }
    }
}

void TOmnetTkApp::messageSent( cMessage *msg )
{
    char buf[256];
    msg->info( buf );
    CHK(Tcl_VarEval(interp, "catch {\n"
                            " .messagewindow.main.text insert end"
                            "    {SENT\t ",buf,"\n}\n"
                            " .messagewindow.main.text see end\n"
                            "}\n",
                            NULL));

    if (animation_ok && opt_animation_enabled)
    {
        // find suitable inspectors and do animate the message...
        bool w = simulation.warnings(); simulation.setWarnings( false );
        char msgptr[32], msgkind[16];
        ptrToStr(msg,msgptr);
        sprintf(msgkind,"%d",msg->kind());

        cGate *g = msg->senderGate();
        cGate *arrivalgate = msg->arrivalGate();

        while (g && g->toGate())
        {
            cModule *mod = g->ownerModule();
            if (g->type()=='O') mod = mod->parentModule();

            TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
            if (insp)
            {
                int lastgate = (g->toGate()==arrivalgate);
                CHK(Tcl_VarEval(interp, "graphmodwin_animate ",
                                        insp->windowname, " ",
                                        ptrToStr(g)," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind," ",
                                        (lastgate?"beg":""),
                                        NULL));
            }
            g = g->toGate();
        }
        simulation.setWarnings( w );
    }
}

void TOmnetTkApp::messageDelivered( cMessage *msg )
{
    char buf[256];
    msg->info( buf );
    CHK(Tcl_VarEval(interp, "catch {\n"
                            " .messagewindow.main.text insert end"
                            "    {DELIVD\t ",buf,"\n}\n"
                            " .messagewindow.main.text see end\n"
                            "}\n",
                            NULL));

    if (animation_ok && opt_animation_enabled)
    {
        // find suitable inspectors and do animate the message...
        bool w = simulation.warnings(); simulation.setWarnings( false );
        char msgptr[32], msgkind[16];
        ptrToStr(msg,msgptr);
        sprintf(msgkind,"%d",msg->kind());

        cGate *g = msg->arrivalGate();
        if (g) g = g->fromGate();
        if (g)
        {
            cModule *mod = g->ownerModule();
            if (g->type()=='O') mod = mod->parentModule();

            TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
            if (insp)
            {
                CHK(Tcl_VarEval(interp, "graphmodwin_animate ",
                                        insp->windowname, " ",
                                        ptrToStr(g)," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind,
                                        " end",
                                        NULL));
            }
        }
        simulation.setWarnings( w );
    }
}

void TOmnetTkApp::breakpointHit( const char *label, cSimpleModule *mod )
{
    if (isBreakpointActive(label,mod))
        stopAtBreakpoint(label,mod);
}

void TOmnetTkApp::putmsg(const char *str)
{
    if (!interp)
    {
        TOmnetApp::putmsg(str); // fallback in case Tkenv didn't fire up correctly
        return;
    }
    CHK(Tcl_VarEval(interp,"messagebox {Confirm} {",str,"} info ok",NULL));
}

void TOmnetTkApp::puts(const char *str)
{
    if (!interp)
    {
        TOmnetApp::puts(str); // fallback in case Tkenv didn't fire up correctly
        return;
    }

    cModule *module = simulation.contextModule();
    if (module)
    {
        // print into module window if one exists
        TInspector *insp = findInspector(module,INSP_MODULEOUTPUT);
        if (insp)
        {
            CHK(Tcl_VarEval(interp,
              insp->windowname,".main.text insert end {",str,"}\n",
              insp->windowname,".main.text see end", NULL));
        }
    }

    if (!module || opt_use_mainwindow)
    {
        CHK(Tcl_VarEval(interp,
            ".main.text insert end {",str,"}\n"
            ".main.text see end", NULL));
    }
}

bool TOmnetTkApp::gets(const char *promptstr, char *buf, int len)
{
    char title[70];
    cModule *mod = simulation.contextModule();
    if (mod)
       strncpy(title, mod->fullPath(),69);
    else
       strncpy(title, simulation.networkType()->name(),69);
    title[69]=0;

    CHK(Tcl_Eval(interp,"global opp"));
    Tcl_SetVar2(interp,"opp", "result", buf, TCL_GLOBAL_ONLY);
    CHK(Tcl_VarEval(interp,"inputbox {",title,"} {",promptstr,"} opp(result)",NULL));

    if (interp->result[0]=='0')   // cancel
        return true;
    else    // ok
    {
        char *result = Tcl_GetVar2(interp, "opp", "result", TCL_GLOBAL_ONLY);
        strncpy(buf, result, len-1);
        buf[len-1]='\0';
        return false;
    }
}

int TOmnetTkApp::askYesNo(const char *question)
{
    // should return -1 when CANCEL is pressed
    CHK(Tcl_VarEval(interp,"messagebox {Warning} {",question,"}"
                           " question yesno", NULL));
    return interp->result[0]=='y';
}

//======================================================================
// dummy function to force Unix linkers collect all symbols needed

void _dummy_for_inspectors();
static void _dummy_func()
{
      _dummy_for_inspectors();
}
