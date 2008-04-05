//==========================================================================
//  TKENV.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  contains:  OmnetTkApp member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string>

#include "appreg.h"
#include "bootenv.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "speedometer.h"
#include "cscheduler.h"
#include "ccomponenttype.h"
#include "csimulation.h"
#include "cconfigkey.h"
#include "regmacros.h"

#include "tkdefs.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspector.h"
#include "inspfactory.h"
#include "modinsp.h"
#include "timeutil.h"
#include "stringutil.h"
#include "../utils/ver.h"


// default plugin path -- allow overriding it via compiler option (-D)
// (default image path comes from makefile)
#ifndef OMNETPP_PLUGIN_PATH
#define OMNETPP_PLUGIN_PATH "./plugins"
#endif

#ifdef __APPLE__
void OSXTransformProcess();
#endif


USING_NAMESPACE

//
// Register the Tkenv user interface
//
Register_OmnetApp("Tkenv", Tkenv, 20, "graphical user interface");

//
// The following function can be used to force linking with Tkenv; specify
// -u _tkenv_lib (gcc) or /include:_tkenv_lib (vc++) in the link command.
//
extern "C" TKENV_API void tkenv_lib() {}


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


#define SPEEDOMETER_UPDATEMILLISECS 1000


Register_GlobalConfigEntry(CFGID_TKENV_EXTRA_STACK_KB, "tkenv-extra-stack-kb", CFG_INT, "48", "Specifies the extra amount of stack (in kilobytes) that is reserved for each activity() simple module when the simulation is run under Tkenv.");
Register_GlobalConfigEntry(CFGID_DEFAULT_CONFIG, "tkenv-default-config", CFG_STRING, NULL, "Specifies which config Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigEntry(CFGID_DEFAULT_RUN, "tkenv-default-run", CFG_INT, "0", "Specifies which run (of the default config, see tkenv-default-config) Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigEntry(CFGID_IMAGE_PATH, "tkenv-image-path", CFG_FILENAME, "", "Specifies the path for loading module icons.");
Register_GlobalConfigEntry(CFGID_PLUGIN_PATH, "tkenv-plugin-path", CFG_FILENAME, "", "Specifies the search path for Tkenv plugins. Tkenv plugins are .tcl files that get evaluated on startup.");


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


Tkenv::Tkenv()
{
    interp = NULL;  // Tcl/Tk not set up yet
    simstate = SIM_NONET;
    stopsimulation_flag = false;
    animating = false;
    hasmessagewindow = false;
    isconfigrun = false;

    // set the name here, to prevent warning from cStringPool on shutdown when Cmdenv runs
    inspectorfactories.instance()->setName("inspectorfactories");

    // initialize .tkenvrc config variables
    opt_stepdelay = 300;
    opt_updatefreq_fast = 100;
    opt_updatefreq_express = 10000;
    opt_animation_enabled = true;
    opt_nexteventmarkers = true;
    opt_senddirect_arrows = true;
    opt_anim_methodcalls = true;
    opt_methodcalls_delay = 200;
    opt_animation_msgnames = true;
    opt_animation_msgclassnames = true;
    opt_animation_msgcolors = true;
    opt_penguin_mode = false;
    opt_showlayouting = false;
    opt_usenewlayouter = false;
    opt_bubbles = true;
    opt_animation_speed = 1.5;
    opt_print_banners = true;
    opt_use_mainwindow = true;
    opt_expressmode_autoupdate = true;
}

Tkenv::~Tkenv()
{
}

static void signalHandler(int signum)
{
   cStaticFlag::setExiting();
   exit(2);
}

void Tkenv::setup()
{
    // initialize base class
    EnvirBase::setup();  // includes readOptions()
    if (!initialized)
        return;

    try
    {
        // set signal handler
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

#ifdef __APPLE__
        OSXTransformProcess();
#endif
        // path for the Tcl user interface files
#ifdef OMNETPP_TKENV_DIR
        tkenv_dir = getenv("OMNETPP_TKENV_DIR");
        if (tkenv_dir.empty())
            tkenv_dir = OMNETPP_TKENV_DIR;
#endif

        // path for icon directories
        const char *image_path_env = getenv("OMNETPP_IMAGE_PATH");
        if (image_path_env==NULL && getenv("OMNETPP_BITMAP_PATH")!=NULL)
            fprintf(stderr, "\n<!> WARNING: Obsolete environment variable OMNETPP_BITMAP_PATH found -- "
                            "please change it to OMNETPP_IMAGE_PATH for OMNeT++ 4.0+\n");
        std::string image_path = image_path_env ? image_path_env : OMNETPP_IMAGE_PATH;
        if (!opt_image_path.empty())
            image_path = std::string(opt_image_path.c_str()) + ";" + image_path;

        // path for plugins
        const char *plugin_path_env = getenv("OMNETPP_PLUGIN_PATH");
        std::string plugin_path = plugin_path_env ? plugin_path_env : OMNETPP_PLUGIN_PATH;
        if (!opt_plugin_path.empty())
            plugin_path = std::string(opt_plugin_path.c_str()) + ";" + plugin_path;

        // set up Tcl/Tk
        interp = initTk(args->argCount(), args->argVector());
        if (!interp)
            throw opp_runtime_error("Tkenv: cannot create Tcl interpreter");

        // add OMNeT++'s commands to Tcl
        createTkCommands(interp, tcl_commands);

        Tcl_SetVar(interp, "OMNETPP_IMAGE_PATH", TCLCONST(image_path.c_str()), TCL_GLOBAL_ONLY);
        Tcl_SetVar(interp, "OMNETPP_PLUGIN_PATH", TCLCONST(plugin_path.c_str()), TCL_GLOBAL_ONLY);

        Tcl_SetVar(interp, "OMNETPP_RELEASE", OMNETPP_RELEASE, TCL_GLOBAL_ONLY);
        Tcl_SetVar(interp, "OMNETPP_EDITION", OMNETPP_EDITION, TCL_GLOBAL_ONLY);

        // eval Tcl sources: either from .tcl files or from compiled-in string
        // literal (tclcode.cc)...

#ifdef OMNETPP_TKENV_DIR
        //
        // Case A: TCL code in separate .tcl files
        //
        Tcl_SetVar(interp, "OMNETPP_TKENV_DIR",  TCLCONST(tkenv_dir.c_str()), TCL_GLOBAL_ONLY);
        if (Tcl_EvalFile(interp,opp_concat(tkenv_dir.c_str(),"/tkenv.tcl"))==TCL_ERROR)
            throw opp_runtime_error("Tkenv: %s. (Is the OMNETPP_TKENV_DIR environment variable "
                                    "set correctly? When not set, it defaults to " OMNETPP_TKENV_DIR ")",
                                    Tcl_GetStringResult(interp));
#else
        //
        // Case B: compiled-in TCL code
        //
        // The tclcode.cc file is generated from the Tcl scripts
        // with the tcl2c program (to be compiled from tcl2c.c).
        //
#include "tclcode.cc"
        if (Tcl_Eval(interp,(char *)tcl_code)==TCL_ERROR)
            throw opp_runtime_error("Tkenv: %s", Tcl_GetStringResult(interp));
#endif

        // evaluate main script and build user interface
        if (Tcl_Eval(interp,"start_tkenv")==TCL_ERROR)
            throw opp_runtime_error("Tkenv: %s\n", Tcl_GetStringResult(interp));

        // create windowtitle prefix
        if (getParsimNumPartitions()>0)
        {
            windowtitleprefix.reserve(24);
            sprintf(windowtitleprefix.buffer(), "Proc %d/%d - ", getParsimProcId(), getParsimNumPartitions());
        }
    }
    catch (std::exception& e)
    {
        interp = NULL;
        throw;
    }
}

int Tkenv::run()
{
    if (!initialized)
        return 1;

    CHK(Tcl_Eval(interp,"startup_commands"));
    runTk(interp);

    return 0;
}

void Tkenv::shutdown()
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

    // pull down inspector factories
    inspectorfactories.clear();

    // shut down base class
    EnvirBase::shutdown();
}

