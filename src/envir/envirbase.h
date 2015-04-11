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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIRBASE_H
#define __OMNETPP_ENVIRBASE_H

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


class cXMLDocCache;
class cScheduler;
class cModuleType;
class cIListener;
class cProperty;
class cParsimCommunications;
class cParsimPartition;
class cParsimSynchronizer;
class cResultRecorder;
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
#ifdef WITH_PARSIM
    opp_string parsimcomm_class; // if parsim: cParsimCommunications class to use
    opp_string parsimsynch_class; // if parsim: cParsimSynchronizer class to use
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
    cXMLDocCache *xmlcache;
    int exitcode;

    EnvirOptions *opt;

#ifdef WITH_PARSIM
    cParsimCommunications *parsimcomm;
    cParsimPartition *parsimpartition;
#endif

    // Random number generators. Module RNG's map to these RNG objects.
    int num_rngs;
    cRNG **rngs;

    // log related
    LogFormatter logFormatter;
    bool logFormatUsesEventName;
    std::string currentEventName;
    const char *currentEventClassName;
    int currentModuleId;

    // Output file managers
    EventlogFileManager *eventlogmgr;  // NULL if no eventlog is being written, must be non NULL if record_eventlog is true
    cIOutputVectorManager *outvectormgr;
    cIOutputScalarManager *outscalarmgr;
    cISnapshotManager *snapshotmgr;

    // Data for getUniqueNumber()
    unsigned long nextuniquenumber;

    // lifecycle listeners
    std::vector<cISimulationLifecycleListener*> listeners;

    timeval simbegtime;  // real time when sim. started
    timeval simendtime;  // real time when sim. ended
    timeval laststarted; // real time from where sim. was last cont'd
    timeval elapsedtime; // time spent simulating
    simtime_t simulatedtime;  // sim. time after finishing simulation

  protected:
    // leave to subclasses: virtual void putsmsg(const char *msg);
    // leave to subclasses: virtual bool askyesno(const char *msg);
    virtual std::string makeDebuggerCommand();
    static void crashHandler(int signum);

  public:
    EnvirBase();
    virtual ~EnvirBase();

    // life cycle
    virtual int run(int argc, char *argv[], cConfiguration *config);

    // eventlog recording
    virtual void setEventlogRecording(bool enabled);
    virtual bool hasEventlogRecordingIntervals() const;
    virtual void clearEventlogRecordingIntervals();
    virtual void setLogLevel(LogLevel logLevel);
    virtual void setLogFormat(const char *logFormat);

    // Utility function: optionally appends host name to fname
    virtual void processFileName(opp_string& fname);

    // eventlog callback interface
    virtual void objectDeleted(cObject *object);
    virtual void simulationEvent(cEvent *event);
    // leave to subclasses: virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL);
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
    virtual void moduleCreated(cModule *newmodule);
    virtual void moduleDeleted(cModule *module);
    virtual void gateCreated(cGate *newgate);
    virtual void gateDeleted(cGate *gate);
    virtual void connectionCreated(cGate *srcgate);
    virtual void connectionDeleted(cGate *srcgate);
    virtual void displayStringChanged(cComponent *component);
    virtual void undisposedObject(cObject *obj);
    virtual void log(cLogEntry *entry);

    virtual const char *getCurrentEventName() { return logFormatUsesEventName ? currentEventName.c_str() : NULL; }
    virtual const char *getCurrentEventClassName() { return currentEventClassName; }
    virtual cModule *getCurrentEventModule() { return currentModuleId != -1 ? simulation.getModule(currentModuleId) : NULL; }

    // configuration, model parameters
    virtual void configure(cComponent *component);
    virtual void readParameter(cPar *parameter);
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index);
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=NULL);
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=NULL);
    virtual void forgetXMLDocument(const char *filename);
    virtual void forgetParsedXMLString(const char *content);
    virtual void flushXMLDocumentCache();
    virtual void flushXMLParsedContentCache();
    // leave to subclasses: virtual unsigned getExtraStackForEnvir();
    virtual cConfiguration *getConfig();
    virtual cConfigurationEx *getConfigEx();

    // UI functions
    virtual void bubble(cComponent *component, const char *text);
    // leave to subclasses: virtual std::string gets(const char *prompt, const char *defaultreply=NULL);
    // leave to subclasses: virtual cEnvir& flush();

    // RNGs
    virtual int getNumRNGs() const;
    virtual cRNG *getRNG(int k);
    virtual void getRNGMappingFor(cComponent *component);

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname);
    virtual void deregisterOutputVector(void *vechandle);
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value);
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value);

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL);
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL);

    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty);

    // snapshot file
    virtual std::ostream *getStreamForSnapshot();
    virtual void releaseStreamForSnapshot(std::ostream *os);

    // misc
    virtual int getArgCount() const;
    virtual char **getArgVector() const;
    virtual int getParsimProcId() const;
    virtual int getParsimNumPartitions() const;
    virtual unsigned long getUniqueNumber();
    virtual void attachDebugger();
    virtual bool idle();

    virtual void addLifecycleListener(cISimulationLifecycleListener *listener);
    virtual void removeLifecycleListener(cISimulationLifecycleListener *listener);
    virtual void notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details=NULL);
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

    // Utility function; never returns NULL
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

NAMESPACE_END

#endif

