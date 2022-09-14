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

#include "envir/appreg.h"
#include "envir/inifilecontents.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cnedloader.h"
#include "cmdenv.h"
#include "cmdenvir.h"
#include "cmdenvsimholder.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOption(CFGID_CMDENV_CONFIG_NAME, "cmdenv-config-name", CFG_STRING, nullptr, "Specifies the name of the configuration to be run (for a value `Foo`, section `[Config Foo]` will be used from the ini file). See also `cmdenv-runs-to-execute`. The `-c` command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_CMDENV_RUNS_TO_EXECUTE, "cmdenv-runs-to-execute", CFG_STRING, nullptr, "Specifies which runs to execute from the selected configuration (see `cmdenv-config-name` option). It accepts a filter expression of iteration variables such as `$numHosts>10 && $iatime==1s`, or a comma-separated list of run numbers or run number ranges, e.g. `1,3..4,7..9`. If the value is missing, Cmdenv executes all runs in the selected configuration. The `-r` command line option overrides this setting.")
Register_GlobalConfigOption(CFGID_CMDENV_STOP_BATCH_ON_ERROR, "cmdenv-stop-batch-on-error", CFG_BOOL, "true", "Decides whether Cmdenv should skip the rest of the runs when an error occurs during the execution of one run.")
Register_GlobalConfigOption(CFGID_CMDENV_NUM_THREADS, "cmdenv-num-threads", CFG_INT, "1", "Specifies the number of threads to use when running multiple simulations is requested. (Each simulation will still run sequentially in its thread.) When -1 is given, the number of concurrent threads supported by the hardware will be used.");

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

Cmdenv::Cmdenv()
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it
}

Cmdenv::~Cmdenv()
{
}

void Cmdenv::printUISpecificHelp()
{
    out << "Cmdenv-specific information:\n";
    out << "    Cmdenv executes all runs denoted by the -c and -r options. The number\n";
    out << "    of runs executed and the number of runs that ended with an error are\n";
    out << "    reported at the end.\n";
    out << endl;
}

int Cmdenv::doRunApp()
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

    std::vector<int> runNumbers;
    try {
        runNumbers = resolveRunFilter(configName.c_str(), runFilter.c_str());
    }
    catch (std::exception& e) {
        displayException(e);
        return 1;
    }

    if (runNumbers.empty()) {
        std::cout << "No matching simulation run, exiting without simulation\n"; //TODO refine
        return 1;
    }

    // NED path and the number of threads are decided by the first run; this allows
    // specifying different values for these options in different sections.
    cConfiguration *masterCfg = ini->extractConfig(configName.c_str(), runNumbers[0]);

    nedLoader = SimulationHolder::createConfiguredNedLoader(masterCfg, args);
    nedLoader->loadNedFiles();

    int numThreads = globalCfg->getAsInt(CFGID_CMDENV_NUM_THREADS);
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads <= 0)
            numThreads = 1;
    }
    bool threaded = numThreads != 1;

    delete masterCfg;

    CodeFragments::executeAll(CodeFragments::STARTUP); // app setup is complete

    //TODO important: cSimulation::configure() SHOULD configure simtime-scale and coroutine lib too!!!!

    // implement graceful exit when Ctrl-C is hit during simulation. We want
    // to finish the current event, then normally exit via callFinish() etc
    // so that simulation results are not lost.
    installSigintHandler();

    numRuns = (int)runNumbers.size();
    runsTried = 0;
    numErrors = 0;

    if (!threaded)
        runSimulations(configName.c_str(), runNumbers);
    else {
        if (verbose)
            out << "Running simulations on " << numThreads << " threads\n";
        runSimulationsInThreads(configName.c_str(), runNumbers, numThreads);
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

        if (!finishedOK && stopBatchOnError)
            break;
    }
}

bool Cmdenv::runSimulation(const char *configName, int runNumber)
{
    try {
        if (verbose)
            out << "\nPreparing for running configuration " << configName << ", run #" << runNumber << "..." << endl;

        cConfiguration *cfg = ini->extractConfig(configName, runNumber);
        std::unique_ptr<cConfiguration> deleter(cfg);
        stopBatchOnError = cfg->getAsBool(CFGID_CMDENV_STOP_BATCH_ON_ERROR);

        CmdenvSimulationHolder holder(this);
        holder.runCmdenvSimulation(cfg, (int)runsTried, (int)numRuns);
        return true;
    }
    catch (std::exception& e) {
        displayException(e); //TODO may be duplicate -- if already displayed inside the run call
        return false;
    }
}

void Cmdenv::sigintHandler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM)
        sigintReceived = true;
}

void Cmdenv::installSigintHandler()
{
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigintHandler);
}

void Cmdenv::deinstallSigintHandler()
{
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
}

void Cmdenv::alert(const char *msg)
{
    std::ostream& err = useStderr ? std::cerr : out;
    err << "\n<!> " << msg << endl << endl;
}

}  // namespace cmdenv
}  // namespace omnetpp