void Tkenv::printUISpecificHelp()
{
    ev << "Tkenv-specific options:\n";
    ev << "  -c <configname>\n";
    ev << "                Select a given configuration for execution. With inifile-based\n";
    ev << "                configuration database, this selects the [Config <configname>]\n";
    ev << "                section; the default is the [General] section.\n";
    ev << "                See also: -r.\n";
    ev << "  -r <run>      Set up the specified run number in the configuration selected with\n";
    ev << "                the -c option\n";
    ev << "\n";
}

void Tkenv::rebuildSim()
{
    if (isconfigrun)
         newRun(std::string(config()->getActiveConfigName()).c_str(), config()->getActiveRunNumber());
    else if (simulation.networkType()!=NULL)
         newNetwork(simulation.networkType()->name());
    else
         confirm("Choose File|New Network or File|New Run.");
    if (simulation.systemModule())
         inspect(simulation.systemModule(),INSP_DEFAULT,"",NULL);
}

void Tkenv::doOneStep()
{
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    clearNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();

    animating = true;

    simstate = SIM_RUNNING;
    startClock();
    scheduler->executionResumed();
    try
    {
        cSimpleModule *mod = simulation.selectNextModule();
        if (mod)  // selectNextModule() not interrupted
        {
            if (opt_print_banners)
               printEventBanner(mod);
            simulation.doOneEvent(mod);
            performAnimations();
        }
        updateSimtimeDisplay();
        updateNextModuleDisplay();
        updateInspectors();
        simstate = SIM_READY;
        outvectormgr->flush();
        outscalarmgr->flush();
        if (feventlog)
            fflush(feventlog);
    }
    catch (cTerminationException& e)
    {
        simstate = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        displayMessage(e);
    }
    catch (std::exception& e)
    {
        simstate = SIM_ERROR;
        stoppedWithException(e);
        displayError(e);
    }
    stopClock();
    stopsimulation_flag = false;

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        //
        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
        // called yet, because we want to allow the user to force finish() from
        // the GUI -- and finish() has to precede endRun(). endRun() will be called
        // just before a new network gets set up, or on Tkenv shutdown.
        //
        finishSimulation();
    }
}

