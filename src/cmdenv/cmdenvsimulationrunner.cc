//==========================================================================
//  CMDENVSIMULATIONRUNNER.CC - part of
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

#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <thread>

#include "cmddefs.h"
#include "cmdenvapp.h"
#include "common/fileutil.h"
#include "common/stlutil.h"
#include "common/stringutil.h"
#include "envir/appbase.h"
#include "envir/appreg.h"
#include "envir/args.h"
#include "envir/envirbase.h"
#include "envir/envirutils.h"
#include "envir/inifilecontents.h"
#include "envir/resultfileutils.h"
#include "envir/runner.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cinedloader.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/crunner.h"
#include "omnetpp/csimulation.h"
#include "sim/netbuilder/cnedloader.h"
#include "sim/pythonutil.h"
#include "cmdenvsimulationrunner.h"
#include "cmdenvenvir.h"

#ifdef WITH_PYTHONSIM
#include <Python.h>
#else
#define Py_BEGIN_ALLOW_THREADS  /*no-op*/
#define Py_END_ALLOW_THREADS    /*no-op*/
#endif

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOption(CFGID_CMDENV_CONFIG_NAME, "cmdenv-config-name", CFG_STRING, nullptr, "Specifies the name of the configuration to be run (for a value `Foo`, section `[Config Foo]` will be used from the ini file). See also `cmdenv-runs-to-execute`. The `-c` command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_CMDENV_RUNS_TO_EXECUTE, "cmdenv-runs-to-execute", CFG_STRING, nullptr, "Specifies which runs to execute from the selected configuration (see `cmdenv-config-name` option). It accepts a filter expression of iteration variables such as `$numHosts>10 && $iatime==1s`, or a comma-separated list of run numbers or run number ranges, e.g. `1,3..4,7..9`. If the value is missing, CmdenvCore executes all runs in the selected configuration. The `-r` command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_CMDENV_STOP_BATCH_ON_ERROR, "cmdenv-stop-batch-on-error", CFG_BOOL, "true", "Decides whether CmdenvCore should skip the rest of the runs when an error occurs during the execution of one run.")
Register_GlobalConfigOption(CFGID_CMDENV_NUM_THREADS, "cmdenv-num-threads", CFG_INT, "1", "Specifies the number of threads to use when running multiple simulations is requested. (Each simulation will still run sequentially in its thread.) When -1 is given, the number of concurrent threads supported by the hardware will be used.");

Register_GlobalConfigOption(CFGID_CMDENV_OUTPUT_FILE, "cmdenv-output-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.out", "When `cmdenv-record-output=true`: file name to redirect standard output to. See also `fname-append-host`.")
Register_GlobalConfigOption(CFGID_CMDENV_REDIRECT_OUTPUT, "cmdenv-redirect-output", CFG_BOOL, "false", "Causes Cmdenv to redirect standard output of simulation runs to a file or separate files per run. This option can be useful with running simulation campaigns (e.g. using opp_runall), and also with parallel simulation. See also: `cmdenv-output-file`, `fname-append-host`.");
Register_GlobalConfigOption(CFGID_CMDENV_EXPRESS_MODE, "cmdenv-express-mode", CFG_BOOL, "true", "Selects normal (debug/trace) or express mode.")
Register_GlobalConfigOption(CFGID_CMDENV_AUTOFLUSH, "cmdenv-autoflush", CFG_BOOL, "false", "Call `fflush(stdout)` after each event banner or status update; affects both express and normal mode. Turning on autoflush may have a performance penalty, but it can be useful with printf-style debugging for tracking down program crashes.")
Register_GlobalConfigOption(CFGID_CMDENV_EVENT_BANNERS, "cmdenv-event-banners", CFG_BOOL, "true", "When `cmdenv-express-mode=false`: turns printing event banners on/off.")
Register_GlobalConfigOption(CFGID_CMDENV_EVENT_BANNER_DETAILS, "cmdenv-event-banner-details", CFG_BOOL, "false", "When `cmdenv-express-mode=false`: print extra information after event banners.")
Register_GlobalConfigOptionU(CFGID_CMDENV_STATUS_FREQUENCY, "cmdenv-status-frequency", "s", "2s", "When `cmdenv-express-mode=true`: print status update every n seconds.")
Register_GlobalConfigOption(CFGID_CMDENV_PERFORMANCE_DISPLAY, "cmdenv-performance-display", CFG_BOOL, "true", "When `cmdenv-express-mode=true`: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")

bool CmdenvSimulationRunner::sigintReceived;


