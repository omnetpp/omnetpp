//==========================================================================
//  TKAPP.CC -
//            for the Tcl/Tk environment of
//                            OMNeT++
//
//  contains:  OmnetTkApp member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

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
#include "speedmtr.h"

#include "tkdefs.h"
#include "tkapp.h"
#include "tklib.h"
#include "inspector.h"
#include "inspfactory.h"

#include "modinsp.h"


//
// Register the Tkenv user interface
//
Register_OmnetApp("Tkenv",TOmnetTkApp,false,20,"Tkenv (Tk-based graphical user interface)");

// some functions that can/should be called from Envir in order to force the
// linker to include the Tkenv library into the executable:
TKENV_API void tkenvDummy() {}
TKENV_API void envirDummy() {}


// widgets in the Tk user interface
#define NETWORK_LABEL         ".statusbar.networklabel"
#define EVENT_LABEL           ".statusbar.eventlabel"
#define TIME_LABEL            ".statusbar.timelabel"
#define NEXT_LABEL            ".statusbar.nextlabel"

#define FESLENGTH_LABEL       ".statusbar2.feslength"
#define TOTALMSGS_LABEL       ".statusbar2.totalmsgs"
#define LIVEMSGS_LABEL        ".statusbar2.livemsgs"

#define SIMSECPERSEC_LABEL    ".statusbar3.simsecpersec"
#define EVENTSPERSEC_LABEL    ".statusbar3.eventspersec"
#define EVENTSPERSIMSEC_LABEL ".statusbar3.eventspersimsec"


#define SPEEDOMETER_UPDATESECS 1.0


// utility function
static bool moduleContains(cModule *potentialparent, cModule *mod)
{
   while (mod)
   {
       if (mod==potentialparent)
           return true;
       mod = mod->parentModule();
   }
   return false;
}


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
  inspectors("inspectors")
{
    interp = 0;  // Tcl/Tk not set up yet
    simstate = SIM_NONET;

    // The opt_* vars will be set by readOptions()
}

TOmnetTkApp::~TOmnetTkApp()
{
}

void TOmnetTkApp::setup()
{
    // initialize base class
    TOmnetApp::setup();  // includes readOptions()
    if (!initialized)
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
        initialized = false; // signal failed setup
        return;
    }

    // add OMNeT++'s commands to Tcl
    createTkCommands( interp, tcl_commands );

    Tcl_SetVar(interp, "OMNETPP_BITMAP_PATH", const_cast<char*>((const char *)bitmap_dir), TCL_GLOBAL_ONLY);

    // eval Tcl sources: either from .tcl files or from compiler-in string
    // literal (tclcode.cc)...

#ifdef OMNETPP_TKENV_DIR
    //
    // Case A: TCL code in separate .tcl files
    //
    Tcl_SetVar(interp, "OMNETPP_TKENV_DIR",  const_cast<char*>((const char *)tkenv_dir), TCL_GLOBAL_ONLY);
    if (Tcl_EvalFile(interp,fastconcat(tkenv_dir,"/tkenv.tcl"))==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s. "
                        "Is the OMNETPP_TKENV_DIR environment variable set correctly? "
                        "When not set, it defaults to " OMNETPP_TKENV_DIR ".\n",
                        interp->result);
        interp = 0;
        initialized = false; // signal failed setup
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
        initialized = false; // signal failed setup
        return;
    }
#endif

    // evaluate main script and build user interface
    if (Tcl_Eval(interp,"start_tkenv")==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s\n", interp->result);
        interp = 0;
        initialized = false; // signal failed setup
        return;
    }

}

int TOmnetTkApp::run()
{
    if (!initialized)
        return 1;

    CHK(Tcl_Eval(interp,"startup_commands"));
    runTk( interp );

    return 0;
}

void TOmnetTkApp::shutdown()
{
    // close all inspectors before exiting
    for(;;)
    {
        cIterator i(inspectors);
        if (i.end())
            break;
        TInspector *insp = (TInspector *) i();
        delete insp;
    }

    // delete network if not yet done
    simulation.deleteNetwork();

    TOmnetApp::shutdown();
}