void Tkenv::runSimulation(int mode, simtime_t until_time, long until_event, cModule *until_module)
{
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    runmode = mode;
    rununtil_time = until_time;
    rununtil_event = until_event;
    rununtil_module = until_module;  // Note: this is NOT supported with RUNMODE_EXPRESS

    stopsimulation_flag = false;

    clearNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();
    Tcl_Eval(interp, "update");

    simstate = SIM_RUNNING;
    startClock();
    scheduler->executionResumed();
    try
    {
        // funky while loop to handle switching to and from EXPRESS mode....
        bool cont = true;
        while (cont)
        {
            if (runmode==RUNMODE_EXPRESS)
                cont = doRunSimulationExpress();
            else
                cont = doRunSimulation();
        }
        simstate = SIM_READY;
        outvectormgr->flush();
        outscalarmgr->flush();
        if (feventlog)
            fflush(feventlog);
    }
    catch (cTerminationException& e)
    {
        simstate = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        displayMessage(e);
    }
    catch (std::exception& e)
    {
        simstate = SIM_ERROR;
        stoppedWithException(e);
        displayError(e);
    }
    stopClock();
    stopsimulation_flag = false;

    animating = true;
    ev.disable_tracing = false;

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        //
        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
        // called yet, because we want to allow the user to force finish() from
        // the GUI -- and finish() has to precede endRun(). endRun() will be called
        // just before a new network gets set up, or on Tkenv shutdown.
        //
        finishSimulation();
    }

    updateNextModuleDisplay();
    clearPerformanceDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

void Tkenv::setSimulationRunMode(int mode)
{
    // This function (and the next one too) is called while runSimulation() is
    // underway, from Tcl code that gets a chance to run via the
    // Tcl_Eval(interp, "update") commands
    runmode = mode;
}

void Tkenv::setSimulationRunUntil(simtime_t until_time, long until_event)
{
    rununtil_time = until_time;
    rununtil_event = until_event;
}

void Tkenv::setSimulationRunUntilModule(cModule *until_module)
{
    rununtil_module = until_module;
}

bool Tkenv::doRunSimulation()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_event, rununtil_module;
    //  - stopsimulation_flag
    //
    Speedometer speedometer;
    speedometer.start(simulation.simTime());
    ev.disable_tracing = false;
    bool firstevent = true;
    while(1)
    {
        if (runmode==RUNMODE_EXPRESS)
            return true;  // should continue, but in a different mode

        // query which module will execute the next event
        cSimpleModule *mod = simulation.selectNextModule();
        if (!mod) break; // selectNextModule() interrupted (parsim)

        // if stepping locally in module, we stop both immediately
        // *before* and *after* executing the event in that module,
        // but we always execute at least one event
        bool untilmodule_reached = rununtil_module && moduleContains(rununtil_module,mod);
        if (untilmodule_reached && !firstevent)
            break;
        firstevent = false;

        animating = (runmode==RUNMODE_NORMAL) || (runmode==RUNMODE_SLOW) || untilmodule_reached;
        bool frequent_updates = (runmode==RUNMODE_NORMAL) || (runmode==RUNMODE_SLOW);

        speedometer.addEvent(simulation.simTime());

        // do a simulation step
        if (opt_print_banners)
            printEventBanner(mod);

        simulation.doOneEvent( mod );
        performAnimations();

        // flush so that output from different modules don't get mixed
        ev.flushLastLine();

        // display update
        if (frequent_updates || simulation.eventNumber()%opt_updatefreq_fast==0)
        {
            updateSimtimeDisplay();
            if (speedometer.millisecsInThisInterval() > SPEEDOMETER_UPDATEMILLISECS)
            {
                speedometer.beginNewInterval();
                updatePerformanceDisplay(speedometer);
            }
            updateInspectors();
            Tcl_Eval(interp, "update");
        }

        // exit conditions
        if (untilmodule_reached) break;
        if (stopsimulation_flag) break;
        if (rununtil_time>0 && simulation.guessNextSimtime()>=rununtil_time) break;
        if (rununtil_event>0 && simulation.eventNumber()>=rununtil_event) break;

        // delay loop for slow simulation
        if (runmode==RUNMODE_SLOW)
        {
            timeval start, now;
            gettimeofday(&start, NULL);
            while ((gettimeofday(&now, NULL), timeval_msec(now-start) < (unsigned long)opt_stepdelay) && !stopsimulation_flag)
                Tcl_Eval(interp, "update");
        }

        checkTimeLimits();
    }
    return false;
}

