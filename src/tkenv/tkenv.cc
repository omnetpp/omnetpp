//==========================================================================
//  TKENV.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  contains:  Tkenv member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include <csignal>
#include <cstdio>
#include <string>
#include <algorithm>

#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/matchexpression.h"
#include "common/fileutil.h"
#include "common/ver.h"
#include "envir/appreg.h"
#include "envir/speedometer.h"
#include "envir/matchableobject.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/platdep/platmisc.h"
#include "tkdefs.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspector.h"
#include "inspectorfactory.h"
#include "moduleinspector.h"
#include "loginspector.h"
#include "gateinspector.h"
#include "genericobjectinspector.h"
#include "watchinspector.h"

// default plugin path -- allow overriding it via compiler option (-D)
// (default image path comes from makefile)
#ifndef OMNETPP_PLUGIN_PATH
#define OMNETPP_PLUGIN_PATH    "./plugins"
#endif

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

using namespace omnetpp::common;
using namespace omnetpp::envir;

namespace omnetpp {
namespace tkenv {

//
// Register the Tkenv user interface
//
Register_OmnetApp("Tkenv", Tkenv, 20, "Tk-based legacy graphical user interface");

//
// The following function can be used to force linking with Tkenv; specify
// -u _tkenv_lib (gcc) or /include:_tkenv_lib (vc++) in the link command.
//
extern "C" TKENV_API void tkenv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" TKENV_API void _tkenv_lib() {}

Register_GlobalConfigOptionU(CFGID_TKENV_EXTRA_STACK, "tkenv-extra-stack", "B", "48KiB", "Specifies the extra amount of stack that is reserved for each `activity()` simple module when the simulation is run under Tkenv.");
Register_GlobalConfigOption(CFGID_TKENV_DEFAULT_CONFIG, "tkenv-default-config", CFG_STRING, nullptr, "Specifies which config Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigOption(CFGID_TKENV_DEFAULT_RUN, "tkenv-default-run", CFG_INT, "0", "Specifies which run (of the default config, see `tkenv-default-config`) Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigOption(CFGID_TKENV_PLUGIN_PATH, "tkenv-plugin-path", CFG_PATH, "", "Specifies the search path for Tkenv plugins. Tkenv plugins are .tcl files that get evaluated on startup.");

// utility function
static bool moduleContains(cModule *potentialparent, cModule *mod)
{
    while (mod) {
        if (mod == potentialparent)
            return true;
        mod = mod->getParentModule();
    }
    return false;
}

TkenvOptions::TkenvOptions()
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    updateFreqFast = 500;
    updateFreqExpress = 1000;
    animationEnabled = true;
    showNextEventMarkers = true;
    showSendDirectArrows = true;
    animateMethodCalls = true;
    methodCallAnimDelay = 200;
    animationMsgNames = true;
    animationMsgClassNames = true;
    animationMsgColors = true;
    penguinMode = false;
    showLayouting = false;
    layouterChoice = LAYOUTER_AUTO;
    arrangeVectorConnections = false;
    iconMinimumSize = 5;
    showBubbles = true;
    animationSpeed = 1.5;
    printEventBanners = true;
    printInitBanners = true;
    shortBanners = false;
    autoupdateInExpress = true;
    stopOnMsgCancel = true;
    stripNamespace = STRIPNAMESPACE_ALL;
    logFormat = "%l %C: ";
    logLevel = LOGLEVEL_TRACE;
    scrollbackLimit = 10000;
}

Tkenv::Tkenv() : opt((TkenvOptions *&)EnvirBase::opt)
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    interp = nullptr;  // Tcl/Tk not set up yet
    ferrorlog = nullptr;
    simulationState = SIM_NONET;
    stopSimulationFlag = false;
    animating = false;
    isConfigRun = false;
    runUntil.msg = nullptr;  // deactivate corresponding checks in eventCancelled()/objectDeleted()
    idleLastUICheck = opp_get_monotonic_clock_usecs();

