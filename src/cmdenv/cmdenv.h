//==========================================================================
//  CMDENV.H - part of
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

#ifndef __OMNETPP_CMDENV_CMDENV_H
#define __OMNETPP_CMDENV_CMDENV_H

#include <map>
#include <atomic>
#include "envir/appbase.h"
#include "envir/speedometer.h"
#include "omnetpp/csimulation.h"
#include "fakegui.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

struct CMDENV_API CmdenvOptions : public AppBaseOptions
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    bool stopBatchOnError = true;
    size_t extraStack = 0;
    std::string outputFile;
    bool redirectOutput = false;
    bool expressMode = false;
    bool interactive = false;
    bool autoflush = true; // all modes
    bool printModuleMsgs = false;  // if normal mode
    bool printEventBanners = true; // if normal mode
    bool detailedEventBanners = false; // if normal mode
    long statusFrequencyMs = 2000; // if express mode
    bool printPerformanceData = false; // if express mode
    bool fakeGUI = false; // all modes
};

/**
 * Command line user interface.
 */
class CMDENV_API Cmdenv : public AppBase
{
   protected:
     CmdenvOptions *&opt;         // alias to EnvirBase::opt

     // set to true on SIGINT/SIGTERM signals
     static bool sigintReceived;

     // the number of runs already started (>1 if multiple runs are running in the same process)
     std::atomic_int numRuns;
     std::atomic_int runsTried;
     std::atomic_int numErrors;

     // logging
     bool logging = true;
     FILE *logStream;

   protected:
     virtual void alert(const char *msg) override;

   public:
     Cmdenv();
     virtual ~Cmdenv() {}


   protected:
     virtual void displayException(std::exception& ex) override;
     virtual void doRun() override;
     virtual void printUISpecificHelp() override;

     virtual CmdenvOptions *createOptions() override {return new CmdenvOptions();}
     virtual void readOptions(cConfiguration *cfg) override;
     virtual void readPerRunOptions(cConfiguration *cfg) override;

     void help();

     bool runSimulation(const char *configName, int runNumber);
     void runSimulations(const char *configName, const std::vector<int>& runNumbers);
     void runSimulationsInThreads(const char *configName, const std::vector<int>& runNumbers, int numThreads);
     std::thread startThread(const char *configName, const std::vector<int>& runNumbers);
     void simulate();

     void installSignalHandler();
     void deinstallSignalHandler();
     static void signalHandler(int signum);
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