bool Tkenv::doRunSimulationExpress()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_event, rununtil_module;
    //  - stopsimulation_flag
    //  - opt_expressmode_autoupdate
    //
    // EXPRESS does not support rununtil_module!
    //

    if (opt_use_mainwindow)
         CHK(Tcl_VarEval(interp,
              ".main.text insert end {...running in Express mode...\n} event\n"
              ".main.text see end", NULL));

    // should print banner into per module windows too!
    // TO BE IMPLEMENTED

    // update, just to get the above notice displayed
    Tcl_Eval(interp, "update");

    // OK, let's begin
    Speedometer speedometer;
    speedometer.start(simulation.simTime());
    ev.disable_tracing = true;
    animating = false;

    do
    {
        cSimpleModule *mod = simulation.selectNextModule();
        if (!mod) break; // selectNextModule() interrupted (parsim)

        speedometer.addEvent(simulation.simTime());

        simulation.doOneEvent(mod);

        if (simulation.eventNumber()%opt_updatefreq_express==0)
        {
            updateSimtimeDisplay();
            if (speedometer.millisecsInThisInterval() > SPEEDOMETER_UPDATEMILLISECS)
            {
                speedometer.beginNewInterval();
                updatePerformanceDisplay(speedometer);
            }
            if (opt_expressmode_autoupdate)
                updateInspectors();
            Tcl_Eval(interp, "update");
            if (runmode!=RUNMODE_EXPRESS)
                return true;  // should continue, but in a different mode
        }
        checkTimeLimits();
    }
    while( !stopsimulation_flag &&
           (rununtil_time<=0 || simulation.guessNextSimtime() < rununtil_time) &&
           (rununtil_event<=0 || simulation.eventNumber() < rununtil_event)
         );
    return false;
}

void Tkenv::startAll()
{
    confirm("Not implemented.");
}

void Tkenv::finishSimulation()
{
    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY || simstate==SIM_TERMINATED || simstate==SIM_ERROR);

    // print banner into main window
    if (opt_use_mainwindow)
         CHK(Tcl_VarEval(interp,
              ".main.text insert end {** Calling finish() methods of modules\n} event\n"
              ".main.text see end", NULL));

    // should print banner into per module windows too!
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); ++it)
    {
        TModuleWindow *insp = dynamic_cast<TModuleWindow *>(*it);
        if (insp)
        {
           CHK(Tcl_VarEval(interp,
               insp->windowName(),".main.text insert end {** Calling finish() method\n} event\n",
               insp->windowName(),".main.text see end",
               NULL));
        }
    }

    // now really call finish()
    try
    {
        simulation.callFinish();
        ev.flushLastLine();

        checkFingerprint();
    }
    catch (std::exception& e)
    {
        displayError(e);
    }

    // then endrun
    try
    {
        endRun();
    }
    catch (std::exception& e)
    {
        displayError(e);
    }
    simstate = SIM_FINISHCALLED;

    updateSimtimeDisplay();
    updateNextModuleDisplay();
    updateInspectors();
}

void Tkenv::loadNedFile(const char *fname, const char *expectedPackage, bool isXML)
{
    try
    {
        simulation.loadNedFile(fname, expectedPackage, isXML);
    }
    catch (std::exception& e)
    {
        displayError(e);
    }
}

void Tkenv::newNetwork(const char *networkname)
{
    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (simstate!=SIM_NONET)
        {
            endRun();
            simulation.deleteNetwork();
            simstate = SIM_NONET;
        }

        cModuleType *network = resolveNetwork(networkname);
        ASSERT(network);

        CHK(Tcl_VarEval(interp, "clear_windows", NULL));

        // set up new network with config General.
        isconfigrun = false;
        config()->activateConfig("General", 0);
        readPerRunOptions();
        opt_network_name = network->name();  // override config setting
        simulation.setupNetwork(network);
        startRun();
        if (feventlog)
            fflush(feventlog);

        simstate = SIM_NEW;
    }
    catch (std::exception& e)
    {
        displayError(e);
        simstate = SIM_ERROR;
    }

    // update GUI
    animating = false; // affects how network graphics is drawn!
    updateNetworkRunDisplay();
    updateNextModuleDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

