//==========================================================================
//  JAVAENV.CC - part of
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

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "appreg.h"
#include "cenvir.h"
#include "csimplemodule.h"
#include "cmessage.h"
#include "ctypes.h"
#include "speedmtr.h"
#include "cscheduler.h"

#include "javaenv.h"
#include "jcallback.h"
#include "platdep/time.h"
#include "../utils/ver.h"


#define SPEEDOMETER_UPDATEMILLISECS 1000


//
// Register the Javaenv user interface
//
Register_OmnetApp("Javaenv",Javaenv,20,"Eclipse-based user interface");

// some functions that can/should be called from Envir in order to force the
// linker to include the Javaenv library into the executable:
JAVAENV_API void javaenvDummy() {}
JAVAENV_API void envirDummy() {}


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

Javaenv::Javaenv(ArgList *args, cConfiguration *config) : TOmnetApp(args, config)
{
    simstate = SIM_NONET;
    stopsimulation_flag = false;
    animating = false;
    jcallback = NULL;

    // The opt_* vars will be set by readOptions()
}

Javaenv::~Javaenv()
{
    delete jcallback;
}

void Javaenv::setJCallback(JNIEnv *jenv, jobject jcallbackobj)
{
    if (jcallbackobj)
    {
        jcallback = new JCallback(jenv, jcallbackobj);
    }
    else
    {
        delete jcallback;
        jcallback = NULL;
    }
}

void Javaenv::setup()
{
    // initialize base class
    TOmnetApp::setup();  // includes readOptions()
    if (!initialized)
        return;
}

void Javaenv::shutdown()
{
    if (!initialized)
        return;

    // delete network if not yet done
    simulation.deleteNetwork();

    TOmnetApp::shutdown();
}

void Javaenv::printUISpecificHelp()
{
    //XXX remove?
}

void Javaenv::doOneStep()
{
    TRACE("doOneStep");
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    //XXX clearNextModuleDisplay();
    //XXX clearPerformanceDisplay();
    //XXX updateSimtimeDisplay();

    breakpointhit_flag = false;
    animating = true;

    simstate = SIM_RUNNING;
    startClock();
    scheduler->executionResumed();
    try
    {
        cSimpleModule *mod = simulation.selectNextModule();
        if (mod)  // selectNextModule() not interrupted
        {
            //XXX if (opt_print_banners)
            //XXX    printEventBanner(mod);
            simulation.doOneEvent(mod);
        }
        //XXX updateSimtimeDisplay();
        //XXX updateNextModuleDisplay();
        //XXX updateInspectors();
        simstate = SIM_READY;
        outvectmgr->flush();
        outscalarmgr->flush();
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
    stopsimulation_flag = false;

    if (simstate==SIM_TERMINATED)
    {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        //
        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
        // called yet, because we want to allow the user to force finish() from
        // the GUI -- and finish() has to precede endRun(). endRun() will be called
        // just before a new network gets set up, or on Javaenv shutdown.
        //
        finishSimulation();
    }
}

void Javaenv::runSimulation(int mode, simtime_t until_time, long until_event, cModule *until_module)
{
    TRACE("runSimulation");
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY);

    runmode = mode;
    rununtil_time = until_time;
    rununtil_event = until_event;
    rununtil_module = until_module;  // Note: this is NOT supported with RUNMODE_EXPRESS

    breakpointhit_flag = false;
    stopsimulation_flag = false;

    //XXX clearNextModuleDisplay();
    //XXX clearPerformanceDisplay();
    //XXX updateSimtimeDisplay();
    //XXX Tcl_Eval(interp, "update");

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
        outvectmgr->flush();
        outscalarmgr->flush();
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
        // just before a new network gets set up, or on Javaenv shutdown.
        //
        finishSimulation();
    }

    //XXX updateNextModuleDisplay();
    //XXX clearPerformanceDisplay();
    //XXX updateSimtimeDisplay();
    //XXX updateInspectors();
}

void Javaenv::setSimulationRunMode(int mode)
{
    // This function (and the next one too) is called while runSimulation() is
    // underway, from Tcl code that gets a chance to run via the
    // Tcl_Eval(interp, "update") commands
    runmode = mode;
}

void Javaenv::setSimulationRunUntil(simtime_t until_time, long until_event)
{
    rununtil_time = until_time;
    rununtil_event = until_event;
}

