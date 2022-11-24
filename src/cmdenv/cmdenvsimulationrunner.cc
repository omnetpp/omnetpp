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
#include "envir/genericenvir.h"
#include "envir/genericeventlooprunner.h"
#include "envir/inifilecontents.h"
#include "envir/resultfileutils.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/ceventlooprunner.h"
#include "sim/netbuilder/cnedloader.h"
#include "cmdenvsimulationrunner.h"
#include "cmdenvnarrator.h"
#include "cmdenvenvir.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

#ifdef WITH_PYTHONSIM
#include <Python.h>
#else
#define Py_BEGIN_ALLOW_THREADS  /*no-op*/
#define Py_END_ALLOW_THREADS    /*no-op*/
#endif

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

// Used for graceful exit when Ctrl-C is hit during simulation. We want to finish the
// current event, then normally exit via callFinish() so that simulation results are not lost.
bool CmdenvSimulationRunner::sigintReceived;


CmdenvSimulationRunner::CmdenvSimulationRunner(std::ostream& out, ArgList *args) : out(out), args(args)
{
    homeThreadId = std::this_thread::get_id();
    narrator = new CmdenvNarrator(out);
}

CmdenvSimulationRunner::~CmdenvSimulationRunner()
{
    delete nedLoader;
    delete narrator;
}

int CmdenvSimulationRunner::runCmdenv(InifileContents *ini)
{
    try {
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

        auto result = runParameterStudy(ini, configName.c_str(), runFilter.c_str());
        return sigintReceived ? 2 : result.numErrors > 0 ? 1 : 0;
    }
    catch (std::exception& e) {
        narrator->displayException(e);
        return 1;
    }
}

CmdenvSimulationRunner::BatchResult CmdenvSimulationRunner::runParameterStudy(InifileContents *ini, const char *configName, const char *runFilter)
{
    std::vector<int> runNumbers = ini->resolveRunFilter(configName, runFilter);
    if (runNumbers.empty())
        throw cRuntimeError("Run filter '%s' does not match any run in config '%s'", runFilter, configName);

    cConfiguration *masterCfg = ini->extractConfig(configName, runNumbers[0]);
    int numThreads = masterCfg->getAsInt(CFGID_CMDENV_NUM_THREADS);
    delete masterCfg;

    bool threaded = numThreads != 1;

    BatchResult result;
    result.numRuns = (int)runNumbers.size();

    if (!threaded)
        result = runSimulations(ini, configName, runNumbers); // does not throw
    else
        result = runSimulationsInThreads(ini, configName, runNumbers, numThreads); // does not throw

    narrator->summary(result.numRuns, result.runsTried, result.numErrors);

    return result;
}

void CmdenvSimulationRunner::ensureNedLoader(cConfiguration *cfg)
{
    if (nedLoader == nullptr) {
        nedLoader = createConfiguredNedLoader(cfg);
        nedLoader->loadNedFiles();
    }
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

CmdenvSimulationRunner::BatchResult CmdenvSimulationRunner::runSimulationsInThreads(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers, int numThreads)
{
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 0)
            numThreads = 1;
    }

    cConfiguration *firstCfg = ini->extractConfig(configName, runNumbers[0]);
    ensureNedLoader(firstCfg);
    delete firstCfg;

    // statically assign jobs to threads
    std::vector<std::vector<int>> runListPerThread(numThreads);
    for (int i = 0; i < runNumbers.size(); i++)
        runListPerThread[i % numThreads].push_back(runNumbers[i]);

    narrator->usingThreads(numThreads);

    BatchState state;

    Py_BEGIN_ALLOW_THREADS

    // create and launch threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; i++) {
        auto fn = [this](BatchState *state, InifileContents *ini, std::string configName, std::vector<int> runNumbers) {
            doRunSimulations(*state, ini, configName.c_str(), runNumbers);
        };
        threads.push_back(std::thread(fn, &state, ini, configName, runListPerThread[i]));
    }

    // wait for them to finish
    for (int i = 0; i < numThreads; i++)
        threads[i].join();

    Py_END_ALLOW_THREADS

    return extractResult(state);
}

CmdenvSimulationRunner::BatchResult CmdenvSimulationRunner::runSimulations(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers)
{
    BatchState state;
    doRunSimulations(state, ini, configName, runNumbers);
    return extractResult(state);
}

void CmdenvSimulationRunner::doRunSimulations(BatchState& state, InifileContents *ini, const char *configName, const std::vector<int>& runNumbers)
{
    state.numRuns = (int)runNumbers.size();
    for (int runNumber : runNumbers) {
        try {
            state.runsTried++;
            doRunSimulation(state, ini, configName, runNumber);
            state.numCompleted++;
        }
        catch (std::exception& e) {
            narrator->displayException(e);  // note: must take care not to print again if it was already printed
            state.numErrors++;
            if (state.stopBatchOnError)
                break;
        }

        // skip further runs if signal was caught
        if (sigintReceived)
            break;
    }
}

