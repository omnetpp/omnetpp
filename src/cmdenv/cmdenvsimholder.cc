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
#include "omnetpp/cconfigoption.h"
#include "cmddefs.h"
#include "cmdenv.h"
#include "cmdenvir.h"
#include "cmdenvsimholder.h"

#include "cmdenvrunner.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

Register_GlobalConfigOption(CFGID_CMDENV_OUTPUT_FILE, "cmdenv-output-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.out", "When `cmdenv-record-output=true`: file name to redirect standard output to. See also `fname-append-host`.")
Register_GlobalConfigOption(CFGID_CMDENV_REDIRECT_OUTPUT, "cmdenv-redirect-output", CFG_BOOL, "false", "Causes Cmdenv to redirect standard output of simulation runs to a file or separate files per run. This option can be useful with running simulation campaigns (e.g. using opp_runall), and also with parallel simulation. See also: `cmdenv-output-file`, `fname-append-host`.");

CmdenvSimulationHolder::CmdenvSimulationHolder(Cmdenv *app) :
        CmdenvSimulationHolder(app->getOutputStream(), app->nedLoader, app->sigintReceived)
{
    setVerbose(app->verbose);
    setUseStderr(app->useStderr);
}

CmdenvSimulationHolder::CmdenvSimulationHolder(std::ostream& out, cINedLoader *nedLoader, bool& sigintReceived) :
        SimulationHolder(out), nedLoader(nedLoader), sigintReceived(sigintReceived)
{
}

static bool sigintReceivedDummy = false;

CmdenvSimulationHolder::CmdenvSimulationHolder(cINedLoader *nedLoader) :
        CmdenvSimulationHolder(std::cout, nedLoader, sigintReceivedDummy)
{
}

void CmdenvSimulationHolder::runCmdenvSimulation(cConfiguration *cfg, int numRuns, int runsTried)
{
    CmdEnvir *envir = new CmdEnvir(out, sigintReceived);
    cSimulation *simulation = new cSimulation("simulation", envir);
    simulation->setNedLoader(nedLoader);

    CmdenvRunner runner(simulation, out, sigintReceived);
    runner.configure(cfg);
    runner.setBatchProgress(runsTried, numRuns);

    bool redirectOutput = cfg->getAsBool(CFGID_CMDENV_REDIRECT_OUTPUT);
    std::string outputFile;
    if (redirectOutput) {
        outputFile = cfg->getAsFilename(CFGID_CMDENV_OUTPUT_FILE);
        outputFile = ResultFileUtils(cfg).augmentFileName(outputFile);
    }

    std::unique_ptr<cSimulation> deleter(simulation);

    configureAndRunSimulation(simulation, cfg, &runner, redirectOutput ? outputFile.c_str() : nullptr);
}

}  // namespace cmdenv
}  // namespace omnetpp