class VerboseListener : public cISimulationLifecycleListener
{
  private:
    std::ostream& out;
  public:
    VerboseListener(std::ostream& out) : out(out) {}
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;
    virtual void listenerRemoved() override;
};

void VerboseListener::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
    case LF_PRE_NETWORK_SETUP: out << "Setting up network \"" << check_and_cast<cModuleType *>(details)->getFullName() << "\"..." << endl; break;
    case LF_PRE_NETWORK_INITIALIZE: out << "Initializing..." << endl; break;
    case LF_ON_SIMULATION_START: out << "\nRunning simulation..." << endl; break;
    case LF_PRE_NETWORK_FINISH: out << "\nCalling finish() at end of Run #" << cSimulation::getActiveSimulation()->getConfig()->getVariable(CFGVAR_RUNNUMBER) << "..." << endl; break;
    case LF_ON_SIMULATION_SUCCESS: out << "\n<!> " << check_and_cast<cException*>(details)->getFormattedMessage() << endl; break;
    case LF_ON_SIMULATION_ERROR: out << "\n<!> " << check_and_cast<cException*>(details)->getFormattedMessage() << endl; break;
    default: break;
    }
}

void VerboseListener::listenerRemoved()
{
    cISimulationLifecycleListener::listenerRemoved();
    delete this;
}

//---

//TODO better ctors!
CmdenvSimulationRunner::CmdenvSimulationRunner()
{
}

CmdenvSimulationRunner::~CmdenvSimulationRunner()
{
}

int CmdenvSimulationRunner::runCmdenv(InifileContents *ini, ArgList *args)
{
    cConfiguration *globalCfg = ini->extractGlobalConfig();
    std::string configName = globalCfg->getAsString(CFGID_CMDENV_CONFIG_NAME);
    std::string runFilter = globalCfg->getAsString(CFGID_CMDENV_RUNS_TO_EXECUTE);
    delete globalCfg;

    // '-c' and '-r' option: configuration to activate, and run numbers to run.
    // Both command-line options take precedence over inifile settings.
    if (args->optionGiven('c'))
        configName = opp_nulltoempty(args->optionValue('c'));
    if (configName.empty())
        configName = "General";

    if (args->optionGiven('r'))
        runFilter = args->optionValue('r');

    return runParameterStudy(ini, configName.c_str(), runFilter.c_str(), args);
}

int CmdenvSimulationRunner::runParameterStudy(InifileContents *ini, const char *configName, const char *runFilter, ArgList *args)
{
    this->args = args;

    std::vector<int> runNumbers;
    try {
        runNumbers = ini->resolveRunFilter(configName, runFilter);
    }
    catch (std::exception& e) {
        displayException(e);
        return 1;
    }

    if (runNumbers.empty()) {
        std::cout << "No matching simulation run\n"; //TODO refine
        return 1;
    }

    // NED path and the number of threads are decided by the first run; this allows
    // specifying different values for these options in different sections.
    cConfiguration *masterCfg = ini->extractConfig(configName, runNumbers[0]);

    nedLoader = createConfiguredNedLoader(masterCfg);
    nedLoader->loadNedFiles();

    int numThreads = masterCfg->getAsInt(CFGID_CMDENV_NUM_THREADS);
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 0)
            numThreads = 1;
    }
    bool threaded = numThreads != 1;

    delete masterCfg;

    CodeFragments::executeAll(CodeFragments::STARTUP); // app setup is complete

    //TODO important: cSimulation::configure() SHOULD configure simtime-scale and coroutine lib too!!!!

    //TODO also into Qtenv
    const char *fname;
    for (int k = 0; (fname = args->argument(k)) != nullptr; k++) {
        if (opp_stringendswith(fname, ".ini")) {
            // ignore -- already processed
        }
        else if (opp_stringendswith(fname, ".py")) {
#ifdef WITH_PYTHONSIM
            ensurePythonInterpreter();
            FILE *f = fopen(fname, "r");
            if (f) {
                PyRun_AnyFile(f, fname);
                fclose(f);
            }
#else
            throw opp_runtime_error("Cannot process command-line argument '%s': No OMNeT++ was compiled with WITH_PYTHONSIM=no", fname);
#endif
        }
        else {
            throw opp_runtime_error("Cannot process command-line argument '%s': Unknown file extension", fname);
        }
    }

    // implement graceful exit when Ctrl-C is hit during simulation. We want
    // to finish the current event, then normally exit via callFinish() etc
    // so that simulation results are not lost.
    installSigintHandler();

    numRuns = (int)runNumbers.size();
    runsTried = 0;
    numErrors = 0;

    if (!threaded)
        runSimulations(ini, configName, runNumbers);
    else {
        if (verbose)
            out << "Running simulations on " << numThreads << " threads\n";
        Py_BEGIN_ALLOW_THREADS
        runSimulationsInThreads(ini, configName, runNumbers, numThreads);
        Py_END_ALLOW_THREADS
    }

    delete nedLoader;

    if (numRuns > 1 && verbose) {
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

    return numErrors > 0 ? 1 : sigintReceived ? 2 : 0;
}