void CmdenvSimulationRunner::runSimulation(InifileContents *ini, const char *configName, int runNumber)
{
    BatchState state;
    doRunSimulation(state, ini, configName, runNumber);
}

void CmdenvSimulationRunner::doRunSimulation(BatchState& state, InifileContents *ini, const char *configName, int runNumber)
{
    narrator->preparing(configName, runNumber);

    std::unique_ptr<cConfiguration> cfg(ini->extractConfig(configName, runNumber));
    cTerminationException *reason = setupAndRunSimulation(state, cfg.get());
    delete reason;
}

cTerminationException *CmdenvSimulationRunner::setupAndRunSimulation(BatchState& state, cConfiguration *cfg)
{
    state.stopBatchOnError = cfg->getAsBool(CFGID_CMDENV_STOP_BATCH_ON_ERROR);

    bool redirectOutput = cfg->getAsBool(CFGID_CMDENV_REDIRECT_OUTPUT);
    std::string outputFile = redirectOutput ? ResultFileUtils(cfg).augmentFileName(cfg->getAsFilename(CFGID_CMDENV_OUTPUT_FILE)) : "";
    const char *redirectFileName = outputFile.c_str();

    narrator->beforeRedirecting(cfg);
    std::ofstream fout;

    if (redirectOutput) {
        narrator->redirectingTo(cfg, redirectFileName);
        mkPath(directoryOf(redirectFileName).c_str());
        fout.open(redirectFileName);
        if (!fout.is_open())
            throw cRuntimeError("Cannot open file '%s' for write", redirectFileName);
        narrator->onRedirectionFileOpen(fout, cfg, redirectFileName);
    }

    std::ostream& simout = redirectOutput ? fout : out;

    narrator->afterRedirecting(cfg, fout);

    ensureNedLoader(cfg);

    std::unique_ptr<cSimulation> tmp(createSimulation(simout));
    cSimulation *simulation = tmp.get();

    std::unique_ptr<cIEventLoopRunner> tmp2(createEventLoopRunner(state, simulation, simout, cfg));
    cIEventLoopRunner *runner = tmp2.get();

    narrator->simulationCreated(simulation, fout);

    cSimulation::setActiveSimulation(simulation);

    //TODO some errors logged from lifecycle listener, but not those thrown directly! -- DO NOT LOG VIA LIFECYCLE LISTENER
    try {
        simulation->setupNetwork(cfg);

        bool isTerminated = !simulation->run(runner, true);
        if (!isTerminated)
            throw cRuntimeError("Simulation paused before running to completion");

        cTerminationException *terminationReason = simulation->getTerminationReason()->dup();
        if (redirectOutput)
            narrator->logException(fout, *terminationReason);  //TODO why not from listener?

        simulation->deleteNetwork();  // note: without this, exceptions during teardown would be swallowed by cSimulation dtor

        return terminationReason;
    }
    catch (cRuntimeError& e) {
        simulation->deleteNetworkOnError(e);
        if (redirectOutput)
            narrator->logException(fout, e);
        throw;
    }
    catch (std::exception& e) {
        cRuntimeError re(e);
        simulation->deleteNetworkOnError(re);
        if (redirectOutput)
            narrator->logException(fout, re);
        throw re;
    }
}

cSimulation *CmdenvSimulationRunner::createSimulation(std::ostream& simout)
{
    CmdenvEnvir *envir = new CmdenvEnvir(simout, sigintReceived);
    envir->setArgs(args);

    return new cSimulation("simulation", envir, nedLoader);
}

cIEventLoopRunner *CmdenvSimulationRunner::createEventLoopRunner(BatchState& state, cSimulation *simulation, std::ostream& simout, cConfiguration *cfg)
{
    GenericEventLoopRunner *runner = new GenericEventLoopRunner(simulation, simout, sigintReceived);
    runner->setExpressMode(cfg->getAsBool(CFGID_CMDENV_EXPRESS_MODE));
    runner->setAutoFlush(cfg->getAsBool(CFGID_CMDENV_AUTOFLUSH));
    runner->setStatusFrequencyMs(1000*cfg->getAsDouble(CFGID_CMDENV_STATUS_FREQUENCY));
    runner->setPrintPerformanceData(cfg->getAsBool(CFGID_CMDENV_PERFORMANCE_DISPLAY));
    runner->setPrintThreadId(std::this_thread::get_id() != homeThreadId);
    runner->setPrintEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNERS));
    runner->setDetailedEventBanners(cfg->getAsBool(CFGID_CMDENV_EVENT_BANNER_DETAILS));
    runner->setBatchProgress(state.runsTried, state.numRuns);
    return runner;
}

CmdenvSimulationRunner::BatchResult CmdenvSimulationRunner::extractResult(const BatchState& state)
{
    BatchResult result;
    result.numRuns = state.numRuns;
    result.runsTried = state.runsTried;
    result.numCompleted = state.numCompleted;
    result.numInterrupted = state.numInterrupted;
    result.numErrors = state.numErrors;
    return result;
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


}  // namespace cmdenv
}  // namespace omnetpp

