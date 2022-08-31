//==========================================================================
//  CMDENV.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <algorithm>

#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/fileutil.h"
#include "common/enumstr.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "envir/appreg.h"
#include "envir/args.h"
#include "envir/speedometer.h"
#include "envir/visitor.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cenum.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cresultfilter.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cabstracthistogram.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cdisplaystring.h"
#include "cmddefs.h"
#include "cmdenv.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOption(CFGID_CMDENV_CONFIG_NAME, "cmdenv-config-name", CFG_STRING, nullptr, "Specifies the name of the configuration to be run (for a value `Foo`, section `[Config Foo]` will be used from the ini file). See also `cmdenv-runs-to-execute`. The `-c` command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_CMDENV_RUNS_TO_EXECUTE, "cmdenv-runs-to-execute", CFG_STRING, nullptr, "Specifies which runs to execute from the selected configuration (see `cmdenv-config-name` option). It accepts a filter expression of iteration variables such as `$numHosts>10 && $iatime==1s`, or a comma-separated list of run numbers or run number ranges, e.g. `1,3..4,7..9`. If the value is missing, Cmdenv executes all runs in the selected configuration. The `-r` command line option overrides this setting.")
Register_GlobalConfigOptionU(CFGID_CMDENV_EXTRA_STACK, "cmdenv-extra-stack", "B", "8KiB", "Specifies the extra amount of stack that is reserved for each `activity()` simple module when the simulation is run under Cmdenv.")
Register_PerRunConfigOption(CFGID_CMDENV_STOP_BATCH_ON_ERROR, "cmdenv-stop-batch-on-error", CFG_BOOL, "true", "Decides whether Cmdenv should skip the rest of the runs when an error occurs during the execution of one run.")
Register_PerRunConfigOption(CFGID_CMDENV_INTERACTIVE, "cmdenv-interactive", CFG_BOOL, "false", "Defines what Cmdenv should do when the model contains unassigned parameters. In interactive mode, it asks the user. In non-interactive mode (which is more suitable for batch execution), Cmdenv stops with an error.")
Register_PerRunConfigOption(CFGID_CMDENV_OUTPUT_FILE, "cmdenv-output-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.out", "When `cmdenv-record-output=true`: file name to redirect standard output to. See also `fname-append-host`.")
Register_PerRunConfigOption(CFGID_CMDENV_REDIRECT_OUTPUT, "cmdenv-redirect-output", CFG_BOOL, "false", "Causes Cmdenv to redirect standard output of simulation runs to a file or separate files per run. This option can be useful with running simulation campaigns (e.g. using opp_runall), and also with parallel simulation. See also: `cmdenv-output-file`, `fname-append-host`.");
Register_PerRunConfigOption(CFGID_CMDENV_EXPRESS_MODE, "cmdenv-express-mode", CFG_BOOL, "true", "Selects normal (debug/trace) or express mode.")
Register_PerRunConfigOption(CFGID_CMDENV_AUTOFLUSH, "cmdenv-autoflush", CFG_BOOL, "false", "Call `fflush(stdout)` after each event banner or status update; affects both express and normal mode. Turning on autoflush may have a performance penalty, but it can be useful with printf-style debugging for tracking down program crashes.")
Register_PerRunConfigOption(CFGID_CMDENV_EVENT_BANNERS, "cmdenv-event-banners", CFG_BOOL, "true", "When `cmdenv-express-mode=false`: turns printing event banners on/off.")
Register_PerRunConfigOption(CFGID_CMDENV_EVENT_BANNER_DETAILS, "cmdenv-event-banner-details", CFG_BOOL, "false", "When `cmdenv-express-mode=false`: print extra information after event banners.")
Register_PerRunConfigOptionU(CFGID_CMDENV_STATUS_FREQUENCY, "cmdenv-status-frequency", "s", "2s", "When `cmdenv-express-mode=true`: print status update every n seconds.")
Register_PerRunConfigOption(CFGID_CMDENV_PERFORMANCE_DISPLAY, "cmdenv-performance-display", CFG_BOOL, "true", "When `cmdenv-express-mode=true`: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")
Register_PerRunConfigOption(CFGID_CMDENV_LOG_PREFIX, "cmdenv-log-prefix", CFG_STRING, "[%l]\t", "Specifies the format string that determines the prefix of each log line. The format string may contain format directives in the syntax `%x` (a `%` followed by a single format character).  For example `%l` stands for log level, and `%J` for source component. See the manual for the list of available format characters.");
Register_PerRunConfigOption(CFGID_CMDENV_FAKE_GUI, "cmdenv-fake-gui", CFG_BOOL, "false", "Causes Cmdenv to lie to simulations that is a GUI (isGui()=true), and to periodically invoke refreshDisplay() during simulation execution.");
Register_PerObjectConfigOption(CFGID_CMDENV_LOGLEVEL, "cmdenv-log-level", KIND_MODULE, CFG_STRING, "TRACE", "Specifies the per-component level of detail recorded by log statements, output below the specified level is omitted. Available values are (case insensitive): `off`, `fatal`, `error`, `warn`, `info`, `detail`, `debug` or `trace`. Note that the level of detail is also controlled by the globally specified runtime log level and the `COMPILETIME_LOGLEVEL` macro that is used to completely remove log statements from the executable.")