void Tkenv::newRun(const char *configname, int runnumber)
{
    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (simstate!=SIM_NONET)
        {
            endRun();
            simulation.deleteNetwork();
            simstate = SIM_NONET;
        }

        // set up new network
        isconfigrun = true;
        config()->activateConfig(configname, runnumber);
        readPerRunOptions();

        if (opt_network_name.empty())
        {
            confirm("No network specified in the configuration.");
            return;
        }

        cModuleType *network = resolveNetwork(opt_network_name.c_str());
        ASSERT(network);

        CHK(Tcl_VarEval(interp, "clear_windows", NULL));

        simulation.setupNetwork(network);
        startRun();
        if (feventlog)
            fflush(feventlog);

        simstate = SIM_NEW;
    }
    catch (std::exception& e)
    {
        displayError(e);
        simstate = SIM_ERROR;
    }

    // update GUI
    animating = false; // affects how network graphics is drawn!
    updateNetworkRunDisplay();
    updateNextModuleDisplay();
    updateSimtimeDisplay();
    updateInspectors();
}

TInspector *Tkenv::inspect(cObject *obj, int type, const char *geometry, void *dat)
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
        confirm(opp_stringf("Class `%s' has no associated inspectors.", obj->className()).c_str());
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
        confirm(opp_stringf("Class `%s' has no `%s' inspector.",obj->className(),insptypeNameFromCode(type)).c_str());
        return NULL;
    }

    // everything ok, finish inspector
    inspectors.insert(inspectors.end(), insp);
    insp->createWindow();
    insp->update();

    return insp;
}

TInspector *Tkenv::findInspector(cObject *obj, int type)
{
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end(); ++it)
    {
        TInspector *insp = *it;
        if (insp->getObject()==obj && insp->getType()==type)
            return insp;
    }
    return NULL;
}

void Tkenv::deleteInspector(TInspector *insp)
{
    inspectors.remove(insp);
    delete insp;
}

void Tkenv::updateInspectors()
{
    // update inspectors
    for (TInspectorList::iterator it = inspectors.begin(); it!=inspectors.end();)
    {
        TInspector *insp = *it;
        TInspectorList::iterator next = ++it;
        if (insp->isMarkedForDeletion())
            deleteInspector(insp);
        else
            insp->update();
        it = next;
    }

    // update object tree
    CHK(Tcl_VarEval(interp, "updateTreeManager",NULL));

    // trim log in main window
    CHK(Tcl_VarEval(interp, "mainlogwindow_trimlines",NULL));

    // try opening "pending" inspectors
    CHK(Tcl_VarEval(interp, "inspectorupdate_callback",NULL));
}

void Tkenv::createSnapshot( const char *label )
{
    simulation.snapshot(&simulation, label );
}

void Tkenv::updateGraphicalInspectorsBeforeAnimation()
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

void Tkenv::updateNetworkRunDisplay()
{
    char runnr[10];
    const char *networkname;

    if (config()->getActiveRunNumber())
        sprintf(runnr, "?");
    else
        sprintf(runnr, "%d", config()->getActiveRunNumber());

    if (simulation.networkType()==NULL)
        networkname = "(no network)";
    else
        networkname = simulation.networkType()->name();

    CHK(Tcl_VarEval(interp, NETWORK_LABEL " config -text {",
                        "Run #",runnr,": ",networkname,
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, "wm title . {OMNeT++/Tkenv - ", getWindowTitlePrefix(), networkname,"}",NULL));
}