cINedLoader *CmdenvSimulationRunner::createConfiguredNedLoader(cConfiguration *cfg)
{
    cINedLoader *nedLoader = new cNedLoader("nedLoader");
    nedLoader->removeFromOwnershipTree();
    std::string nArg = args == nullptr ? "" : opp_join(args->optionValues('n'), ";", true);
    std::string xArg = args == nullptr ? "" : opp_join(args->optionValues('x'), ";", true);
    nedLoader->configure(cfg, nArg.c_str() , xArg.c_str());
    return nedLoader;
}

void CmdenvSimulationRunner::runSimulationsInThreads(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers, int numThreads)
{
    // statically assign jobs to threads
    std::vector<std::vector<int>> runListPerThread(numThreads);
    for (int i = 0; i < runNumbers.size(); i++)
        runListPerThread[i % numThreads].push_back(runNumbers[i]);

    // create and launch threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++)
        threads.push_back(startThread(ini, configName, runListPerThread[i]));

    // wait for them to finish
    for (int i = 0; i < numThreads; i++)
        threads[i].join();
}

std::thread CmdenvSimulationRunner::startThread(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers)
{
    auto fn = [this](InifileContents *ini, std::string configName, std::vector<int> runNumbers) {
        runSimulations(ini, configName.c_str(), runNumbers);
    };
    return std::thread(fn, ini, configName, runNumbers);
}

void CmdenvSimulationRunner::runSimulations(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers)
{
    for (int runNumber : runNumbers) {
        runsTried++;

        bool finishedOK = runSimulation(ini, configName, runNumber);
        if (!finishedOK)
            numErrors++;

        // skip further runs if signal was caught
        if (sigintReceived)
            break;

        if (!finishedOK && stopBatchOnError)
            break;
    }
}

bool CmdenvSimulationRunner::runSimulation(InifileContents *ini, const char *configName, int runNumber)
{
    try {
        if (verbose)
            out << "\nPreparing for running configuration " << configName << ", run #" << runNumber << "..." << endl;

        std::unique_ptr<cConfiguration> cfg(ini->extractConfig(configName, runNumber));
        stopBatchOnError = cfg->getAsBool(CFGID_CMDENV_STOP_BATCH_ON_ERROR);

        bool redirectOutput = cfg->getAsBool(CFGID_CMDENV_REDIRECT_OUTPUT);
        std::string outputFile = redirectOutput ? ResultFileUtils(cfg.get()).augmentFileName(cfg->getAsFilename(CFGID_CMDENV_OUTPUT_FILE)) : "";
        cTerminationException *reason = setupAndRunSimulation(cfg.get(), nullptr, redirectOutput ? outputFile.c_str() : nullptr);
        delete reason;
        return true;
    }
    catch (std::exception& e) {
        if (!verbose || useStderr) // if verbose, it was already printed (maybe only in the redirection file though)
            displayException(e);
        return false;
    }
}

static void deleteNetworkOnError(cSimulation *simulation, cRuntimeError& error)
{
    try {
        simulation->deleteNetwork();
    }
    catch (std::exception& e) {
        error.addNestedException(e);
    }
}

inline const char *opp_nulltodefault(const char *s, const char *defaultString)  {return s == nullptr ? defaultString : s;}