//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv", Cmdenv, 10, "command-line user interface");

//
// The following function can be used to force linking with Cmdenv; specify
// -u _cmdenv_lib (gcc) or /include:_cmdenv_lib (vc++) in the link command.
//
extern "C" CMDENV_API void cmdenv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" CMDENV_API void _cmdenv_lib() {}

bool Cmdenv::sigintReceived;

// utility function for printing elapsed time
static char *timeToStr(double t, char *buf = nullptr)
{
    static char buf2[64];
    char *b = buf ? buf : buf2;

    int sec = (int) floor(t);
    if (t < 3600)
        sprintf(b, "%lgs (%dm %02ds)", t, int(sec/60L), int(sec%60L));
    else if (t < 86400)
        sprintf(b, "%lgs (%dh %02dm)", t, int(sec/3600L), int((sec%3600L)/60L));
    else
        sprintf(b, "%lgs (%dd %02dh)", t, int(sec/86400L), int((sec%86400L)/3600L));

    return b;
}

Cmdenv::Cmdenv() : opt((CmdenvOptions *&)EnvirBase::opt)
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    logStream = fopen(".cmdenv-log", "w");
    if (!logStream)
        logStream = stdout;
}

void Cmdenv::readOptions()
{
    EnvirBase::readOptions();

    cConfiguration *cfg = getConfig();

    // note: configName and runFilter will possibly be overwritten
    // with the -c, -r command-line options in our setup() method
    opt->configName = cfg->getAsString(CFGID_CMDENV_CONFIG_NAME);
    opt->runFilter = cfg->getAsString(CFGID_CMDENV_RUNS_TO_EXECUTE);
    opt->extraStack = (size_t)cfg->getAsDouble(CFGID_CMDENV_EXTRA_STACK);
}

