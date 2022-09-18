//==========================================================================
//  CMDENVSIMHOLDER.CC - part of
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

#include "envir/args.h"
#include "envir/resultfileutils.h"
#include "envir/inifilecontents.h"
#include "envir/runner.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/crunner.h"
#include "omnetpp/checkandcast.h"
#include "cmddefs.h"
#include "cmdenv.h"
#include "cmdenvir.h"
#include "cmdenvsimholder.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOption(CFGID_CMDENV_OUTPUT_FILE, "cmdenv-output-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.out", "When `cmdenv-record-output=true`: file name to redirect standard output to. See also `fname-append-host`.")
Register_GlobalConfigOption(CFGID_CMDENV_REDIRECT_OUTPUT, "cmdenv-redirect-output", CFG_BOOL, "false", "Causes Cmdenv to redirect standard output of simulation runs to a file or separate files per run. This option can be useful with running simulation campaigns (e.g. using opp_runall), and also with parallel simulation. See also: `cmdenv-output-file`, `fname-append-host`.");
Register_GlobalConfigOption(CFGID_CMDENV_EXPRESS_MODE, "cmdenv-express-mode", CFG_BOOL, "true", "Selects normal (debug/trace) or express mode.")
Register_GlobalConfigOption(CFGID_CMDENV_AUTOFLUSH, "cmdenv-autoflush", CFG_BOOL, "false", "Call `fflush(stdout)` after each event banner or status update; affects both express and normal mode. Turning on autoflush may have a performance penalty, but it can be useful with printf-style debugging for tracking down program crashes.")
Register_GlobalConfigOption(CFGID_CMDENV_EVENT_BANNERS, "cmdenv-event-banners", CFG_BOOL, "true", "When `cmdenv-express-mode=false`: turns printing event banners on/off.")
Register_GlobalConfigOption(CFGID_CMDENV_EVENT_BANNER_DETAILS, "cmdenv-event-banner-details", CFG_BOOL, "false", "When `cmdenv-express-mode=false`: print extra information after event banners.")
Register_GlobalConfigOptionU(CFGID_CMDENV_STATUS_FREQUENCY, "cmdenv-status-frequency", "s", "2s", "When `cmdenv-express-mode=true`: print status update every n seconds.")
Register_GlobalConfigOption(CFGID_CMDENV_PERFORMANCE_DISPLAY, "cmdenv-performance-display", CFG_BOOL, "true", "When `cmdenv-express-mode=true`: print detailed performance information. Turning it on results in a 3-line entry printed on each update, containing ev/sec, simsec/sec, ev/simsec, number of messages created/still present/currently scheduled in FES.")

CmdenvSimulationHolder::CmdenvSimulationHolder(Cmdenv *app) :
        CmdenvSimulationHolder(app->getOutputStream(), app->nedLoader, app->sigintReceived)
{
    setVerbose(app->verbose);
    setUseStderr(app->useStderr);
}

CmdenvSimulationHolder::CmdenvSimulationHolder(std::ostream& out, cINedLoader *nedLoader, bool& sigintReceived) :
        SimulationHolder(out, nedLoader), sigintReceived(sigintReceived)
{
}

static bool sigintReceivedDummy = false;

CmdenvSimulationHolder::CmdenvSimulationHolder(cINedLoader *nedLoader) :
        CmdenvSimulationHolder(std::cout, nedLoader, sigintReceivedDummy)
{
}

cSimulation *CmdenvSimulationHolder::createSimulation()
{
    CmdEnvir *envir = new CmdEnvir(out, sigintReceived);
    return new cSimulation("simulation", envir, nedLoader);
}

void CmdenvSimulationHolder::configureRunner(IRunner *irunner, cConfiguration *cfg)
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

void CmdenvSimulationHolder::setupAndRunSimulation(cSimulation *simulation, cConfiguration *cfg, IRunner *runner, const char *redirectFileName)
{
    Runner localRunner(simulation, out, sigintReceived);
    if (runner == nullptr)
        runner = &localRunner;
    configureRunner(runner, cfg);

    std::string cmdenvOutputFile;
    if (redirectFileName == nullptr && cfg->getAsBool(CFGID_CMDENV_REDIRECT_OUTPUT)) {
        cmdenvOutputFile = cfg->getAsFilename(CFGID_CMDENV_OUTPUT_FILE);
        cmdenvOutputFile = ResultFileUtils(cfg).augmentFileName(cmdenvOutputFile);
        redirectFileName = cmdenvOutputFile.c_str();
    }

    SimulationHolder::setupAndRunSimulation(simulation, cfg, runner, redirectFileName);
}

}  // namespace cmdenv
}  // namespace omnetpp

