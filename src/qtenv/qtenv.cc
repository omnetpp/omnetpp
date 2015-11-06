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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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

#include <QApplication>
#include <QTreeView>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QFontDatabase>

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
#include "omnetpp/platdep/timeutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "qtenvdefs.h"
#include "qtenv.h"
#include "tklib.h"
#include "inspector.h"
#include "inspectorfactory.h"
#include "inspectorutil.h"
#include "moduleinspector.h"
#include "loginspector.h"
#include "gateinspector.h"
#include "genericobjectinspector.h"
#include "watchinspector.h"
#include "mainwindow.h"
#include "treeitemmodel.h"
#include "timelineinspector.h"
#include "objecttreeinspector.h"

#define emit

#define emit

// default plugin path -- allow overriding it via compiler option (-D)
// (default image path comes from makefile)
#ifndef OMNETPP_PLUGIN_PATH
#define OMNETPP_PLUGIN_PATH    "./plugins"
#endif

using namespace omnetpp::common;
using namespace omnetpp::envir;

namespace omnetpp {
namespace qtenv {
//
// Register the Qtenv user interface
//
Register_OmnetApp("Qtenv", Qtenv, 15, "Qt-based graphical user interface");  //note: priority to be changed to 30 when Qtenv becomes the default

//
// The following function can be used to force linking with Qtenv; specify
// -u _qtenv_lib (gcc) or /include:_qtenv_lib (vc++) in the link command.
//
extern "C" QTENV_API void qtenv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" QTENV_API void _qtenv_lib() {}

#define LL                             INT64_PRINTF_FORMAT

#define SPEEDOMETER_UPDATEMILLISECS    1000

Register_GlobalConfigOptionU(CFGID_TKENV_EXTRA_STACK, "tkenv-extra-stack", "B", "48KiB", "Specifies the extra amount of stack that is reserved for each activity() simple module when the simulation is run under Tkenv.");
Register_GlobalConfigOption(CFGID_TKENV_DEFAULT_CONFIG, "tkenv-default-config", CFG_STRING, nullptr, "Specifies which config Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigOption(CFGID_TKENV_DEFAULT_RUN, "tkenv-default-run", CFG_INT, "0", "Specifies which run (of the default config, see tkenv-default-config) Tkenv should set up automatically on startup. The default is to ask the user.");
Register_GlobalConfigOption(CFGID_TKENV_IMAGE_PATH, "tkenv-image-path", CFG_PATH, "", "Specifies the path for loading module icons.");
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

QtenvOptions::QtenvOptions()
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

void Qtenv::storeOptsInPrefs() {

    /* // I guess these are not needed anyways
    size_t extraStack;        // per-module extra stack for activity() modules
    opp_string imagePath;     // directory of module icon files

    // this one is sure not
    opp_string pluginPath;    // path for loading Tcl and binary plugins
    */

    setPref("default-configname", opt->defaultConfig.c_str());
    setPref("default-runnumber", opt->defaultRun);

    setPref("updatefreq_fast_ms", QVariant::fromValue<int>(opt->updateFreqFast));
    setPref("updatefreq_express_ms", QVariant::fromValue<int>(opt->updateFreqExpress));
    setPref("event_banners", opt->printEventBanners);
    setPref("init_banners", opt->printInitBanners);
    setPref("short_banners", opt->shortBanners);
    setPref("animation_enabled", opt->animationEnabled);
    setPref("nexteventmarkers", opt->showNextEventMarkers);
    setPref("senddirect_arrows", opt->showSendDirectArrows);
    setPref("anim_methodcalls", opt->animateMethodCalls);
    setPref("methodcalls_delay", opt->methodCallAnimDelay);
    setPref("animation_msgnames", opt->animationMsgNames);
    setPref("animation_msgclassnames", opt->animationMsgClassNames);
    setPref("animation_msgcolors", opt->animationMsgColors);
    setPref("silent_event_filters", getSilentEventFilters());
    setPref("penguin_mode", opt->penguinMode);
    setPref("showlayouting", opt->showLayouting);

    QString layouterChoiceString;
    switch (opt->layouterChoice) {
    case LAYOUTER_FAST:     layouterChoiceString = "fast";     break;
    case LAYOUTER_ADVANCED: layouterChoiceString = "advanced"; break;
    case LAYOUTER_AUTO:     layouterChoiceString = "auto";     break;
    }
    setPref("layouterchoice", layouterChoiceString);

    setPref("arrangevectorconnections", opt->arrangeVectorConnections);
    setPref("iconminsize", opt->iconMinimumSize);
    setPref("bubbles", opt->showBubbles);
    setPref("animation_speed", opt->animationSpeed);
    setPref("expressmode_autoupdate", opt->autoupdateInExpress);
    setPref("stoponmsgcancel", opt->stopOnMsgCancel);

    QString stripNamespaceString;
    switch (opt->stripNamespace) {
    case STRIPNAMESPACE_NONE:    stripNamespaceString = "none";    break;
    case STRIPNAMESPACE_OMNETPP: stripNamespaceString = "omnetpp"; break;
    case STRIPNAMESPACE_ALL:     stripNamespaceString = "all";     break;
    }
    setPref("stripnamespace", stripNamespaceString);

    setPref("logformat", opt->logFormat.c_str());

    QString logLevelString;
    switch (opt->logLevel) {
    case LOGLEVEL_FATAL:  logLevelString = "fatal";  break;
    case LOGLEVEL_ERROR:  logLevelString = "error";  break;
    case LOGLEVEL_WARN:   logLevelString = "warn";   break;
    case LOGLEVEL_INFO:   logLevelString = "info";   break;
    case LOGLEVEL_DETAIL: logLevelString = "detail"; break;
    case LOGLEVEL_DEBUG:  logLevelString = "debug";  break;
    case LOGLEVEL_TRACE:  logLevelString = "trace";  break;
    }
    setPref("loglevel", logLevelString);

    setPref("scrollbacklimit", opt->scrollbackLimit);
    setPref("logbuffer_maxnumevents", logBuffer.getMaxNumEntries());
}

void Qtenv::restoreOptsFromPrefs() {
    auto pref = getPref("default-configname");
    if (pref.isValid()) opt->defaultConfig = pref.toString().toStdString();

    pref = getPref("default-runnumber");
    if (pref.isValid()) opt->defaultRun = pref.toInt();

    pref = getPref("updatefreq_fast_ms");
    if (pref.isValid()) opt->updateFreqFast = pref.toLongLong();

    pref = getPref("updatefreq_express_ms");
    if (pref.isValid()) opt->updateFreqExpress = pref.toLongLong();

    pref = getPref("event_banners");
    if (pref.isValid()) opt->printEventBanners = pref.toBool();

    pref = getPref("init_banners");
    if (pref.isValid()) opt->printInitBanners = pref.toBool();

    pref = getPref("short_banners");
    if (pref.isValid()) opt->shortBanners = pref.toBool();

    pref = getPref("animation_enabled");
    if (pref.isValid()) opt->animationEnabled = pref.toBool();

    pref = getPref("nexteventmarkers");
    if (pref.isValid()) opt->showNextEventMarkers = pref.toBool();

    pref = getPref("senddirect_arrows");
    if (pref.isValid()) opt->showSendDirectArrows = pref.toBool();

    pref = getPref("anim_methodcalls");
    if (pref.isValid()) opt->animateMethodCalls = pref.toBool();

    pref = getPref("methodcalls_delay");
    if (pref.isValid()) opt->methodCallAnimDelay = pref.toInt();

    pref = getPref("animation_msgnames");
    if (pref.isValid()) opt->animationMsgNames = pref.toBool();

    pref = getPref("animation_msgclassnames");
    if (pref.isValid()) opt->animationMsgClassNames = pref.toBool();

    pref = getPref("animation_msgcolors");
    if (pref.isValid()) opt->animationMsgColors = pref.toBool();

    pref = getPref("silent_event_filters");
    if (pref.isValid()) setSilentEventFilters(pref.toString().toStdString().c_str());

    pref = getPref("penguin_mode");
    if (pref.isValid()) opt->penguinMode = pref.toBool();

    pref = getPref("showlayouting");
    if (pref.isValid()) opt->showLayouting = pref.toBool();

    pref = getPref("layouterchoice");
    if (pref.isValid()) {
        QString layouterChoiceString = pref.toString();
        if (layouterChoiceString == "fast") {
            opt->layouterChoice = LAYOUTER_FAST;
        } else if (layouterChoiceString == "advanced") {
            opt->layouterChoice = LAYOUTER_ADVANCED;
        } else if (layouterChoiceString == "auto") {
            opt->layouterChoice = LAYOUTER_AUTO;
        }
    }

    pref = getPref("arrangevectorconnections");
    if (pref.isValid()) opt->arrangeVectorConnections = pref.toBool();

    pref = getPref("iconminsize");
    if (pref.isValid()) opt->iconMinimumSize = pref.toInt();

    pref = getPref("bubbles");
    if (pref.isValid()) opt->showBubbles = pref.toBool();

    pref = getPref("animation_speed");
    if (pref.isValid()) setAnimationSpeed(pref.toDouble());

    pref = getPref("expressmode_autoupdate");
    if (pref.isValid()) opt->autoupdateInExpress = pref.toBool();

    pref = getPref("stoponmsgcancel");
    if (pref.isValid()) opt->stopOnMsgCancel = pref.toBool();

    pref = getPref("stripnamespace");
    if (pref.isValid()) {
        QString stripNamespaceString = pref.toString();
        if (stripNamespaceString == "none") {
            opt->stripNamespace = STRIPNAMESPACE_NONE;
        } else if (stripNamespaceString == "omnetpp") {
            opt->stripNamespace = STRIPNAMESPACE_OMNETPP;
        } else if (stripNamespaceString == "all") {
            opt->stripNamespace = STRIPNAMESPACE_ALL;
        }
    }

    pref = getPref("logformat");
    if (pref.isValid()) opt->logFormat = pref.toString().toStdString().c_str();

    pref = getPref("loglevel");
    if (pref.isValid()) {
        QString logLevelString = pref.toString();
        if (logLevelString == "fatal") {
            opt->logLevel = LOGLEVEL_FATAL;
        } else if (logLevelString == "error") {
            opt->logLevel = LOGLEVEL_ERROR;
        } else if (logLevelString == "warn") {
            opt->logLevel = LOGLEVEL_WARN;
        } else if (logLevelString == "info") {
            opt->logLevel = LOGLEVEL_INFO;
        } else if (logLevelString == "detail") {
            opt->logLevel = LOGLEVEL_DETAIL;
        } else if (logLevelString == "debug") {
            opt->logLevel = LOGLEVEL_DEBUG;
        } else if (logLevelString == "trace") {
            opt->logLevel = LOGLEVEL_TRACE;
        }
    }

    pref = getPref("scrollbacklimit");
    if (pref.isValid()) opt->scrollbackLimit = pref.toInt();

    pref = getPref("logbuffer_maxnumevents");
    if (pref.isValid()) logBuffer.setMaxNumEntries(pref.toInt());
}

Qtenv::Qtenv() : opt((QtenvOptions *&)EnvirBase::opt)
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    //TCLKILL interp = nullptr;  // Tcl/Tk not set up yet
    ferrorlog = nullptr;
    simulationState = SIM_NONET;
    stopSimulationFlag = false;
    animating = false;
    isConfigRun = false;
    rununtil_msg = nullptr;  // deactivate corresponding checks in eventCancelled()/objectDeleted()
    gettimeofday(&idleLastUICheck, nullptr);

    // set the name here, to prevent warning from cStringPool on shutdown when Cmdenv runs
    inspectorfactories.getInstance()->setName("inspectorfactories");

    localPrefKeys.insert("default-configname");
    localPrefKeys.insert("default-runnumber");

    animator = new Animator();
}

Qtenv::~Qtenv()
{
    for (int i = 0; i < (int)silentEventFilters.size(); i++)
        delete silentEventFilters[i];
    delete animator;
}

static void signalHandler(int signum)
{
    cStaticFlag::setExiting();
    exit(2);
}

void Qtenv::doRun()
{
    //
    // SETUP
    //
    try {
        // set signal handler
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // path for icon directories
        const char *image_path_env = getenv("OMNETPP_IMAGE_PATH");
        if (image_path_env == nullptr && getenv("OMNETPP_BITMAP_PATH") != nullptr)
            fprintf(stderr, "\n<!> WARNING: Obsolete environment variable OMNETPP_BITMAP_PATH found -- "
                            "please change it to OMNETPP_IMAGE_PATH for " OMNETPP_PRODUCT " 4.0\n");
        std::string image_path = opp_isempty(image_path_env) ? OMNETPP_IMAGE_PATH : image_path_env;
        // strip away the /; sequence from the beginning (a workaround for MinGW path conversion). See #785
        if (image_path.find("/;") == 0)
            image_path.erase(0, 2);

        if (!opt->imagePath.empty())
            image_path = std::string(opt->imagePath.c_str()) + ";" + image_path;

        // path for plugins
        const char *plugin_path_env = getenv("OMNETPP_PLUGIN_PATH");
        std::string plugin_path = plugin_path_env ? plugin_path_env : OMNETPP_PLUGIN_PATH;
        if (!opt->pluginPath.empty())
            plugin_path = std::string(opt->pluginPath.c_str()) + ";" + plugin_path;

        icons.loadImages(image_path.c_str());

        // we need to flush streams, otherwise output written from Tcl tends to overtake
        // output written from C++ so far, at least in the IDE's console view
        fflush(stdout);
        fflush(stderr);

        // these three have to be available for the whole lifetime of the application
        static int argc = 1;
        static char arg[] = { 'a', 'p', 'p', 0 };
        static char *argv[] = { arg, nullptr };
        app = new QApplication(argc, argv);

        globalPrefs = new QSettings(QDir::homePath() + "/.qtenv.ini", QSettings::IniFormat);
        localPrefs = new QSettings(".qtenv.ini", QSettings::IniFormat);

        restoreOptsFromPrefs();

        // create windowtitle prefix
        if (getParsimNumPartitions() > 0) {
            windowTitlePrefix.reserve(24);
            sprintf(windowTitlePrefix.buffer(), "Proc %d/%d - ", getParsimProcId(), getParsimNumPartitions());
        }

        mainWindow = new MainWindow(this);
        mainWindow->show();

        initFonts();

        mainWindow->setFont(boldFont);

        mainInspector = static_cast<GenericObjectInspector *>(addEmbeddedInspector(InspectorFactory::get("GenericObjectInspectorFactory"), mainWindow->getObjectInspectorArea()));
        mainNetworkView = static_cast<ModuleInspector *>(addEmbeddedInspector(InspectorFactory::get("ModuleInspectorFactory"), mainWindow->getMainInspectorArea()));
        mainLogView = static_cast<LogInspector *>(addEmbeddedInspector(InspectorFactory::get("LogInspectorFactory"), mainWindow->getLogInspectorArea()));
        mainTimeLine = static_cast<TimeLineInspector *>(addEmbeddedInspector(InspectorFactory::get("TimeLineInspectorFactory"), mainWindow->getTimeLineArea()));
        mainObjectTree = static_cast<ObjectTreeInspector *>(addEmbeddedInspector(InspectorFactory::get("ObjectTreeInspectorFactory"), mainWindow->getObjectTreeArea()));

        mainWindow->restoreGeometry();

        connect(mainNetworkView, SIGNAL(inspectedObjectChanged(cObject*)), mainLogView, SLOT(setObject(cObject*)));

        setLogFormat(opt->logFormat.c_str());

        //
        // RUN
        //
        QApplication::exec();
    }
    catch (std::exception& e) {
        //TCLKILL interp = nullptr;
        throw;
    }
    //
    // SHUTDOWN
    //

    // close all inspectors before exiting
    for ( ; ; ) {
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

    mainWindow->storeGeometry();
    saveFonts();

    delete mainWindow;
    mainWindow = nullptr;

    storeOptsInPrefs();

    delete globalPrefs;
    globalPrefs = nullptr;
    delete localPrefs;
    localPrefs = nullptr;

    delete app;
    app = nullptr;
}

void Qtenv::printUISpecificHelp()
{
    std::cout << "Tkenv-specific options:\n";
    std::cout << "  -c <configname>\n";
    std::cout << "                Select a given configuration for execution. With inifile-based\n";
    std::cout << "                configuration database, this selects the [Config <configname>]\n";
    std::cout << "                section; the default is the [General] section.\n";
    std::cout << "                See also: -r.\n";
    std::cout << "  -r <run>      Set up the specified run number in the configuration selected with\n";
    std::cout << "                the -c option\n";
}

void Qtenv::rebuildSim()
{
    if (isConfigRun)
        newRun(std::string(getConfigEx()->getActiveConfigName()).c_str(), getConfigEx()->getActiveRunNumber());
    else if (getSimulation()->getNetworkType() != nullptr)
        newNetwork(getSimulation()->getNetworkType()->getName());
    else
        confirm("Choose File|New Network or File|New Run.");
}

void Qtenv::doOneStep()
{
    ASSERT(simulationState == SIM_NEW || simulationState == SIM_READY);

    animating = true;
    rununtil_msg = nullptr;  // deactivate corresponding checks in eventCancelled()/objectDeleted()
    simulationState = SIM_RUNNING;

    updateStatusDisplay();

    startClock();
    notifyLifecycleListeners(LF_ON_SIMULATION_RESUME);
    try {
        cEvent *event = getSimulation()->takeNextEvent();
        if (event) {  // takeNextEvent() not interrupted
            printEventBanner(event);
            getSimulation()->executeEvent(event);
            performAnimations();
        }
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

void Qtenv::runSimulation(int mode, simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg, cModule *until_module)
{
    ASSERT(simulationState == SIM_NEW || simulationState == SIM_READY);

    runMode = mode;
    rununtil_time = until_time;
    rununtil_eventnum = until_eventnum;
    rununtil_msg = until_msg;
    rununtil_module = until_module;  // Note: this is NOT supported with RUNMODE_EXPRESS

    stopSimulationFlag = false;
    simulationState = SIM_RUNNING;

    updateStatusDisplay();
    QCoreApplication::processEvents();

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
    disableTracing = false;
    recordEventlog = false;
    rununtil_msg = nullptr;

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

void Qtenv::setSimulationRunMode(int mode)
{
    // This function (and the next one too) is called while runSimulation() is
    // underway, from Tcl code that gets a chance to run via Tcl_Eval(interp, "update") commands
    runMode = mode;
}

void Qtenv::setSimulationRunUntil(simtime_t until_time, eventnumber_t until_eventnum, cMessage *until_msg)
{
    rununtil_time = until_time;
    rununtil_eventnum = until_eventnum;
    rununtil_msg = until_msg;
}

void Qtenv::setSimulationRunUntilModule(cModule *until_module)
{
    rununtil_module = until_module;
}

// note: also updates "since" (sets it to the current time) if answer is "true"
inline bool elapsed(long millis, struct timeval& since)
{
    struct timeval now;
    gettimeofday(&now, nullptr);
    bool ret = timeval_diff_usec(now, since) > 1000*millis;
    if (ret)
        since = now;
    return ret;
}

inline void resetElapsedTime(struct timeval& t)
{
    gettimeofday(&t, nullptr);
}

bool Qtenv::doRunSimulation()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_eventnum, rununtil_msg, rununtil_module;
    //  - stopsimulation_flag
    //
    speedometer.start(getSimulation()->getSimTime());
    disableTracing = false;
    bool firstevent = true;

    struct timeval last_update;
    gettimeofday(&last_update, nullptr);

    while (true) {
        if (runMode == RUNMODE_EXPRESS)
            return true;  // should continue, but in a different mode

        // query which module will execute the next event
        cEvent *event = getSimulation()->takeNextEvent();
        if (!event)
            break;  // takeNextEvent() interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (rununtil_msg && event == rununtil_msg) {
            getSimulation()->putBackEvent(event);
            break;
        }

        // if stepping locally in module, we stop both immediately
        // *before* and *after* executing the event in that module,
        // but we always execute at least one event
        cModule *mod = event->isMessage() ? static_cast<cMessage *>(event)->getArrivalModule() : nullptr;
        bool untilmodule_reached = rununtil_module && moduleContains(rununtil_module, mod);
        if (untilmodule_reached && !firstevent) {
            getSimulation()->putBackEvent(event);
            break;
        }
        firstevent = false;

        animating = (runMode == RUNMODE_NORMAL) || untilmodule_reached;
        bool frequent_updates = (runMode == RUNMODE_NORMAL);

        speedometer.addEvent(getSimulation()->getSimTime());

        // do a simulation step
        printEventBanner(event);
        getSimulation()->executeEvent(event);
        performAnimations();

        // flush so that output from different modules don't get mixed
        cLogProxy::flushLastLine();

        // display update
        if (frequent_updates || ((getSimulation()->getEventNumber()&0x0f) == 0 && elapsed(opt->updateFreqFast, last_update))) {
            updateStatusDisplay();
            refreshInspectors();
            if (speedometer.getMillisSinceIntervalStart() > SPEEDOMETER_UPDATEMILLISECS)
                speedometer.beginNewInterval();
            QCoreApplication::processEvents();
            resetElapsedTime(last_update);  // exclude UI update time [bug #52]
        }

        // exit conditions
        if (untilmodule_reached)
            break;
        if (stopSimulationFlag)
            break;
        if (rununtil_time > SIMTIME_ZERO && getSimulation()->guessNextSimtime() >= rununtil_time)
            break;
        if (rununtil_eventnum > 0 && getSimulation()->getEventNumber() >= rununtil_eventnum)
            break;

        checkTimeLimits();
    }
    return false;
}

bool Qtenv::doRunSimulationExpress()
{
    //
    // IMPORTANT:
    // The following variables may change during execution (as a result of user interaction
    // during Tcl_Eval("update"):
    //  - runmode, rununtil_time, rununtil_eventnum, rununtil_msg, rununtil_module;
    //  - stopsimulation_flag
    //  - opt->expressmode_autoupdate
    //
    // EXPRESS does not support rununtil_module!
    //

    char info[128];
    sprintf(info, "** Running in Express mode from event #%" LL "d  t=%s ...\n",
            getSimulation()->getEventNumber(), SIMTIME_STR(getSimulation()->getSimTime()));
    logBuffer.addInfo(info);

    // update, just to get the above notice displayed
    QCoreApplication::processEvents();

    // OK, let's begin
    speedometer.start(getSimulation()->getSimTime());
    disableTracing = true;
    animating = false;

    struct timeval last_update;
    gettimeofday(&last_update, nullptr);

    bool result = false;
    do {
        cEvent *event = getSimulation()->takeNextEvent();
        if (!event)
            break;  // takeNextEvent() interrupted (parsim)

        // "run until message": stop if desired event was reached
        if (rununtil_msg && event == rununtil_msg) {
            getSimulation()->putBackEvent(event);
            break;
        }

        speedometer.addEvent(getSimulation()->getSimTime());

        getSimulation()->executeEvent(event);

        if ((getSimulation()->getEventNumber()&0xff) == 0 && elapsed(opt->updateFreqExpress, last_update)) {
            updateStatusDisplay();
            if (opt->autoupdateInExpress)
                refreshInspectors();
            if (speedometer.getMillisSinceIntervalStart() > SPEEDOMETER_UPDATEMILLISECS)
                speedometer.beginNewInterval();
            // Qt: Tcl_Eval(interp, "update");
            QCoreApplication::processEvents();
            resetElapsedTime(last_update);  // exclude UI update time [bug #52]
            if (runMode != RUNMODE_EXPRESS) {
                result = true;  // should continue, but in a different mode
                break;
            }
        }
        checkTimeLimits();
    } while (!stopSimulationFlag &&
             (rununtil_time <= SIMTIME_ZERO || getSimulation()->guessNextSimtime() < rununtil_time) &&
             (rununtil_eventnum <= 0 || getSimulation()->getEventNumber() < rununtil_eventnum)
             );

    sprintf(info, "** Leaving Express mode at event #%" LL "d  t=%s\n",
            getSimulation()->getEventNumber(), SIMTIME_STR(getSimulation()->getSimTime()));
    logBuffer.addInfo(info);

    return result;
}

void Qtenv::startAll()
{
    confirm("Not implemented.");
}

void Qtenv::finishSimulation()
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

void Qtenv::loadNedFile(const char *fname, const char *expectedPackage, bool isXML)
{
    try {
        getSimulation()->loadNedFile(fname, expectedPackage, isXML);
    }
    catch (std::exception& e) {
        displayException(e);
    }
}

void Qtenv::newNetwork(const char *networkname)
{
    try {
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

void Qtenv::newRun(const char *configname, int runnumber)
{
    try {
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
            confirm("No network specified in the configuration.");
            return;
        }

        cModuleType *network = resolveNetwork(opt->networkName.c_str());
        ASSERT(network);

        setupNetwork(network);
        startRun();

        simulationState = SIM_NEW;
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

void Qtenv::setupNetwork(cModuleType *network)
{
    answers.clear();
    logBuffer.clear();

    EnvirBase::setupNetwork(network);

    mainNetworkView->doSetObject(getSimulation()->getSystemModule());
    mainLogView->doSetObject(getSimulation()->getSystemModule());
    mainInspector->doSetObject(getSimulation()->getSystemModule());

    // collapsing all nodes in the object tree, because even if a new network is
    // loaded, there is a chance that some objects will be on the same place
    // (have the same pointer) as some of the old ones, so random nodes may
    // be expanded in the new tree depending on what was expanded before
    // TODO this should be done in the tree view inspector
    // mainwindow->getObjectTree()->collapseAll();
}

Inspector *Qtenv::inspect(cObject *obj, int type, bool ignoreEmbedded)
{
    // first, try finding and displaying existing inspector
    Inspector *inspector = findFirstInspector(obj, type, ignoreEmbedded);
    if (inspector) {
        inspector->showWindow();
        return inspector;
    }

    InspectorFactory *factory = findInspectorFactoryFor(obj, type);
    if (!factory) {
        confirm(opp_stringf("Class `%s' has no associated inspectors.", obj->getClassName()).c_str());
        return nullptr;
    }

    int actualType = factory->getInspectorType();
    inspector = findFirstInspector(obj, actualType, ignoreEmbedded);
    if (inspector) {
        inspector->showWindow();
        return inspector;
    }

    // create inspector
    inspector = factory->createInspector(mainWindow, true);
    if (!inspector) {
        // message: object has no such inspector
        confirm(opp_stringf("Class `%s' has no `%s' inspector.", obj->getClassName(), insptypeNameFromCode(type)).c_str());
        return nullptr;
    }

    // everything ok, finish inspector
    inspectors.push_back(inspector);
    // TODO geometry
    // insp->createWindow(Inspector::makeWindowName().c_str(), geometry);
    inspector->setObject(obj);

    return inspector;
}

Inspector *Qtenv::addEmbeddedInspector(InspectorFactory *factory, QWidget *parent)
{
    Inspector *insp = factory->createInspector(parent, false);
    inspectors.push_back(insp);

    connect(insp, SIGNAL(selectionChanged(cObject*)), this, SLOT(onSelectionChanged(cObject*)));
    connect(insp, SIGNAL(objectDoubleClicked(cObject*)), this, SLOT(onObjectDoubleClicked(cObject *)));

    insp->refresh();
    return insp;
}

Inspector *Qtenv::findFirstInspector(cObject *obj, int type, bool ignoreEmbedded)
{
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        Inspector *insp = *it;
        if (insp->getObject() == obj && insp->getType() == type && (!ignoreEmbedded || insp->isToplevel()))
            return insp;
    }
    return nullptr;
}

void Qtenv::deleteInspector(Inspector *insp)
{
    inspectors.remove(insp);
    delete insp;
}

void Qtenv::refreshInspectors()
{
    // qDebug() << "refreshInspectors()";

    // update inspectors
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ) {
        Inspector *insp = *it;
        InspectorList::iterator next = ++it;
        insp->refresh();
        it = next;
    }

    animator->redrawMessages();

    // clear the change flags on all inspected canvases
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it)
        (*it)->clearObjectChangeFlags();

    // try opening "pending" inspectors
    //TCLKILL CHK(Tcl_VarEval(interp, "inspectorUpdateCallback", TCL_NULL));
}

void Qtenv::redrawInspectors()
{
    // update inspectors (and close the ones marked for deletion)
    refreshInspectors();

    // redraw them
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it)
        (*it)->redraw();
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

void Qtenv::createSnapshot(const char *label)
{
    getSimulation()->snapshot(getSimulation(), label);
}

void Qtenv::updateGraphicalInspectorsBeforeAnimation()
{
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        Inspector *insp = *it;
        if (dynamic_cast<ModuleInspector *>(insp) && static_cast<ModuleInspector *>(insp)->needsRedraw()) {
            insp->refresh();
        }
    }
}

std::string Qtenv::getWindowTitle()
{
    const char *configName = getConfigEx()->getActiveConfigName();
    int runNumber = getConfigEx()->getActiveRunNumber();
    const char *inifile = getConfigEx()->getFileName();

    std::stringstream os;
    os << OMNETPP_PRODUCT "/Qtenv - " << getWindowTitlePrefix();
    if (opp_isempty(configName))
        os << "No network";
    else
        os << configName << " #" << runNumber;
    if (!opp_isempty(inifile))
        os << " - " << inifile;
    os << " - " << getWorkingDir();
    return os.str();
}

void Qtenv::updateNetworkRunDisplay()
{
    mainWindow->updateNetworkRunDisplay();
    mainWindow->setWindowTitle(getWindowTitle().c_str());
}

void Qtenv::updateStatusDisplay()
{
    mainWindow->updateStatusDisplay();
}

void Qtenv::printEventBanner(cEvent *event)
{
    cObject *target = event->getTargetObject();
    cMessage *msg = event->isMessage() ? static_cast<cMessage *>(event) : nullptr;
    cModule *module = msg ? msg->getArrivalModule() : nullptr;

    // produce banner text
    char banner[2*MAX_OBJECTFULLPATH+2*MAX_CLASSNAME+60];
    char *p = banner;
    p += sprintf(p, "** Event #%" LL "d  T=%s  ",
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

void Qtenv::setAnimationSpeed(float speed) {
    opt->animationSpeed = speed;
    setPref("animation_speed", opt->animationSpeed);
    emit animationSpeedChanged(speed);
}

void Qtenv::displayException(std::exception& ex)
{
    // print exception text into main window
    cException *e = dynamic_cast<cException *>(&ex);
    if (e && e->getSimulationStage() != CTX_NONE) {
        std::string txt = opp_stringf("<!> %s\n", e->getFormattedMessage().c_str());
        logBuffer.addInfo(txt.c_str());
    }

    // dialog via our printfmsg()
    EnvirBase::displayException(ex);
}

void Qtenv::componentInitBegin(cComponent *component, int stage)
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

void Qtenv::setSilentEventFilters(const char *filterLines)
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
        for (int i = 0; i < (int)tmp.size(); i++)
            delete tmp[i];
        throw;
    }
    // parsing successful, store the result
    for (int i = 0; i < (int)silentEventFilters.size(); i++)
        delete silentEventFilters[i];
    silentEventFilterLines = opp_trim(filterLines) + "\n";
    silentEventFilters = tmp;
}

bool Qtenv::isSilentEvent(cMessage *msg)
{
    MatchableObjectAdapter wrappedMsg(MatchableObjectAdapter::FULLNAME, msg);
    for (int i = 0; i < (int)silentEventFilters.size(); i++)
        if (silentEventFilters[i]->matches(&wrappedMsg))
            return true;

    return false;
}

// =========================================================================

void Qtenv::readOptions()
{
    EnvirBase::readOptions();

    cConfiguration *cfg = getConfig();

    opt->extraStack = (size_t)cfg->getAsDouble(CFGID_TKENV_EXTRA_STACK);

    const char *s = args->optionValue('c');
    opt->defaultConfig = s ? s : cfg->getAsString(CFGID_TKENV_DEFAULT_CONFIG);

    const char *r = args->optionValue('r');
    opt->defaultRun = r ? atoi(r) : cfg->getAsInt(CFGID_TKENV_DEFAULT_RUN);

    opt->imagePath = cfg->getAsPath(CFGID_TKENV_IMAGE_PATH).c_str();
    opt->pluginPath = cfg->getAsPath(CFGID_TKENV_PLUGIN_PATH).c_str();
}

void Qtenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();
}

void Qtenv::askParameter(cPar *par, bool unassigned)
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
            prompt = std::string("Enter parameter `") + par->getFullPath() + "':";

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

bool Qtenv::idle()
{
    // bug #56: refresh inspectors so that there aren't dead objects on the UI
    // while running Tk "update" (below). This only needs to be done in Fast
    // mode, because in normal Run mode inspectors are already up to date here
    // (they are refreshed after every event), and in Express mode all user
    // interactions are disabled except for the STOP button.
    if (runMode == RUNMODE_FAST) {
        // updateInspectors() may be costly, so do not check the UI too often
        timeval now;
        gettimeofday(&now, nullptr);
        if (timeval_msec(now - idleLastUICheck) < 500)
            return false;

        // refresh inspectors
        updateStatusDisplay();
        refreshInspectors();
    }

    // process UI events
    eState origsimstate = simulationState;
    simulationState = SIM_BUSY;
    QCoreApplication::processEvents();
    simulationState = origsimstate;

    bool stop = stopSimulationFlag;
    stopSimulationFlag = false;

    if (runMode == RUNMODE_FAST)
        gettimeofday(&idleLastUICheck, nullptr);
    return stop;
}

void Qtenv::objectDeleted(cObject *object)
{
    if (object == rununtil_msg) {
        // message to "run until" deleted -- stop the simulation by other means
        rununtil_msg = nullptr;
        rununtil_eventnum = getSimulation()->getEventNumber();
        if (simulationState == SIM_RUNNING || simulationState == SIM_BUSY)
            confirm("Message to run until has just been deleted.");
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

void Qtenv::simulationEvent(cEvent *event)
{
    EnvirBase::simulationEvent(event);

    if (animating && opt->animationEnabled && event->isMessage()) {
        cMessage *msg = static_cast<cMessage *>(event);
        cGate *arrivalGate = msg->getArrivalGate();
        if (!arrivalGate)
            return;

        // if arrivalgate is connected, msg arrived on a connection, otherwise via sendDirect()
        updateGraphicalInspectorsBeforeAnimation();
        if (arrivalGate->getPreviousGate()) {
            animateDelivery(msg);
        }
        else {
            animateDeliveryDirect(msg);
        }
    }
}

void Qtenv::messageSent_OBSOLETE(cMessage *msg, cGate *directToGate)  // FIXME needed?
{
    if (animating && opt->animationEnabled && !isSilentEvent(msg)) {
        // find suitable inspectors and do animate the message...
        updateGraphicalInspectorsBeforeAnimation();  // actually this will draw `msg' too (which would cause "phantom message"),
                                                     // but we'll manually remove it before animation
        if (!directToGate) {
            // message was sent via a gate (send())
            animateSend(msg, msg->getSenderGate(), msg->getArrivalGate());
        }
        else {
            // sendDirect() was used
            animateSendDirect(msg, getSimulation()->getModule(msg->getSenderModuleId()), directToGate);
            animateSend(msg, directToGate, msg->getArrivalGate());
        }
    }
}

void Qtenv::messageScheduled(cMessage *msg)
{
    EnvirBase::messageScheduled(msg);
}

void Qtenv::messageCancelled(cMessage *msg)
{
    if (msg == rununtil_msg && opt->stopOnMsgCancel) {
        if (simulationState == SIM_RUNNING || simulationState == SIM_BUSY)
            confirm(opp_stringf("Run-until message `%s' got cancelled.", msg->getName()).c_str());
        rununtil_msg = nullptr;
        rununtil_eventnum = getSimulation()->getEventNumber();  // stop the simulation using the eventnumber limit
    }
    EnvirBase::messageCancelled(msg);
}

void Qtenv::beginSend(cMessage *msg)
{
    EnvirBase::beginSend(msg);

    if (!disableTracing)
        logBuffer.beginSend(msg);
}

void Qtenv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);

    if (!disableTracing)
        logBuffer.messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void Qtenv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    EnvirBase::messageSendHop(msg, srcGate);

    if (!disableTracing)
        logBuffer.messageSendHop(msg, srcGate);
}

void Qtenv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    EnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);

