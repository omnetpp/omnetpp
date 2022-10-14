//==========================================================================
//  SIMULATIONHOLDER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <fstream>
#include <sstream>
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cnedloader.h"
#include "omnetpp/crunner.h"
#include "omnetpp/checkandcast.h"
#include "sim/netbuilder/cnedloader.h"
#include "appbase.h"
#include "args.h"
#include "envirbase.h"
#include "envirutils.h"
#include "runner.h"
#include "simulationholder.h"

#ifdef WITH_PARSIM
#include "sim/parsim/cparsimpartition.h"
#include "sim/parsim/creceivedexception.h"
#endif

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

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
    case LF_PRE_NETWORK_FINISH: out << "\nCalling finish() at end of Run #" << cSimulation::getActiveSimulation()->getConfig()->getVariable(CFGVAR_RUNNUMBER) << "..." << endl;
    default: break;
    }
}

void VerboseListener::listenerRemoved()
{
    cISimulationLifecycleListener::listenerRemoved();
    delete this;
}

//---

cINedLoader *SimulationHolder::createConfiguredNedLoader(cConfiguration *cfg, ArgList *args)
{
    cINedLoader *nedLoader = new cNedLoader("nedLoader");
    nedLoader->removeFromOwnershipTree();
    std::string nArg = args == nullptr ? "" : opp_join(args->optionValues('n'), ";", true);
    std::string xArg = args == nullptr ? "" : opp_join(args->optionValues('x'), ";", true);
    nedLoader->configure(cfg, nArg.c_str() , xArg.c_str());
    return nedLoader;
}

void SimulationHolder::setupAndRunSimulation(cConfiguration *cfg, IRunner *runner, const char *redirectFileName)
{
    std::unique_ptr<cSimulation> simulation(createSimulation());
    setupAndRunSimulation(simulation.get(), cfg, runner, redirectFileName);
}

cSimulation *SimulationHolder::createSimulation()
{
    return new cSimulation("simulation", nedLoader);
}

void SimulationHolder::setupAndRunSimulation(cSimulation *simulation, cConfiguration *cfg, IRunner *runner, const char *redirectFileName)
{
    try {
        const char *configName = cfg->getVariable(CFGVAR_CONFIGNAME);
        const char *runNumber = cfg->getVariable(CFGVAR_RUNNUMBER);
        const char *iterVars = cfg->getVariable(CFGVAR_ITERATIONVARS);
        const char *runId = cfg->getVariable(CFGVAR_RUNID);
        const char *repetition = cfg->getVariable(CFGVAR_REPETITION);
        if (!verbose)
            out << configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << endl; // print before redirection; useful as progress indication from opp_runall

        if (redirectFileName) {
            if (verbose)
                out << "Redirecting output to file \"" << redirectFileName << "\"..." << endl;
            startOutputRedirection(redirectFileName);
            if (verbose)
                out << "\nRunning configuration " << configName << ", run #" << runNumber << "..." << endl;
        }

        if (verbose) {
            if (!opp_isempty(iterVars))
                out << "Scenario: " << iterVars << ", $repetition=" << repetition << endl;
            out << "Assigned runID=" << runId << endl;
        }

        // set up and run simulation
        if (verbose)
            simulation->addLifecycleListener(new VerboseListener(out));

        cSimulation::setActiveSimulation(simulation);

        simulation->setupNetwork(cfg);
        bool isTerminated = !simulation->run(runner, false);
        if (isTerminated) {
            cTerminationException *e = simulation->getTerminationReason();
            terminationReason = e->dup();
        }

        simulation->callFinish();

        simulation->deleteNetwork();

        if (isOutputRedirected())
            logException(*terminationReason);
        if (isOutputRedirected())
            stopOutputRedirection();
    }
    catch (cRuntimeError& e) {
        deleteNetworkOnError(simulation, e);
        if (isOutputRedirected())
            logException(e);
        if (isOutputRedirected())
            stopOutputRedirection();
        throw;
    }
    catch (std::exception& e) {
        cRuntimeError re(e);
        deleteNetworkOnError(simulation, re);
        if (isOutputRedirected())
            logException(re);
        if (isOutputRedirected())
            stopOutputRedirection();
        throw re;
    }
}

void SimulationHolder::deleteNetworkOnError(cSimulation *simulation, cRuntimeError& error)
{
    try {
        simulation->deleteNetwork();
    }
    catch (std::exception& e) {
        error.addNestedException(e);
    }
}

void SimulationHolder::startOutputRedirection(const char *fileName)
{
    ASSERT(!isOutputRedirected());

    mkPath(directoryOf(fileName).c_str());

    std::filebuf *fbuf = new std::filebuf;
    fbuf->open(fileName, std::ios_base::out);
    if (!fbuf->is_open())
       throw cRuntimeError("Cannot open output redirection file '%s'", fileName);
    out.rdbuf(fbuf);
    redirectionFilename = fileName;
}

void SimulationHolder::stopOutputRedirection()
{
    ASSERT(isOutputRedirected());
    std::streambuf *fbuf = out.rdbuf();
    fbuf->pubsync();
    out.rdbuf(std::cout.rdbuf());
    delete fbuf;
    redirectionFilename = "";
}

bool SimulationHolder::isOutputRedirected()
{
    return out.rdbuf() != std::cout.rdbuf();
}

void SimulationHolder::logException(std::exception& ex)
{
    std::string msg = dynamic_cast<cException *>(&ex) ? ((cException*)(&ex))->getFormattedMessage() : ex.what();  //TODO override what()?
    if (dynamic_cast<cTerminationException*>(&ex) == nullptr && msg.substr(0,5) != "Error")
        msg = "Error: " + msg;
    out << "\n<!> " << msg << endl << endl;
}


}  // namespace envir
}  // namespace omnetpp