    // set the name here, to prevent warning from cStringPool on shutdown when Cmdenv runs
    inspectorfactories.getInstance()->setName("inspectorfactories");
}

Tkenv::~Tkenv()
{
    for (auto & silentEventFilter : silentEventFilters)
        delete silentEventFilter;
}

static void signalHandler(int signum)
{
    cStaticFlag::setExiting();
    exit(2);
}

void Tkenv::doRun()
{
    //
    // SETUP
    //
    try {
        // set signal handler
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

#ifdef __APPLE__
// The following is required to display the menu correctly on Mac OS X 10.5.
// An alternative solution would be to create a bundled application;
// as much as putting the executable into a directory called Contents/MacOS,
// or even creating Contents directory next to the executable would do,
// but its not an elegant solution.
//
        ProcessSerialNumber psn;
        GetCurrentProcess(&psn);
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif
        // path for the Tcl user interface files
#ifdef OMNETPP_TKENV_DIR
        tkenvDir = opp_emptytodefault(getenv("OMNETPP_TKENV_DIR"), OMNETPP_TKENV_DIR);
#endif

        // path for plugins
        std::string pluginPath = opp_emptytodefault(getenv("OMNETPP_PLUGIN_PATH"), OMNETPP_PLUGIN_PATH);
        if (!opt->pluginPath.empty())
            pluginPath = opt->pluginPath + ";" + pluginPath;

        // set up Tcl/Tk
        interp = initTk(args->getArgCount(), args->getArgVector());
        if (!interp)
            throw opp_runtime_error("Tkenv: Cannot create Tcl interpreter");

        // add OMNeT++'s commands to Tcl
        createTkCommands(interp, tcl_commands);

        Tcl_SetVar(interp, "OMNETPP_IMAGE_PATH", TCLCONST(opt->imagePath.c_str()), TCL_GLOBAL_ONLY);
        Tcl_SetVar(interp, "OMNETPP_PLUGIN_PATH", TCLCONST(pluginPath.c_str()), TCL_GLOBAL_ONLY);
        Tcl_SetVar(interp, "OMNETPP_LIB_DIR", OMNETPP_LIB_DIR, TCL_GLOBAL_ONLY);

        Tcl_SetVar(interp, "OMNETPP_RELEASE", OMNETPP_RELEASE, TCL_GLOBAL_ONLY);
        Tcl_SetVar(interp, "OMNETPP_EDITION", OMNETPP_EDITION, TCL_GLOBAL_ONLY);
        Tcl_SetVar(interp, "OMNETPP_BUILDID", OMNETPP_BUILDID, TCL_GLOBAL_ONLY);

        // we need to flush streams, otherwise output written from Tcl tends to overtake
        // output written from C++ so far, at least in the IDE's console view
        fflush(stdout);
        fflush(stderr);

        // eval Tcl sources: either from .tcl files or from compiled-in string
        // literal (tclcode.cc)...

#ifdef OMNETPP_TKENV_DIR
        //
        // Case A: TCL code in separate .tcl files
        //
        Tcl_SetVar(interp, "OMNETPP_TKENV_DIR", TCLCONST(tkenvDir.c_str()), TCL_GLOBAL_ONLY);
        if (Tcl_EvalFile(interp, opp_concat(tkenvDir.c_str(), "/tkenv.tcl")) == TCL_ERROR) {
            logTclError(__FILE__, __LINE__, interp);
            throw opp_runtime_error("Tkenv: %s. (Is the OMNETPP_TKENV_DIR environment variable "
                                    "set correctly? When not set, it defaults to " OMNETPP_TKENV_DIR ")",
                    Tcl_GetStringResult(interp));
        }
#else
        //
        // Case B: compiled-in TCL code
        //
        // The tclcode.cc file is generated from the Tcl scripts
        // with the tcl2c program (to be compiled from tcl2c.c).
        //
#include "tclcode.cc"
        if (Tcl_Eval(interp, (char *)tcl_code) == TCL_ERROR) {
            logTclError(__FILE__, __LINE__, interp);
            throw opp_runtime_error("Tkenv: %s", Tcl_GetStringResult(interp));
        }
#endif

        // evaluate main script and build user interface
        if (Tcl_Eval(interp, "startTkenv") == TCL_ERROR) {
            logTclError(__FILE__, __LINE__, interp);
            throw opp_runtime_error("Tkenv: %s\n", Tcl_GetStringResult(interp));
        }

        // create windowtitle prefix
        if (getParsimNumPartitions() > 0) {
            char tmp[32];
            sprintf(tmp, "Proc %d/%d - ", getParsimProcId(), getParsimNumPartitions());
            windowTitlePrefix = tmp;
        }

        mainInspector = (GenericObjectInspector *)InspectorFactory::get("GenericObjectInspectorFactory")->createInspector();
        addEmbeddedInspector(".inspector", mainInspector);

        mainNetworkView = (ModuleInspector *)InspectorFactory::get("ModuleInspectorFactory")->createInspector();
        addEmbeddedInspector(".network", mainNetworkView);

        mainLogView = (LogInspector *)InspectorFactory::get("LogInspectorFactory")->createInspector();
        addEmbeddedInspector(".log", mainLogView);

        setLogFormat(opt->logFormat.c_str());
    }
    catch (std::exception& e) {
        interp = nullptr;
        throw;
    }
    //
    // RUN
    //
    CHK(Tcl_Eval(interp, "startupCommands"));
    runTk(interp);

    //
    // SHUTDOWN
    //

    // close all inspectors before exiting
    for (;;) {
        InspectorList::iterator it = inspectors.begin();
        if (it == inspectors.end())
            break;
        Inspector *insp = *it;
        inspectors.erase(it);
        delete insp;
    }

    // clear log
    logBuffer.clear();  // FIXME how is the log cleared between runs??????????????

    // delete network if not yet done
    if (simulationState != SIM_NONET && simulationState != SIM_FINISHCALLED)
        endRun();
    getSimulation()->deleteNetwork();

    // pull down inspector factories
    inspectorfactories.clear();
}

void Tkenv::printUISpecificHelp()
{
    out << "\n";
    out << "Tkenv-specific information:\n";
    out << "    Tkenv allows the user to select a simulation run interactively.\n";
    out << "    The -c and -r options only serve as hints or default values for\n";
    out << "    the GUI.\n";
}

void Tkenv::rebuildSim()
{
    if (isConfigRun)
        newRun(std::string(getConfigEx()->getActiveConfigName()).c_str(), getConfigEx()->getActiveRunNumber());
    else if (getSimulation()->getNetworkType() != nullptr)
        newNetwork(getSimulation()->getNetworkType()->getName());
    else
        confirm(INFO, "Choose File|New Network or File|New Run.");
}

void Tkenv::doOneStep()
{
    ASSERT(simulationState == SIM_NEW || simulationState == SIM_READY);

    animating = true;
    runUntil.msg = nullptr;  // deactivate corresponding checks in eventCancelled()/objectDeleted()
    simulationState = SIM_RUNNING;

    updateStatusDisplay();

    startClock();
    notifyLifecycleListeners(LF_ON_SIMULATION_RESUME);
    try {
        cEvent *event = getSimulation()->takeNextEvent();
        if (event) {  // takeNextEvent() not interrupted
            getSimulation()->executeEvent(event);

            // updating before animation, so a more recent (less unreasonable) state is shown while the animations are running
            callRefreshDisplay();
            updateStatusDisplay();
            refreshInspectors();
            Tcl_Eval(interp, "update idletasks");
            performAnimations();
        }
        // display update, even if we already did one before the animations
        callRefreshDisplay();
        updateStatusDisplay();
        refreshInspectors();
        simulationState = SIM_READY;
        notifyLifecycleListeners(LF_ON_SIMULATION_PAUSE);
    }
    catch (cTerminationException& e) {
        simulationState = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);
        displayException(e);
    }
    catch (std::exception& e) {
        simulationState = SIM_ERROR;
        updateStatusDisplay();
        stoppedWithException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    stopClock();
    stopSimulationFlag = false;

    if (simulationState == SIM_TERMINATED) {
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

void Tkenv::runSimulation(int mode, simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg, cModule *until_module)
{
    ASSERT(simulationState == SIM_NEW || simulationState == SIM_READY);

    runMode = mode;
    runUntil.time = until_time;
    runUntil.eventNumber = until_eventnum;
    runUntil.msg = until_msg;
    runUntil.module = until_module;  // Note: this is NOT supported with RUNMODE_EXPRESS

    stopSimulationFlag = false;
    simulationState = SIM_RUNNING;

    updateStatusDisplay();
    Tcl_Eval(interp, "update");

    startClock();
    notifyLifecycleListeners(LF_ON_SIMULATION_RESUME);
    try {
        // funky while loop to handle switching to and from EXPRESS mode....
        bool cont = true;
        while (cont) {
            if (runMode == RUNMODE_EXPRESS)
                cont = doRunSimulationExpress();
            else
                cont = doRunSimulation();
        }
        if (runMode != RUNMODE_NORMAL) // in NORMAL mode, doRunSimulation() already calls refreshDisplay() after each event
            callRefreshDisplay();
        simulationState = SIM_READY;
        notifyLifecycleListeners(LF_ON_SIMULATION_PAUSE);
    }
    catch (cTerminationException& e) {
        simulationState = SIM_TERMINATED;
        stoppedWithTerminationException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);
        displayException(e);
    }
    catch (std::exception& e) {
        simulationState = SIM_ERROR;
        stoppedWithException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    stopClock();
    stopSimulationFlag = false;

    animating = true;
    loggingEnabled = true;
    runUntil.msg = nullptr;

    if (simulationState == SIM_TERMINATED) {
        // call wrapper around simulation.callFinish() and simulation.endRun()
        //
        // NOTE: if the simulation is in SIM_ERROR, we don't want endRun() to be
        // called yet, because we want to allow the user to force finish() from
        // the GUI -- and finish() has to precede endRun(). endRun() will be called
        // just before a new network gets set up, or on Tkenv shutdown.
        //
        finishSimulation();
    }

    updateStatusDisplay();
    refreshInspectors();
}

void Tkenv::setSimulationRunMode(int mode)
{
    // This function (and the next one too) is called while runSimulation() is
    // underway, from Tcl code that gets a chance to run via the
    // Tcl_Eval(interp, "update") commands
    runMode = mode;
}

void Tkenv::setSimulationRunUntil(simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg)
{
    runUntil.time = until_time;
    runUntil.eventNumber = until_eventnum;
    runUntil.msg = until_msg;
}

void Tkenv::setSimulationRunUntilModule(cModule *until_module)
{
    runUntil.module = until_module;
}

// note: also updates "since" (sets it to the current time) if answer is "true"
inline bool elapsed(long millis, int64_t& since)
{
    int64_t now = opp_get_monotonic_clock_usecs();
    bool ret = (now - since) >= millis * 1000;
    if (ret)
        since = now;
    return ret;
}

bool Tkenv::doRunSimulation()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_eventnum, rununtil_msg, rununtil_module;
    //  - stopsimulation_flag
    //
    speedometer.start(getSimulation()->getSimTime());
    loggingEnabled = true;
    bool firstevent = true;

    int64_t last_update = opp_get_monotonic_clock_usecs();

    while (true) {
        if (runMode == RUNMODE_EXPRESS)
            return true;  // should continue, but in a different mode

        // query which module will execute the next event
        cEvent *event = getSimulation()->takeNextEvent();
        if (!event)
            break;  // takeNextEvent() interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (runUntil.msg && event == runUntil.msg) {
            getSimulation()->putBackEvent(event);
            break;
        }

        // if stepping locally in module, we stop both immediately
        // *before* and *after* executing the event in that module,
        // but we always execute at least one event
        cModule *mod = event->isMessage() ? static_cast<cMessage *>(event)->getArrivalModule() : nullptr;
        bool untilmodule_reached = runUntil.module && moduleContains(runUntil.module, mod);
        if (untilmodule_reached && !firstevent) {
            getSimulation()->putBackEvent(event);
            break;
        }
        firstevent = false;

        animating = (runMode == RUNMODE_NORMAL) || untilmodule_reached;
        bool frequent_updates = (runMode == RUNMODE_NORMAL);

        speedometer.addEvent(getSimulation()->getSimTime());

        // do a simulation step
        getSimulation()->executeEvent(event);
        if (animating) {
            // updating before animation, so a more recent (less unreasonable) state is shown while the animations are running
            callRefreshDisplay();
            updateStatusDisplay();
            refreshInspectors();
            Tcl_Eval(interp, "update idletasks");
            performAnimations();
        }

        // flush so that output from different modules don't get mixed
        cLogProxy::flushLastLine();

        // display update, even if we already did one before the animations
        if (frequent_updates || ((getSimulation()->getEventNumber()&0x0f) == 0 && elapsed(opt->updateFreqFast, last_update))) {
            if (!frequent_updates || ((long)speedometer.getMillisSinceIntervalStart() > opt->updateFreqFast && speedometer.getNumEventsSinceIntervalStart() >= 3)) // do not start new interval at every event
                speedometer.beginNewInterval();  // should precede updateStatusDisplay()
            callRefreshDisplay();
            refreshInspectors();
            updateStatusDisplay();
            Tcl_Eval(interp, "update");
            last_update = opp_get_monotonic_clock_usecs();  // exclude UI update time [bug #52]
        }

        // exit conditions
        if (untilmodule_reached)
            break;
        if (stopSimulationFlag)
            break;
        if (runUntil.time > SIMTIME_ZERO && getSimulation()->guessNextSimtime() >= runUntil.time)
            break;
        if (runUntil.eventNumber > 0 && getSimulation()->getEventNumber() + 1 >= runUntil.eventNumber)
            break;

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
    //  - runMode, runUntil.time, runUntil.eventNumber, runUntil.msg, runUntil.module;
    //  - stopSimulationFlag
    //  - opt->autoupdateInExpress
    //
    // EXPRESS does not support runUntil.module!
    //

    char info[128];
    sprintf(info, "** Running in Express mode from event #%" PRId64 "  t=%s ...\n",
            getSimulation()->getEventNumber(), SIMTIME_STR(getSimulation()->getSimTime()));
    logBuffer.addInfo(info);

    // update, just to get the above notice displayed
    Tcl_Eval(interp, "update");

    // OK, let's begin
    speedometer.start(getSimulation()->getSimTime());
    loggingEnabled = false;
    animating = false;

    int64_t last_update = opp_get_monotonic_clock_usecs();

    bool result = false;
    do {
        cEvent *event = getSimulation()->takeNextEvent();
        if (!event)
            break;  // takeNextEvent() interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (runUntil.msg && event == runUntil.msg) {
            getSimulation()->putBackEvent(event);
            break;
        }

        speedometer.addEvent(getSimulation()->getSimTime());

        getSimulation()->executeEvent(event);

        if ((getSimulation()->getEventNumber()&0xff) == 0 && elapsed(opt->updateFreqExpress, last_update)) {
            speedometer.beginNewInterval();  // should precede updateStatusDisplay()
            if (opt->autoupdateInExpress) {
                callRefreshDisplay();
                refreshInspectors();
            }
            updateStatusDisplay();
            Tcl_Eval(interp, "update");
            last_update = opp_get_monotonic_clock_usecs();  // exclude UI update time [bug #52]
            if (runMode != RUNMODE_EXPRESS) {
                result = true;  // should continue, but in a different mode
                break;
            }
        }
        checkTimeLimits();
    } while (!stopSimulationFlag &&
             (runUntil.time <= SIMTIME_ZERO || getSimulation()->guessNextSimtime() < runUntil.time) &&
             (runUntil.eventNumber <= 0 || getSimulation()->getEventNumber() + 1 < runUntil.eventNumber)
             );

    sprintf(info, "** Leaving Express mode at event #%" PRId64 "  t=%s\n",
            getSimulation()->getEventNumber(), SIMTIME_STR(getSimulation()->getSimTime()));
    logBuffer.addInfo(info);

    return result;
}

void Tkenv::startAll()
{
    confirm(INFO, "Not implemented.");
}

void Tkenv::finishSimulation()
{
    // strictly speaking, we shouldn't allow callFinish() after SIM_ERROR, but it comes handy in practice...
    ASSERT(simulationState == SIM_NEW || simulationState == SIM_READY || simulationState == SIM_TERMINATED || simulationState == SIM_ERROR);

    if (simulationState == SIM_NEW || simulationState == SIM_READY) {
        cTerminationException e("The user has finished the simulation");
        stoppedWithTerminationException(e);
    }

    logBuffer.addInfo("** Calling finish() methods of modules\n");

    // now really call finish()
    try {
        getSimulation()->callFinish();
        callRefreshDisplay();
        cLogProxy::flushLastLine();

        checkFingerprint();
    }
    catch (std::exception& e) {
        stoppedWithException(e);
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    // then endrun
    try {
        endRun();
    }
    catch (std::exception& e) {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
    }
    simulationState = SIM_FINISHCALLED;

    updateStatusDisplay();
    refreshInspectors();
}

void Tkenv::loadNedFile(const char *fname, const char *expectedPackage, bool isXML)
{
    try {
        getSimulation()->loadNedFile(fname, expectedPackage, isXML);
    }
    catch (std::exception& e) {
        displayException(e);
    }
}

void Tkenv::newNetwork(const char *networkname)
{
    try {
        animating = false;  // affects how network graphics is drawn by refreshInspectors()

        // finish & cleanup previous run if we haven't done so yet
        if (simulationState != SIM_NONET) {
            if (simulationState != SIM_FINISHCALLED)
                endRun();
            getSimulation()->deleteNetwork();
            simulationState = SIM_NONET;
        }

        cModuleType *network = resolveNetwork(networkname);
        ASSERT(network);

        // set up new network with config General.
        isConfigRun = false;
        getConfigEx()->activateConfig("General", 0);
        readPerRunOptions();
        opt->networkName = network->getName();  // override config setting

        setupNetwork(network);
        startRun();
        simulationState = SIM_NEW;
        callRefreshDisplay();
    }
    catch (std::exception& e) {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
        simulationState = SIM_ERROR;
    }
    // update GUI
    animating = false;  // affects how network graphics is drawn!
    updateNetworkRunDisplay();
    updateStatusDisplay();
    refreshInspectors();
}

void Tkenv::newRun(const char *configname, int runnumber)
{
    try {
        animating = false;  // affects how network graphics is drawn by refreshInspectors()

        // finish & cleanup previous run if we haven't done so yet
        if (simulationState != SIM_NONET) {
            if (simulationState != SIM_FINISHCALLED)
                endRun();
            getSimulation()->deleteNetwork();
            simulationState = SIM_NONET;
        }

        // set up new network
        isConfigRun = true;
        getConfigEx()->activateConfig(configname, runnumber);
        readPerRunOptions();

        if (opt->networkName.empty()) {
            confirm(ERROR, "No network specified in the configuration.");
            return;
        }

        cModuleType *network = resolveNetwork(opt->networkName.c_str());
        ASSERT(network);

        setupNetwork(network);
        startRun();
        simulationState = SIM_NEW;
        callRefreshDisplay();
    }
    catch (std::exception& e) {
        notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
        displayException(e);
        simulationState = SIM_ERROR;
    }
    // update GUI
    animating = false;  // affects how network graphics is drawn!
    updateNetworkRunDisplay();
    updateStatusDisplay();
    refreshInspectors();
}

void Tkenv::setupNetwork(cModuleType *network)
{
    answers.clear();
    logBuffer.clear();
    componentHistory.clear();

    EnvirBase::setupNetwork(network);

    mainNetworkView->doSetObject(getSimulation()->getSystemModule());
    mainLogView->doSetObject(getSimulation()->getSystemModule());
    mainInspector->doSetObject(getSimulation()->getSystemModule());
}

Inspector *Tkenv::inspect(cObject *obj, int type, bool ignoreEmbedded, const char *geometry)
{
    // first, try finding and displaying existing inspector
    Inspector *inspector = findFirstInspector(obj, type, ignoreEmbedded);
    if (inspector) {
        inspector->showWindow();
        return inspector;
    }

    InspectorFactory *factory = findInspectorFactoryFor(obj, type);
    if (!factory) {
        confirm(ERROR, opp_stringf("Class '%s' has no associated inspectors.", obj->getClassName()).c_str());
        return nullptr;
    }

    int actualType = factory->getInspectorType();
    inspector = findFirstInspector(obj, actualType, ignoreEmbedded);
    if (inspector) {
        inspector->showWindow();
        return inspector;
    }

    // create inspector
    inspector = factory->createInspector();
    if (!inspector) {
        confirm(ERROR, opp_stringf("Class '%s' has no '%s' inspector.", obj->getClassName(), insptypeNameFromCode(type)).c_str());
        return nullptr;
    }

    // everything ok, finish inspector
    inspectors.push_back(inspector);
    inspector->createWindow(Inspector::makeWindowName().c_str(), geometry);
    inspector->setObject(obj);

    return inspector;
}

void Tkenv::addEmbeddedInspector(const char *widget, Inspector *insp)
{
    inspectors.push_back(insp);
    insp->useWindow(widget);
    insp->refresh();
}

Inspector *Tkenv::findFirstInspector(const cObject *obj, int type, bool ignoreEmbedded)
{
    for (auto insp : inspectors) {
        if (insp->getObject() == obj && insp->getType() == type && (!ignoreEmbedded || insp->isToplevel()))
            return insp;
    }
    return nullptr;
}

Inspector *Tkenv::findInspector(const char *widget)
{
    for (auto insp : inspectors) {
        if (strcmp(insp->getWindowName(), widget) == 0)
            return insp;
    }
    return nullptr;
}

void Tkenv::deleteInspector(Inspector *insp)
{
    inspectors.remove(insp);
    delete insp;
}

void Tkenv::callRefreshDisplay()
{
    ASSERT(simulationState == SIM_NEW || simulationState == SIM_READY || simulationState == SIM_RUNNING || simulationState == SIM_TERMINATED);
    getSimulation()->getSystemModule()->callRefreshDisplay();  // Beware: this may throw a cRuntimeError, so needs to be under a try/catch
}

void Tkenv::refreshInspectors()
{
    // update inspectors
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ) {
        Inspector *insp = *it;
        InspectorList::iterator next = ++it;
        if (insp->isMarkedForDeletion())
            deleteInspector(insp);
        else
            insp->refresh();
        it = next;
    }

    // clear the change flags on all inspected canvases
    for (auto & inspector : inspectors)
        inspector->clearObjectChangeFlags();

    // update object tree
    CHK(Tcl_VarEval(interp, "treeManager:update", TCL_NULL));

    // try opening "pending" inspectors
    CHK(Tcl_VarEval(interp, "inspectorUpdateCallback", TCL_NULL));
}

void Tkenv::redrawInspectors()
{
    // update inspectors (and close the ones marked for deletion)
    refreshInspectors();

    // redraw them
    for (auto & inspector : inspectors)
        inspector->redraw();
}

inline LogInspector *isLogInspectorFor(cModule *mod, Inspector *insp)
{
    if (insp->getObject() != mod || insp->getType() != INSP_MODULEOUTPUT)
        return nullptr;
    return dynamic_cast<LogInspector *>(insp);
}

inline ModuleInspector *isModuleInspectorFor(cModule *mod, Inspector *insp)
{
    if (insp->getObject() != mod || insp->getType() != INSP_GRAPHICAL)
        return nullptr;
    return dynamic_cast<ModuleInspector *>(insp);
}

void Tkenv::createSnapshot(const char *label)
{
    getSimulation()->snapshot(getSimulation(), label);
}

void Tkenv::updateGraphicalInspectorsBeforeAnimation()
{
    for (auto insp : inspectors) {
        if (dynamic_cast<ModuleInspector *>(insp) && static_cast<ModuleInspector *>(insp)->needsRedraw()) {
            insp->refresh();
        }
    }
}

std::string Tkenv::getWindowTitle()
{
    const char *configName = getConfigEx()->getActiveConfigName();
    int runNumber = getConfigEx()->getActiveRunNumber();
    const char *inifile = getConfigEx()->getFileName();

    std::stringstream os;
    os << OMNETPP_PRODUCT "/Tkenv - " << getWindowTitlePrefix();
    if (opp_isempty(configName))
        os << "No network";
    else
        os << configName << " #" << runNumber;
    if (!opp_isempty(inifile))
        os << " - " << inifile;
    os << " - " << getWorkingDir();
    return os.str();
}

void Tkenv::updateNetworkRunDisplay()
{
    CHK(Tcl_VarEval(interp, "mainWindow:updateNetworkRunDisplay", TCL_NULL));
    CHK(Tcl_VarEval(interp, "wm title . ", TclQuotedString(getWindowTitle().c_str()).get(), TCL_NULL));
}

void Tkenv::updateSimtimeDisplay()
{
    CHK(Tcl_VarEval(interp, "mainWindow:updateSimtimeDisplay", TCL_NULL));
}

void Tkenv::updateStatusDisplay()
{
    CHK(Tcl_VarEval(interp, "mainWindow:updateStatusDisplay", TCL_NULL));
    CHK(Tcl_Eval(interp, "redrawTimeline"));
}

void Tkenv::printEventBanner(cEvent *event)
{
    cObject *target = event->getTargetObject();
    cMessage *msg = event->isMessage() ? static_cast<cMessage *>(event) : nullptr;
    cModule *module = msg ? msg->getArrivalModule() : nullptr;

    // produce banner text
    char banner[2*MAX_OBJECTFULLPATH+2*MAX_CLASSNAME+60];
    char *p = banner;
    p += sprintf(p, "** Event #%" PRId64 "  t=%s  ",
                getSimulation()->getEventNumber(),
                SIMTIME_STR(getSimulation()->getSimTime()));

    if (opt->shortBanners) {
        // just object names
        if (target)
            p += sprintf(p, "%s ", target->getFullPath().c_str());
        p += sprintf(p, "on %s", event->getFullName());
    }
    else {
        // print event and module type names and IDs, too
        if (module)
            p += sprintf(p, "%s (%s, id=%d) ",
                        module->getFullPath().c_str(),
                        module->getComponentType()->getName(),
                        module->getId());
        else if (target)
            p += sprintf(p, "%s (%s) ",
                        target->getFullPath().c_str(),
                        target->getClassName());
        if (msg)
            p += sprintf(p, " on %s%s (%s, id=%ld)",
                        msg->isSelfMessage() ? "selfmsg " : "",
                        msg->getFullName(),
                        msg->getClassName(),
                        msg->getId());
        else
            p += sprintf(p, " on %s (%s)",
                        event->getFullName(),
                        event->getClassName());
    }
    strcpy(p, "\n");

    // insert into log buffer
    logBuffer.addEvent(getSimulation()->getEventNumber(), getSimulation()->getSimTime(), module, banner);
}

void Tkenv::displayException(std::exception& ex)
{
    // print exception text into main window
    cException *e = dynamic_cast<cException *>(&ex);
    if (e && e->getSimulationStage() != CTX_NONE) {
        std::string txt = opp_stringf("<!> %s\n", e->getFormattedMessage().c_str());
        logBuffer.addInfo(txt.c_str());
    }

    // pop up dialog
    confirm(ERROR, getFormattedMessage(ex).c_str());
}

void Tkenv::componentInitBegin(cComponent *component, int stage)
{
    if (!opt->printInitBanners || runMode == RUNMODE_EXPRESS)
        return;

    // produce banner text
    char banner[MAX_OBJECTFULLPATH+60];
    sprintf(banner, "Initializing %s %s, stage %d\n",
            component->isModule() ? "module" : "channel", component->getFullPath().c_str(), stage);

    // insert into log buffer
    logBuffer.addInitialize(component, banner);
}

void Tkenv::setSilentEventFilters(const char *filterLines)
{
    // parse into tmp
    MatchExpressions tmp;
    try {
        StringTokenizer tokenizer(filterLines, "\n");
        while (tokenizer.hasMoreTokens()) {
            const char *line = tokenizer.nextToken();
            if (!opp_isblank(line)) {
                tmp.push_back(new MatchExpression());
                tmp.back()->setPattern(line, false, true, true);
            }
        }
    }
    catch (std::exception& e) {  // parse error
        for (auto & matchExpr : tmp)
            delete matchExpr;
        throw;
    }
    // parsing successful, store the result
    for (auto & silentEventFilter : silentEventFilters)
        delete silentEventFilter;
    silentEventFilterLines = opp_trim(filterLines) + "\n";
    silentEventFilters = tmp;
}

bool Tkenv::isSilentEvent(cMessage *msg)
{
    MatchableObjectAdapter wrappedMsg(MatchableObjectAdapter::FULLNAME, msg);
    for (auto & silentEventFilter : silentEventFilters)
        if (silentEventFilter->matches(&wrappedMsg))
            return true;

    return false;
}

//=========================================================================

void Tkenv::readOptions()
{
    EnvirBase::readOptions();

    cConfiguration *cfg = getConfig();

    opt->extraStack = (size_t)cfg->getAsDouble(CFGID_TKENV_EXTRA_STACK);

    const char *s = args->optionValue('c');
    opt->defaultConfig = s ? s : cfg->getAsString(CFGID_TKENV_DEFAULT_CONFIG);

    const char *r = args->optionValue('r');
    opt->defaultRun = r ? atoi(r) : cfg->getAsInt(CFGID_TKENV_DEFAULT_RUN);

    opt->pluginPath = cfg->getAsPath(CFGID_TKENV_PLUGIN_PATH).c_str();
}

void Tkenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();
}

