//==========================================================================
//  ENVIRBASE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    EnvirBase:  abstract base class for simulation applications
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_ENVIRBASE_H
#define __OMNETPP_ENVIR_ENVIRBASE_H

#include "omnetpp/carray.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cexception.h"
#include "omnetpp/envirext.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cresultlistener.h"
#include "omnetpp/cevent.h"
#include "logformatter.h"
#include "args.h"
#include "envirdefs.h"
#include "stopwatch.h"

namespace omnetpp {

class cScheduler;
class cModuleType;
class cIListener;
class cProperty;
class cParsimCommunications;
class cParsimPartition;
class cParsimSynchronizer;
class cResultRecorder;
class cIEventlogManager;

namespace envir {

class XMLDocCache;
class SignalSource;

// assumed maximum length for getFullPath() string.
// note: this maximum actually not enforced anywhere
#define MAX_OBJECTFULLPATH  1024

// maximum length of string passed in Enter_Method() (longer strings will be truncated)
#define MAX_METHODCALL 1024

#define ARGSPEC "h?f:u:l:c:r:n:x:i:p:q:e:avwsm"

#ifdef USE_PORTABLE_COROUTINES  /* coroutine stacks reside in main stack area */

# define TOTAL_STACK_SIZE    (2*1024*1024)
# define MAIN_STACK_SIZE     (128*1024)

#else /* nonportable coroutines, stacks are allocated on heap */

# define TOTAL_STACK_SIZE    0  // dummy value
# define MAIN_STACK_SIZE     0  // dummy value

#endif


struct ENVIR_API EnvirOptions
{
    EnvirOptions();
    virtual ~EnvirOptions() {}

    size_t totalStack;
    bool parsim;
    std::string networkName;
    std::string inifileNetworkDir;
    std::string imagePath;
    std::string nedPath;
    std::string nedExcludedPackages;

    int numRNGs;
    std::string rngClass;
    int seedset; // which set of seeds to use

    std::string schedulerClass;
    std::string eventlogManagerClass;
    std::string outputVectorManagerClass;
    std::string outputScalarManagerClass;
    std::string snapshotmanagerClass;
    std::string futureeventsetClass;
#ifdef WITH_PARSIM
    int parsimNumPartitions = 0;
    std::string parsimcommClass; // if parsim: cParsimCommunications class to use
    std::string parsimsynchClass; // if parsim: cParsimSynchronizer class to use
#endif

    bool debugStatisticsRecording;
    bool checkSignals;
    bool fnameAppendHost;

    bool useStderr;
    bool verbose;
    bool warnings;
    bool printUndisposed;

    simtime_t simtimeLimit;
    simtime_t warmupPeriod;

    double realTimeLimit;
    double cpuTimeLimit;
};

/**
 * Abstract base class for the user interface. Concrete user interface
 * implementations (Cmdenv, Qtenv) should be derived from this class.
 */
class ENVIR_API EnvirBase : public cEnvir
{
  protected:
    cConfigurationEx *cfg;
    ArgList *args;
    XMLDocCache *xmlCache;
    int exitCode;

    EnvirOptions *opt;

    std::ostream out;
    std::string redirectionFilename;

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
    bool logFormatUsesEventClassName;
    const char *currentEventClassName;
    int currentModuleId;

    // Indicates whether eventlog recording is currently enabled (note: eventlogManager contains further filters).
    // It MUST be in sync with EventlogFileManager::isRecordingEnabled.
    bool recordEventlog;

    // Output file managers
    cIEventlogManager *eventlogManager;  // nullptr if no eventlog is being written, must be non nullptr if record_eventlog is true
    cIOutputVectorManager *outvectorManager;
    cIOutputScalarManager *outScalarManager;
    cISnapshotManager *snapshotManager;

    // Data for getUniqueNumber()
    unsigned long nextUniqueNumber;

    // lifecycle listeners
    std::vector<cISimulationLifecycleListener*> listeners;

    // CPU and real time limit checking
    Stopwatch stopwatch;

    simtime_t simulatedTime;  // sim. time after finishing simulation

  public:

    bool attachDebuggerOnErrors = false;


  public:
    EnvirBase();
    virtual ~EnvirBase();

    // eventlog recording
    virtual void setEventlogRecording(bool enabled);
    virtual bool getEventlogRecording() const {return recordEventlog;}
    virtual void setLogLevel(LogLevel logLevel);
    virtual void setLogFormat(const char *logFormat);

    // Utility function: optionally appends host name to fname
    virtual void processFileName(std::string& fname);

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) override;
    virtual void simulationEvent(cEvent *event) override;
    virtual void componentInitBegin(cComponent *component, int stage) override;
    virtual void messageScheduled(cMessage *msg) override;
    virtual void messageCancelled(cMessage *msg) override;
    virtual void beginSend(cMessage *msg, const SendOptions& options) override;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result) override;
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
    virtual void preconfigure(cComponent *component) override;
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
    virtual std::string resolveResourcePath(const char *fileName, cComponentType *context) override;

    // UI functions
    virtual void bubble(cComponent *component, const char *text) override;
    // leave to subclasses: virtual std::string gets(const char *prompt, const char *defaultreply=nullptr);
    // leave to subclasses: virtual cEnvir& flush();

    // RNGs
    virtual int getNumRNGs() const override;
    virtual cRNG *getRNG(int k) override;

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname) override;
    virtual void deregisterOutputVector(void *vechandle) override;
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) override;
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value) override;

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) override;
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) override;
    virtual void recordParameter(cPar *par) override;
    virtual void recordComponentType(cComponent *component) override;

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
    virtual void refOsgNode(osg::Node *scene) override {}
    virtual void unrefOsgNode(osg::Node *scene) override {}
    virtual bool idle() override;

    virtual bool ensureDebugger(cRuntimeError *error = nullptr) override;
    //@}

  protected:

    virtual EnvirOptions *createOptions() {return new EnvirOptions();}
    virtual void readOptions();
    virtual void readPerRunOptions();
    int parseSimtimeResolution(const char *resolution);

    // Called internally from readParameter(), to interactively prompt the
    // user for a parameter value.
    virtual void askParameter(cPar *par, bool unassigned) = 0;

    // Utility function for getXMLDocument() and getParsedXMLString()
    cXMLElement *resolveXMLPath(cXMLElement *documentnode, const char *path);

    // Set up RNG mapping for the component
    virtual void setupRNGMapping(cComponent *component);
};

template<class T>
T *createByClassName(const char *className, const char *what) {
    cObject *obj = createOneIfClassIsKnown(className);
    if (obj == nullptr)
        throw cRuntimeError("Cannot create %s: class \"%s\" not found", what, className);
    T *result = dynamic_cast<T *>(obj);
    if (result == nullptr)
        throw cRuntimeError("Cannot create %s: class \"%s\" is not subclassed from %s", what, className, opp_typename(typeid(T)));
    return result;
}

}  // namespace envir
}  // namespace omnetpp

#endif

