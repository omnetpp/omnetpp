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

#ifndef __ENVIRBASE_H
#define __ENVIRBASE_H

#include "carray.h"
#include "ccomponent.h"
#include "globals.h"
#include "cenvir.h"
#include "cexception.h"
#include "args.h"
#include "envirdefs.h"
#include "envirext.h"
#include "eventlogfilemgr.h"
#include "cconfiguration.h"
#include "timeutil.h"
#include "cresultlistener.h"

NAMESPACE_BEGIN


class cXMLDocCache;
class cScheduler;
class cModuleType;
class cIListener;
class cProperty;
// WITH_PARSIM:
class cParsimCommunications;
class cParsimPartition;
class cParsimSynchronizer;
// endif
class cResultRecorder;
class SignalSource;

// assumed maximum length for getFullPath() string.
// note: this maximum actually not enforced anywhere
#define MAX_OBJECTFULLPATH  1024

// maximum length of string passed in Enter_Method() (longer strings will be truncated)
#define MAX_METHODCALL 1024


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

    //
    // Configuration options
    //
    size_t opt_total_stack;
    opp_string opt_scheduler_class;
    bool opt_parsim;
// WITH_PARSIM (note: no #ifdef to preserve class layout!)
    opp_string opt_parsimcomm_class; // if opt_parsim: cParsimCommunications class to use
    opp_string opt_parsimsynch_class; // if opt_parsim: cParsimSynchronizer class to use
// end
    opp_string opt_network_name;
    opp_string opt_inifile_network_dir; // dir of the inifile containing "network="

    int opt_num_rngs;
    opp_string opt_rng_class;
    int opt_seedset; // which set of seeds to use

    opp_string opt_outputvectormanager_class;
    opp_string opt_outputscalarmanager_class;
    opp_string opt_snapshotmanager_class;
    bool opt_debug_statistics_recording;
    bool opt_check_signals;
    bool opt_fname_append_host;

    bool opt_warnings;
    bool opt_print_undisposed;

    simtime_t opt_simtimelimit;
    long opt_cputimelimit;
    simtime_t opt_warmupperiod;

    opp_string opt_fingerprint;

// WITH_PARSIM (note: no #ifdef to preserve class layout!)
    cParsimCommunications *parsimcomm;
    cParsimPartition *parsimpartition;