void Cmdenv::readPerRunOptions()
{
    EnvirBase::readPerRunOptions();

    cConfiguration *cfg = getConfig();
    opt->stopBatchOnError = cfg->getAsBool(CFGID_CMDENV_STOP_BATCH_ON_ERROR);
    opt->expressMode = cfg->getAsBool(CFGID_CMDENV_EXPRESS_MODE);
    opt->interactive = cfg->getAsBool(CFGID_CMDENV_INTERACTIVE);
    opt->autoflush = cfg->getAsBool(CFGID_CMDENV_AUTOFLUSH);
    opt->printEventBanners = cfg->getAsBool(CFGID_CMDENV_EVENT_BANNERS);
    opt->detailedEventBanners = cfg->getAsBool(CFGID_CMDENV_EVENT_BANNER_DETAILS);
    opt->statusFrequencyMs = 1000*cfg->getAsDouble(CFGID_CMDENV_STATUS_FREQUENCY);
    opt->printPerformanceData = cfg->getAsBool(CFGID_CMDENV_PERFORMANCE_DISPLAY);
    setLogFormat(getConfig()->getAsString(CFGID_CMDENV_LOG_PREFIX).c_str());
    opt->outputFile = cfg->getAsFilename(CFGID_CMDENV_OUTPUT_FILE).c_str();
    opt->redirectOutput = cfg->getAsBool(CFGID_CMDENV_REDIRECT_OUTPUT);
    opt->fakeGUI = cfg->getAsBool(CFGID_CMDENV_FAKE_GUI);
    delete fakeGUI;
    fakeGUI = nullptr;
    if (opt->fakeGUI) {
        out << "\n*** WARNING: FAKEGUI IS AN EXPERIMENTAL FEATURE -- DO NOT RELY ON FINGERPRINTS GENERATED UNDER FAKEGUI UNTIL CODE IS FINALIZED!\n" << endl;
        fakeGUI = new FakeGUI();
        fakeGUI->readConfigOptions(cfg);
    }
}

void Cmdenv::doRun()
{
    {
        // '-c' and '-r' option: configuration to activate, and run numbers to run.
        // Both command-line options take precedence over inifile settings.
        // (NOTE: inifile settings *already* got read at this point! as EnvirBase::setup()
        // invokes readOptions()).

        if (args->optionGiven('c'))  // note: do not overwrite value from cmdenv-config-name option
            opt->configName = args->optionValue('c');
        if (opt->configName.empty())
            opt->configName = "General";

        if (args->optionGiven('r'))  // note: do not overwrite value from cmdenv-runs-to-execute option!
            opt->runFilter = args->optionValue('r');

        std::vector<int> runNumbers;
        try {
            runNumbers = resolveRunFilter(opt->configName.c_str(), opt->runFilter.c_str());
        }
        catch (std::exception& e) {
            displayException(e);
            exitCode = 1;
            return;
        }

        numRuns = (int)runNumbers.size();
        runsTried = 0;
        int numErrors = 0;
        for (int runNumber : runNumbers) {
            runsTried++;
            bool finishedOK = false;
            bool networkSetupDone = false;
            bool endRunRequired = false;
            try {
                if (opt->verbose)
                    out << "\nPreparing for running configuration " << opt->configName << ", run #" << runNumber << "..." << endl;

                cfg->activateConfig(opt->configName.c_str(), runNumber);
                readPerRunOptions();

                const char *iterVars = cfg->getVariable(CFGVAR_ITERATIONVARS);
                const char *runId = cfg->getVariable(CFGVAR_RUNID);
                const char *repetition = cfg->getVariable(CFGVAR_REPETITION);
                if (!opt->verbose)
                    out << opt->configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << endl; // print before redirection; useful as progress indication from opp_runall

                if (opt->redirectOutput) {
                    processFileName(opt->outputFile);
                    if (opt->verbose)
                        out << "Redirecting output to file \"" << opt->outputFile << "\"..." << endl;
                    startOutputRedirection(opt->outputFile.c_str());
                    if (opt->verbose)
                        out << "\nRunning configuration " << opt->configName << ", run #" << runNumber << "..." << endl;
                }

                if (opt->verbose) {
                    if (iterVars && strlen(iterVars) > 0)
                        out << "Scenario: " << iterVars << ", $repetition=" << repetition << endl;
                    out << "Assigned runID=" << runId << endl;
                }

                // find network
                if (opt->networkName.empty())
                    throw cRuntimeError("No network specified (missing or empty network= configuration option)");
                cModuleType *network = resolveNetwork(opt->networkName.c_str());
                ASSERT(network);

                endRunRequired = true;

                // set up network
                if (opt->verbose)
                    out << "Setting up network \"" << opt->networkName.c_str() << "\"..." << endl;

                setupNetwork(network);
                networkSetupDone = true;

                // prepare for simulation run
                if (opt->verbose)
                    out << "Initializing..." << endl;

                loggingEnabled = !opt->expressMode;

                prepareForRun();

                // run the simulation
                if (opt->verbose)
                    out << "\nRunning simulation..." << endl;

                // simulate() should only throw exception if error occurred and
                // finish() should not be called.
                notifyLifecycleListeners(LF_ON_SIMULATION_START);
                simulate();
                loggingEnabled = true;

                if (opt->verbose)
                    out << "\nCalling finish() at end of Run #" << runNumber << "..." << endl;
                getSimulation()->callFinish();
                cLogProxy::flushLastLine();

                checkFingerprint();

                notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);

                finishedOK = true;
            }
            catch (std::exception& e) {
                loggingEnabled = true;
                stoppedWithException(e);
                notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
                displayException(e);
            }

            // send LF_ON_RUN_END notification
            if (endRunRequired) {
                try {
                    notifyLifecycleListeners(LF_ON_RUN_END);
                }
                catch (std::exception& e) {
                    finishedOK = false;
                    notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
                    displayException(e);
                }
            }

            // delete network
            if (networkSetupDone) {
                try {
                    getSimulation()->deleteNetwork();
                }
                catch (std::exception& e) {
                    numErrors++;
                    notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
                    displayException(e);
                }
            }

            // stop redirecting into file
            stopOutputRedirection();

            if (!finishedOK)
                numErrors++;

            // skip further runs if signal was caught
            if (sigintReceived)
                break;

            if (!finishedOK && opt->stopBatchOnError)
                break;
        }

        if (numRuns > 1 && opt->verbose) {
            int numSkipped = numRuns - runsTried;
            int numSuccess = runsTried - numErrors;
            out << "\nRun statistics: total " << numRuns;
            if (numSuccess > 0)
                out << ", successful " << numSuccess;
            if (numErrors > 0)
                out << ", errors " << numErrors;
            if (numSkipped > 0)
                out << ", skipped " << numSkipped;
            out << endl;
        }

        exitCode = numErrors > 0 ? 1 : sigintReceived ? 2 : 0;
    }
}

