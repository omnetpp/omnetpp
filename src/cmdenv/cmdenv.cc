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

#include <thread>

#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/fileutil.h"
#include "common/enumstr.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "envir/appreg.h"
#include "envir/args.h"
#include "envir/speedometer.h"
#include "envir/resultfileutils.h"
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
#include "runner.h"

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

Cmdenv::Cmdenv() : opt((CmdenvOptions *&)AppBase::opt)
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it

    logStream = fopen(".cmdenv-log", "w");
    if (!logStream)
        logStream = stdout;
}

void Cmdenv::readOptions(cConfiguration *cfg)
{
    AppBase::readOptions(cfg);

    // note: configName and runFilter will possibly be overwritten
    // with the -c, -r command-line options in our setup() method
    opt->extraStack = (size_t)cfg->getAsDouble(CFGID_CMDENV_EXTRA_STACK);
}

void Cmdenv::readPerRunOptions(cConfiguration *cfg)
{
    AppBase::readPerRunOptions(cfg);

    opt->stopBatchOnError = cfg->getAsBool(CFGID_CMDENV_STOP_BATCH_ON_ERROR);
    opt->expressMode = cfg->getAsBool(CFGID_CMDENV_EXPRESS_MODE);
    opt->interactive = cfg->getAsBool(CFGID_CMDENV_INTERACTIVE);
    opt->autoflush = cfg->getAsBool(CFGID_CMDENV_AUTOFLUSH);
    opt->printEventBanners = cfg->getAsBool(CFGID_CMDENV_EVENT_BANNERS);
    opt->detailedEventBanners = cfg->getAsBool(CFGID_CMDENV_EVENT_BANNER_DETAILS);
    opt->statusFrequencyMs = 1000*cfg->getAsDouble(CFGID_CMDENV_STATUS_FREQUENCY);
    opt->printPerformanceData = cfg->getAsBool(CFGID_CMDENV_PERFORMANCE_DISPLAY);
    getEnvir()->setLogFormat(cfg->getAsString(CFGID_CMDENV_LOG_PREFIX).c_str());
    opt->outputFile = cfg->getAsFilename(CFGID_CMDENV_OUTPUT_FILE).c_str();
    opt->redirectOutput = cfg->getAsBool(CFGID_CMDENV_REDIRECT_OUTPUT);
    opt->fakeGUI = cfg->getAsBool(CFGID_CMDENV_FAKE_GUI);
    delete fakeGUI;
    fakeGUI = nullptr;
    if (opt->fakeGUI) {
        out << "\n*** WARNING: FAKEGUI IS AN EXPERIMENTAL FEATURE -- DO NOT RELY ON FINGERPRINTS GENERATED UNDER FAKEGUI UNTIL CODE IS FINALIZED!\n" << endl;
        fakeGUI = new FakeGUI();
        fakeGUI->configure(getSimulation(), cfg);
    }
}

