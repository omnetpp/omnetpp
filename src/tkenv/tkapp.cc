//==========================================================================
//  TKAPP.CC -
//            for the Tcl/Tk environment of
//                            OMNeT++
//
//  contains:  OmnetTkApp member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "appreg.h"
#include "cinifile.h"
#include "cenvir.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "ctypes.h"
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
Register_OmnetApp("Tkenv",TOmnetTkApp,20,"Tkenv (Tk-based graphical user interface)");

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
  TOmnetApp(args, inifile)
{
    interp = 0;  // Tcl/Tk not set up yet
    simstate = SIM_NONET;
    stop_simulation = false;

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
                        Tcl_GetStringResult(interp));
        interp = 0;
        initialized = false; // signal failed setup
        return;
    }
#else
    //
    // Case B: compiled-in TCL code
    //
    // The tclcode.cc file is generated from the Tcl scripts
    // with the tcl2c program (to be compiled from tcl2c.c).
    //
#   include "tclcode.cc"
    if (Tcl_Eval(interp,(char *)tcl_code)==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s\n", Tcl_GetStringResult(interp));
        interp = 0;
        initialized = false; // signal failed setup
        return;
    }
#endif

    // evaluate main script and build user interface
    if (Tcl_Eval(interp,"start_tkenv")==TCL_ERROR)
    {
        fprintf(stderr, "\n<!> Error starting Tkenv: %s\n", Tcl_GetStringResult(interp));
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
        TInspectorList::iterator it = inspectors.begin();
        if (it==inspectors.end())
            break;
        TInspector *insp = *it;
        inspectors.erase(it);
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
    animating = true;

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
        stoppedWithTerminationException(e);
        displayMessage(e);
        delete e;
    }
    catch (cException *e)
    {
        simstate = SIM_ERROR;
        stoppedWithException(e);
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
    animating = !fastexec;

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
                animating = true;
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
            bool frequent_updates = !fastexec;
            if (frequent_updates || simulation.eventNumber()%opt_updatefreq_fast==0)
            {
                updateSimtimeDisplay();
                if (speedometer.secondsInThisInterval() > SPEEDOMETER_UPDATESECS)
                {
                    speedometer.beginNewInterval();
                    updatePerformanceDisplay(speedometer);
                }
                updateInspectors();
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
        stoppedWithTerminationException(e);
        displayMessage(e);
        delete e;
    }
    catch (cException *e)
    {
        simstate = SIM_ERROR;
        stoppedWithException(e);
        displayError(e);
        delete e;
    }
    stopClock();
    stop_simulation = false;

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        finishSimulation();
    }

    animating = true;

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
    animating = false;

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
        stoppedWithTerminationException(e);
        displayMessage(e);
        delete e;
    }
    catch (cException *e)
    {
        simstate = SIM_ERROR;
        stoppedWithException(e);
        displayError(e);
        delete e;
    }
    stopClock();
    stop_simulation = false;
    ev.disable_tracing = false;

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        finishSimulation();
    }

    animating = true;

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

void TOmnetTkApp::loadNedFile(const char *fname)
{
    try
    {
        simulation.loadNedFile(fname);
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
    }
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
    static char buf[MAX_OBJECTFULLPATH+100];
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
    if (existing_insp)
    {
        existing_insp->showWindow();
        return existing_insp;
    }

    // create inspector
    cInspectorFactory *p = findInspectorFactoryFor(obj,type);
    if (!p)
    {
        CHK(Tcl_VarEval(interp,"messagebox {Confirm}"
                        " {Class `",obj->className(),"' has no associated inspectors.} info ok",NULL));
        return NULL;
    }

    int actualtype = p->inspectorType();
    existing_insp = findInspector(obj, actualtype);
    if (existing_insp)
    {
        existing_insp->showWindow();
        return existing_insp;
    }

    TInspector *insp = p->createInspectorFor(obj, actualtype, geometry, dat);
    if (!insp)
    {
        // message: object has no such inspector
        CHK(Tcl_VarEval(interp,"messagebox {Confirm}"
                               " {Class `",obj->className(),"' has no `",
                               insptypeNameFromCode(type),
                               "' inspector.} info ok",NULL));
        return NULL;
    }

    // everything ok, finish inspector
    inspectors.insert(inspectors.end(), insp);
    insp->createWindow();
    insp->update();

    return insp;
}