    if (!disableTracing)
        logBuffer.messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void Qtenv::endSend(cMessage *msg)
{
    EnvirBase::endSend(msg);

    if (!disableTracing)
        logBuffer.endSend(msg);
}

void Qtenv::messageDeleted(cMessage *msg)
{
    EnvirBase::messageDeleted(msg);
}

void Qtenv::componentMethodBegin(cComponent *fromComp, cComponent *toComp, const char *methodFmt, va_list va, bool silent)
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
            for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
                if (insp) {
                    numinsp++;
                    animator->animateMethodcallAscent(insp, mod, methodText);
                }
            }
        }
        else if (i->from == nullptr) {
            // animate descent towards destination module
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
                if (insp) {
                    numinsp++;
                    animator->animateMethodcallDescent(insp, mod, methodText);
                }
            }
        }
        else {
            cModule *enclosingmod = i->from->getParentModule();
            for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
                if (insp) {
                    numinsp++;
                    animator->animateMethodcallHoriz(insp, i->from, i->to, methodText);
                }
            }
        }
    }
}

void Qtenv::componentMethodEnd()
{
    EnvirBase::componentMethodEnd();
}

void Qtenv::moduleCreated(cModule *newmodule)
{
    EnvirBase::moduleCreated(newmodule);

    cModule *mod = newmodule->getParentModule();

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(mod, *it);
        if (insp)
            insp->submoduleCreated(newmodule);
    }
}