void Tkenv::updateSimtimeDisplay()
{
    // event and time display
    char buf[16];
    sprintf(buf, "%lu", simulation.eventNumber());
    CHK(Tcl_VarEval(interp, EVENT_LABEL " config -text {"
                        "Event #", buf,
                        "}", NULL ));
    CHK(Tcl_VarEval(interp, TIME_LABEL " config -text {"
                        "T=", SIMTIME_STR(simulation.guessNextSimtime()),
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

    // time axis
    CHK(Tcl_Eval(interp, "redraw_timeline"));
}

void Tkenv::updateNextModuleDisplay()
{
    cSimpleModule *mod = NULL;

    if (simstate==SIM_NEW || simstate==SIM_READY || simstate==SIM_RUNNING)
        mod = simulation.guessNextModule();

    char id[16];
    std::string modname;
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
    CHK(Tcl_VarEval(interp, NEXT_LABEL " config -text {Next: ",modname.c_str(),id,"}",NULL));
}

void Tkenv::clearNextModuleDisplay()
{
    CHK(Tcl_VarEval(interp, NEXT_LABEL " config -text {Running...}", NULL ));
}

void Tkenv::updatePerformanceDisplay(Speedometer& speedometer)
{
    char buf[16];
    sprintf(buf, "%g", speedometer.simSecPerSec());
    CHK(Tcl_VarEval(interp, SIMSECPERSEC_LABEL " config -text {Simsec/sec: ", buf, "}", NULL));
    sprintf(buf, "%g", speedometer.eventsPerSec());
    CHK(Tcl_VarEval(interp, EVENTSPERSEC_LABEL " config -text {Ev/sec: ", buf, "}", NULL));
    sprintf(buf, "%g", speedometer.eventsPerSimSec());
    CHK(Tcl_VarEval(interp, EVENTSPERSIMSEC_LABEL " config -text {Ev/simsec: ", buf, "}", NULL));
}

void Tkenv::clearPerformanceDisplay()
{
    CHK(Tcl_VarEval(interp, SIMSECPERSEC_LABEL " config -text {Simsec/sec: n/a}", NULL));
    CHK(Tcl_VarEval(interp, EVENTSPERSEC_LABEL " config -text {Ev/sec: n/a}", NULL));
    CHK(Tcl_VarEval(interp, EVENTSPERSIMSEC_LABEL " config -text {Ev/simsec: n/a}", NULL));
}

void Tkenv::printEventBanner(cSimpleModule *module)
{
    char banner[MAX_OBJECTFULLPATH+60];
    sprintf(banner,"** Event #%ld.  T=%s.  Module #%u `%s'\n",
            simulation.eventNumber(),
            SIMTIME_STR(simulation.simTime()),
            module->id(),
            module->fullPath().c_str()
          );

    // insert into main window
    if (opt_use_mainwindow)
        CHK(Tcl_VarEval(interp,
              ".main.text insert end {",banner,"} event\n"
              ".main.text see end", NULL));

    // and into the message window
    if (hasmessagewindow)
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

void Tkenv::readOptions()
{
    EnvirBase::readOptions();

    cConfiguration *cfg = config();

    opt_extrastack_kb = cfg->getAsInt(CFGID_TKENV_EXTRA_STACK_KB);

    const char *s = args->optionValue('c');
    opt_default_config = s ? s : cfg->getAsString(CFGID_DEFAULT_CONFIG);

    const char *r = args->optionValue('r');
    opt_default_run = r ? atoi(r) : cfg->getAsInt(CFGID_DEFAULT_RUN);

    opt_image_path = cfg->getAsFilename(CFGID_IMAGE_PATH).c_str();
    opt_plugin_path = cfg->getAsFilename(CFGID_PLUGIN_PATH).c_str();
}

void Tkenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();
}

bool Tkenv::idle()
{
    eState origsimstate = simstate;
    simstate = SIM_BUSY;
    Tcl_Eval(interp, "update");
    simstate = origsimstate;

    bool stop = stopsimulation_flag;
    stopsimulation_flag = false;
    return stop;
}

void Tkenv::objectDeleted(cObject *object)
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

void Tkenv::simulationEvent(cMessage *msg)
{
    EnvirBase::simulationEvent(msg);

    // display in message window
    if (hasmessagewindow)
        CHK(Tcl_VarEval(interp,
            "catch {\n"
            " .messagewindow.main.text insert end {DELIVD:\t (",msg->className(),")",msg->fullName(),"}\n"
            " .messagewindow.main.text insert end ",TclQuotedString(msg->info().c_str()).get(),"\n"
            " .messagewindow.main.text insert end {\n}\n"
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

void Tkenv::messageSent_OBSOLETE(cMessage *msg, cGate *directToGate) //FIXME needed?
{
    // display in message window
    if (hasmessagewindow)
        CHK(Tcl_VarEval(interp,
            "catch {\n"
            " .messagewindow.main.text insert end {SENT:\t (", msg->className(),")", msg->fullName(), "}\n"
            " .messagewindow.main.text insert end ", TclQuotedString(msg->info().c_str()).get(), "\n"
            " .messagewindow.main.text insert end {\n}\n"
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

void Tkenv::messageScheduled(cMessage *msg)
{
    EnvirBase::messageScheduled(msg);
}

void Tkenv::messageCancelled(cMessage *msg)
{
    EnvirBase::messageCancelled(msg);
}

void Tkenv::beginSend(cMessage *msg)
{
    EnvirBase::beginSend(msg);
}

void Tkenv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void Tkenv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    EnvirBase::messageSendHop(msg, srcGate);
}

void Tkenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void Tkenv::endSend(cMessage *msg)
{
    EnvirBase::endSend(msg);
}

void Tkenv::messageDeleted(cMessage *msg)
{
    EnvirBase::messageDeleted(msg);
}

void Tkenv::componentMethodBegin(cComponent *fromComp, cComponent *toComp, const char *method)
{
    EnvirBase::componentMethodBegin(fromComp, toComp, method);

    if (!animating || !opt_anim_methodcalls)
        return;

    if (!fromComp->isModule() || !toComp->isModule())
        return; // not yet animated

    updateGraphicalInspectorsBeforeAnimation();

    cModule *from = (cModule *)fromComp;
    cModule *to = (cModule *)toComp;

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

void Tkenv::componentMethodEnd()
{
    EnvirBase::componentMethodEnd();
}

void Tkenv::moduleCreated(cModule *newmodule)
{
    EnvirBase::moduleCreated(newmodule);

    cModule *mod = newmodule->parentModule();
    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->submoduleCreated(newmodule);
}

void Tkenv::moduleDeleted(cModule *module)
{
    EnvirBase::moduleDeleted(module);

    cModule *mod = module->parentModule();
    TInspector *insp = findInspector(mod,INSP_GRAPHICAL);
    if (!insp) return;
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    assert(modinsp);
    modinsp->submoduleDeleted(module);
}

void Tkenv::moduleReparented(cModule *module, cModule *oldparent)
{
    EnvirBase::moduleReparented(module, oldparent);

    // pretend it got deleted from under the 1st module, and got created under the 2nd
    TInspector *insp = findInspector(oldparent,INSP_GRAPHICAL);
    TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
    if (modinsp) modinsp->submoduleDeleted(module);

    cModule *mod = module->parentModule();
    TInspector *insp2 = findInspector(mod,INSP_GRAPHICAL);
    TGraphicalModWindow *modinsp2 = dynamic_cast<TGraphicalModWindow *>(insp2);
    if (modinsp2) modinsp2->submoduleCreated(module);
}

void Tkenv::connectionCreated(cGate *srcgate)
{
    EnvirBase::connectionCreated(srcgate);

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

void Tkenv::connectionRemoved(cGate *srcgate)
{
    EnvirBase::connectionRemoved(srcgate);

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

void Tkenv::displayStringChanged(cComponent *component)
{
    EnvirBase::displayStringChanged(component);

    if (dynamic_cast<cModule *>(component))
        moduleDisplayStringChanged((cModule *)component);
    else if (dynamic_cast<cChannel *>(component))
        channelDisplayStringChanged((cChannel *)component);
}

void Tkenv::channelDisplayStringChanged(cChannel *channel)
{
    cGate *gate = channel->fromGate();

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

void Tkenv::moduleDisplayStringChanged(cModule *module)
{
    // refresh inspector where this module is a submodule
    cModule *parentmodule = module->parentModule();
    TInspector *insp = findInspector(parentmodule,INSP_GRAPHICAL);
    if (insp)
    {
        TGraphicalModWindow *parentmodinsp = dynamic_cast<TGraphicalModWindow *>(insp);
        assert(parentmodinsp);
        parentmodinsp->displayStringChanged(module);
    }

    // refresh inspector where this module is the parent (i.e. this is a
    // background display string change)
    insp = findInspector(module,INSP_GRAPHICAL);
    if (insp)
    {
        TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
        assert(modinsp);
        modinsp->displayStringChanged();
    }
}

void Tkenv::animateSend(cMessage *msg, cGate *fromgate, cGate *togate)
{
    char msgptr[32];
    ptrToStr(msg,msgptr);

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
                                    msgptr, " ",
                                    ptrToStr(g)," ",
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


void Tkenv::findDirectPath(cModule *srcmod, cModule *destmod, PathVec& pathvec)
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

void Tkenv::animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate)
{
    char msgptr[32];
    ptrToStr(msg,msgptr);

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
                                        msgptr, " ",
                                        parentptr," ",
                                        modptr," ",
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
                                        msgptr, " ",
                                        parentptr," ",
                                        modptr," ",
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
                                        msgptr, " ",
                                        fromptr," ",
                                        toptr," ",
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


void Tkenv::animateDelivery(cMessage *msg)
{
    char msgptr[32];
    ptrToStr(msg,msgptr);

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
                                msgptr, " ",
                                ptrToStr(g)," ",
                                "end",
                                NULL));
    }
}

void Tkenv::animateDeliveryDirect(cMessage *msg)
{
    char msgptr[32];
    ptrToStr(msg,msgptr);

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
                                msgptr, " ",
                                ptrToStr(destmod),
                                NULL));
    }
}

void Tkenv::performAnimations()
{
    CHK(Tcl_VarEval(interp, "perform_animations", NULL));
}

void Tkenv::bubble(cComponent *component, const char *text)
{
    EnvirBase::bubble(component, text);

    if (disable_tracing)
        return;

    if (!opt_bubbles)
        return;

    if (dynamic_cast<cModule *>(component))
    {
        // module bubble
        cModule *mod = (cModule *)component;
        cModule *parentmod = mod->parentModule();
        TInspector *insp = findInspector(parentmod,INSP_GRAPHICAL);
        if (!insp) return;
        TGraphicalModWindow *modinsp = dynamic_cast<TGraphicalModWindow *>(insp);
        assert(modinsp);
        modinsp->bubble(mod, text);
    }
    else if (dynamic_cast<cChannel *>(component))
    {
        //TODO channel bubble
    }
}

void Tkenv::confirm(const char *msg)
{
    if (!interp)
        ::printf("\n<!> %s\n\n", msg); // fallback in case Tkenv didn't fire up correctly
    else
        CHK(Tcl_VarEval(interp, "messagebox {Confirm} ",TclQuotedString(msg).get()," info ok", NULL));
}

void Tkenv::putsmsg(const char *msg)
{
    confirm(msg);
}

void Tkenv::sputn(const char *s, int n)
{
    EnvirBase::sputn(s, n);

    if (disable_tracing)
        return;

    if (!interp)
    {
        ::fwrite(s,1,n,stdout); // fallback in case Tkenv didn't fire up correctly
        return;
    }

    // rough guard against forgotten "\n"'s in the code
    if (n>5000)
    {
        const char *s2 = "... [line too long, truncated]\n";
        this->sputn(s, 5000);
        this->sputn(s2, strlen(s2));
        return;
    }

    // we'll need to quote Tcl special characters: {}[]$ etc; do it on demand
    bool quotedstr_is_set = false;
    TclQuotedString quotedstr;

    // output string into main window
    cModule *module = simulation.contextModule();
    const char *tag = (!module) ? "log" : "";
    if (!module || opt_use_mainwindow)
    {
        quotedstr.set(s,n);
        quotedstr_is_set = true;
        CHK(Tcl_VarEval(interp,
            ".main.text insert end ",quotedstr.get()," ", tag ,"\n"
            ".main.text see end", NULL));
    }

    // print into module window and all parent compound module windows if they exist
    cModule *mod = module;
    while (mod)
    {
        TInspector *insp = findInspector(mod,INSP_MODULEOUTPUT);
        if (insp)
        {
            // Tcl-quote string if not done yet
            if (!quotedstr_is_set)
            {
                quotedstr.set(s,n);
                quotedstr_is_set = true;
            }
            CHK(Tcl_VarEval(interp,
              insp->windowName(),".main.text insert end ",quotedstr.get()," ", tag, "\n",
              insp->windowName(),".main.text see end", NULL));
        }
        mod = mod->parentModule();
    }
}

cEnvir& Tkenv::flush()
{
    // Tk doesn't need flush(), it displays everything ASAP anyway
    return *this;
}

std::string Tkenv::gets(const char *msg, const char *defaultreply)
{
    char title[70];
    cModule *mod = simulation.contextModule();
    if (mod)
       strncpy(title, mod->fullPath().c_str(),69);
    else
       strncpy(title, simulation.networkType()->name(),69);
    title[69]=0;

    CHK(Tcl_Eval(interp, "global opp"));
    Tcl_SetVar2(interp, "opp", "result", (char *)defaultreply, TCL_GLOBAL_ONLY);
    CHK(Tcl_VarEval(interp, "inputbox ",TclQuotedString(title).get()," ",TclQuotedString(msg).get()," opp(result)",NULL));

    if (Tcl_GetStringResult(interp)[0]=='0')
        throw cRuntimeError(eCANCEL);

    // ok
    std::string result = Tcl_GetVar2(interp, "opp", "result", TCL_GLOBAL_ONLY);
    return result;
}

bool Tkenv::askyesno(const char *question)
{
    // should return -1 when CANCEL is pressed
    CHK(Tcl_VarEval(interp, "messagebox {Tkenv} ",TclQuotedString(question).get()," question yesno", NULL));
    return Tcl_GetStringResult(interp)[0]=='y';
}

unsigned Tkenv::extraStackForEnvir()
{
     return 1024*opt_extrastack_kb;
}

//======================================================================
// dummy function to force Unix linkers collect all symbols needed

void _dummy_for_objinsp();
void _dummy_for_modinsp();
void _dummy_for_statinsp();

void _dummy_func() {
  _dummy_for_objinsp();
  _dummy_for_modinsp();
  _dummy_for_statinsp();
}