void TOmnetTkApp::rebuildSim()
{
    if (simulation.runNumber()>0)
         newRun( simulation.runNumber() );
    else if (simulation.networkType()!=NULL)
         newNetwork( simulation.networkType()->name() );
    else
         CHK(Tcl_VarEval(interp,"messagebox {Confirm} {",
                                "Choose File|New Network or File|New Run.",
                                "} info ok",NULL));
    if (simulation.systemModule())
         inspect( simulation.systemModule(),INSP_DEFAULT,"",NULL);
}

void TOmnetTkApp::doOneStep()
{
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    clearNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();

    bkpt_hit = false;
    animation_ok = true;

    simstate = SIM_RUNNING;
    startClock();
    try
    {
        cSimpleModule *mod = simulation.selectNextModule();
        ASSERT(mod!=NULL);

        if (opt_print_banners)
           printEventBanner(mod);

        simulation.doOneEvent( mod );

        updateSimtimeDisplay();
        updateNextModuleDisplay();
        updateInspectors();

        simstate = SIM_READY;
    }
    catch (cTerminationException *e)
    {
        simstate = SIM_TERMINATED;
        displayMessage(e);
        delete e;
    }
    catch (cException *e)
    {
        simstate = SIM_ERROR;
        displayError(e);
        delete e;
    }
    stopClock();

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        finishSimulation();
    }
}

void TOmnetTkApp::runSimulation( simtime_t until_time, long until_event,
                                 bool slowexec, bool fastexec,
                                 cSimpleModule *stepwithinmodule)
{
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    bkpt_hit = false;
    stop_simulation = false;
    animation_ok = !fastexec && !stepwithinmodule;

    clearNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();
    Tcl_Eval(interp, "update");

    simstate = SIM_RUNNING;
    startClock();
    Speedometer speedometer;

    try
    {
        bool firstevent = true;
        while(1)
        {
            // query which module will execute the next event
            cSimpleModule *mod = simulation.selectNextModule();
            ASSERT(mod!=NULL);

            // if stepping locally in module, we stop both immediately
            // *before* and *after* executing the event in that module,
            // but we always execute at least one event
            bool stepwithinmodule_reached = stepwithinmodule && moduleContains(stepwithinmodule,mod);
            if (stepwithinmodule_reached)
            {
                if (!firstevent) break;
                animation_ok = true;
            }
            firstevent = false;

            // do a simulation step
            if (opt_print_banners)
                printEventBanner(mod);

            simulation.doOneEvent( mod );

            speedometer.addEvent(simulation.simTime());

            // exit conditions
            if (stepwithinmodule_reached) break;
            if (bkpt_hit || stop_simulation) break;
            if (until_time>0 && simulation.simTime()>=until_time) break;
            if (until_event>0 && simulation.eventNumber()>=until_event) break;

            // display update
            bool frequent_updates = !fastexec && !stepwithinmodule;
            if (frequent_updates || simulation.eventNumber()%opt_updatefreq_fast==0)
            {
                updateSimtimeDisplay();
                if (speedometer.secondsInThisInterval() > SPEEDOMETER_UPDATESECS)
                {
                    speedometer.beginNewInterval();
                    updatePerformanceDisplay(speedometer);
                }
                if (!stepwithinmodule) updateInspectors();
                Tcl_Eval(interp, "update");
            }

            // delay loop for slow simulation
            if (slowexec)
            {
                clock_t start = clock();
                double dclk = opt_stepdelay / 1000.0 * CLOCKS_PER_SEC;
                while (clock()-start<dclk && !stop_simulation)
                    Tcl_Eval(interp, "update");
            }

            checkTimeLimits();
        }
        simstate = SIM_READY;
    }
    catch (cTerminationException *e)
    {
        simstate = SIM_TERMINATED;
        displayMessage(e);
        delete e;
    }
    catch (cException *e)
    {
        simstate = SIM_ERROR;
        displayError(e);
        delete e;
    }
    stopClock();

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        finishSimulation();
    }

    updateNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

