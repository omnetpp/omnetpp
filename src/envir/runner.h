//==========================================================================
//  RUNNER.H - part of
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

#ifndef __OMNETPP_ENVIR_RUNNER_H
#define __OMNETPP_ENVIR_RUNNER_H

#include "envirdefs.h"
#include "omnetpp/csimulation.h"
#include "stopwatch.h"
#include "speedometer.h"

namespace omnetpp {

class cEvent;
class cSimulation;

namespace envir {

class IFakeGUI;

class IRunner {
  public:
    virtual ~IRunner();
    virtual void run() = 0;
};

class ENVIR_API Runner : public IRunner
{
  protected:
    cSimulation *simulation; // not owned
    IFakeGUI *fakeGUI = nullptr; // not owned
    std::ostream& out;
    bool& sigintReceived;
    int runsTried=1, numRuns=1; // for progress reporting

    bool expressMode = true;
    bool autoflush = true;
    long statusFrequencyMs = 2000;
    bool printPerformanceData = true;
    bool printThreadId = false;
    bool printEventBanners = false;
    bool detailedEventBanners = false;

    Stopwatch stopwatch; // CPU and real time limit checking
    Speedometer speedometer;
    simtime_t simulatedTime;  // sim. time after finishing simulation

  protected:
    virtual void doRunNormal();
    virtual void doRunExpressWithFakeGUI();
    virtual void doRunExpressNoFakeGui();
    virtual void doRunExpressNoFakeGuiNoTimelimit();
    virtual void doRunExpressNoStatusUpdates();

    virtual void printEventBanner(eventnumber_t eventNumber, cEvent *event);
    virtual void printStatusUpdate();
    virtual std::string getProgressPercentage();

    bool elapsed(long millis, int64_t& since);
    double getElapsedSecs(); //TODO make it accessible to cException, so it can be put into error messages (maybe move stopwatch into cSimulation?)

  public:
    Runner(cSimulation *simulation, std::ostream& out, bool& sigintReceived) : simulation(simulation), out(out), sigintReceived(sigintReceived) {}
    virtual ~Runner() {}
    virtual void configure(cConfiguration *cfg);

    virtual void setFakeGUI(IFakeGUI *fakeGUI) {this->fakeGUI = fakeGUI;}
    virtual void setSimulationTimeLimit(simtime_t limit) {simulation->setSimulationTimeLimit(limit);}
    virtual void setCPUTimeLimit(double limit) {stopwatch.setCPUTimeLimit(limit);}
    virtual void setRealTimeLimit(double limit) {stopwatch.setRealTimeLimit(limit);}
    virtual void setExpressMode(bool express) {expressMode = express;}
    virtual void setAutoFlush(bool flush) {autoflush = flush;}
    virtual void setStatusFrequencyMs(long freq) {statusFrequencyMs = freq;}
    virtual void setPrintPerformanceData(bool print) {printPerformanceData = print;}
    virtual void setPrintThreadId(bool print) {printThreadId = print;}
    virtual void setPrintEventBanners(bool print) {printEventBanners = print;}
    virtual void setDetailedEventBanners(bool enable) {detailedEventBanners = enable;}
    virtual void setBatchProgress(int runsTried, int numRuns) {this->runsTried = runsTried; this->numRuns = numRuns;}

    virtual void run();
};

}  // namespace envir
}  // namespace omnetpp

#endif