void Qtenv::moduleDeleted(cModule *module)
{
    EnvirBase::moduleDeleted(module);

    componentHistory.componentDeleted(module);

    cModule *mod = module->getParentModule();

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(mod, *it);
        if (insp)
            insp->submoduleDeleted(module);
    }
}

void Qtenv::moduleReparented(cModule *module, cModule *oldParent, int oldId)
{
    EnvirBase::moduleReparented(module, oldParent, oldId);

    componentHistory.componentReparented(module, oldParent, oldId);

    // pretend it got deleted from under the 1st module, and got created under the 2nd
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(oldParent, *it);
        if (insp)
            insp->submoduleDeleted(module);
    }

    cModule *mod = module->getParentModule();
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(mod, *it);
        if (insp)
            insp->submoduleCreated(module);
    }
}

void Qtenv::connectionCreated(cGate *srcgate)
{
    EnvirBase::connectionCreated(srcgate);

    // notify compound module where the connection (whose source is this gate) is displayed
    cModule *notifymodule = nullptr;
    if (srcgate->getType() == cGate::OUTPUT)
        notifymodule = srcgate->getOwnerModule()->getParentModule();
    else
        notifymodule = srcgate->getOwnerModule();

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(notifymodule, *it);
        if (insp)
            insp->connectionCreated(srcgate);
    }
}