// note: also updates "since" (sets it to the current time) if answer is "true"
inline bool elapsed(long millis, int64_t& since)
{
    int64_t now = opp_get_monotonic_clock_usecs();
    bool ret = (now - since) > millis * 1000;
    if (ret)
        since = now;
    return ret;
}

void Cmdenv::simulate()
{
    // implement graceful exit when Ctrl-C is hit during simulation. We want
    // to finish the current event, then normally exit via callFinish() etc
    // so that simulation results are not lost.
    installSignalHandler();

    startClock();
    sigintReceived = false;

    Speedometer speedometer;  // only used by Express mode, but we need it in catch blocks too

    cSimulation *simulation = getSimulation();

    // The following macro was originally written as a lambda, but on macOS it caused
    // the program to crash while writing to the `out` stream after returning from simulate(),
    // due to some spurious compiler bug which only manifested in MODE=debug.
    // Converting `auto finally = [&] { ... }` to a macro solved the issue.

#define FINALLY() { \
        if (opt->expressMode) \
            doStatusUpdate(speedometer); \
        loggingEnabled = true; \
        stopClock(); \
        deinstallSignalHandler(); \
    }

    try {
        if (!opt->expressMode) {
            while (true) {
                cEvent *event = simulation->takeNextEvent();
                if (!event)
                    throw cTerminationException("Scheduler interrupted while waiting");

                // flush *between* printing event banner and event processing, so that
                // if event processing crashes, it can be seen which event it was
                if (opt->autoflush)
                    out.flush();

                if (fakeGUI)
                    fakeGUI->beforeEvent(event);

                // execute event
                simulation->executeEvent(event);

                if (fakeGUI)
                    fakeGUI->afterEvent();

                // flush so that output from different modules don't get mixed
                cLogProxy::flushLastLine();

                checkTimeLimits();

                if (sigintReceived)
                    throw cTerminationException("SIGINT or SIGTERM received, exiting");
            }
        }
        else {
            speedometer.start(simulation->getSimTime());

            int64_t last_update = opp_get_monotonic_clock_usecs();

            doStatusUpdate(speedometer);

            while (true) {
                cEvent *event = simulation->takeNextEvent();
                if (!event)
                    throw cTerminationException("Scheduler interrupted while waiting");

                speedometer.addEvent(simulation->getSimTime());

                // print event banner from time to time
                if ((simulation->getEventNumber()&0xff) == 0 && elapsed(opt->statusFrequencyMs, last_update))
                    doStatusUpdate(speedometer);

                if (fakeGUI)
                    fakeGUI->beforeEvent(event);

                // execute event
                simulation->executeEvent(event);

                if (fakeGUI)
                    fakeGUI->afterEvent();

                checkTimeLimits();  // potential place to gain a few cycles

                if (sigintReceived)
                    throw cTerminationException("SIGINT or SIGTERM received, exiting");
            }
        }
        FINALLY();
    }
    catch (cTerminationException& e) {
        FINALLY();
        stoppedWithTerminationException(e);
        displayException(e);
        return;
    }
    catch (std::exception& e) {
        FINALLY();
        throw;
    }