cTerminationException *CmdenvSimulationRunner::setupAndRunSimulation(cConfiguration *cfg, cIRunner *runner, const char *redirectFileName)
{
    //TODO more factories? createSimulation(), createEnvir(), createRunner()?

    bool redirectOutput = redirectFileName != nullptr;

    const char *configName = opp_nulltodefault(cfg->getVariable(CFGVAR_CONFIGNAME), "?");
    const char *runNumber = opp_nulltodefault(cfg->getVariable(CFGVAR_RUNNUMBER), "?");
    const char *iterVars = opp_nulltodefault(cfg->getVariable(CFGVAR_ITERATIONVARS), "?");
    const char *runId = opp_nulltodefault(cfg->getVariable(CFGVAR_RUNID), "?");
    const char *repetition = opp_nulltodefault(cfg->getVariable(CFGVAR_REPETITION), "?");
    if (!verbose)
        out << configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << endl; // print before redirection; useful as progress indication from opp_runall

    std::ofstream fout;

    if (redirectOutput) {
        if (verbose)
            out << "Redirecting output to file \"" << redirectFileName << "\"..." << endl;
        mkPath(directoryOf(redirectFileName).c_str());
        fout.open(redirectFileName);
        if (!fout.is_open())
            throw cRuntimeError("Cannot open file '%s' for write", redirectFileName);
        if (verbose)
            //TODO fout << "\nRunning configuration " << configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << "..." << endl;
            fout << "\nRunning configuration " << configName << ", run #" << runNumber << "..." << endl;
    }

    std::ostream& simout = redirectOutput ? fout : out;

    if (verbose) {
        if (!opp_isempty(iterVars))
            simout << "Scenario: " << iterVars << ", $repetition=" << repetition << endl; //TODO redundant, remove
        simout << "Assigned runID=" << runId << endl;  //TODO merge into printouts above
    }

    std::unique_ptr<cSimulation> tmp(createSimulation(simout));
    cSimulation *simulation = tmp.get();

    Runner localRunner(simulation, simout, sigintReceived);
    if (runner == nullptr)
        runner = &localRunner;
    configureRunner(runner, cfg);

    if (verbose)
        simulation->addLifecycleListener(new VerboseListener(simout));

    cSimulation::setActiveSimulation(simulation);

    try {
        simulation->setupNetwork(cfg);

        bool isTerminated = !simulation->run(runner, true);
        if (!isTerminated)
            throw cRuntimeError("Simulation paused before running to completion");

        cTerminationException *terminationReason = simulation->getTerminationReason()->dup();
        if (redirectOutput)
            logException(fout, *terminationReason);
        return terminationReason;
    }
    catch (cRuntimeError& e) {
        deleteNetworkOnError(simulation, e);
        if (redirectOutput)
            logException(fout, e);
        throw;
    }
    catch (std::exception& e) {
        cRuntimeError re(e);
        deleteNetworkOnError(simulation, re);
        if (redirectOutput)
            logException(fout, re);
        throw re;
    }
}

cSimulation *CmdenvSimulationRunner::createSimulation(std::ostream& out)
{
    CmdenvEnvir *envir = new CmdenvEnvir(out, sigintReceived);
    envir->setArgs(args);
    envir->setVerbose(verbose);

    return new cSimulation("simulation", envir, nedLoader);
}

void CmdenvSimulationRunner::configureRunner(cIRunner *irunner, cConfiguration *cfg)
{
    if (Runner *runner = dynamic_cast<Runner*>(irunner)) {
        runner->setExpressMode(cfg->getAsBool(CFGID_CMDENV_EXPRESS_MODE));
        runner->setAutoFlush(cfg->getAsBool(CFGID_CMDENV_AUTOFLUSH));
        runner->setStatusFrequencyMs(1000*cfg->getAsDouble(CFGID_CMDENV_STATUS_FREQUENCY));
        runner->setPrintPerformanceData(cfg->getAsBool(CFGID_CMDENV_PERFORMANCE_DISPLAY));
        runner->setPrintThreadId(false); //TODO
        runner->setPrintEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNERS));
        runner->setDetailedEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNER_DETAILS));
        runner->setBatchProgress(runsTried, numRuns);
    }
}

void CmdenvSimulationRunner::sigintHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
        sigintReceived = true;
}

void CmdenvSimulationRunner::installSigintHandler()
{
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigintHandler);
}

void CmdenvSimulationRunner::deinstallSigintHandler()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

void CmdenvSimulationRunner::displayException(std::exception& ex)
{
    std::string msg = cException::getFormattedMessage(ex);
    std::ostream& os = (cException::isError(ex) && useStderr) ? std::cerr : out;
    os << "\n<!> " << msg << endl << endl;
}

void CmdenvSimulationRunner::alert(const char *msg)
{
    std::ostream& err = useStderr ? std::cerr : out;
    err << "\n<!> " << msg << endl << endl;
}

void CmdenvSimulationRunner::logException(std::ostream& out, std::exception& ex)
{
    std::string msg = cException::getFormattedMessage(ex);
    out << "\n<!> " << msg << endl << endl;
}


}  // namespace cmdenv
}  // namespace omnetpp

