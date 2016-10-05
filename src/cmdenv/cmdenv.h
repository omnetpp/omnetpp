//==========================================================================
//  CMDENV.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMDENV_CMDENV_H
#define __OMNETPP_CMDENV_CMDENV_H

#include <map>
#include "envir/envirbase.h"
#include "envir/speedometer.h"
#include "omnetpp/csimulation.h"

namespace omnetpp {
namespace cmdenv {

using namespace omnetpp::envir;

struct CMDENV_API CmdenvOptions : public EnvirOptions
{
    CmdenvOptions();
    std::string configName;
    std::string runFilter;
    size_t extraStack;
    std::string outputFile;
    bool redirectOutput;
    bool expressMode;
    bool interactive;
    bool autoflush; // all modes
    bool printModuleMsgs;  // if normal mode
    bool printEventBanners; // if normal mode
    bool detailedEventBanners; // if normal mode
    long statusFrequencyMs; // if express mode
    bool printPerformanceData; // if express mode
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

     // logging
     bool logging;
     FILE *logStream;

   protected:
     virtual void log(cLogEntry *entry) override;
     virtual void putsmsg(const char *s) override;
     virtual bool askyesno(const char *question) override;
     virtual void printEventBanner(cEvent *event);
     virtual void doStatusUpdate(Speedometer& speedometer);

   public:
     Cmdenv();
     virtual ~Cmdenv();

     // redefined virtual funcs:
     virtual void componentInitBegin(cComponent *component, int stage) override;
     virtual void simulationEvent(cEvent *event) override;

     virtual bool isGUI() const override {return false;}
     virtual bool isExpressMode() const override {return opt->expressMode;}
     virtual std::string gets(const char *prompt, const char *defaultReply) override;
     virtual bool idle() override;
     virtual unsigned getExtraStackForEnvir() const override;
     virtual void debug(const char *fmt,...);

     virtual void getImageSize(const char *imageName, int& outWidth, int& outHeight) override;
     virtual void getTextExtent(const cFigure::Font& font, const char *text, int& outWidth, int& outHeight, int& outAscent) override;
     virtual double getAnimationTime() const override {return 0;}
     virtual double getAnimationSpeed() const override {return 0;}
     virtual double getRemainingAnimationHoldTime() const override {return 0;}

   protected:
     virtual void displayException(std::exception& ex) override;
     virtual void doRun() override;
     virtual void printUISpecificHelp() override;

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

} // namespace cmdenv
}  // namespace omnetpp

#endif