void Tkenv::askParameter(cPar *par, bool unassigned)
{
    // use a value entered by the user earlier ("[x] use this value for similar parameters")
    std::string key = std::string(((cComponent *)par->getOwner())->getNedTypeName()) + ":" + par->getName();
    if (answers.find(key) != answers.end()) {
        std::string answer = answers[key];
        par->parse(answer.c_str());
        return;
    }

    // really ask
    bool success = false;
    bool useForAll = false;
    while (!success) {
        cProperties *props = par->getProperties();
        cProperty *prop = props->get("prompt");
        std::string prompt = prop ? prop->getValue(cProperty::DEFAULTKEY) : "";
        if (prompt.empty())
            prompt = std::string("Enter parameter '") + par->getFullPath() + "':";

        std::string reply;
        std::string title = unassigned ? "Unassigned Parameter" : "Requested to Ask Parameter";
        bool ok = inputDialog(title.c_str(), prompt.c_str(),
                    "Use this value for all similar parameters",
                    par->str().c_str(), reply, useForAll);
        if (!ok)
            throw cRuntimeError(E_CANCEL);

        try {
            par->parse(reply.c_str());
            success = true;
            if (useForAll)
                answers[key] = reply;
        }
        catch (std::exception& e) {
            printfmsg("%s -- please try again.", e.what());
        }
    }
}