#undef FINALLY
}

void Cmdenv::printEventBanner(cEvent *event)
{
    out << "** Event #" << getSimulation()->getEventNumber()
        << "  t=" << getSimulation()->getSimTime()
        << progressPercentage() << "   ";  // note: IDE launcher uses this to track progress

    if (event->isMessage()) {
        cModule *mod = static_cast<cMessage *>(event)->getArrivalModule();
        out << mod->getFullPath() << " (" << mod->getComponentType()->getName() << ", id=" << mod->getId() << ")";
    }
    else if (event->getTargetObject()) {
        cObject *target = event->getTargetObject();
        out << target->getFullPath() << " (" << target->getClassName() << ")";
    }
    out << "\n"; // note: "\n" not endl, because we don't want auto-flush on each event
    if (opt->detailedEventBanners) {
        out << "   Elapsed: " << timeToStr(getElapsedSecs())
            << "   Messages: created: " << cMessage::getTotalMessageCount()
            << "  present: " << cMessage::getLiveMessageCount()
            << "  in FES: " << getSimulation()->getFES()->getLength() << "\n"; // note: "\n" not endl, because we don't want auto-flush on each event
    }
}

void Cmdenv::doStatusUpdate(Speedometer& speedometer)
{
    speedometer.beginNewInterval();

    if (opt->printPerformanceData) {
        out << "** Event #" << getSimulation()->getEventNumber()
            << "   t=" << getSimulation()->getSimTime()
            << "   Elapsed: " << timeToStr(getElapsedSecs())
            << "" << progressPercentage() << endl;  // note: IDE launcher uses this to track progress

        out << "     Speed:     ev/sec=" << speedometer.getEventsPerSec()
            << "   simsec/sec=" << speedometer.getSimSecPerSec()
            << "   ev/simsec=" << speedometer.getEventsPerSimSec() << endl;

        out << "     Messages:  created: " << cMessage::getTotalMessageCount()
            << "   present: " << cMessage::getLiveMessageCount()
            << "   in FES: " << getSimulation()->getFES()->getLength() << endl;
    }
    else {
        out << "** Event #" << getSimulation()->getEventNumber() << "   t=" << getSimulation()->getSimTime()
            << "   Elapsed: " << timeToStr(getElapsedSecs())
            << progressPercentage() // note: IDE launcher uses this to track progress
            << "   ev/sec=" << speedometer.getEventsPerSec() << endl;
    }

    // status update is always autoflushed (not only if opt->autoflush is on)
    out.flush();
}

