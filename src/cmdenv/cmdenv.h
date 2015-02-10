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

#include <map>
#include "csimulation.h"
#include "envirbase.h"
#include "httpserver.h"
#include "serializer.h"
#include "json.h"

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
#ifdef SIMFRONTEND_SUPPORT
    bool httpControlled;  // when true, user input is expected via HTTP
    long updatefreqExpress; //XXX from Tkenv
    long updatefreqFast; //XXX from Tkenv
#endif
};

/**
 * Command line user interface.
 */
class CMDENV_API Cmdenv : public EnvirBase
#ifdef SIMFRONTEND_SUPPORT
    , public cHttpRequestHandler
#endif
{
#ifdef SIMFRONTEND_SUPPORT
    public:
       /**
        * Application state.
        * State transitions:
        *    NONETWORK -> READY <-> RUNNING -> TERMINATED -> FINISHCALLED -> NONETWORK
        * plus:
        *    READY -> FINISHCALLED
        *    (any-state) -> ERROR -> NONET
        * TODO: BUSY needed?
        */
       enum State {
           SIM_NONETWORK = 0,
           SIM_READY,
           SIM_RUNNING,
           SIM_TERMINATED,
           SIM_ERROR,
           SIM_FINISHCALLED
       };

       enum RunMode {
           RUNMODE_NONE,
           RUNMODE_NORMAL,
           RUNMODE_FAST,
           RUNMODE_EXPRESS
       };

       enum StoppingReason {
           STOP_NONE,
           STOP_UNTILSIMTIME,
           STOP_UNTILEVENT,
           STOP_UNTILMODULE,
           STOP_UNTILMESSAGE,
           STOP_REALTIMECHUNK,
           STOP_STOPCOMMAND,
           STOP_TERMINATION
       };

       enum Command {
           CMD_NONE = 0,
           CMD_SETUPNETWORK,
           CMD_SETUPRUN,
           CMD_REBUILD,
           CMD_STEP,
           CMD_RUN,
           CMD_STOP,
           CMD_FINISH,
           CMD_QUIT
       };

       enum UserInputState {
           INPSTATE_NONE,
           INPSTATE_INITIATED,
           INPSTATE_WAITINGFORREPLY,
           INPSTATE_REPLYARRIVED
       };

       enum UserInputType {
           INP_NONE,
           INP_ERROR, //XXX currently unused (goes via INP_MSGDIALOG)
           INP_ASKPARAMETER, //XXX currently unused (goes via INP_GETS)
           INP_GETS,
           INP_ASKYESNO,
           INP_MSGDIALOG
       };
#endif

    protected:
     CmdenvOptions *&opt;         // alias to EnvirBase::opt

#ifdef SIMFRONTEND_SUPPORT
     typedef std::map<std::string,std::string> StringMap;

     State state;                 // simulation state
     Command command;             // command received via HTTP; set to CMD_NONE when the command has been carried out
     StringMap commandArgs;       // command args

     RunMode runMode;             // the current mode the simulation is executing under
     StoppingReason stoppingReason; // why the last Run command finished
     bool isConfigRun;            // true after newRun(), and false after newConfig()

     Serializer *serializer;      // JSON serializer and object-to-id mapping

     bool collectJsonLog;
     JsonArray *jsonLog;          // animLog entries from the last event

     struct {
         simtime_t simTime;       // time limit in current "Run Until" execution, or zero
         eventnumber_t eventNumber;// event number in current "Run Until" execution, or zero
         cMessage *msg;           // stop before this event; also when this message gets canceled
         cModule *module;         // stop before and after events in this module; not supported in EXPRESS mode
         bool hasRealTimeLimit;   // whether realTime (next field) is valid
         struct timeval realTime; // stop when system clock reaches this time
     } runUntil;

     bool stopSimulationFlag;     // indicates that the simulation should be stopped (STOP button pressed in the UI)
     timeval idleLastUICheck;     // gettimeofday() time when idle() last run the Tk "update" command  XXX comment

     struct {
         UserInputState state;
         UserInputType type;
         JsonObject *request;
         std::string reply;
         bool cancel;
     } userInput;
#endif

     // set to true on SIGINT/SIGTERM signals
     static bool sigintReceived;

     // stream to write output to
     FILE *fout;

     // logging
     bool logging;
     FILE *logStream;

   protected:
     virtual void log(cLogEntry *entry);
     virtual void putsmsg(const char *s);
     virtual bool askyesno(const char *question);
     virtual void printEventBanner(cEvent *event);
     virtual void doStatusUpdate(Speedometer& speedometer);
#ifdef SIMFRONTEND_SUPPORT
     JsonNode *jsonWrapObjectId(cObject *obj) {return jsonWrap(serializer->getIdStringForObject(obj));}
#endif

   public:
     Cmdenv();
     virtual ~Cmdenv();

     // redefined virtual funcs:
     virtual void componentInitBegin(cComponent *component, int stage);
     virtual void moduleCreated(cModule *newmodule);
     virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate);
     virtual void simulationEvent(cEvent *event);
     virtual void objectDeleted(cObject *object);

     virtual void bubble(cComponent *component, const char *text);
     virtual void messageScheduled(cMessage *msg);
     virtual void messageCancelled(cMessage *msg);
     virtual void beginSend(cMessage *msg);
     virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
     virtual void messageSendHop(cMessage *msg, cGate *srcGate);
     virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
     virtual void endSend(cMessage *msg);
     virtual void messageCreated(cMessage *msg);
     virtual void messageCloned(cMessage *msg, cMessage *clone);
     virtual void messageDeleted(cMessage *msg);
     virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId);
     virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent);
     virtual void componentMethodEnd();
     virtual void moduleDeleted(cModule *module);
     virtual void gateCreated(cGate *newgate);
     virtual void gateDeleted(cGate *gate);
     virtual void connectionCreated(cGate *srcgate);
     virtual void connectionDeleted(cGate *srcgate);
     virtual void displayStringChanged(cComponent *component);

     virtual bool isGUI() const;
     virtual std::string gets(const char *prompt, const char *defaultReply);
     virtual bool idle();
     virtual unsigned getExtraStackForEnvir() const;
     virtual void debug(const char *fmt,...);

   protected:
     virtual void displayException(std::exception& ex);
     virtual void doRun();
     virtual void printUISpecificHelp();

     virtual EnvirOptions *createOptions() {return new CmdenvOptions();}
     virtual void readOptions();
     virtual void readPerRunOptions();
     virtual void askParameter(cPar *par, bool unassigned);

#ifdef SIMFRONTEND_SUPPORT
     virtual void processHttpRequests(bool blocking);
     virtual bool handleHttpRequest(cHttpRequest *request); // called back from processHttpRequests()
     virtual void processCommand(int command);
     virtual std::string getUserInput(UserInputType type, JsonObject *details);

     // from Tkenv:
     virtual void newNetwork(const char *networkname);
     virtual void newRun(const char *configname, int runnumber);
     virtual void rebuildSim();
     virtual void doOneStep();
     virtual void runSimulation(RunMode mode, long realTimeMillis=0, simtime_t untilSimTime=0, eventnumber_t untilEventNumber=0, cMessage *untilMessage=NULL, cModule *untilModule=NULL);
     virtual int getSimulationRunMode() const {return runMode;}
     virtual void setStopSimulationFlag() {stopSimulationFlag = true;}
     virtual bool getStopSimulationFlag() {return stopSimulationFlag;}
     virtual bool doRunSimulation();
     virtual bool doRunSimulationExpress();
     virtual void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()
#endif

     void help();
     void simulate();
     const char *progressPercentage();

     void installSignalHandler();
     void deinstallSignalHandler();
     static void signalHandler(int signum);
};

NAMESPACE_END


#endif