void TOmnetTkApp::runSimulationExpress(simtime_t until_time,long until_event)
{
    // implements 'express run'
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    ev.disable_tracing = true;
    bkpt_hit = false;
    stop_simulation = false;
    animation_ok = false;

    clearNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();
    Tcl_Eval(interp, "update");

    simstate = SIM_RUNNING;
    startClock();
    Speedometer speedometer;

    try
    {
        do
        {
            cSimpleModule *mod = simulation.selectNextModule();
            ASSERT(mod!=NULL);

            simulation.doOneEvent( mod );

            speedometer.addEvent(simulation.simTime());

            if (simulation.eventNumber()%opt_updatefreq_express==0)
            {
                updateSimtimeDisplay();
                if (speedometer.secondsInThisInterval() > SPEEDOMETER_UPDATESECS)
                {
                    speedometer.beginNewInterval();
                    updatePerformanceDisplay(speedometer);
                }
                Tcl_Eval(interp, "update");
            }
            checkTimeLimits();
        }
        while(  !bkpt_hit && !stop_simulation &&
                (until_time<=0 || simulation.simTime()<until_time) &&
                (until_event<=0 || simulation.eventNumber()<until_event)
             );
        simstate = SIM_READY;
    }
    catch (cTerminationException *e)
    {
        simstate = SIM_TERMINATED;
        displayMessage(e);
        delete e;
    }
    catch (cException *e)
    {
        simstate = SIM_ERROR;
        displayError(e);
        delete e;
    }
    stopClock();
    ev.disable_tracing = false;

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        finishSimulation();
    }

    updateSimtimeDisplay();
    updateNextModuleDisplay();
    clearPerformanceDisplay();
    updateInspectors();
}

void TOmnetTkApp::startAll()
{
    CHK(Tcl_VarEval(interp,"messagebox {Confirm} {Not implemented} info ok",NULL));
}

void TOmnetTkApp::finishSimulation()
{
    // hmm... after SIM_ERROR, we shouldn't allow callFinish() in theory..
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY || simstate==SIM_TERMINATED || simstate==SIM_ERROR);

    // print banner into main window
    if (opt_use_mainwindow)
         CHK(Tcl_VarEval(interp,
              ".main.text insert end {** Calling finish() methods of modules\n} event\n"
              ".main.text see end", NULL));

    // should print banner into per module windows too!
    // TO BE IMPLEMENTED

    // now really call finish()
    try
    {
        simulation.callFinish();
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
    }

    // then endrun
    try
    {
        simulation.endRun();
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
    }
    simstate = SIM_FINISHCALLED;

    updateSimtimeDisplay();
    updateNextModuleDisplay();
    updateInspectors();
}

void TOmnetTkApp::newNetwork(const char *network_name)
{
    cNetworkType *network = findNetwork( network_name );
    if (!network)
    {
        CHK(Tcl_VarEval(interp,"messagebox {Confirm} {Network '", network_name, "' not found.} info ok",NULL));
        return;
    }

    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (simstate!=SIM_NONET)
        {
            simulation.endRun();
            simulation.deleteNetwork();
            simstate = SIM_NONET;
        }

        CHK(Tcl_VarEval(interp, "clear_windows", NULL));

        // set up new network
        readPerRunOptions(0);
        makeOptionsEffective();
        opt_network_name = network->name();

        // pass run number 0 to setupNetwork(): this means that only the [All runs]
        // section of the ini file will be searched for parameter values
        simulation.setupNetwork(network, 0);
        startRun();

        simstate = SIM_NEW;
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
        simstate = SIM_ERROR;
    }

    // update GUI
    updateNetworkRunDisplay();
    updateNextModuleDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

void TOmnetTkApp::newRun(int run)
{
    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (simstate!=SIM_NONET)
        {
            simulation.endRun();
            simulation.deleteNetwork();
            simstate = SIM_NONET;
        }
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
        simstate = SIM_ERROR;
        return;
    }

    // set up new network
    run_nr = run;
    readPerRunOptions( run_nr );
    makeOptionsEffective();

    cNetworkType *network = findNetwork( opt_network_name );
    if (!network)
    {
        CHK(Tcl_VarEval(interp,"messagebox {Confirm} {Network '", (const char *)opt_network_name, "' not found.} info ok",NULL));
        return;
    }

    CHK(Tcl_VarEval(interp, "clear_windows", NULL));

    try
    {
        simulation.setupNetwork(network, run_nr);
        startRun();
        simstate = SIM_NEW;
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
        simstate = SIM_ERROR;
    }

    // update GUI
    updateNetworkRunDisplay();
    updateNextModuleDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

