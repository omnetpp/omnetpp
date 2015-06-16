//==========================================================================
//  ENVIRBASE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    EnvirBase:  abstract base class for simulation applications
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

#ifndef __OMNETPP_ENVIR_ENVIRBASE_H
#define __OMNETPP_ENVIR_ENVIRBASE_H

#include "omnetpp/carray.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/envirext.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cresultlistener.h"
#include "omnetpp/cevent.h"
#include "omnetpp/platdep/timeutil.h"
#include "logformatter.h"
#include "args.h"
#include "envirdefs.h"
#include "eventlogfilemgr.h"

NAMESPACE_BEGIN

class cScheduler;
class cModuleType;
class cIListener;
class cProperty;
class cParsimCommunications;
class cParsimPartition;
class cParsimSynchronizer;
class cResultRecorder;

namespace envir {

class cXMLDocCache;
class SignalSource;

// assumed maximum length for getFullPath() string.
// note: this maximum actually not enforced anywhere
#define MAX_OBJECTFULLPATH  1024

// maximum length of string passed in Enter_Method() (longer strings will be truncated)
#define MAX_METHODCALL 1024


struct ENVIR_API EnvirOptions
{
    EnvirOptions();
    virtual ~EnvirOptions() {}

    size_t totalStack;
    bool parsim;
    opp_string networkName;
    opp_string inifileNetworkDir; // dir of the inifile containing "network="

    int numRNGs;
    opp_string rngClass;
    int seedset; // which set of seeds to use

    opp_string schedulerClass;
    opp_string outputVectorManagerClass;
    opp_string outputScalarManagerClass;
    opp_string snapshotmanagerClass;
    opp_string futureeventsetClass;
#ifdef WITH_PARSIM
    opp_string parsimcommClass; // if parsim: cParsimCommunications class to use
    opp_string parsimsynchClass; // if parsim: cParsimSynchronizer class to use
#endif

    bool debugStatisticsRecording;
    bool checkSignals;
    bool fnameAppendHost;

    bool warnings;
    bool printUndisposed;

    simtime_t simtimeLimit;
    long cpuTimeLimit;
    simtime_t warmupPeriod;

    opp_string expectedFingerprint;
};

/**
 * Abstract base class for the user interface. Concrete user interface
 * implementations (Cmdenv, Tkenv) should be derived from this class.
 */
class ENVIR_API EnvirBase : public cRunnableEnvir
{
  protected:
    cConfigurationEx *cfg;
    ArgList *args;
    cXMLDocCache *xmlCache;
    int exitCode;

    EnvirOptions *opt;

#ifdef WITH_PARSIM
    cParsimCommunications *parsimComm;
    cParsimPartition *parsimPartition;
#endif

    // Random number generators. Module RNG's map to these RNG objects.
    int numRNGs;
    cRNG **rngs;

    // log related
    LogFormatter logFormatter;
    bool logFormatUsesEventName;
    std::string currentEventName;
    const char *currentEventClassName;
    int currentModuleId;

    // Output file managers
    EventlogFileManager *eventlogManager;  // nullptr if no eventlog is being written, must be non nullptr if record_eventlog is true
    cIOutputVectorManager *outvectorManager;
    cIOutputScalarManager *outScalarManager;
    cISnapshotManager *snapshotManager;

    // Data for getUniqueNumber()
    unsigned long nextUniqueNumber;

    // lifecycle listeners
    std::vector<cISimulationLifecycleListener*> listeners;

    timeval simBegTime;  // real time when sim. started
    timeval simEndTime;  // real time when sim. ended
    timeval lastStarted; // real time from where sim. was last cont'd
    timeval elapsedTime; // time spent simulating
    simtime_t simulatedTime;  // sim. time after finishing simulation

  protected:
    // leave to subclasses: virtual void putsmsg(const char *msg);
    // leave to subclasses: virtual bool askyesno(const char *msg);
    virtual std::string makeDebuggerCommand();
    static void crashHandler(int signum);

  public:
    EnvirBase();
    virtual ~EnvirBase();

    // life cycle
    virtual int run(int argc, char *argv[], cConfiguration *config) override;

    // eventlog recording
    virtual void setEventlogRecording(bool enabled);
    virtual bool hasEventlogRecordingIntervals() const;
    virtual void clearEventlogRecordingIntervals();
    virtual void setLogLevel(LogLevel logLevel);
    virtual void setLogFormat(const char *logFormat);