void Qtenv::connectionDeleted(cGate *srcgate)
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

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(notifymodule, *it);
        if (insp)
            insp->connectionDeleted(srcgate);
    }
}

void Qtenv::displayStringChanged(cComponent *component)
{
    EnvirBase::displayStringChanged(component);

    if (cModule *module = dynamic_cast<cModule *>(component))
        moduleDisplayStringChanged(module);
    else if (cChannel *channel = dynamic_cast<cChannel *>(component))
        channelDisplayStringChanged(channel);
}

void Qtenv::channelDisplayStringChanged(cChannel *channel)
{
    cGate *gate = channel->getSourceGate();

    // notify module inspector which displays connection
    cModule *notifymodule;
    if (gate->getType() == cGate::OUTPUT)
        notifymodule = gate->getOwnerModule()->getParentModule();
    else
        notifymodule = gate->getOwnerModule();

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(notifymodule, *it);
        if (insp)
            insp->displayStringChanged(gate);
    }

    // graphical gate inspector windows: normally a user doesn't have many such windows open
    // (typically, none at all), so we can afford simply refreshing all of them
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        Inspector *insp = *it;
        GateInspector *gateinsp = dynamic_cast<GateInspector *>(insp);
        if (gateinsp)
            gateinsp->displayStringChanged(gate);
    }
}