bool Tkenv::idle()
{
    // bug #56: refresh inspectors so that there aren't dead objects on the UI
    // while running Tk "update" (below). This only needs to be done in Fast
    // mode, because in normal Run mode inspectors are already up to date here
    // (they are refreshed after every event), and in Express mode all user
    // interactions are disabled except for the STOP button.
    if (runMode == RUNMODE_FAST) {
        // updateInspectors() may be costly, so do not check the UI too often
        int64_t now = opp_get_monotonic_clock_usecs();
        if ((now - idleLastUICheck) < 500 * 1000)
            return false;

        // refresh inspectors
        updateStatusDisplay();
        refreshInspectors();
    }

    // process UI events
    eState origsimstate = simulationState;
    simulationState = SIM_BUSY;
    Tcl_Eval(interp, "update");
    simulationState = origsimstate;

    bool stop = stopSimulationFlag;
    stopSimulationFlag = false;

    if (runMode == RUNMODE_FAST)
        idleLastUICheck = opp_get_monotonic_clock_usecs();
    return stop;
}

void Tkenv::objectDeleted(cObject *object)
{
    if (object == runUntil.msg) {
        // message to "run until" deleted -- stop the simulation by other means
        runUntil.msg = nullptr;
        runUntil.eventNumber = getSimulation()->getEventNumber();
        if (simulationState == SIM_RUNNING || simulationState == SIM_BUSY)
            confirm(INFO, "Message to run until has just been deleted.");
    }

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ) {
        InspectorList::iterator next = it;
        ++next;
        Inspector *insp = *it;

        insp->objectDeleted(object);

        if (insp->getObject() == object && insp->isToplevel()) {
            inspectors.erase(it);
            delete insp;
        }
        it = next;
    }
}