bool TOmnetTkApp::isBreakpointActive(const char *, cSimpleModule *)
{
    // args: label, module
    if (!opt_bkpts_enabled)
         return false;

    // To be implemented! Should be able to stop depending on all/selected labels, all/selected modules, etc.
    return true;
}

void TOmnetTkApp::stopAtBreakpoint(const char *label, cSimpleModule *mod)
{
    updateSimtimeDisplay();

    // pop up a dialog
    char buf[256];
    sprintf(buf, (mod->usesActivity() ?
                    "Breakpoint \"%s\" hit in module %s (id=%d)." :
                    "Breakpoint \"%s\" hit in module %s (id=%d). "
                    "Break will occur after completing current event, "
                    "ie. after module's handleMessage() returns."),
            label, mod->fullPath(), mod->id() );
    CHK(Tcl_VarEval(interp,"messagebox {Confirm} {",buf,"} info ok",NULL));

    // bkpt_hit will cause event loop to exit in runSimulation...()
    bkpt_hit = true;

    if (mod->usesActivity())
    {
        mod->pause();
    }
}

TInspector *TOmnetTkApp::inspect(cObject *obj, int type, const char *geometry, void *dat)
{
    // create inspector object & window or display existing one
    TInspector *existing_insp = findInspector(obj, type);
    if (existing_insp && existing_insp->windowExists())
    {
        existing_insp->showWindow();
        return existing_insp;
    }

    if (existing_insp)
    {
        // there is an inspector, but without window: error!
        fprintf(stderr, "deleting inspector without window\n");
        delete existing_insp;
    }

    // create inspector
    cInspectorFactory *p = findInspectorFactoryFor(obj,type);
    if (!p)
    {
        CHK(Tcl_VarEval(interp,"messagebox {Confirm}"
                        " {Class `",obj->className(),"' has no associated inspectors.} info ok",NULL));
        return NULL;
    }

    TInspector *insp = p->createInspectorFor(obj,p->inspectorType(),geometry,dat);
    if (!insp)
    {
        // message: object has no such inspector
        CHK(Tcl_VarEval(interp,"messagebox {Confirm}"
                               " {Class `",obj->className(),"' has no `",
                               insptype_name_from_code(type),
                               "' inspector.} info ok",NULL));
        return NULL;
    }

    // An inspector was created. Now, its actual type can be different
    // from the 'type' parameter we passed. For example,
    // INSP_DEFAULT becomes INSP_OBJECT, INSP_CONTAINER etc.
    // So we have to check again if an inspector with the
    // actual parameters (type & object) already exists.
    cObject *actual_obj = insp->getObject();
    int actual_type = insp->getType();
    if (actual_obj!=obj || actual_type!=type)
    {
        existing_insp = findInspector(actual_obj, actual_type);
        if (existing_insp)
        {
            delete insp;
            existing_insp->showWindow();
            return existing_insp;
        }
    }

    // everything ok, finish inspector
    insp->setOwner(&inspectors); // insert into inspector list
    insp->createWindow();
    insp->update();

    return insp;
}