void Qtenv::moduleDisplayStringChanged(cModule *module)
{
    // refresh inspector where this module is a submodule
    cModule *parentmodule = module->getParentModule();

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(parentmodule, *it);
        if (insp)
            insp->displayStringChanged(module);
    }

    // refresh inspector where this module is the parent (i.e. this is a
    // background display string change)
    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(module, *it);
        if (insp)
            insp->displayStringChanged();
    }
}

void Qtenv::animateSend(cMessage *msg, cGate *fromgate, cGate *togate)
{
    char msgptr[32];
    ptrToStr(msg, msgptr);

    cGate *g = fromgate;
    cGate *arrivalgate = togate;

    while (g && g->getNextGate()) {
        cModule *mod = g->getOwnerModule();
        if (g->getType() == cGate::OUTPUT)
            mod = mod->getParentModule();
        bool isLastGate = (g->getNextGate() == arrivalgate);
        for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
            ModuleInspector *insp = isModuleInspectorFor(mod, *it);
            if (insp)
                animator->animateSendOnConn(insp, g, msg, (isLastGate ? ANIM_BEGIN : ANIM_THROUGH));
        }
        g = g->getNextGate();
    }
}

// helper for animateSendDirect() functions
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

void Qtenv::findDirectPath(cModule *srcmod, cModule *destmod, PathVec& pathvec)
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