const char *Cmdenv::progressPercentage()
{
    double simtimeRatio = -1;
    if (opt->simtimeLimit > 0)
        simtimeRatio = getSimulation()->getSimTime() / opt->simtimeLimit;

    double elapsedTimeRatio = -1;
    if (opt->realTimeLimit > 0)
        elapsedTimeRatio = stopwatch.getElapsedSecs() / opt->realTimeLimit;

    double cpuTimeRatio = -1;
    if (opt->cpuTimeLimit > 0)
        cpuTimeRatio = stopwatch.getCPUUsageSecs() / opt->cpuTimeLimit;

    double ratio = std::max(simtimeRatio, std::max(elapsedTimeRatio, cpuTimeRatio));
    ratio = std::min(ratio, 1.0);  // eliminate occasional "101% completed" message
    if (ratio == -1)
        return "";
    else {
        double totalRatio = (ratio + runsTried - 1) / numRuns;
        static char buf[32];
        // DO NOT change the "% completed" string. The IDE launcher plugin matches
        // against this string for detecting user input
        snprintf(buf, 32, "  %d%% completed  (%d%% total)", (int)(100*ratio), (int)(100*totalRatio));
        return buf;
    }
}

void Cmdenv::displayException(std::exception& ex)
{
    EnvirBase::displayException(ex);
}

void Cmdenv::componentInitBegin(cComponent *component, int stage)
{
    EnvirBase::componentInitBegin(component, stage);

    // TODO: make this an EV_INFO in the component?
    if (!opt->expressMode && opt->printEventBanners && component->getLogLevel() != LOGLEVEL_OFF)
        out << "Initializing " << (component->isModule() ? "module" : "channel") << " " << component->getFullPath() << ", stage " << stage << endl;
}

void Cmdenv::simulationEvent(cEvent *event)
{
    EnvirBase::simulationEvent(event);

    // print event banner if necessary
    if (!opt->expressMode && opt->printEventBanners)
        if (!event->isMessage() || static_cast<cMessage *>(event)->getArrivalModule()->getLogLevel() != LOGLEVEL_OFF)
            printEventBanner(event);
}

void Cmdenv::signalHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
        sigintReceived = true;
}

void Cmdenv::installSignalHandler()
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
}

void Cmdenv::deinstallSignalHandler()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

//-----------------------------------------------------

void Cmdenv::configure(cComponent *component)
{
    EnvirBase::configure(component);

    LogLevel level = cLog::resolveLogLevel(getEnvir()->getConfig()->getAsString(component->getFullPath().c_str(), CFGID_CMDENV_LOGLEVEL).c_str());
    component->setLogLevel(level);
}

void Cmdenv::askParameter(cPar *par, bool unassigned)
{
    bool success = false;
    while (!success) {
        cProperties *props = par->getProperties();
        cProperty *prop = props->get("prompt");
        std::string prompt = prop ? prop->getValue(cProperty::DEFAULTKEY) : "";
        std::string reply;

        // ask the user. note: gets() will signal "cancel" by throwing an exception
        if (!prompt.empty())
            reply = this->gets(prompt.c_str(), par->str().c_str());
        else
            // DO NOT change the "Enter parameter" string. The IDE launcher plugin matches
            // against this string for detecting user input
            reply = this->gets((std::string("Enter parameter '")+par->getFullPath()+"' ("+(unassigned ? "unassigned" : "ask")+"):").c_str(), par->str().c_str());

        try {
            par->parse(reply.c_str());
            success = true;
        }
        catch (std::exception& e) {
            out << "<!> " << e.what() << " -- please try again" << endl;
        }
    }
}

void Cmdenv::alert(const char *msg)
{
    out << "\n<!> " << msg << endl << endl;
}