void Javaenv::setSimulationRunUntilModule(cModule *until_module)
{
    rununtil_module = until_module;
}

bool Javaenv::doRunSimulation()
{
    TRACE("doRunSimulation");
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_event, rununtil_module;
    //  - breakpointhit_flag, stopsimulation_flag
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
        //XXX if (opt_print_banners)
        //XXX     printEventBanner(mod);

        simulation.doOneEvent( mod );

        // flush so that output from different modules don't get mixed
        ev.flushlastline();

        // display update
        if (frequent_updates || simulation.eventNumber()%opt_updatefreq_fast==0)
        {
            //XXX updateSimtimeDisplay();
            if (speedometer.millisecsInThisInterval() > SPEEDOMETER_UPDATEMILLISECS)
            {
                speedometer.beginNewInterval();
                //XXX updatePerformanceDisplay(speedometer);
            }
            //XXX updateInspectors();
            //XXX Tcl_Eval(interp, "update");
        }

        // exit conditions
        if (untilmodule_reached) break;
        if (breakpointhit_flag || stopsimulation_flag) break;
        if (rununtil_time>0 && simulation.guessNextSimtime()>=rununtil_time) break;
        if (rununtil_event>0 && simulation.eventNumber()>=rununtil_event) break;

        // delay loop for slow simulation
        if (runmode==RUNMODE_SLOW)
        {
            timeval start, now;
            gettimeofday(&start, NULL);
            while ((gettimeofday(&now, NULL), timeval_msec(now-start) < (unsigned long)opt_stepdelay) && !stopsimulation_flag)
                ; //XXX Tcl_Eval(interp, "update");
        }

        checkTimeLimits();
    }
    return false;
}

bool Javaenv::doRunSimulationExpress()
{
    TRACE("doRunSimulationExpress");
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_event, rununtil_module;
    //  - breakpointhit_flag, stopsimulation_flag
    //  - opt_expressmode_autoupdate
    //
    // EXPRESS does not support rununtil_module!
    //

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

        simulation.doOneEvent( mod );

        if (simulation.eventNumber()%opt_updatefreq_express==0)
        {
            //XXX updateSimtimeDisplay();
            if (speedometer.millisecsInThisInterval() > SPEEDOMETER_UPDATEMILLISECS)
            {
                speedometer.beginNewInterval();
                //XXX updatePerformanceDisplay(speedometer);
            }
            //XXX if (opt_expressmode_autoupdate)
            //XXX    updateInspectors();
            //XXX Tcl_Eval(interp, "update");
            if (runmode!=RUNMODE_EXPRESS)
                return true;  // should continue, but in a different mode
        }
        checkTimeLimits();
    }
    while(  !breakpointhit_flag && !stopsimulation_flag &&
            (rununtil_time<=0 || simulation.guessNextSimtime()<rununtil_time) &&
            (rununtil_event<=0 || simulation.eventNumber()<rununtil_event)
         );
    return false;
}

void Javaenv::finishSimulation()
{
    TRACE("finishSimulation");
    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
    ASSERT(simstate==SIM_NEW || simstate==SIM_READY || simstate==SIM_TERMINATED || simstate==SIM_ERROR);

    // now really call finish()
    try
    {
        simulation.callFinish();
        ev.flushlastline();
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
    }

    // then endrun
    try
    {
        endRun();
    }
    catch (cException *e)
    {
        displayError(e);
        delete e;
    }
    simstate = SIM_FINISHCALLED;

    //XXX updateSimtimeDisplay();
    //XXX updateNextModuleDisplay();
    //XXX updateInspectors();
}

void Javaenv::loadNedFile(const char *fname)
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

