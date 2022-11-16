//==========================================================================
//  CMDENVNARRATOR.CC - part of
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

#include "cmdenvnarrator.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/ccomponenttype.h"

namespace omnetpp {
namespace cmdenv {

void ICmdenvNarrator::simulationCreated(cSimulation *simulation, std::ofstream& fout)
{
    // note: several simulations may be running concurrently!
    VerboseListener *listener = new VerboseListener(this, fout);
    simulation->addLifecycleListener(listener);
}

void CmdenvNarrator::usingThreads(int numThreads)
{
    if (verbose)
        out << "Running simulations on " << numThreads << " threads\n";
}

void CmdenvNarrator::preparing(const char *configName, int runNumber)
{
    if (verbose)
        out << "\nPreparing for running configuration " << configName << ", run #" << runNumber << "..." << endl;
}

void CmdenvNarrator::summary(int numRuns, int runsTried, int numErrors)
{
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
}

inline const char *opp_nulltodefault(const char *s, const char *defaultString)  {return s == nullptr ? defaultString : s;}

void CmdenvNarrator::beforeRedirecting(cConfiguration *cfg)
{
    const char *configName = opp_nulltodefault(cfg->getVariable(CFGVAR_CONFIGNAME), "?");
    const char *runNumber = opp_nulltodefault(cfg->getVariable(CFGVAR_RUNNUMBER), "?");
    const char *iterVars = opp_nulltodefault(cfg->getVariable(CFGVAR_ITERATIONVARS), "?");
    const char *runId = opp_nulltodefault(cfg->getVariable(CFGVAR_RUNID), "?");
    const char *repetition = opp_nulltodefault(cfg->getVariable(CFGVAR_REPETITION), "?");
    if (!verbose)
        out << configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << " --> runID=" << runId << endl; // print before redirection; useful as progress indication from opp_runall
}

void CmdenvNarrator::redirectingTo(cConfiguration *cfg, const char *redirectFileName)
{
    if (verbose)
        out << "Redirecting output to file \"" << redirectFileName << "\"..." << endl;
}

void CmdenvNarrator::onRedirectionFileOpen(std::ostream& fout, cConfiguration *cfg, const char *redirectFileName)
{
    const char *configName = opp_nulltodefault(cfg->getVariable(CFGVAR_CONFIGNAME), "?");
    const char *runNumber = opp_nulltodefault(cfg->getVariable(CFGVAR_RUNNUMBER), "?");
    const char *iterVars = opp_nulltodefault(cfg->getVariable(CFGVAR_ITERATIONVARS), "?");
    const char *runId = opp_nulltodefault(cfg->getVariable(CFGVAR_RUNID), "?");
    const char *repetition = opp_nulltodefault(cfg->getVariable(CFGVAR_REPETITION), "?");
    if (verbose)
        fout << "\nRunning configuration " << configName << " run " << runNumber << ": " << iterVars << ", $repetition=" << repetition << " --> runID=" << runId << endl;
}

void CmdenvNarrator::simulationLifecycleEvent(cSimulation *simulation, std::ofstream& fout, SimulationLifecycleEventType eventType, cObject *details)
{
    if (verbose) {
        std::ostream& simout = fout.is_open() ? fout : out;
        switch (eventType) {
            case LF_PRE_NETWORK_SETUP: simout << "Setting up network \"" << check_and_cast<cModuleType *>(details)->getFullName() << "\"..." << endl; break;
            case LF_PRE_NETWORK_INITIALIZE: simout << "Initializing..." << endl; break;
            case LF_ON_SIMULATION_START: simout << "\nRunning simulation..." << endl; break;
            case LF_PRE_NETWORK_FINISH: simout << "\nCalling finish() at end of Run #" << simulation->getConfig()->getVariable(CFGVAR_RUNNUMBER) << "..." << endl; break;
            case LF_ON_SIMULATION_SUCCESS: simout << "\n<!> " << check_and_cast<cException*>(details)->getFormattedMessage() << endl; break;
            case LF_ON_SIMULATION_ERROR: simout << "\n<!> " << check_and_cast<cException*>(details)->getFormattedMessage() << endl; break;
            default: break;
        }
    }
}

void CmdenvNarrator::displayException(std::exception& ex)
{
    // if verbose, it was already printed (maybe only in the redirection file though)
    if (!verbose || useStderr) {
        std::string msg = cException::getFormattedMessage(ex);
        std::ostream& stream = (cException::isError(ex) && useStderr) ? std::cerr : out;
        stream << "\n<!> " << msg << endl << endl;
    }
}

void CmdenvNarrator::logException(std::ofstream& fout, std::exception& ex)
{
    std::string msg = cException::getFormattedMessage(ex);
    fout << "\n<!> " << msg << endl << endl;
}

}  // namespace cmdenv
}  // namespace omnetpp