TInspector *TOmnetTkApp::findInspector(cObject *obj, int type)
{
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); ++it)
    {
        TInspector *insp = *it;
        if (insp->getObject()==obj && insp->getType()==type)
            return insp;
    }
    return NULL;
}

void TOmnetTkApp::deleteInspector(TInspector *insp)
{
    inspectors.remove(insp);
    delete insp;
}

void TOmnetTkApp::updateInspectors()
{
    // update object tree
    CHK(Tcl_VarEval(interp, "updateTreeManager",NULL));

    // update inspectors
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); ++it)
    {
        TInspector *insp = *it;
        insp->update();
    }

    // try opening "pending" inspectors
    CHK(Tcl_VarEval(interp, "inspectorupdate_callback",NULL));
}

void TOmnetTkApp::createSnapshot( const char *label )
{
    simulation.snapshot(&simulation, label );
}

void TOmnetTkApp::updateGraphicalInspectorsBeforeAnimation()
{
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); ++it)
    {
        TInspector *insp = *it;
        if (dynamic_cast<TGraphicalModWindow *>(insp) && static_cast<TGraphicalModWindow *>(insp)->needsRedraw())
        {
            insp->update();
        }
    }
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
        mod = simulation.guessNextModule();

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
    char banner[MAX_OBJECTFULLPATH+60];
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
        TModuleWindow *insp = static_cast<TModuleWindow *>(findInspector(mod,INSP_MODULEOUTPUT));
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

    // Note: most entries below should be obsoleted (with .tkenvrc taking over)
    opt_stepdelay = long(1000*ini_file->getAsTime( "Tkenv", "slowexec-delay", 0.3 )+.5);
    opt_updatefreq_fast = ini_file->getAsInt( "Tkenv", "update-freq-fast", 50);
    opt_updatefreq_express = ini_file->getAsInt( "Tkenv", "update-freq-express", 10000 );
    opt_bkpts_enabled = ini_file->getAsBool( "Tkenv", "breakpoints-enabled", true );
    opt_animation_enabled = ini_file->getAsBool( "Tkenv", "animation-enabled", true );
    opt_nexteventmarkers = ini_file->getAsBool( "Tkenv", "next-event-markers", true );
    opt_senddirect_arrows = ini_file->getAsBool( "Tkenv", "senddirect-arrows", true );
    opt_anim_methodcalls = ini_file->getAsBool( "Tkenv", "anim-methodcalls", true );
    opt_methodcalls_delay = long(1000*ini_file->getAsTime( "Tkenv", "methodcalls-delay", 0.2)+.5);
    opt_animation_msgnames = ini_file->getAsBool( "Tkenv", "animation-msgnames", true );
    opt_animation_msgcolors = ini_file->getAsBool( "Tkenv", "animation-msgcolors", true );
    opt_penguin_mode = ini_file->getAsBool( "Tkenv", "penguin-mode", false );
    opt_showlayouting = ini_file->getAsBool( "Tkenv", "show-layouting", false);
    opt_bubbles = ini_file->getAsBool( "Tkenv", "show-bubbles", true );
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

bool TOmnetTkApp::idle()
{
    eState origsimstate = simstate;
    simstate = SIM_BUSY;
    Tcl_Eval(interp, "update");
    simstate = origsimstate;

    bool stop = stop_simulation;
    stop_simulation = false;
    return stop;
}

void TOmnetTkApp::objectDeleted(cObject *object)
{
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); )
    {
        TInspectorList::iterator next = it;
        ++next;
        TInspector *insp = *it;
        if (insp->getObject()==object)
        {
            inspectors.erase(it); // with std::list, "next" remains valid
            insp->hostObjectDeleted();
            delete insp;
        }
        else
        {
            // notify the inspector, maybe it's interested in learning that
            insp->objectDeleted(object);
        }
        it = next;
    }
}

