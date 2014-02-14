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

#ifndef __CMDENV_H
#define __CMDENV_H

#include "csimulation.h"
#include "envirbase.h"

NAMESPACE_BEGIN

class Speedometer;

struct CMDENV_API CmdenvOptions : public EnvirOptions
{
    CmdenvOptions();
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
class CMDENV_API Cmdenv : public EnvirBase
{
    protected:
     CmdenvOptions *&opt;         // alias to EnvirBase::opt

     // set to true on SIGINT/SIGTERM signals
     static bool sigint_received;

     // stream to write output to
     FILE *fout;

   protected:
     virtual void sputn(const char *s, int n);
     virtual void putsmsg(const char *s);
     virtual bool askyesno(const char *question);
     virtual void printEventBanner(cSimpleModule *mod);
     virtual void doStatusUpdate(Speedometer& speedometer);

   public:
     Cmdenv();
     virtual ~Cmdenv();

     // redefined virtual funcs:
     virtual void componentInitBegin(cComponent *component, int stage);
     virtual void moduleCreated(cModule *newmodule);
     virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate);
     virtual void simulationEvent(cMessage *msg);
     virtual bool isGUI() const {return false;}
     virtual cEnvir& flush();
     virtual std::string gets(const char *prompt, const char *defaultReply);
     virtual bool idle();
     virtual unsigned getExtraStackForEnvir() const;

   protected:
     virtual void run();
     virtual void printUISpecificHelp();

     virtual EnvirOptions *createOptions() {return new CmdenvOptions();}
     virtual void readOptions();
     virtual void readPerRunOptions();
     virtual void askParameter(cPar *par, bool unassigned);

     // new functions:
     void help();
     void simulate();
     const char *progressPercentage();

     void installSignalHandler();
     void deinstallSignalHandler();
     static void signalHandler(int signum);
};

NAMESPACE_END


#endif