void Qtenv::animateSendDirect(cMessage *msg, cModule *frommodule, cGate *togate)
{
    PathVec pathvec;
    findDirectPath(frommodule, togate->getOwnerModule(), pathvec);

    cModule *arrivalmod = msg->getArrivalGate()->getOwnerModule();

    PathVec::iterator i;
    for (i = pathvec.begin(); i != pathvec.end(); i++) {
        if (i->to == nullptr) {
            // ascent
            cModule *mod = i->from;
            cModule *enclosingmod = mod->getParentModule();
            for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
                if (insp)
                    animator->animateSenddirectAscent(insp, mod, msg);
            }
        }
        else if (i->from == nullptr) {
            // descent
            cModule *mod = i->to;
            cModule *enclosingmod = mod->getParentModule();
            bool isArrival = (mod == arrivalmod);
            for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
                if (insp)
                    animator->animateSenddirectDescent(insp, mod, msg, isArrival ? ANIM_BEGIN : ANIM_THROUGH);
            }
        }
        else {
            cModule *enclosingmod = i->from->getParentModule();
            bool isArrival = (i->to == arrivalmod);
            for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
                ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
                if (insp)
                    animator->animateSenddirectHoriz(insp, i->from, i->to, msg, isArrival ? ANIM_BEGIN : ANIM_THROUGH);
            }
        }
    }
}