void Cmdenv::log(cLogEntry *entry)
{
    EnvirBase::log(entry);

    if (!logFormatter.isBlank())
        out << logFormatter.formatPrefix(entry);

    out.write(entry->text, entry->textLength);
    if (opt->autoflush)
        out.flush();
}

std::string Cmdenv::gets(const char *prompt, const char *defaultReply)
{
    if (!opt->interactive)
        throw cRuntimeError("The simulation attempted to prompt for user input, set cmdenv-interactive=true to allow it: \"%s\"", prompt);

    out << prompt;
    if (!opp_isempty(defaultReply))
        out << "(default: " << defaultReply << ") ";
    out.flush();

    {
        std::string buffer;
        std::getline(std::cin, buffer);
        if (buffer == "\x1b")  // ESC?
            throw cRuntimeError(E_CANCEL);
        return buffer;
    }
}

bool Cmdenv::askYesNo(const char *question)
{
    if (!opt->interactive)
        throw cRuntimeError("Simulation needs user input in non-interactive mode (prompt text: \"%s (y/n)\")", question);

    {
        for (;;) {
            out << question <<" (y/n) ";
            out.flush();
            std::string buffer;
            std::getline(std::cin, buffer);
            if (buffer == "\x1b")  // ESC?
                throw cRuntimeError(E_CANCEL);
            if (buffer == "y" || buffer == "Y")
                return true;
            else if (buffer == "n" || buffer == "N")
                return false;
            else
                out << "Please type 'y' or 'n'!" << endl;
        }
    }
}

bool Cmdenv::idle()
{
    return sigintReceived;
}

double Cmdenv::getAnimationTime() const
{
    return fakeGUI ? fakeGUI->getAnimationTime() : 0;
}

double Cmdenv::getAnimationSpeed() const
{
    return fakeGUI ? fakeGUI->getAnimationSpeed() : 0;
}

double Cmdenv::getRemainingAnimationHoldTime() const
{
    return fakeGUI ? fakeGUI->getRemainingAnimationHoldTime() : 0;
}

void Cmdenv::getImageSize(const char *imageName, double& outWidth, double& outHeight)
{
    if (fakeGUI)
        fakeGUI->getImageSize(imageName, outWidth, outHeight);
    else
        outWidth = outHeight = 32;
}

void Cmdenv::getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent)
{
    if (!*text)
        outWidth = outHeight = outAscent = 0;
    else if (fakeGUI)
        fakeGUI->getTextExtent(font, text, outWidth, outHeight, outAscent);
    else {
        outWidth = 10 * strlen(text);
        outHeight = 12;
        outAscent = 8;
    }
}

void Cmdenv::appendToImagePath(const char *directory)
{
    if (fakeGUI)
        fakeGUI->appendToImagePath(directory);
}

void Cmdenv::loadImage(const char *fileName, const char *imageName)
{
    if (fakeGUI)
        fakeGUI->loadImage(fileName, imageName);
}

cFigure::Rectangle Cmdenv::getSubmoduleBounds(const cModule *submodule)
{
    return fakeGUI ? fakeGUI->getSubmoduleBounds(submodule) : cFigure::Rectangle(NAN, NAN, NAN, NAN);
}

std::vector<cFigure::Point> Cmdenv::getConnectionLine(const cGate *sourceGate)
{
    return fakeGUI ? fakeGUI->getConnectionLine(sourceGate) : std::vector<cFigure::Point>();
}

double Cmdenv::getZoomLevel(const cModule *module)
{
    return fakeGUI ? fakeGUI->getZoomLevel(module) : NAN;
}

void Cmdenv::printUISpecificHelp()
{
    out << "Cmdenv-specific information:\n";
    out << "    Cmdenv executes all runs denoted by the -c and -r options. The number\n";
    out << "    of runs executed and the number of runs that ended with an error are\n";
    out << "    reported at the end.\n";
    out << endl;
}

unsigned Cmdenv::getExtraStackForEnvir() const
{
    return opt->extraStack;
}
}  // namespace cmdenv
}  // namespace omnetpp