TInspector *TOmnetTkApp::findInspector(cObject *obj, int type)
{
    for (cIterator i(inspectors); !i.end(); i++)
    {
        TInspector *insp = (TInspector *) i();
        if (insp->getObject()==obj && insp->getType()==type)
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

    // try opening "pending" inspectors
    CHK(Tcl_VarEval(interp, "inspectorupdate_callback",NULL));
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

    CHK(Tcl_VarEval(interp, NETWORK_LABEL " config -text {",
                        "Run #",runnr,": ",networkname,
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, "wm title . {OMNeT++/Tkenv - ",networkname,"}",NULL));
}

void TOmnetTkApp::updateSimtimeDisplay()
{
    // event and time display
    char buf[16];
    sprintf(buf, "%lu", simulation.eventNumber());
    CHK(Tcl_VarEval(interp, EVENT_LABEL " config -text {"
                        "Event #", buf,
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, TIME_LABEL " config -text {"
                        "T=", simtimeToStr( simulation.simTime() ),
                        "}", NULL ));

    // statistics
    sprintf(buf, "%u", simulation.msgQueue.length());
    CHK(Tcl_VarEval(interp, FESLENGTH_LABEL " config -text {"
                        "Msgs scheduled: ", buf,
                        "}", NULL ));
    sprintf(buf, "%lu", cMessage::totalMessageCount());
    CHK(Tcl_VarEval(interp, TOTALMSGS_LABEL " config -text {"
                        "Msgs created: ", buf,
                        "}", NULL ));
    sprintf(buf, "%lu", cMessage::liveMessageCount());
    CHK(Tcl_VarEval(interp, LIVEMSGS_LABEL " config -text {"
                        "Msgs present: ", buf,
                        "}", NULL ));

}

void TOmnetTkApp::updateNextModuleDisplay()
{
    cSimpleModule *mod = NULL;

    if (simstate==SIM_NEW || simstate==SIM_READY || simstate==SIM_RUNNING)
    {
        try
        {
            mod = simulation.selectNextModule();
        }
        catch (cException *e)
        {
            // simply ignore the exception -- selectNextModule should be idempotent,
            // so we'll display it another time.
            delete e;
        }
    }

    char id[16];
    const char *modname;
    if (mod)
    {
        modname = mod->fullPath();
        sprintf(id," (id=%u)", mod->id());
    }
    else
    {
        modname = "n/a";
        id[0]=0;
    }
    CHK(Tcl_VarEval(interp, NEXT_LABEL " config -text {Next: ",modname,id,"}",NULL));
}

void TOmnetTkApp::clearNextModuleDisplay()
{
    CHK(Tcl_VarEval(interp, NEXT_LABEL " config -text {"
                        "Running..."
                        "}", NULL ));
}

void TOmnetTkApp::updatePerformanceDisplay(Speedometer& speedometer)
{
    char buf[16];
    sprintf(buf, "%g", speedometer.simSecPerSec());
    CHK(Tcl_VarEval(interp, SIMSECPERSEC_LABEL " config -text {"
                        "Simsec/sec: ", buf,
                        "}", NULL ));
    sprintf(buf, "%g", speedometer.eventsPerSec());
    CHK(Tcl_VarEval(interp, EVENTSPERSEC_LABEL " config -text {"
                        "Ev/sec: ", buf,
                        "}", NULL ));
    sprintf(buf, "%g", speedometer.eventsPerSimSec());
    CHK(Tcl_VarEval(interp, EVENTSPERSIMSEC_LABEL " config -text {"
                        "Ev/simsec: ", buf,
                        "}", NULL ));
}

void TOmnetTkApp::clearPerformanceDisplay()
{
    CHK(Tcl_VarEval(interp, SIMSECPERSEC_LABEL " config -text {"
                        "Simsec/sec: n/a"
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, EVENTSPERSEC_LABEL " config -text {"
                        "Ev/sec: n/a"
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, EVENTSPERSIMSEC_LABEL " config -text {"
                        "Ev/simsec: n/a"
                        "}", NULL ));
}

void TOmnetTkApp::printEventBanner(cSimpleModule *module)
{
    char banner[256];
    if (module->phase()[0]==0)
    {
        sprintf(banner,"** Event #%ld.  T=%s.  Module #%u `%s'\n",
                simulation.eventNumber(),
                simtimeToStr( simulation.simTime() ),
                module->id(),
                module->fullPath()
              );
    }
    else
    {
        sprintf(banner,"** Event #%ld.  T=%s.  Module `%s' (id=%u). Phase `%s'.\n",
                simulation.eventNumber(),
                simtimeToStr( simulation.simTime() ),
                module->fullPath(),
                module->id(),
                module->phase()
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

    // print banner into module window and all parent module windows if they exist
    cModule *mod = module;
    while (mod)
    {
        TModuleWindow *insp = (TModuleWindow *)findInspector(mod,INSP_MODULEOUTPUT);
        if (insp)
        {
           CHK(Tcl_VarEval(interp,
               insp->windowName(),".main.text insert end {",banner,"} event\n",
               insp->windowName(),".main.text see end",
               NULL));
        }
        mod = mod->parentModule();
    }
}

//=========================================================================
void TOmnetTkApp::readOptions()
{
    TOmnetApp::readOptions();

    opt_extrastack = ini_file->getAsInt("Tkenv", "extra-stack", TKENV_EXTRASTACK);
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
        // beware because inspectors are also cObjects...
        if (insp!=object && insp->getObject()==object)
        {
            insp->hostObjectDeleted();
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
    // display in message window
    static char buf[1000]; // module names might be very long
    msg->info( buf );
    CHK(Tcl_VarEval(interp, "catch {\n"
                            " .messagewindow.main.text insert end"
                            "    {SENT:\t ",buf,"\n}\n"
                            " .messagewindow.main.text see end\n"
                            "}\n",
                            NULL));

    if (animation_ok && opt_animation_enabled)
    {
        // find suitable inspectors and do animate the message...
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
                                        insp->windowName(), " ",
                                        ptrToStr(g)," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind," ",
                                        (lastgate?"beg":""),
                                        NULL));
            }
            g = g->toGate();
        }
    }
}