void TOmnetTkApp::messageSent( cMessage *msg, cGate *directToGate)
{
    // display in message window
    static char buf[MAX_OBJECTINFO];
    msg->info(buf);
    CHK(Tcl_VarEval(interp, "catch {\n"
                            " .messagewindow.main.text insert end"
                            "    {SENT:\t (",msg->className(),")",msg->fullName(),"  ",buf,"\n}\n"
                            " .messagewindow.main.text see end\n"
                            "}\n",
                            NULL));

    if (animating && opt_animation_enabled)
    {
        // find suitable inspectors and do animate the message...
        updateGraphicalInspectorsBeforeAnimation(); // actually this will draw `msg' too (which would cause "phantom message"),
                                                    // but we'll manually remove it before animation
        if (!directToGate)
        {
            // message was sent via a gate (send())
            animateSend(msg, msg->senderGate(), msg->arrivalGate());
        }
        else
        {
            // sendDirect() was used
            animateSendDirect(msg, simulation.module(msg->senderModuleId()), directToGate);
            animateSend(msg, directToGate, msg->arrivalGate());
        }
    }
}

void TOmnetTkApp::messageDelivered( cMessage *msg )
{
    // display in message window
    static char buf[MAX_OBJECTINFO];
    msg->info(buf);
    CHK(Tcl_VarEval(interp, "catch {\n"
                            " .messagewindow.main.text insert end"
                            "    {DELIVD:\t (",msg->className(),")",msg->fullName(),"  ",buf,"\n}\n"
                            " .messagewindow.main.text see end\n"
                            "}\n",
                            NULL));

    if (animating && opt_animation_enabled)
    {
        cGate *arrivalGate = msg->arrivalGate();
        if (!arrivalGate)
            return;

        // if arrivalgate is connected, msg arrived on a connection, otherwise via sendDirect()
        updateGraphicalInspectorsBeforeAnimation();
        if (arrivalGate->fromGate())
        {
            animateDelivery(msg);
        }
        else
        {
            animateDeliveryDirect(msg);
        }
    }
}

void TOmnetTkApp::moduleMethodCalled(cModule *from, cModule *to, const char *method)
{
    if (!animating || !opt_anim_methodcalls)
        return;

    updateGraphicalInspectorsBeforeAnimation();

    // find modules along the way
    PathVec pathvec;
    findDirectPath(from, to, pathvec);

    PathVec::iterator i;
    int numinsp = 0;
    for (i=pathvec.begin(); i!=pathvec.end(); i++)
    {
        if (i->to==NULL)
        {
            // ascent
            cModule *mod = i->from;
            cModule *enclosingmod = mod->parentModule();
            //ev << "DBG: animate ascent inside " << enclosingmod->fullPath()
            //   << " from " << mod->fullPath() << endl;
            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                numinsp++;
                char parentptr[30], modptr[30];
                strcpy(parentptr,ptrToStr(enclosingmod));
                strcpy(modptr,ptrToStr(mod));
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_methodcall_ascent ",
                                        insp->windowName(), " ",
                                        parentptr," ",
                                        modptr," ",
                                        " {",method,"} ",
                                        NULL));
            }
        }
        else if (i->from==NULL)
        {
            // animate descent towards destmod
            cModule *mod = i->to;
            cModule *enclosingmod = mod->parentModule();
            //ev << "DBG: animate descent in " << enclosingmod->fullPath() <<
            //   " to " << mod->fullPath() << endl;

            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                numinsp++;
                char parentptr[30], modptr[30];
                strcpy(parentptr,ptrToStr(enclosingmod));
                strcpy(modptr,ptrToStr(mod));
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_methodcall_descent ",
                                        insp->windowName(), " ",
                                        parentptr," ",
                                        modptr," ",
                                        " {",method,"} ",
                                        NULL));
            }
        }
        else
        {
            cModule *enclosingmod = i->from->parentModule();
            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                numinsp++;
                char fromptr[30], toptr[30];
                strcpy(fromptr,ptrToStr(i->from));
                strcpy(toptr,ptrToStr(i->to));
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_methodcall_horiz ",
                                        insp->windowName(), " ",
                                        fromptr," ",
                                        toptr," ",
                                        " {",method,"} ",
                                        NULL));
            }
        }
    }

    if (numinsp>0)
    {
        // leave it there for a while
        CHK(Tcl_Eval(interp, "graphmodwin_animate_methodcall_wait"));

        // then remove all arrows
        for (i=pathvec.begin(); i!=pathvec.end(); i++)
        {
            cModule *mod= i->from ? i->from : i->to;
            cModule *enclosingmod = mod->parentModule();
            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_methodcall_cleanup ",
                                        insp->windowName(),
                                        NULL));
            }
        }
    }
}