void Qtenv::animateDelivery(cMessage *msg)
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

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(mod, *it);
        if (insp)
            animator->animateSendOnConn(insp, g, msg, ANIM_END);
    }
}

void Qtenv::onSelectionChanged(cObject *object) {
    mainInspector->setObject(object);
}

void Qtenv::onObjectDoubleClicked(cObject *object) {
    if (cModule *module = dynamic_cast<cModule *>(object)) {
        mainNetworkView->setObject(module);
    } else {
        inspect(object, INSP_DEFAULT, true);
    }
}

void Qtenv::animateDeliveryDirect(cMessage *msg)
{
    char msgptr[32];
    ptrToStr(msg, msgptr);

    // find suitable inspectors and do animate the message...
    cGate *g = msg->getArrivalGate();
    ASSERT(g);
    cModule *destmod = g->getOwnerModule();
    cModule *mod = destmod->getParentModule();

    for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
        ModuleInspector *insp = isModuleInspectorFor(mod, *it);
        if (insp)
            animator->animateSenddirectDelivery(insp, destmod, msg);
    }
}

void Qtenv::performAnimations()
{
    animator->play();
}

void Qtenv::bubble(cComponent *component, const char *text)
{
    EnvirBase::bubble(component, text);

    if (disableTracing)
        return;

    if (!opt->showBubbles)
        return;

    if (component->getParentModule()) {
        cModule *enclosingmod = component->getParentModule();
        for (InspectorList::iterator it = inspectors.begin(); it != inspectors.end(); ++it) {
            ModuleInspector *insp = isModuleInspectorFor(enclosingmod, *it);
            if (insp)
                insp->bubble(component, text);
        }
    }
}

void Qtenv::confirm(const char *msg)
{
    QMessageBox::information(mainWindow, "Confirm", msg, QMessageBox::StandardButton::Ok);
}

void Qtenv::putsmsg(const char *msg)
{
    confirm(msg);
}

void Qtenv::log(cLogEntry *entry)
{
    EnvirBase::log(entry);

    if (disableTracing)
        return;

    std::string prefix = logFormatter.formatPrefix(entry);
    const char *s = entry->text;
    int n = entry->textLength;

    /*
    if (!interp) {
        // fallback in case Tkenv didn't fire up correctly
        ::fputs(prefix.c_str(), stdout);
        (void)::fwrite(s, 1, n, stdout);
        return;
    }*/

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

bool Qtenv::inputDialog(const char *title, const char *prompt,
        const char *checkboxLabel, const char *defaultValue,
        std::string& outResult, bool& inoutCheckState)
{
    QDialog *dialog = new QDialog();
    dialog->setFont(boldFont);
    dialog->setWindowTitle(title);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel(prompt));
    QLineEdit *edit = new QLineEdit(defaultValue);
    layout->addWidget(edit);

    QCheckBox *checkBox;
    if(checkboxLabel)
    {
        checkBox = new QCheckBox(checkboxLabel);
        layout->addWidget(checkBox);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));
    layout->addWidget(buttonBox);

    dialog->setLayout(layout);

    if(dialog->exec() == QDialog::Rejected)
        return false;

    outResult = edit->text().toStdString();
    if(checkBox)
        inoutCheckState = checkBox->isChecked();

    delete dialog;
    return true;
}