void TOmnetTkApp::messageDelivered( cMessage *msg )
{
    // display in message window
    static char buf[1000]; // module names might be very long
    msg->info( buf );
    CHK(Tcl_VarEval(interp, "catch {\n"
                            " .messagewindow.main.text insert end"
                            "    {DELIVD:\t ",buf,"\n}\n"
                            " .messagewindow.main.text see end\n"
                            "}\n",
                            NULL));

    if (animation_ok && opt_animation_enabled)
    {
        // find suitable inspectors and do animate the message...
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
                                        insp->windowName(), " ",
                                        ptrToStr(g)," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind,
                                        " end",
                                        NULL));
            }
        }
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

    // we have to quote the string for Tcl in case it contains { or }.
    // minimize heap usage: use local buffer whenever possible.
    int flags;
    char buf[128];
    int quotedlen = Tcl_ScanElement(const_cast<char *>(str), &flags);
    bool isshort = quotedlen<128;
    char *quotedstr = isshort ? buf : Tcl_Alloc(quotedlen+1);
    Tcl_ConvertElement(const_cast<char *>(str), quotedstr, flags);

    // output string into main window
    cModule *module = simulation.contextModule();
    if (!module || opt_use_mainwindow)
    {
        CHK(Tcl_VarEval(interp,
            ".main.text insert end ",quotedstr,"\n"
            ".main.text see end", NULL));
    }

    // print into module window and all parent compound module windows if they exist
    cModule *mod = module;
    while (mod)
    {
        TInspector *insp = findInspector(mod,INSP_MODULEOUTPUT);
        if (insp)
        {
            CHK(Tcl_VarEval(interp,
              insp->windowName(),".main.text insert end ",quotedstr,"\n",
              insp->windowName(),".main.text see end", NULL));
        }
        mod = mod->parentModule();
    }

    // dealloc buffer if necessary
    if (!isshort)
        Tcl_Free(quotedstr);
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

    // ok
    const char *result = Tcl_GetVar2(interp, "opp", "result", TCL_GLOBAL_ONLY);
    strncpy(buf, result, len-1);
    buf[len-1]='\0';
    return false;
}

int TOmnetTkApp::askYesNo(const char *question)
{
    // should return -1 when CANCEL is pressed
    CHK(Tcl_VarEval(interp,"messagebox {Warning} {",question,"}"
                           " question yesno", NULL));
    return interp->result[0]=='y';
}

unsigned TOmnetTkApp::extraStackForEnvir()
{
     return opt_extrastack;
}

//======================================================================
// dummy function to force Unix linkers collect all symbols needed

void _dummy_for_objinsp();
void _dummy_for_modinsp();
void _dummy_for_statinsp();
void _dummy_for_structinsp();

void _dummy_func() {
  _dummy_for_objinsp();
  _dummy_for_modinsp();
  _dummy_for_statinsp();
  _dummy_for_structinsp();
}