// end

    // Random number generators. Module RNG's map to these RNG objects.
    int num_rngs;
    cRNG **rngs;

    // Output file managers
    EventlogFileManager *eventlogmgr;  // NULL if no eventlog is being written, must be non NULL if record_eventlog is true
    cOutputVectorManager *outvectormgr;
    cOutputScalarManager *outscalarmgr;
    cSnapshotManager *snapshotmgr;

    // Data for getUniqueNumber()
    unsigned long nextuniquenumber;

    timeval simbegtime;  // real time when sim. started
    timeval simendtime;  // real time when sim. ended
    timeval laststarted; // real time from where sim. was last cont'd
    timeval elapsedtime; // time spent simulating
    simtime_t simulatedtime;  // sim. time after finishing simulation

  protected:
    // leave to subclasses: virtual void putsmsg(const char *msg);
    // leave to subclasses: virtual bool askyesno(const char *msg);
    virtual void sputn(const char *s, int n);
    virtual std::string makeDebuggerCommand();
    static void crashHandler(int signum);

  public:
    /**
     * Constructor.
     */
    EnvirBase();

    /**
     * Destructor.
     */
    virtual ~EnvirBase();

    /** @name Functions called from cEnvir's similar functions */
    //@{
    // life cycle
    virtual int run(int argc, char *argv[], cConfiguration *config);

    virtual void setEventlogRecording(bool enabled);
    virtual bool hasEventlogRecordingIntervals() const;
    virtual void clearEventlogRecordingIntervals();

    // eventlog callback interface
    virtual void objectDeleted(cObject *object);
    virtual void simulationEvent(cMessage *msg);
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
    virtual void moduleReparented(cModule *module, cModule *oldparent);
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
    //@}

  protected:
    // functions added locally
    virtual bool simulationRequired();
    virtual bool setup();  // true==OK
    virtual void run() = 0;
    virtual void shutdown();

    virtual void setupNetwork(cModuleType *network);
    virtual void startRun();
    virtual void endRun();

    // utility function; never returns NULL
    cModuleType *resolveNetwork(const char *networkname);

    void printHelp();
    void setupEventLog();

    /**
     * Prints the contents of a registration list to the standard output.
     */
    void dumpComponentList(const char *category);

    /**
     * To be redefined to print Cmdenv or Tkenv-specific help on available
     * command-line options. Invoked from printHelp().
     */
    virtual void printUISpecificHelp() = 0;

    /**
     * Used internally to read opt_xxxxx setting from ini file.
     * Can be overloaded in subclasses, to support new options.
     */
    virtual void readOptions();
    virtual void readPerRunOptions();

    /**
     * Called internally from readParameter(), to interactively prompt the
     * user for a parameter value.
     */
    virtual void askParameter(cPar *par, bool unassigned) = 0;

    /**
     * Called from configure(component); adds result recording listeners
     * for each declared signal (@statistic property) in the component.
     */
    virtual void addResultRecorders(cComponent *component);


  public:
    // Utility function: optionally appends host name to fname
    virtual void processFileName(opp_string& fname);

  protected:
    // Utility function: checks simulation fingerprint and displays a message accordingly
    void checkFingerprint();

    // Utility function: adds result recording listeners for the given signal (if it's non-null) or for the given @statistic property.
    // If signal is specified, it will override the source= key in statisticProperty.
    // The index of statisticProperty is ignored; statisticName will be used as name of the statistic instead.
    virtual void doAddResultRecorders(cComponent *component, std::string& componentFullPath, const char *statisticName, cProperty *statisticProperty, simsignal_t signal=SIMSIGNAL_NULL);
    // Utility function for addResultRecorders()
    std::vector<std::string> extractRecorderList(const char *modesOption, cProperty *statisticProperty);
    // Utility function for addResultRecorders()
    SignalSource doStatisticSource(cComponent *component, const char *statisticName, const char *sourceSpec, bool needWarmupFilter);
    // Utility function for addResultRecorders()
    void doResultRecorder(const SignalSource& source, const char *mode, bool scalarsEnabled, bool vectorsEnabled, cComponent *component, const char *statisticName, cProperty *attrsProperty);
    // Utility function for addResultRecorders()
    void dumpResultRecorders(cComponent *component);
    void dumpResultRecorderChain(cResultListener *listener, int depth);
    // Utility function for getXMLDocument() and getParsedXMLString()
    cXMLElement *resolveXMLPath(cXMLElement *documentnode, const char *path);

    /**
     * Original command-line args.
     */
    ArgList *argList()  {return args;}

    /**
     * Display the exception.
     */
    virtual void displayException(std::exception& e);

    /** @name Measuring elapsed time. */
    //@{
    /**
     * Checks if the current simulation has reached the simulation
     * or real time limits, and if so, throws an appropriate exception.
     */
    void checkTimeLimits();

    /**
     * Resets the clock measuring the elapsed (real) time spent in this
     * simulation run.
     */
    void resetClock();

    /**
     * Start measuring elapsed (real) time spent in this simulation run.
     */
    void startClock();

    /**
     * Stop measuring elapsed (real) time spent in this simulation run.
     */
    void stopClock();

    /**
     * Elapsed time
     */
    timeval totalElapsed();
    //@}

    //@{
    /**
     * Hook called when the simulation terminates normally.
     * Its current use is to notify parallel simulation part.
     */
    void stoppedWithTerminationException(cTerminationException& e);

    /**
     * Hook called when the simulation is stopped with an error.
     * Its current use is to notify parallel simulation part.
     */
    void stoppedWithException(std::exception& e);
    //@}
};

NAMESPACE_END


#endif