    // Utility function: optionally appends host name to fname
    virtual void processFileName(opp_string& fname);

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) override;
    virtual void simulationEvent(cEvent *event) override;
    // leave to subclasses: virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=nullptr);
    virtual void messageScheduled(cMessage *msg) override;
    virtual void messageCancelled(cMessage *msg) override;
    virtual void beginSend(cMessage *msg) override;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay) override;
    virtual void endSend(cMessage *msg) override;
    virtual void messageCreated(cMessage *msg) override;
    virtual void messageCloned(cMessage *msg, cMessage *clone) override;
    virtual void messageDeleted(cMessage *msg) override;
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) override;
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) override;
    virtual void componentMethodEnd() override;
    virtual void moduleCreated(cModule *newmodule) override;
    virtual void moduleDeleted(cModule *module) override;
    virtual void gateCreated(cGate *newgate) override;
    virtual void gateDeleted(cGate *gate) override;
    virtual void connectionCreated(cGate *srcgate) override;
    virtual void connectionDeleted(cGate *srcgate) override;
    virtual void displayStringChanged(cComponent *component) override;
    virtual void undisposedObject(cObject *obj) override;
    virtual void log(cLogEntry *entry) override;

    virtual const char *getCurrentEventName() override { return logFormatUsesEventName ? currentEventName.c_str() : nullptr; }
    virtual const char *getCurrentEventClassName() override { return currentEventClassName; }
    virtual cModule *getCurrentEventModule() override { return currentModuleId != -1 ? getSimulation()->getModule(currentModuleId) : nullptr; }

    // configuration, model parameters
    virtual void configure(cComponent *component) override;
    virtual void readParameter(cPar *parameter) override;
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index) override;
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=nullptr) override;
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=nullptr) override;
    virtual void forgetXMLDocument(const char *filename) override;
    virtual void forgetParsedXMLString(const char *content) override;
    virtual void flushXMLDocumentCache() override;
    virtual void flushXMLParsedContentCache() override;
    // leave to subclasses: virtual unsigned getExtraStackForEnvir();
    virtual cConfiguration *getConfig() override;
    virtual cConfigurationEx *getConfigEx() override;

    // UI functions
    virtual void bubble(cComponent *component, const char *text) override;
    // leave to subclasses: virtual std::string gets(const char *prompt, const char *defaultreply=nullptr);
    // leave to subclasses: virtual cEnvir& flush();

    // RNGs
    virtual int getNumRNGs() const override;
    virtual cRNG *getRNG(int k) override;
    virtual void getRNGMappingFor(cComponent *component) override;

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname) override;
    virtual void deregisterOutputVector(void *vechandle) override;
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) override;
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value) override;

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) override;
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) override;

    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty) override;

    // snapshot file
    virtual std::ostream *getStreamForSnapshot() override;
    virtual void releaseStreamForSnapshot(std::ostream *os) override;

    // misc
    virtual int getArgCount() const override;
    virtual char **getArgVector() const override;
    virtual int getParsimProcId() const override;
    virtual int getParsimNumPartitions() const override;
    virtual unsigned long getUniqueNumber() override;
    virtual void attachDebugger() override;
    virtual bool idle() override;

    virtual void addLifecycleListener(cISimulationLifecycleListener *listener) override;
    virtual void removeLifecycleListener(cISimulationLifecycleListener *listener) override;
    virtual void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details=nullptr) override;
    //@}

  protected:
    // functions added locally
    virtual bool simulationRequired();
    virtual bool setup();  // does not throw; returns true if OK to go on
    virtual void run();  // does not throw; delegates to doRun()
    virtual void shutdown(); // does not throw
    virtual void doRun() = 0;

    virtual void setupNetwork(cModuleType *network);
    virtual void startRun();
    virtual void endRun();

    ArgList *argList()  {return args;}
    void printHelp();
    void setupEventLog();
    void dumpComponentList(const char *category);
    virtual void printUISpecificHelp() = 0;

    virtual EnvirOptions *createOptions() {return new EnvirOptions();}
    virtual void readOptions();
    virtual void readPerRunOptions();

    // Utility function; never returns nullptr
    cModuleType *resolveNetwork(const char *networkname);

    // Called internally from readParameter(), to interactively prompt the
    // user for a parameter value.
    virtual void askParameter(cPar *par, bool unassigned) = 0;

    virtual void displayException(std::exception& e);

    // Utility function: checks simulation fingerprint and displays a message accordingly
    void checkFingerprint();

    // Called from configure(component); adds result recording listeners
    // for each declared signal (@statistic property) in the component.
    virtual void addResultRecorders(cComponent *component);

    // Utility function: adds result recording listeners for the given signal (if it's non-null) or for the given @statistic property.
    // If signal is specified, it will override the source= key in statisticProperty.
    // The index of statisticProperty is ignored; statisticName will be used as name of the statistic instead.
    virtual void doAddResultRecorders(cComponent *component, std::string& componentFullPath, const char *statisticName, cProperty *statisticProperty, simsignal_t signal=SIMSIGNAL_NULL);

    // Utility functions for addResultRecorders()
    std::vector<std::string> extractRecorderList(const char *modesOption, cProperty *statisticProperty);
    SignalSource doStatisticSource(cComponent *component, const char *statisticName, const char *sourceSpec, bool needWarmupFilter);
    void doResultRecorder(const SignalSource& source, const char *mode, bool scalarsEnabled, bool vectorsEnabled, cComponent *component, const char *statisticName, cProperty *attrsProperty);
    void dumpResultRecorders(cComponent *component);
    void dumpResultRecorderChain(cResultListener *listener, int depth);

    // Utility function for getXMLDocument() and getParsedXMLString()
    cXMLElement *resolveXMLPath(cXMLElement *documentnode, const char *path);

    // Measuring elapsed time
    void checkTimeLimits();
    void resetClock();
    void startClock();
    void stopClock();
    timeval totalElapsed();

    // Hook called when the simulation terminates normally.
    // Its current use is to notify parallel simulation part.
    void stoppedWithTerminationException(cTerminationException& e);

    // Hook called when the simulation is stopped with an error.
    // Its current use is to notify parallel simulation part.
    void stoppedWithException(std::exception& e);
};

} // namespace envir
NAMESPACE_END

#endif