void Tkenv::simulationEvent(cEvent *event)
{
    EnvirBase::simulationEvent(event);

    if (loggingEnabled)
        printEventBanner(event);  // must be done here, because eventnum and simtime are updated inside executeEvent()

    if (animating && opt->animationEnabled) {
        updateSimtimeDisplay();  // so that the correct (new) simulation time is displayed during animation
        updateGraphicalInspectorsBeforeAnimation();
    }

    if (animating && opt->animationEnabled && event->isMessage()) {
        cMessage *msg = static_cast<cMessage *>(event);
        cGate *arrivalGate = msg->getArrivalGate();
        if (!arrivalGate)
            return;

        // if arrivalgate is connected, msg arrived on a connection, otherwise via sendDirect()
        if (arrivalGate->getPreviousGate()) {
            animateDelivery(msg);
        }
        else {
            animateDeliveryDirect(msg);
        }
    }
}

void Tkenv::messageScheduled(cMessage *msg)
{
    EnvirBase::messageScheduled(msg);
}

void Tkenv::messageCancelled(cMessage *msg)
{
    if (msg == runUntil.msg && opt->stopOnMsgCancel) {
        if (simulationState == SIM_RUNNING || simulationState == SIM_BUSY)
            confirm(INFO, opp_stringf("Run-until message '%s' got cancelled.", msg->getName()).c_str());
        runUntil.msg = nullptr;
        runUntil.eventNumber = getSimulation()->getEventNumber();  // stop the simulation using the event number limit
    }
    EnvirBase::messageCancelled(msg);
}

