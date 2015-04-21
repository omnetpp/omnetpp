//==========================================================================
//  CMDENV.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __QTENV_H
#define __QTENV_H



#include <map>
#include <clistener.h>
#include <csimulation.h>

#include <envirbase.h>
#include "ccanvas.h"
#include "canvasrenderer.h"

class MainWindow;
class QApplication;

NAMESPACE_BEGIN

class Speedometer;

struct QTENV_API QtenvOptions : public EnvirOptions
{
    QtenvOptions();
    opp_string configName;
    opp_string runsToExec;
    size_t extraStack;
    opp_string outputFile;
    bool expressMode;
    bool interactive;
    bool autoflush; // all modes
    bool printModuleMsgs;  // if normal mode
    bool printEventBanners; // if normal mode
    bool detailedEventBanners; // if normal mode
    bool messageTrace; // if normal mode
    long statusFrequencyMs; // if express mode
    bool printPerformanceData; // if express mode
};

/**
 * Command line user interface.
 */
class QTENV_API Qtenv : public EnvirBase
{
    protected:
     QtenvOptions *&opt;         // alias to EnvirBase::opt

     // set to true on SIGINT/SIGTERM signals
     static bool sigintReceived;

     // logging
     bool logging;
     FILE *logStream;
     
     MainWindow *mainwindow;
     CanvasRenderer cRenderer;

   protected:
     virtual void log(cLogEntry *entry);
     virtual void putsmsg(const char *s);
     virtual bool askyesno(const char *question);
     virtual void printEventBanner(cEvent *event);
     virtual void doStatusUpdate(Speedometer& speedometer);

   public:
     Qtenv();
     virtual ~Qtenv();

     // redefined virtual funcs:
     virtual void componentInitBegin(cComponent *component, int stage);
     virtual void moduleCreated(cModule *newmodule);
     virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate);
     virtual void simulationEvent(cEvent *event);

     virtual bool isGUI() const;
     virtual std::string gets(const char *prompt, const char *defaultReply);
     virtual bool idle();
     virtual unsigned getExtraStackForEnvir() const;
     virtual void debug(const char *fmt,...);

   protected:
     virtual void displayException(std::exception& ex);
     virtual void doRun();
     virtual void printUISpecificHelp();

     virtual EnvirOptions *createOptions() {return new QtenvOptions();}
     virtual void readOptions();
     virtual void readPerRunOptions();
     virtual void askParameter(cPar *par, bool unassigned);

     void help();
     void simulate();
     const char *progressPercentage();

     void installSignalHandler();
     void deinstallSignalHandler();
     static void signalHandler(int signum);
};

NAMESPACE_END


#endif