void Javaenv::newNetwork(const char *network_name)
{
    TRACE("newNetwork");
    cNetworkType *network = findNetwork( network_name );
    if (!network)
    {
        //XXX CHK(Tcl_VarEval(interp,"messagebox {Confirm} {Network '", network_name, "' not found.} info ok",NULL));
        return;
    }

    try
    {
        // finish & cleanup previous run if we haven't done so yet
        if (simstate!=SIM_NONET)
        {
            endRun();
            simulation.deleteNetwork();
            simstate = SIM_NONET;
        }

        //XXX CHK(Tcl_VarEval(interp, "clear_windows", NULL));

        // set up new network
        readPerRunOptions(0);
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
    animating = false; // affects how network graphics is drawn!
    //XXX updateNetworkRunDisplay();
    //XXX updateNextModuleDisplay();
    //XXX updateSimtimeDisplay();
    //XXX updateInspectors();
}

void Javaenv::newRun(int run)
{
    TRACE("newRun");
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
        run_nr = run;
        readPerRunOptions(run_nr);

        cNetworkType *network = findNetwork(opt_network_name.c_str());
        if (!network)
        {
            //XXX CHK(Tcl_VarEval(interp,"messagebox {Confirm} {Network '", opt_network_name.c_str(), "' not found.} info ok",NULL));
            return;
        }

        //XXX CHK(Tcl_VarEval(interp, "clear_windows", NULL));

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
    animating = false; // affects how network graphics is drawn!
    //XXX updateNetworkRunDisplay();
    //XXX updateNextModuleDisplay();
    //XXX updateSimtimeDisplay();
    //XXX updateInspectors();
}

bool Javaenv::isBreakpointActive(const char *, cSimpleModule *)
{
    // args: label, module
    if (!opt_bkpts_enabled)
         return false;

    // To be implemented! Should be able to stop depending on all/selected labels, all/selected modules, etc.
    return true;
}

//=========================================================================
void Javaenv::readOptions()
{
    TOmnetApp::readOptions();

    cConfiguration *cfg = getConfig();

    opt_extrastack_kb = cfg->getAsInt("Javaenv", "extra-stack-kb", JAVAENV_EXTRASTACK_KB);

    char *r = args->argValue('r');
    if (r)
        opt_default_run = atoi(r);
    else
        opt_default_run = cfg->getAsInt( "Javaenv", "default-run", 0);

    // Note: most entries below should be obsoleted (with .javaenvrc taking over)
    opt_stepdelay = long(1000*cfg->getAsTime( "Javaenv", "slowexec-delay", 0.3 )+.5);
    opt_updatefreq_fast = cfg->getAsInt( "Javaenv", "update-freq-fast", 50);
    opt_updatefreq_express = cfg->getAsInt( "Javaenv", "update-freq-express", 10000 );
    opt_bkpts_enabled = cfg->getAsBool( "Javaenv", "breakpoints-enabled", true );
    opt_animation_enabled = cfg->getAsBool( "Javaenv", "animation-enabled", true );
    opt_nexteventmarkers = cfg->getAsBool( "Javaenv", "next-event-markers", true );
    opt_senddirect_arrows = cfg->getAsBool( "Javaenv", "senddirect-arrows", true );
    opt_anim_methodcalls = cfg->getAsBool( "Javaenv", "anim-methodcalls", true );
    opt_methodcalls_delay = long(1000*cfg->getAsTime( "Javaenv", "methodcalls-delay", 0.2)+.5);
    opt_animation_msgnames = cfg->getAsBool( "Javaenv", "animation-msgnames", true );
    opt_animation_msgclassnames = cfg->getAsBool( "Javaenv", "animation-msgclassnames", true );
    opt_animation_msgcolors = cfg->getAsBool( "Javaenv", "animation-msgcolors", true );
    opt_penguin_mode = cfg->getAsBool( "Javaenv", "penguin-mode", false );
    opt_showlayouting = cfg->getAsBool( "Javaenv", "show-layouting", false);
    opt_bubbles = cfg->getAsBool( "Javaenv", "show-bubbles", true );
    opt_animation_speed = cfg->getAsDouble( "Javaenv", "animation-speed", 1.5);
    if (opt_animation_speed<0) opt_animation_speed=0;
    if (opt_animation_speed>3) opt_animation_speed=3;
    opt_print_banners = cfg->getAsBool( "Javaenv", "print-banners", true );
    opt_use_mainwindow = cfg->getAsBool( "Javaenv", "use-mainwindow", true );
    opt_expressmode_autoupdate = cfg->getAsBool( "Javaenv", "expressmode-autoupdate", true );
    opt_bitmap_path = cfg->getAsFilenames( "Javaenv", "bitmap-path", "").c_str();
    opt_plugin_path = cfg->getAsFilenames( "Javaenv", "plugin-path", "").c_str();
}

void Javaenv::readPerRunOptions(int run_nr)
{
    TOmnetApp::readPerRunOptions( run_nr );
}

bool Javaenv::idle()
{
    eState origsimstate = simstate;
    simstate = SIM_BUSY;
    bool stop = false;
    if (jcallback) stop = jcallback->idle();
    //XXX Tcl_Eval(interp, "update");
    simstate = origsimstate;
    stopsimulation_flag = false;  //XXX do we need this flag any more????
    return stop;
}

void Javaenv::objectDeleted(cObject *object)
{
    TOmnetApp::objectDeleted(object);
    if (jcallback) jcallback->objectDeleted(object);
}

void Javaenv::messageDelivered(cMessage *msg)
{
    TOmnetApp::messageDelivered(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageDelivered(msg);
}

void Javaenv::messageScheduled(cMessage *msg)
{
    TOmnetApp::messageScheduled(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageScheduled(msg);
}

void Javaenv::messageCancelled(cMessage *msg)
{
    TOmnetApp::messageCancelled(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageCancelled(msg);
}

void Javaenv::beginSend(cMessage *msg)
{
    TOmnetApp::beginSend(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->beginSend(msg);
}

void Javaenv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    TOmnetApp::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void Javaenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay)
{
    TOmnetApp::messageSendHop(msg, srcGate, propagationDelay);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendHop(msg, srcGate, propagationDelay);
}

void Javaenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay, simtime_t transmissionStartTime)
{
    TOmnetApp::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay, transmissionStartTime);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendHop(msg, srcGate, propagationDelay, transmissionDelay, transmissionStartTime);
}

void Javaenv::moduleMethodCalled(cModule *from, cModule *to, const char *method)
{
    TOmnetApp::moduleMethodCalled(from, to, method);
    if (ev.disable_tracing) return; //XXX
    if (!animating || !opt_anim_methodcalls)
        return;
    if (jcallback) jcallback->moduleMethodCalled(from, to, method);
}

void Javaenv::moduleCreated(cModule *newmodule)
{
    TOmnetApp::moduleCreated(newmodule);
    if (jcallback) jcallback->moduleCreated(newmodule);
}

void Javaenv::moduleDeleted(cModule *module)
{
    TOmnetApp::moduleDeleted(module);
    if (jcallback) jcallback->moduleDeleted(module);
}

void Javaenv::moduleReparented(cModule *module, cModule *oldparent)
{
    TOmnetApp::moduleReparented(module, oldparent);
    if (jcallback) jcallback->moduleReparented(module, oldparent);
}

void Javaenv::connectionCreated(cGate *srcgate)
{
    TOmnetApp::connectionCreated(srcgate);
    if (jcallback) jcallback->connectionCreated(srcgate);
}

void Javaenv::connectionRemoved(cGate *srcgate)
{
    TOmnetApp::connectionRemoved(srcgate);
    if (jcallback) jcallback->connectionRemoved(srcgate);
}

void Javaenv::displayStringChanged(cGate *gate)
{
    TOmnetApp::displayStringChanged(gate);
    if (jcallback) jcallback->displayStringChanged(gate);
}

void Javaenv::displayStringChanged(cModule *submodule)
{
    TOmnetApp::displayStringChanged(submodule);
    if (jcallback) jcallback->displayStringChanged(submodule);
}

void Javaenv::backgroundDisplayStringChanged(cModule *parentmodule)
{
    TOmnetApp::backgroundDisplayStringChanged(parentmodule);
    if (jcallback) jcallback->backgroundDisplayStringChanged(parentmodule);
}

void Javaenv::breakpointHit(const char *label, cSimpleModule *mod)
{
    TOmnetApp::breakpointHit(label, mod);
    if (jcallback) jcallback->breakpointHit(label, mod);
}

void Javaenv::bubble(cModule *mod, const char *text)
{
    TOmnetApp::bubble(mod, text);
    if (jcallback) jcallback->bubble(mod, text);
}

void Javaenv::putmsg(const char *str)
{
    //XXX
}

void Javaenv::sputn(const char *s, int n)
{
    TOmnetApp::sputn(s, n);

    // rough guard against forgotten "\n"'s in the code
    if (n>5000)
    {
        const char *s2 = "... [line too long, truncated]\n";
        this->sputn(s, 5000);
        this->sputn(s2, strlen(s2));
        return;
    }

    //XXX
}

void Javaenv::flush()
{
    //XXX
}

bool Javaenv::gets(const char *promptstr, char *buf, int len)
{
    //XXX
    return false;
}

int Javaenv::askYesNo(const char *question)
{
    //XXX
    return false;
}

unsigned Javaenv::extraStackForEnvir()
{
     return 1024*opt_extrastack_kb;
}