void Tkenv::beginSend(cMessage *msg)
{
    EnvirBase::beginSend(msg);

    if (animating && opt->animationEnabled && !isSilentEvent(msg)) {
        // actually this will draw `msg' too (which would cause "phantom message"),
        // but we'll manually remove it before animation
        updateGraphicalInspectorsBeforeAnimation();
    }

    if (loggingEnabled)
        logBuffer.beginSend(msg);
}

void Tkenv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);

    if (animating && opt->animationEnabled && !isSilentEvent(msg))
        animateSendDirect(msg, msg->getSenderModule(), toGate);

    if (loggingEnabled)
        logBuffer.messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void Tkenv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    EnvirBase::messageSendHop(msg, srcGate);

    if (animating && opt->animationEnabled && !isSilentEvent(msg)) {
        bool isLastHop = srcGate->getNextGate()==msg->getArrivalGate();
        animateSendHop(msg, srcGate, isLastHop);
    }

    if (loggingEnabled)
        logBuffer.messageSendHop(msg, srcGate);
}

void Tkenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay, bool discard)
{
    EnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay, discard);

    if (animating && opt->animationEnabled && !isSilentEvent(msg)) {
        bool isLastHop = srcGate->getNextGate()==msg->getArrivalGate();
        animateSendHop(msg, srcGate, isLastHop);
    }

    if (loggingEnabled)
        logBuffer.messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void Tkenv::endSend(cMessage *msg)
{
    EnvirBase::endSend(msg);

    if (loggingEnabled)
        logBuffer.endSend(msg);
}

void Tkenv::messageDeleted(cMessage *msg)
{
    EnvirBase::messageDeleted(msg);
}

void Tkenv::componentMethodBegin(cComponent *fromComp, cComponent *toComp, const char *methodFmt, va_list va, bool silent)
{
    va_list va2;
    va_copy(va2, va);  // see bug #107
    EnvirBase::componentMethodBegin(fromComp, toComp, methodFmt, va2, silent);
    va_end(va2);

    if (silent || !animating || !opt->animateMethodCalls)
        return;

    if (!methodFmt)
        return;  // Enter_Method_Silent

    if (!fromComp->isModule() || !toComp->isModule())
        return;  // calls to/from channels are not yet animated

    updateGraphicalInspectorsBeforeAnimation();

    static char methodText[MAX_METHODCALL];
    vsnprintf(methodText, MAX_METHODCALL, methodFmt, va);
    methodText[MAX_METHODCALL-1] = '\0';

    cModule *from = (cModule *)fromComp;
    cModule *to = (cModule *)toComp;

    // find modules along the way
    PathVec pathvec;
    findDirectPath(from, to, pathvec);

    PathVec::iterator i;
    int numinsp = 0;
    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        if (i->to == nullptr) {
            // animate ascent from source module
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();
            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp) {
                    numinsp++;
                    insp->animateMethodcallAscent(mod, methodText);
                }
            }
        }
        else if (i->from == nullptr) {
            // animate descent towards destination module
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp) {
                    numinsp++;
                    insp->animateMethodcallDescent(mod, methodText);
                }
            }
        }
        else {
            cModule *enclosingmod = i->from->getParentModule();
            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp) {
                    numinsp++;
                    insp->animateMethodcallHoriz(i->from, i->to, methodText);
                }
            }
        }
    }

    if (numinsp > 0) {
        // leave it there for a while
        ModuleInspector::animateMethodcallDelay(interp);

        // then remove all arrows
        for (i = pathvec.begin(); i != pathvec.end(); i++) {
            cModule *mod = i->from ? i->from : i->to;
            cModule *enclosingmod = mod->getParentModule();

            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp)
                    insp->animateMethodcallCleanup();
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

    cModule *mod = newmodule->getParentModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(mod, inspector);
        if (insp)
            insp->submoduleCreated(newmodule);
    }
}

void Tkenv::moduleDeleted(cModule *module)
{
    EnvirBase::moduleDeleted(module);

    componentHistory.componentDeleted(module);

    cModule *mod = module->getParentModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(mod, inspector);
        if (insp)
            insp->submoduleDeleted(module);
    }
}

void Tkenv::moduleReparented(cModule *module, cModule *oldParent, int oldId)
{
    EnvirBase::moduleReparented(module, oldParent, oldId);

    componentHistory.componentReparented(module, oldParent, oldId);

    // pretend it got deleted from under the 1st module, and got created under the 2nd
    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(oldParent, inspector);
        if (insp)
            insp->submoduleDeleted(module);
    }

    cModule *mod = module->getParentModule();
    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(mod, inspector);
        if (insp)
            insp->submoduleCreated(module);
    }
}

void Tkenv::connectionCreated(cGate *srcgate)
{
    EnvirBase::connectionCreated(srcgate);

    // notify compound module where the connection (whose source is this gate) is displayed
    cModule *notifymodule = nullptr;
    if (srcgate->getType() == cGate::OUTPUT)
        notifymodule = srcgate->getOwnerModule()->getParentModule();
    else
        notifymodule = srcgate->getOwnerModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(notifymodule, inspector);
        if (insp)
            insp->connectionCreated(srcgate);
    }
}

void Tkenv::connectionDeleted(cGate *srcgate)
{
    EnvirBase::connectionDeleted(srcgate);

    if (srcgate->getChannel())
        componentHistory.componentDeleted(srcgate->getChannel());

    // notify compound module where the connection (whose source is this gate) is displayed
    // note: almost the same code as above
    cModule *notifymodule;
    if (srcgate->getType() == cGate::OUTPUT)
        notifymodule = srcgate->getOwnerModule()->getParentModule();
    else
        notifymodule = srcgate->getOwnerModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(notifymodule, inspector);
        if (insp)
            insp->connectionDeleted(srcgate);
    }
}

void Tkenv::displayStringChanged(cComponent *component)
{
    EnvirBase::displayStringChanged(component);

    if (cModule *module = dynamic_cast<cModule *>(component))
        moduleDisplayStringChanged(module);
    else if (cChannel *channel = dynamic_cast<cChannel *>(component))
        channelDisplayStringChanged(channel);
}

void Tkenv::channelDisplayStringChanged(cChannel *channel)
{
    cGate *gate = channel->getSourceGate();

    // notify module inspector which displays connection
    cModule *notifymodule;
    if (gate->getType() == cGate::OUTPUT)
        notifymodule = gate->getOwnerModule()->getParentModule();
    else
        notifymodule = gate->getOwnerModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(notifymodule, inspector);
        if (insp)
            insp->displayStringChanged(gate);
    }

    // graphical gate inspector windows: normally a user doesn't have many such windows open
    // (typically, none at all), so we can afford simply refreshing all of them
    for (auto insp : inspectors) {
        GateInspector *gateinsp = dynamic_cast<GateInspector *>(insp);
        if (gateinsp)
            gateinsp->displayStringChanged(gate);
    }
}

void Tkenv::moduleDisplayStringChanged(cModule *module)
{
    // refresh inspector where this module is a submodule
    cModule *parentmodule = module->getParentModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(parentmodule, inspector);
        if (insp)
            insp->displayStringChanged(module);
    }

    // refresh inspector where this module is the parent (i.e. this is a
    // background display string change)
    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(module, inspector);
        if (insp)
            insp->displayStringChanged();
    }
}