void TOmnetTkApp::moduleCreated(cModule *newmodule)
{
    cModule *mod = newmodule->parentModule();
    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->submoduleCreated(newmodule);
}

void TOmnetTkApp::moduleDeleted(cModule *module)
{
    cModule *mod = module->parentModule();
    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->submoduleDeleted(module);
}

void TOmnetTkApp::connectionCreated(cGate *srcgate)
{
    // notify compound module where the connection (whose source is this gate) is displayed
    cModule *notifymodule = NULL;
    if (srcgate->type()=='O')
        notifymodule = srcgate->ownerModule()->parentModule();
    else
        notifymodule = srcgate->ownerModule();
    TInspector *insp = findInspector(notifymodule,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->connectionCreated(srcgate);
}

void TOmnetTkApp::connectionRemoved(cGate *srcgate)
{
    // notify compound module where the connection (whose source is this gate) is displayed
    // note: almost the same code as above
    cModule *notifymodule;
    if (srcgate->type()=='O')
        notifymodule = srcgate->ownerModule()->parentModule();
    else
        notifymodule = srcgate->ownerModule();
    TInspector *insp = findInspector(notifymodule,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->connectionRemoved(srcgate);
}

void TOmnetTkApp::displayStringChanged(cGate *gate)
{
    // if gate is not connected, nothing is displayed, so nothing to do
    if (!gate->toGate())  return;

    // notify module inspector which displays connection
    cModule *notifymodule;
    if (gate->type()=='O')
        notifymodule = gate->ownerModule()->parentModule();
    else
        notifymodule = gate->ownerModule();

    TInspector *insp = findInspector(notifymodule,INSP_GRAPHICAL);
    if (insp)
    {
        TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
        assert(modinsp);
        modinsp->displayStringChanged(gate);
    }

    // graphical gate inspector windows: normally a user doesn't have many such windows open
    // (typically, none at all), so we can afford simply refreshing all of them
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); ++it)
    {
        TInspector *insp = *it;
        TGraphicalGateWindow *gateinsp = dynamic_cast<TGraphicalGateWindow *>(insp);
        if (gateinsp)
            gateinsp->displayStringChanged(gate);
    }
}

void TOmnetTkApp::displayStringChanged(cModule *submodule)
{
    cModule *mod = submodule->parentModule();
    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->displayStringChanged(submodule);
}

