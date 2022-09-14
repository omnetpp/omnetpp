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

extern cConfigOption *CFGID_NETWORK;

namespace envir {

Register_GlobalConfigOption(CFGID_DEBUG_STATISTICS_RECORDING, "debug-statistics-recording", CFG_BOOL, "false", "Turns on the printing of debugging information related to statistics recording (`@statistic` properties)");

inline EnvirBase *getActiveEnvir() {return dynamic_cast<EnvirBase*>(cSimulation::getActiveEnvir());}
inline cSimulation *getActiveSimulation() {return cSimulation::getActiveSimulation();}

cINedLoader *SimulationHolder::createConfiguredNedLoader(cConfiguration *cfg, ArgList *args)
{
    cINedLoader *nedLoader = new cNedLoader("nedLoader");
    nedLoader->removeFromOwnershipTree();
    std::string nArg = args == nullptr ? "" : opp_join(args->optionValues('n'), ";", true);
    std::string xArg = args == nullptr ? "" : opp_join(args->optionValues('x'), ";", true);
    nedLoader->configure(cfg, nArg.c_str() , xArg.c_str());
    return nedLoader;
}

void SimulationHolder::setupNetwork(cModuleType *networkType)
{
    EnvirBase *envir = getActiveEnvir();
    cSimulation *simulation = getActiveSimulation();

    envir->clearCurrentEventInfo();

    simulation->setupNetwork(networkType);
    envir->getEventlogManager()->flush();

    bool debugStatisticsRecording = envir->getConfig()->getAsBool(CFGID_DEBUG_STATISTICS_RECORDING);
    if (debugStatisticsRecording)
        EnvirUtils::dumpResultRecorders(out, simulation->getSystemModule());
}

void SimulationHolder::configureAndRunSimulation(cSimulation *simulation, cConfiguration *cfg, IRunner *runner, const char *redirectFileName)
{
    simulation->configure(cfg);
    runConfiguredSimulation(simulation, runner, redirectFileName);
}

void SimulationHolder::runConfiguredSimulation(cSimulation *simulation, IRunner *runner, const char *redirectFileName)  //TODO misleading name (the network is not actually set up yet)
{
    cSimulation::setActiveSimulation(simulation);

    bool endRunRequired = false;

    try {
        cConfiguration *cfg = simulation->getConfig();

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

        // find network
        std::string networkName = cfg->getAsString(CFGID_NETWORK);
        std::string inifileNetworkDir  = cfg->getConfigEntry(CFGID_NETWORK->getName()).getBaseDirectory();
        if (networkName.empty())
            throw cRuntimeError("No network specified (missing or empty network= configuration option)");
        cModuleType *network = simulation->resolveNetwork(networkName.c_str(), inifileNetworkDir.c_str());
        ASSERT(network);

        endRunRequired = true;

        // set up network
        if (verbose)
            out << "Setting up network \"" << networkName.c_str() << "\"..." << endl;

        setupNetwork(network);

        // prepare for simulation run
        if (verbose)
            out << "Initializing..." << endl;

        cEnvir *envir = simulation->getEnvir();
        cLog::setLoggingEnabled(!envir->isExpressMode());

        simulation->callInitialize();
        cLogProxy::flushLastLine();

        // run the simulation
        if (verbose)
            out << "\nRunning simulation..." << endl;

        simulation->run(runner, false);

        bool isTerminated = simulation->getState() == cSimulation::SIM_TERMINATED;
        if (isTerminated) {
            cTerminationException *e = simulation->getTerminationReason();
            printException(*e);  // must display the exception here (and not inside catch), so that it doesn't appear out-of-order in the output
            terminationReason = e;
        }

        cLog::setLoggingEnabled(true);

        if (verbose)
            out << "\nCalling finish() at end of Run #" << runNumber << "..." << endl;

        simulation->callFinish();
        cLogProxy::flushLastLine();
    }
    catch (std::exception& e) {
        cLog::setLoggingEnabled(true);
        printException(e);  // must display the exception here (and not inside catch), so that it doesn't appear out-of-order in the output
        afterRunFinally(simulation, endRunRequired);
        throw;
    }
    afterRunFinally(simulation, endRunRequired);
}

void SimulationHolder::afterRunFinally(cSimulation *simulation, bool endRunRequired)
{
    //TODO should any exception in here cause nonzero Cmdenv exit code?

    try {
        simulation->deleteNetwork();
    }
    catch (std::exception& e) {
        printException(e, "during cleanup");
    }

    // stop redirecting into file
    if (isOutputRedirected())
        stopOutputRedirection();
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

std::ostream& SimulationHolder::err()
{
    std::ostream& err = useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (useStderr ? std::cerr : std::cout) << "<!> Error -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> Error: ";
    return err;
}

std::ostream& SimulationHolder::errWithoutPrefix()
{
    std::ostream& err = useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (useStderr ? std::cerr : std::cout) << "<!> Error -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> ";
    return err;
}

std::ostream& SimulationHolder::warn()
{
    std::ostream& err = useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (useStderr ? std::cerr : std::cout) << "<!> Warning -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> Warning: ";
    return err;
}

std::string SimulationHolder::getFormattedMessage(std::exception& ex)
{
    if (cException *e = dynamic_cast<cException *>(&ex))
        return e->getFormattedMessage();
    else
        return ex.what();
}

void SimulationHolder::printException(std::exception& ex, const char *when)
{
    std::string msg = getFormattedMessage(ex);
    if (!opp_isempty(when))
        msg = std::string("Error ") + when + ": " + msg;
    if (dynamic_cast<cTerminationException*>(&ex) != nullptr)
        out << endl << "<!> " << msg << endl;
    else if (msg.substr(0,5) == "Error")
        errWithoutPrefix() << msg << endl;
    else
        err() << msg << endl;
}

}  // namespace envir
}  // namespace omnetpp