std::string Qtenv::gets(const char *promt, const char *defaultReply)
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

bool Qtenv::askyesno(const char *question)
{
    // should return -1 when CANCEL is pressed
    //TCLKILL CHK(Tcl_VarEval(interp, "messagebox {Tkenv} ", TclQuotedString(question).get(), " question yesno", TCL_NULL));
    //TCLKILL return Tcl_GetStringResult(interp)[0] == 'y';
    return false;
}

unsigned Qtenv::getExtraStackForEnvir() const
{
    return opt->extraStack;
}
/*TCLKILL
void Qtenv::logTclError(const char *file, int line, Tcl_Interp *interp)
{
    logTclError(file, line, Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY));
}

void Qtenv::logTclError(const char *file, int line, const char *text)
{
    openTkenvlogIfNeeded();
    FILE *f = ferrorlog ? ferrorlog : stderr;
    ::fprintf(f, "Tcl error: %s#%d: %s\n\n\n", file, line, text);
    ::fflush(f);
}

void Qtenv::openTkenvlogIfNeeded()
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
*/
void Qtenv::setPref(const QString &key, const QVariant &value) {
    (localPrefKeys.contains(key) ? localPrefs : globalPrefs)->setValue(key, value);
}

QVariant Qtenv::getPref(const QString &key, const QVariant &defaultValue) {
    QSettings *settings = localPrefKeys.contains(key) ? localPrefs : globalPrefs;
    return settings->contains(key) ? settings->value(key) : defaultValue;
}

void Qtenv::runSimulationLocal(int runMode, cObject *object, Inspector *insp)
{
    MainWindow::eMode mode = MainWindow::runModeToMode(runMode);
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    if (mainWindow->isRunning()) {
        mainWindow->setGuiForRunmode(mode);
        getQtenv()->setSimulationRunMode(runMode);
        mainWindow->setRunUntilModule(insp);
    }
    else {
        if (!mainWindow->networkReady())
            return;
        mainWindow->setGuiForRunmode(mode);
        if (object == nullptr && insp)
            object = insp->getObject();

        cModule *mod = dynamic_cast<cModule *>(object);
        if (!mod) {
            // TODO log "object is not a module"
            return;
        }
        getQtenv()->runSimulation(runMode, 0, 0, nullptr, mod);
        mainWindow->setGuiForRunmode(MainWindow::NOT_RUNNING);
    }
}

void Qtenv::inspect()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        inspect(objTypePair.first, objTypePair.second, true);
    }
}

void Qtenv::runUntilModule()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        ActionDataTriplet objTypePair = variant.value<ActionDataTriplet>();
        runSimulationLocal(objTypePair.first.second, objTypePair.first.first, objTypePair.second);
    }
}

void Qtenv::runUntilMessage()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        getQtenv()->getMainWindow()->runUntilMsg(static_cast<cMessage *>(objTypePair.first), objTypePair.second);
    }
}

void Qtenv::excludeMessage()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid())
        getQtenv()->getMainWindow()->excludeMessageFromAnimation(variant.value<cObject *>());
}

void Qtenv::utilitiesSubMenu()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        InspectorUtil::copyToClipboard(static_cast<cMessage *>(objTypePair.first), objTypePair.second);
    }
}

void Qtenv::initFonts()
{
    // set up fonts
    #ifdef Q_WS_WIN
    // Windows
    QFont normalFamily = getFirstAvailableFontFamily({"Segoe UI", "MS Sans Serif", "Arial"}, 9);
    boldFont = getPref("font-bold", normalFamily).value<QFont>();
    canvasFont = getPref("font-canvas", normalFamily).value<QFont>();
    timelineFont = getPref("font-timeline", getFirstAvailableFontFamily({"Segoe Condensed", "Gill Sans MT Condensed", "Liberation Sans Narrow"},
                                                                        normalFamily.pointSize(), normalFamily)).value<QFont>();
    logFont = getPref("font-log", getFirstAvailableFontFamily({"DejaVu Sans Mono", "Courier New", "Consolas", "Terminal"}, 9)).value<QFont>();

    #elif defined(Q_WS_MAC)
    // Mac
    QFont normalFamily = getFirstAvailableFontFamily({"Lucida Grande", "Helvetica"}, 13);
    boldFont = getPref("font-bold", normalFamily).value<QFont>();
    canvasFont = getPref("font-canvas", normalFamily).value<QFont>();
    timelineFont = getPref("font-timeline", getFirstAvailableFontFamily({"Arial Narrow"}, normalFamily.pointSize(), normalFamily)).value<QFont>();
    logFont = getPref("font-log", getFirstAvailableFontFamily({"Monaco", "Courier"}, 13)).value<QFont>();

    #else
    // Linux and other systems
    QFont normalFamily = getFirstAvailableFontFamily({"Ubuntu", "Arial", "Verdana", "Helvetica", "Tahoma", "DejaVu Sans", "Nimbus Sans L",
                                                      "FreeSans", "Sans"}, 9);
    boldFont = getPref("font-bold", normalFamily).value<QFont>();
    canvasFont = getPref("font-canvas", normalFamily).value<QFont>();
    timelineFont = getPref("font-timeline", getFirstAvailableFontFamily({"Ubuntu Condensed", "Arial Narrow", "DejaVu Sans Condensed"},
                                                                        normalFamily.pointSize(), normalFamily)).value<QFont>();
    logFont = getPref("font-log", getFirstAvailableFontFamily({"Ubuntu Mono", "DejaVu Sans Mono", "Courier New", "FreeMono", "Courier"},
                                                              9)).value<QFont>();
    #endif
}

// Returns the first font family from the given preference list that is
// available on the system. If none are available, returns defaultValue.
QFont Qtenv::getFirstAvailableFontFamily(QStringList preferenceList, int pointSize, QFont defaultValue)
{
    for(QString str : preferenceList)
    {
        QFontDatabase fontDb;
        QFont font = fontDb.font(str, "Normal", pointSize);
        if(font != QFont())
            return font;
    }
    return defaultValue;
}

void Qtenv::saveFonts()
{
    setPref("font-bold", boldFont);
    setPref("font-canvas", canvasFont);
    setPref("font-timeline", timelineFont);
    setPref("font-log", logFont);
}

void Qtenv::updateQtFonts()
{
    emit fontChanged();
    mainWindow->setFont(boldFont);
}

// ======================================================================
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

} // namespace qtenv
} // namespace omnetpp