void Cmdenv::doRun()
{
    {
        cConfiguration *globalCfg = ini->extractGlobalConfig();
        readOptions(globalCfg);
        loadNEDFiles(globalCfg, args);

        //TODO how?
        // if (envir->getAttachDebuggerOnErrors())
        //    installCrashHandler();

        CodeFragments::executeAll(CodeFragments::STARTUP); // app setup is complete

        // '-c' and '-r' option: configuration to activate, and run numbers to run.
        // Both command-line options take precedence over inifile settings.

        std::string configName = globalCfg->getAsString(CFGID_CMDENV_CONFIG_NAME);
        std::string runFilter = globalCfg->getAsString(CFGID_CMDENV_RUNS_TO_EXECUTE);

        delete globalCfg;

        if (args->optionGiven('c'))
            configName = opp_nulltoempty(args->optionValue('c'));
        if (configName.empty())
            configName = "General";

        if (args->optionGiven('r'))
            runFilter = args->optionValue('r');

        std::vector<int> runNumbers;
        try {
            runNumbers = resolveRunFilter(configName.c_str(), runFilter.c_str());
        }
        catch (std::exception& e) {
            displayException(e);
            exitCode = 1;
            return;
        }

        numRuns = (int)runNumbers.size();
        runsTried = 0;
        numErrors = 0;

        bool threaded = false; //TODO config
        if (!threaded) {
            runSimulations(configName.c_str(), runNumbers);
        }
        else {
            int numThreads = 3; //TODO config
            runSimulationsInThreads(configName.c_str(), runNumbers, numThreads);
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

void Cmdenv::runSimulationsInThreads(const char *configName, const std::vector<int>& runNumbers, int numThreads)
{
    // statically assign jobs to threads
    std::vector<std::vector<int>> runListPerThread(numThreads);
    for (int i = 0; i < runNumbers.size(); i++)
        runListPerThread[i % numThreads].push_back(runNumbers[i]);

    // create and launch threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++)
        threads.push_back(startThread(configName, runListPerThread[i]));

    // wait for them to finish
    for (int i = 0; i < numThreads; i++)
        threads[i].join();
}

std::thread Cmdenv::startThread(const char *configName, const std::vector<int>& runNumbers)
{
    auto fn = [this](std::string configName, std::vector<int> runNumbers) {
        runSimulations(configName.c_str(), runNumbers);
    };
    return std::thread(fn, configName, runNumbers);
}

void Cmdenv::runSimulations(const char *configName, const std::vector<int>& runNumbers)
{
    for (int runNumber : runNumbers) {
        runsTried++;

        bool finishedOK = runSimulation(configName, runNumber);
        if (!finishedOK)
            numErrors++;

        // skip further runs if signal was caught
        if (sigintReceived)
            break;

        if (!finishedOK && opt->stopBatchOnError)
            break;
    }
}

bool Cmdenv::runSimulation(const char *configName, int runNumber)
{
    EnvirBase *envir = new EnvirBase(this);
    cSimulation *simulation = new cSimulation("simulation", envir);  //TODO: finally: delete simulation
    simulation->setNedLoader(nedLoader);
    cSimulation::setActiveSimulation(simulation);

    cConfiguration *globalCfg = ini->extractGlobalConfig();
    envir->initialize(simulation, globalCfg, args);
    readOptions(globalCfg);
    delete globalCfg;

    bool finishedOK = false;
    bool networkSetupDone = false;
    bool endRunRequired = false;

    cConfiguration *cfg = nullptr;
    try {
        if (opt->verbose)
            out << "\nPreparing for running configuration " << configName << ", run #" << runNumber << "..." << endl;

        cfg = ini->extractConfig(configName, runNumber);
        readPerRunOptions(cfg); //TODO opts are global!!!

        const char *iterVars = cfg->getVariable(CFGVAR_ITERATIONVARS);
        const char *runId = cfg->getVariable(CFGVAR_RUNID);
        const char *repetition = cfg->getVariable(CFGVAR_REPETITION);
        if (!opt->verbose)
            out << configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << endl; // print before redirection; useful as progress indication from opp_runall

        if (opt->redirectOutput) {
            opt->outputFile = ResultFileUtils(cfg).augmentFileName(opt->outputFile);
            if (opt->verbose)
                out << "Redirecting output to file \"" << opt->outputFile << "\"..." << endl;
            startOutputRedirection(opt->outputFile.c_str());
            if (opt->verbose)
                out << "\nRunning configuration " << configName << ", run #" << runNumber << "..." << endl;
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

        envir->setLoggingEnabled(!opt->expressMode);

        simulation->callInitialize();
        cLogProxy::flushLastLine();

        // run the simulation
        if (opt->verbose)
            out << "\nRunning simulation..." << endl;

        // simulate() should only throw exception if error occurred and
        // finish() should not be called.
        notifyLifecycleListeners(LF_ON_SIMULATION_START);
        simulate();
        envir->setLoggingEnabled(true);

        if (opt->verbose)
            out << "\nCalling finish() at end of Run #" << runNumber << "..." << endl;
        getSimulation()->callFinish();
        cLogProxy::flushLastLine();

        checkFingerprint();

        notifyLifecycleListeners(LF_ON_SIMULATION_SUCCESS);

        finishedOK = true;
    }
    catch (std::exception& e) {
        envir->setLoggingEnabled(true);
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
            finishedOK = false;
            notifyLifecycleListeners(LF_ON_SIMULATION_ERROR);
            displayException(e);
        }
    }

    // stop redirecting into file
    stopOutputRedirection();

    delete simulation;  // deletes envir too
    delete cfg;

    return finishedOK;
}

void Cmdenv::simulate()
{
    // implement graceful exit when Ctrl-C is hit during simulation. We want
    // to finish the current event, then normally exit via callFinish() etc
    // so that simulation results are not lost.
    installSignalHandler();

    sigintReceived = false;

    try {
        cSimulation *simulation = cSimulation::getActiveSimulation();

        Runner runner(simulation, fakeGUI, out, sigintReceived);
        if (opt->simtimeLimit >= SIMTIME_ZERO)
            runner.setSimulationTimeLimit(opt->simtimeLimit);
        runner.setCPUTimeLimit(opt->cpuTimeLimit);
        runner.setRealTimeLimit(opt->realTimeLimit);
        runner.setExpressMode(opt->expressMode);
        runner.setAutoFlush(opt->autoflush);
        runner.setStatusFrequencyMs(opt->statusFrequencyMs);
        runner.setPrintPerformanceData(opt->printPerformanceData);
        runner.setPrintThreadId(false); //TODO
        runner.setPrintEventBanners(opt->printEventBanners);
        runner.setDetailedEventBanners(opt->detailedEventBanners);
        runner.setBatchProgress((int)runsTried, (int)numRuns);

        runner.run();

        deinstallSignalHandler(); // TODO this might not be the best place, if needed at all
    }
    catch (cTerminationException& e) {
        deinstallSignalHandler();
        stoppedWithTerminationException(e);
        displayException(e);
        return;
    }
    catch (std::exception& e) {
        deinstallSignalHandler();
        throw;
    }
}

void Cmdenv::displayException(std::exception& ex)
{
    AppBase::displayException(ex);
}

void Cmdenv::componentInitBegin(cComponent *component, int stage)
{
    AppBase::componentInitBegin(component, stage);

    // TODO: make this an EV_INFO in the component?
    if (!opt->expressMode && opt->printEventBanners && component->getLogLevel() != LOGLEVEL_OFF)
        out << "Initializing " << (component->isModule() ? "module" : "channel") << " " << component->getFullPath() << ", stage " << stage << endl;
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

void Cmdenv::configureComponent(cComponent *component)
{
    AppBase::configureComponent(component);

    cConfiguration *cfg = component->getSimulation()->getEnvir()->getConfig();
    LogLevel level = cLog::resolveLogLevel(cfg->getAsString(component->getFullPath().c_str(), CFGID_CMDENV_LOGLEVEL).c_str());
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
    AppBase::log(entry);

    LogFormatter& logFormatter = getEnvir()->getLogFormatter();
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

