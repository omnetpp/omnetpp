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
#include <memory>
#include "envir/args.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cinedloader.h"
#include "cmdenvnarrator.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

/**
 * Exposes the core functionality of Cmdenv, in a reusable and customizable form.
 */
class CMDENV_API CmdenvSimulationRunner
{
   public:
    struct BatchResult {
         int numRuns;
         int runsTried; // i.e. started
         int numCompleted;
         int numInterrupted;
         int numErrors;
    };

   protected:
     std::ostream& out;
     cINedLoader *nedLoader = nullptr;
     ArgList *args = nullptr;
     ICmdenvNarrator *narrator;
     std::thread::id homeThreadId;

     static bool sigintReceived;  // set to true on SIGINT/SIGTERM signals

     struct SimulationSummary {
          bool successful = true;
          simtime_t simulatedTime = 0;
          double elapsedSecs = 0;
          eventnumber_t eventsSimulated = 0;
          std::shared_ptr<cTerminationException> terminationReason = nullptr;
     };

     struct BatchState {
          std::atomic_int numRuns{0};
          std::atomic_int runsTried{0}; // i.e. started
          std::atomic_int numCompleted{0};
          std::atomic_int numInterrupted{0};
          std::atomic_int numErrors{0};
          std::atomic_bool stopBatchOnError{0};
     };

   protected:
     // overridable factory methods
     virtual cINedLoader *createConfiguredNedLoader(cConfiguration *cfg);
     virtual cSimulation *createSimulation(std::ostream& simout);
     virtual cIEventLoopRunner *createEventLoopRunner(BatchState& state, cSimulation *simulation, std::ostream& simout, cConfiguration *cfg);

     // internal
     virtual void ensureNedLoader(cConfiguration *cfg);
     virtual void doRunSimulations(BatchState& state, InifileContents *ini, const char *configName, const std::vector<int>& runNumbers);
     virtual void doRunSimulation(BatchState& state, InifileContents *ini, const char *configName, int runNumber); // note: throws on error
     virtual BatchResult extractResult(const BatchState& state);
     virtual SimulationSummary setupAndRunMultithreadedParallelSimulation(BatchState& state, cConfiguration *cfg);
     virtual SimulationSummary setupAndRunSimulation(BatchState& state, cConfiguration *cfg, int partitionId=-1);
     static void sigintHandler(int signum);

   public:
     CmdenvSimulationRunner(std::ostream& out, ArgList *args);
     virtual ~CmdenvSimulationRunner();

     static void installSigintHandler();
     static void deinstallSigintHandler();

     virtual void setVerbose(bool verbose) {narrator->setVerbose(verbose);}
     virtual void setUseStderr(bool useStderr) {narrator->setUseStderr(useStderr);}

     virtual int runCmdenv(InifileContents *ini); // does not throw, returns exit code
     virtual BatchResult runParameterStudy(InifileContents *ini, const char *configName, const char *runFilter);
     virtual BatchResult runSimulations(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers);
     virtual BatchResult runSimulationsInThreads(InifileContents *ini, const char *configName, const std::vector<int>& runNumbers, int numThreads=-1);
     virtual void runSimulation(InifileContents *ini, const char *configName, int runNumber); // note: throws on error
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

