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
#include "envir/envirbase.h"
#include "envir/speedometer.h"
#include "omnetpp/csimulation.h"
#include "fakegui.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

struct CMDENV_API CmdenvOptions : public EnvirOptions
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    std::string configName;
    std::string runFilter;
    bool stopBatchOnError = true;
    size_t extraStack = 0;
    std::string outputFile;
    bool redirectOutput = false;
    bool expressMode = false;
    bool interactive = false;
    bool logDuringInitialize = true;
    bool logDuringSimulation = true;
    bool logDuringFinish = true;
    bool logDuringCleanup = true;
    bool autoflush = true; // all modes
    bool printModuleMsgs = false;  // if normal mode
    bool printEventBanners = true; // if normal mode
    bool detailedEventBanners = false; // if normal mode
    bool printProgressUpdates = true; // if express mode
    long statusFrequencyMs = 2000; // if express mode
    bool printPerformanceData = false; // if express mode
    bool fakeGUI = false; // all modes

};

/**
 * Command line user interface.
 */
class CMDENV_API Cmdenv : public EnvirBase
{
   protected:
     CmdenvOptions *&opt;         // alias to EnvirBase::opt

     // set to true on SIGINT/SIGTERM signals
     static bool sigintReceived;

     // the number of runs already started (>1 if multiple runs are running in the same process)
     int runsTried = 0;
     int numRuns = 0;

     FakeGUI *fakeGUI = nullptr;

     // The user is expected to set a function pointer that checks for specific events and set a
     // breakpoint on the handleMatchingEvent function. This is much faster than using the conditional
     // breakpoint of the debugger, because that would require a context switch at each event.
     bool (*matchEventCondition)(cEvent *) = [] (omnetpp::cEvent *event) -> bool { return false; };

   protected:
     virtual void log(cLogEntry *entry) override;
     virtual void alert(const char *msg) override;
     virtual bool askYesNo(const char *question) override;
     virtual void printEventBanner(cEvent *event);
     virtual void doStatusUpdate(Speedometer& speedometer);

     virtual void handleMatchingEvent(cEvent *event) { }
     static void setMatchEventCondition(bool (*f)(cEvent *));

   public:
     Cmdenv();
     virtual ~Cmdenv() {}

     virtual void componentInitBegin(cComponent *component, int stage) override;
     virtual void simulationEvent(cEvent *event) override;

     virtual bool isGUI() const override {return opt->fakeGUI;}
     virtual bool isExpressMode() const override {return opt->expressMode;}
     virtual std::string gets(const char *prompt, const char *defaultReply) override;
     virtual bool idle() override;
     virtual void pausePoint() override { /* currently no-op */ };
     virtual unsigned getExtraStackForEnvir() const override;

     virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) override;
     virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) override;
     virtual void appendToImagePath(const char *directory) override;
     virtual void loadImage(const char *fileName, const char *imageName=nullptr) override;
     virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) override;
     virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) override;
     virtual double getZoomLevel(const cModule *module) override;
     virtual double getAnimationTime() const override;
     virtual double getAnimationSpeed() const override;
     virtual double getRemainingAnimationHoldTime() const override;

   protected:
     virtual void displayException(std::exception& ex) override;
     virtual void doRun() override;
     virtual void printUISpecificHelp() override;
     virtual void loadNEDFiles() override { EnvirBase::loadNEDFiles(); }

     virtual EnvirOptions *createOptions() override {return new CmdenvOptions();}
     virtual void readOptions() override;
     virtual void readPerRunOptions() override;
     virtual void configure(cComponent *component) override;
     virtual void askParameter(cPar *par, bool unassigned) override;

     void help();
     void simulate();
     const char *progressPercentage();

     void installSignalHandler();
     void deinstallSignalHandler();
     static void signalHandler(int signum);
};

}  // namespace cmdenv
}  // namespace omnetpp

#endif