void TOmnetTkApp::displayStringAsParentChanged(cModule *parentmodule)
{
    TInspector *insp = findInspector(parentmodule,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->displayStringAsParentChanged();
}

void TOmnetTkApp::animateSend(cMessage *msg, cGate *fromgate, cGate *togate)
{
    char msgptr[32], msgkind[16];
    ptrToStr(msg,msgptr);
    sprintf(msgkind,"%d",msg->kind());

    cGate *g = fromgate;
    cGate *arrivalgate = togate;

    while (g && g->toGate())
    {
        cModule *mod = g->ownerModule();
        if (g->type()=='O') mod = mod->parentModule();

        TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
        if (insp)
        {
            int lastgate = (g->toGate()==arrivalgate);
            CHK(Tcl_VarEval(interp, "graphmodwin_animate_on_conn ",
                                    insp->windowName(), " ",
                                    ptrToStr(g)," ",
                                    msgptr,
                                    " {",msg->fullName(),"} ",
                                    msgkind," ",
                                    (lastgate?"beg":"thru"),
                                    NULL));
        }
        g = g->toGate();
    }
}

// helper for animateSendDirect() functions
static cModule *findSubmoduleTowards(cModule *parentmod, cModule *towardsgrandchild)
{
    if (parentmod==towardsgrandchild)
       return NULL; // shortcut -- we don't have to go up to the top to see we missed

    // search upwards from 'towardsgrandchild'
    cModule *m = towardsgrandchild;
    while (m && m->parentModule()!=parentmod)
       m = m->parentModule();
    return m;
}


void TOmnetTkApp::findDirectPath(cModule *srcmod, cModule *destmod, PathVec& pathvec)
{
    // for animation purposes, we assume that the message travels up
    // in the module hierarchy until it finds the first compound module
    // that also contains the destination module (possibly somewhere deep),
    // and then it descends to the destination module. We have to find the
    // list of modules visited during the travel.

    // first, find "lowest common ancestor" module
    cModule *commonparent = srcmod;
    while (commonparent)
    {
        // try to find commonparent among ancestors of destmod
        cModule *m = destmod;
        while (m && commonparent!=m)
            m = m->parentModule();
        if (commonparent==m)
            break;
        commonparent = commonparent->parentModule();
    }

    // commonparent should exist, worst case it's the system module,
    // but let's have the following "if" anyway...
    if (!commonparent)
        return;

    // animate the ascent of the message until commonparent (excluding).
    // The second condition, destmod==commonparent covers case when we're sending
    // to an output gate of the parent (grandparent, etc) gate.
    cModule *mod = srcmod;
    while (mod!=commonparent && (mod->parentModule()!=commonparent || destmod==commonparent))
    {
        pathvec.push_back(sPathEntry(mod,NULL));
        mod = mod->parentModule();
    }

    // animate within commonparent
    if (commonparent!=srcmod && commonparent!=destmod)
    {
        cModule *from = findSubmoduleTowards(commonparent, srcmod);
        cModule *to = findSubmoduleTowards(commonparent, destmod);
        pathvec.push_back(sPathEntry(from,to));
    }

    // descend from commonparent to destmod
    mod = findSubmoduleTowards(commonparent, destmod);
    if (mod && srcmod!=commonparent)
        mod = findSubmoduleTowards(mod, destmod);
    while (mod)
    {
        // animate descent towards destmod
        pathvec.push_back(sPathEntry(NULL,mod));
        // find module 'under' mod, towards destmod (this will return NULL if mod==destmod)
        mod = findSubmoduleTowards(mod, destmod);
    }
}

void TOmnetTkApp::animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate)
{
    char msgptr[32], msgkind[16];
    ptrToStr(msg,msgptr);
    sprintf(msgkind,"%d",msg->kind());

    PathVec pathvec;
    findDirectPath(frommodule, togate->ownerModule(), pathvec);

    cModule *arrivalmod = msg->arrivalGate()->ownerModule();

    PathVec::iterator i;
    for (i=pathvec.begin(); i!=pathvec.end(); i++)
    {
        if (i->to==NULL)
        {
            // ascent
            cModule *mod = i->from;
            cModule *enclosingmod = mod->parentModule();
            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                char parentptr[30], modptr[30];
                strcpy(parentptr,ptrToStr(enclosingmod));
                strcpy(modptr,ptrToStr(mod));
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_senddirect_ascent ",
                                        insp->windowName(), " ",
                                        parentptr," ",
                                        modptr," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind," ",
                                        "thru", // cannot be "beg" (msg ball cannot stay on encl.module rect)
                                        NULL));
            }
        }
        else if (i->from==NULL)
        {
            // animate descent towards destmod
            cModule *mod = i->to;
            cModule *enclosingmod = mod->parentModule();
            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                char parentptr[30], modptr[30];
                strcpy(parentptr,ptrToStr(enclosingmod));
                strcpy(modptr,ptrToStr(mod));
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_senddirect_descent ",
                                        insp->windowName(), " ",
                                        parentptr," ",
                                        modptr," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind," ",
                                        (mod==arrivalmod?"beg":"thru"),
                                        NULL));
            }
        }
        else
        {
            cModule *enclosingmod = i->from->parentModule();
            TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
            if (insp)
            {
                char fromptr[30], toptr[30];
                strcpy(fromptr,ptrToStr(i->from));
                strcpy(toptr,ptrToStr(i->to));
                CHK(Tcl_VarEval(interp, "graphmodwin_animate_senddirect_horiz ",
                                        insp->windowName(), " ",
                                        fromptr," ",
                                        toptr," ",
                                        msgptr,
                                        " {",msg->fullName(),"} ",
                                        msgkind," ",
                                        (i->to==arrivalmod?"beg":"thru"),
                                        NULL));
            }
        }
    }

    // then remove all arrows
    for (i=pathvec.begin(); i!=pathvec.end(); i++)
    {
        cModule *mod= i->from ? i->from : i->to;
        cModule *enclosingmod = mod->parentModule();
        TInspector *insp = findInspector(enclosingmod,INSP_GRAPHICAL);
        if (insp)
        {
            CHK(Tcl_VarEval(interp, "graphmodwin_animate_senddirect_cleanup ",
                                    insp->windowName(),
                                    NULL));
        }
    }
}