void Tkenv::animateSendHop(cMessage *msg, cGate *srcGate, bool isLastHop)
{
    char msgptr[32];
    ptrToStr(msg, msgptr);

    cModule *mod = srcGate->getOwnerModule();
    if (srcGate->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();
    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(mod, inspector);
        if (insp)
            insp->animateSendOnConn(srcGate, msg, (isLastHop ? ANIM_BEGIN : ANIM_THROUGH));
    }
}

// helper for animateSendDirectHop() functions
static cModule *findSubmoduleTowards(cModule *parentmod, cModule *towardsgrandchild)
{
    if (parentmod == towardsgrandchild)
        return nullptr;  // shortcut -- we don't have to go up to the top to see we missed

    // search upwards from 'towardsgrandchild'
    cModule *m = towardsgrandchild;
    while (m && m->getParentModule() != parentmod)
        m = m->getParentModule();
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
    cModule *commonparent = findCommonAncestor(srcmod, destmod);
    Assert(commonparent != nullptr);  // commonparent should exist, worst case it's the system module

    // animate the ascent of the message until commonparent (excluding).
    // The second condition, destmod==commonparent covers case when we're sending
    // to an output gate of the parent (grandparent, etc) gate.
    cModule *mod = srcmod;
    while (mod != commonparent && (mod->getParentModule() != commonparent || destmod == commonparent)) {
        pathvec.push_back(sPathEntry(mod, nullptr));
        mod = mod->getParentModule();
    }

    // animate within commonparent
    if (commonparent != srcmod && commonparent != destmod) {
        cModule *from = findSubmoduleTowards(commonparent, srcmod);
        cModule *to = findSubmoduleTowards(commonparent, destmod);
        pathvec.push_back(sPathEntry(from, to));
    }

    // descend from commonparent to destmod
    mod = findSubmoduleTowards(commonparent, destmod);
    if (mod && srcmod != commonparent)
        mod = findSubmoduleTowards(mod, destmod);
    while (mod) {
        // animate descent towards destmod
        pathvec.push_back(sPathEntry(nullptr, mod));
        // find module 'under' mod, towards destmod (this will return nullptr if mod==destmod)
        mod = findSubmoduleTowards(mod, destmod);
    }
}

void Tkenv::animateSendDirect(cMessage *msg, cModule *srcModule, cGate *destGate)
{
    PathVec pathvec;
    findDirectPath(srcModule, destGate->getOwnerModule(), pathvec);

    // for checking whether the sendDirect target module is also the final destination of the msg
    cModule *arrivalmod = destGate->getNextGate()==nullptr ? destGate->getOwnerModule() : nullptr;

    PathVec::iterator i;
    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        if (i->to == nullptr) {
            // ascent
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();
            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp)
                    insp->animateSenddirectAscent(mod, msg);
            }
        }
        else if (i->from == nullptr) {
            // descent
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            bool isArrival = (mod == arrivalmod);
            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp)
                    insp->animateSenddirectDescent(mod, msg, isArrival ? ANIM_BEGIN : ANIM_THROUGH);
            }
        }
        else {
            // level
            cModule *enclosingmod = i->from->getParentModule();
            bool isArrival = (i->to == arrivalmod);
            for (auto & inspector : inspectors) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
                if (insp)
                    insp->animateSenddirectHoriz(i->from, i->to, msg, isArrival ? ANIM_BEGIN : ANIM_THROUGH);
            }
        }
    }

    // then remove all arrows
    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        cModule *mod = i->from ? i->from : i->to;
        cModule *enclosingmod = mod->getParentModule();

        for (auto & inspector : inspectors) {
            ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
            if (insp)
                insp->animateSenddirectCleanup();
        }
    }
}

void Tkenv::animateDelivery(cMessage *msg)
{
    char msgptr[32];
    ptrToStr(msg, msgptr);

    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    g = g->getPreviousGate();
    ASSERT(g);

    cModule *mod = g->getOwnerModule();
    if (g->getType() == cGate::OUTPUT)
        mod = mod->getParentModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(mod, inspector);
        if (insp)
            insp->animateSendOnConn(g, msg, ANIM_END);
    }
}

void Tkenv::animateDeliveryDirect(cMessage *msg)
{
    char msgptr[32];
    ptrToStr(msg, msgptr);

    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    for (auto & inspector : inspectors) {
        ModuleInspector *insp = isModuleInspectorFor(mod, inspector);
        if (insp)
            insp->animateSenddirectDelivery(destmod, msg);
    }
}

void Tkenv::performAnimations()
{
    ModuleInspector::performAnimations(interp);
}

void Tkenv::bubble(cComponent *component, const char *text)
{
    EnvirBase::bubble(component, text);

    if (!loggingEnabled)
        return;

    if (!opt->showBubbles)
        return;

    if (component->getParentModule()) {
        cModule *enclosingmod = component->getParentModule();
        for (auto & inspector : inspectors) {
            ModuleInspector *insp = isModuleInspectorFor(enclosingmod, inspector);
            if (insp)
                insp->bubble(component, text);
        }
    }
}

void Tkenv::confirm(DialogKind kind, const char *msg)
{
    if (!interp) {
        // fallback in case Tkenv didn't fire up correctly
        const char *prefix = kind==ERROR ? "Error: " : kind==WARNING ? "Warning: " : "";
        out << "\n<!> " << prefix << msg << endl << endl;
    }
    else {
        const char *icon = kind==INFO ? "info" : kind==WARNING ? "warning" : "error";
        CHK(Tcl_VarEval(interp, "messagebox {Confirm} ", TclQuotedString(msg).get(), " ", icon, " ok", TCL_NULL));
    }
}

void Tkenv::alert(const char *msg)
{
    confirm(WARNING, msg);
}

void Tkenv::log(cLogEntry *entry)
{
    EnvirBase::log(entry);

    if (!loggingEnabled)
        return;

    std::string prefix = logFormatter.formatPrefix(entry);
    const char *s = entry->text;
    int n = entry->textLength;

    if (!interp) {
        // fallback in case Tkenv didn't fire up correctly
        out << prefix;
        out.write(s, n);
        return;
    }

    // rough guard against forgotten "\n"'s in the code
    const int maxLen = 5000;
    if (n > maxLen) {
        const char *ellipsis = "... [line too long, truncated]\n";
        strcpy(const_cast<char *>(s) + maxLen - strlen(ellipsis), ellipsis);  // khmm...
        n = maxLen;
    }

    // insert into log buffer
    cModule *module = getSimulation()->getContextModule();
    if (module)
        logBuffer.addLogLine(prefix.c_str(), s, n);
    else
        logBuffer.addInfo(s, n);
}

bool Tkenv::inputDialog(const char *title, const char *prompt,
        const char *checkboxLabel, const char *defaultValue,
        std::string& outResult, bool& inoutCheckState)
{
    CHK(Tcl_Eval(interp, "global opp"));
    Tcl_SetVar2(interp, "opp", "result", (char *)defaultValue, TCL_GLOBAL_ONLY);
    Tcl_SetVar2(interp, "opp", "check", (char *)(inoutCheckState ? "1" : "0"), TCL_GLOBAL_ONLY);
    if (checkboxLabel == nullptr)
        CHK(Tcl_VarEval(interp, "inputbox ",
                        TclQuotedString(title).get(), " ",
                        TclQuotedString(prompt).get(), " opp(result) ", TCL_NULL));
    else
        CHK(Tcl_VarEval(interp, "inputbox ",
                        TclQuotedString(title).get(), " ",
                        TclQuotedString(prompt).get(), " opp(result) ",
                        TclQuotedString(checkboxLabel).get(), " opp(check)", TCL_NULL));

    if (Tcl_GetStringResult(interp)[0] == '0') {
        return false;  // cancel
    }
    else {
        outResult = Tcl_GetVar2(interp, "opp", "result", TCL_GLOBAL_ONLY);
        inoutCheckState = Tcl_GetVar2(interp, "opp", "check", TCL_GLOBAL_ONLY)[0] == '1';
        return true;  // OK
    }
}

