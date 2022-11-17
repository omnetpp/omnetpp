//==========================================================================
//  CMDENVSIMULATIONRUNNER.H - part of
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

#ifndef __OMNETPP_CMDENV_CMDENVSIMULATIONRUNNER_H
#define __OMNETPP_CMDENV_CMDENVSIMULATIONRUNNER_H

#include <map>
#include <atomic>
#include <thread>
#include "envir/appbase.h"
#include "envir/speedometer.h"
#include "omnetpp/csimulation.h"
#include "fakegui.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

/**
 * Exposes the core functionality of Cmdenv, in a reusable and customizable form.
 */
class CMDENV_API CmdenvSimulationRunner
{
   protected:
     std::ostream& out;

     cINedLoader *nedLoader = nullptr;
     ArgList *args = nullptr;

     std::thread::id homeThreadId;

     bool verbose;
     bool useStderr;

     // set to true on SIGINT/SIGTERM signals
     static bool sigintReceived;

     // the number of runs already started (>1 if multiple runs are running in the same process)
     std::atomic_int numRuns;
     std::atomic_int runsTried;
     std::atomic_int numErrors;

     bool stopBatchOnError = true;

   protected:
     virtual cINedLoader *createConfiguredNedLoader(cConfiguration *cfg);
     virtual cSimulation *createSimulation(std::ostream& simout);
     virtual cIEventLoopRunner *createEventLoopRunner(cSimulation *simulation, std::ostream& simout, cConfiguration *cfg);
     virtual void logException(std::ostream& out, std::exception& e);

     virtual void alert(const char *msg);
     virtual void displayException(std::exception& ex);
     std::thread startThread(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers);

     void installSigintHandler();
     void deinstallSigintHandler();
     static void sigintHandler(int signum);

   public:
     CmdenvSimulationRunner(std::ostream& out, ArgList *args);
     virtual ~CmdenvSimulationRunner();

     virtual void setVerbose(bool verbose) {this->verbose = verbose;}
     virtual void setUseStderr(bool useStderr) {this->useStderr = useStderr;}

     virtual int runCmdenv(InifileContents *ini);
     virtual int runParameterStudy(InifileContents *ini, const char *configName, const char *runFilter);

     virtual void runSimulations(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers);
     virtual void runSimulationsInThreads(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers, int numThreads);
     virtual bool runSimulation(InifileContents *ini, const char *configName, int runNumber);

     virtual cTerminationException *setupAndRunSimulation(cConfiguration *cfg, const char *redirectFileName=nullptr);
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