void TOmnetTkApp::animateDelivery(cMessage *msg)
{
    char msgptr[32], msgkind[16];
    ptrToStr(msg,msgptr);
    sprintf(msgkind,"%d",msg->kind());

    // find suitable inspectors and do animate the message...
    cGate *g = msg->arrivalGate();
    ASSERT(g);
    g = g->fromGate();
    ASSERT(g);

    cModule *mod = g->ownerModule();
    if (g->type()=='O') mod = mod->parentModule();

    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (insp)
    {
        CHK(Tcl_VarEval(interp, "graphmodwin_animate_on_conn ",
                                insp->windowName(), " ",
                                ptrToStr(g)," ",
                                msgptr,
                                " {",msg->fullName(),"} ",
                                msgkind,
                                " end",
                                NULL));
    }
}

void TOmnetTkApp::animateDeliveryDirect(cMessage *msg)
{
    char msgptr[32], msgkind[16];
    ptrToStr(msg,msgptr);
    sprintf(msgkind,"%d",msg->kind());

    // find suitable inspectors and do animate the message...
    cGate *g = msg->arrivalGate();
    ASSERT(g);
    cModule *destmod = g->ownerModule();
    cModule *mod = destmod->parentModule();

    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (insp)
    {
        CHK(Tcl_VarEval(interp, "graphmodwin_animate_senddirect_delivery ",
                                insp->windowName(), " ",
                                ptrToStr(destmod)," ",
                                msgptr,
                                " {",msg->fullName(),"} ",
                                msgkind,
                                NULL));
    }
}


void TOmnetTkApp::breakpointHit( const char *label, cSimpleModule *mod )
{
    if (isBreakpointActive(label,mod))
        stopAtBreakpoint(label,mod);
}

void TOmnetTkApp::bubble(cModule *mod, const char *text)
{
    if (!opt_bubbles) return;
    cModule *parentmod = mod->parentModule();
    TInspector *insp = findInspector(parentmod,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->bubble(mod, text);
}

void TOmnetTkApp::putmsg(const char *str)
{
    if (!interp)
    {
        ::printf("<!> %s\n", str); // fallback in case Tkenv didn't fire up correctly
        return;
    }
    CHK(Tcl_VarEval(interp,"messagebox {Confirm} {",str,"} info ok",NULL));
}

void TOmnetTkApp::puts(const char *str)
{
    if (!interp)
    {
        ::printf("%s", str); // fallback in case Tkenv didn't fire up correctly
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
    const char *tag = (!module) ? "log" : "";
    if (!module || opt_use_mainwindow)
    {
        CHK(Tcl_VarEval(interp,
            ".main.text insert end ",quotedstr," ", tag ,"\n"
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
              insp->windowName(),".main.text insert end ",quotedstr," ", tag, "\n",
              insp->windowName(),".main.text see end", NULL));
        }
        mod = mod->parentModule();
    }

    // dealloc buffer if necessary
    if (!isshort)
        Tcl_Free(quotedstr);
}

void TOmnetTkApp::flush()
{
    // Tk doesn't need flush(), it displays everything ASAP anyway
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

    if (Tcl_GetStringResult(interp)[0]=='0')   // cancel
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
    return Tcl_GetStringResult(interp)[0]=='y';
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