std::string Tkenv::gets(const char *promt, const char *defaultReply)
{
    cModule *mod = getSimulation()->getContextModule();
    std::string title = mod ? mod->getFullPath() : getSimulation()->getNetworkType()->getName();
    std::string result;
    bool dummy;
    bool ok = inputDialog(title.c_str(), promt, nullptr, defaultReply, result, dummy);
    if (!ok)
        throw cRuntimeError(E_CANCEL);
    return result;
}

bool Tkenv::askYesNo(const char *question)
{
    // should return -1 when CANCEL is pressed
    CHK(Tcl_VarEval(interp, "messagebox {Tkenv} ", TclQuotedString(question).get(), " question yesno", TCL_NULL));
    return Tcl_GetStringResult(interp)[0] == 'y';
}

void Tkenv::getImageSize(const char *imageName, double& outWidth, double& outHeight)
{
    const char *image = Tcl_GetVar2(interp, "bitmaps", TCLCONST(imageName), TCL_GLOBAL_ONLY);
    if (!image)
        image = Tcl_GetVar2(interp, "icons", "unknown", TCL_GLOBAL_ONLY);
    ASSERT(image);

    CHK(Tcl_VarEval(interp, "image width ", image, TCL_NULL));
    outWidth = opp_atol(Tcl_GetStringResult(interp));

    CHK(Tcl_VarEval(interp, "image height ", image, TCL_NULL));
    outHeight = opp_atol(Tcl_GetStringResult(interp));
}

void Tkenv::getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent)
{
    if (!*text) {
        outWidth = outHeight = outAscent = 0;
        return;
    }

    Tcl_Eval(interp, "font actual CanvasFont -family");
    std::string defaultFont = Tcl_GetStringResult(interp);

    Tcl_Eval(interp, "font actual CanvasFont -size");
    int defaultSize = opp_atol(Tcl_GetStringResult(interp));

    int argc = 0;
    const char *argv[100];
    argv[argc++] = "getTextExtent";
    argv[argc++] = font.typeface.empty() ? defaultFont.c_str() : font.typeface.c_str();
    char buf[16]; // scaling correction for 96 DPI (1 point i 1/72 inch)
    argv[argc++] = opp_itoa(buf, (int)std::ceil((font.pointSize <= 0 ? defaultSize : (double)font.pointSize) * (16.0/12.0)));
    argv[argc++] = (font.style & cFigure::FONT_BOLD) ? "bold" : "normal";
    argv[argc++] = (font.style & cFigure::FONT_ITALIC) ? "italic" : "normal";
    argv[argc++] = text;

    Tcl_CmdInfo cmdInfo;
    int success = Tcl_GetCommandInfo(interp, TCLCONST("getTextExtent"), &cmdInfo);
    ASSERT(success == 1);

    invokeTclCommand(interp, &cmdInfo, argc, argv);

    const char *result = Tcl_GetStringResult(interp);
    double x1, y1, x2, y2, ascent;
    sscanf(result, "%lf %lf %lf %lf %lf", &x1, &y1, &x2, &y2, &ascent);
    outWidth = std::max(x2 - x1 - 2, 0.0); // the -2 is to remove the antialiasing fuzz.
    outHeight = std::max(y2 - y1 - 2, 0.0); // should also include the outline, but figures dont use it. YET. XXX
    outAscent = std::max(ascent, 0.0);
}

void Tkenv::appendToImagePath(const char *directory)
{
    if (!isDirectory(directory))
        throw cRuntimeError("appendToImagePath(): path '%s' not found or not a directory", directory);
    CHK(Tcl_VarEval(interp, "loadBitmaps ", TclQuotedString(directory).get(), TCL_NULL));
}

void Tkenv::loadImage(const char *fileName, const char *imageName)
{
    if (!isFile(fileName))
        throw cRuntimeError("loadImage(): path '%s' not found or not a regular file", fileName);
    CHK(Tcl_VarEval(interp, "loadImage ", TclQuotedString(fileName).get(), " {} ",
            TclQuotedString(opp_nulltoempty(imageName)).get(), TCL_NULL));
}

cFigure::Rectangle Tkenv::getSubmoduleBounds(const cModule *submodule)
{
    // an easy solution that works in most practical cases: take the coordinates
    // from the first open inspector that we find for this module
    if (cModule *enclosingModule = submodule->getParentModule()) {
        Inspector *insp = findFirstInspector(enclosingModule, INSP_GRAPHICAL, false);
        if (ModuleInspector *modinsp = dynamic_cast<ModuleInspector*>(insp)) {
            auto c = modinsp->getSubmodulePosition(submodule);
            bool expl, obeys;
            double x, y, sx, sy;
            // only using sx and sy from this!
            modinsp->getSubmoduleCoords(const_cast<cModule *>(submodule), expl, obeys, x, y, sx, sy);

            return cFigure::Rectangle(c.x - sx/2, c.y - sy/2, sx, sy);
        }
    }
    return cFigure::Rectangle(NAN, NAN, NAN, NAN);
}

double Tkenv::getZoomLevel(const cModule *module)
{
    // an easy solution that works in most practical cases: take the value
    // from the first open inspector that we find for this module
    Inspector *insp = findFirstInspector(static_cast<const cObject *>(module), INSP_GRAPHICAL, false);
    if (ModuleInspector *modinsp = dynamic_cast<ModuleInspector*>(insp))
        return modinsp->getZoomLevel();
    return NAN;
}

unsigned Tkenv::getExtraStackForEnvir() const
{
    return opt->extraStack;
}

void Tkenv::logTclError(const char *file, int line, Tcl_Interp *interp)
{
    logTclError(file, line, Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY));
}

void Tkenv::logTclError(const char *file, int line, const char *text)
{
    openTkenvlogIfNeeded();
    FILE *f = ferrorlog ? ferrorlog : stderr;
    ::fprintf(f, "Tcl error: %s#%d: %s\n\n\n", file, line, text);
    ::fflush(f);
}

void Tkenv::openTkenvlogIfNeeded()
{
    if (!ferrorlog) {
        ferrorlog = fopen(".tkenvlog", "a");
        if (!ferrorlog) {
            ::fprintf(stderr, "Tkenv: could not open .tkenvlog for append\n");
            return;
        }
        ::fprintf(ferrorlog, "---- %s ---------------------------------------------------------\n\n\n", opp_makedatetimestring().c_str());
    }
}

//======================================================================
// dummy function to force Unix linkers collect all symbols needed

void _dummy_for_genericobjectinspector();
void _dummy_for_watchinspector();
void _dummy_for_moduleinspector();
void _dummy_for_loginspector();
void _dummy_for_gateinspector();
void _dummy_for_histograminspector();
void _dummy_for_outputvectorinspector();

void _dummy_func()
{
    _dummy_for_genericobjectinspector();
    _dummy_for_watchinspector();
    _dummy_for_moduleinspector();
    _dummy_for_loginspector();
    _dummy_for_gateinspector();
    _dummy_for_histograminspector();
    _dummy_for_outputvectorinspector();
}

}  // namespace tkenv
}  // namespace omnetpp

