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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIRBASE_H
#define __ENVIRBASE_H

#include "carray.h"
#include "globals.h"
#include "cenvir.h"
#include "cexception.h"
#include "args.h"
#include "envirdefs.h"
#include "envirext.h"
#include "eventlogfilemgr.h"
#include "cconfiguration.h"
#include "timeutil.h"

NAMESPACE_BEGIN


class cXMLDocCache;
class cScheduler;
// WITH_PARSIM:
class cParsimCommunications;
class cParsimPartition;
class cParsimSynchronizer;
// endif


/**
 * Abstract base class for the user interface. Concrete user interface
 * implementations (Cmdenv, Tkenv) should be derived from this class.
 */
class ENVIR_API EnvirBase : public cEnvir
{
  protected:
    bool initialized;
    cConfiguration *cfg;
    ArgList *args;
    cXMLDocCache *xmlcache;

    //
    // Configuration options
    //
    int opt_total_stack_kb;
    bool opt_ini_warnings;
    opp_string opt_scheduler_class;
    bool opt_parsim;
// WITH_PARSIM (note: no #ifdef to preserve class layout!)
    opp_string opt_parsimcomm_class; // if opt_parsim: cParsimCommunications class to use
    opp_string opt_parsimsynch_class; // if opt_parsim: cParsimSynchronizer class to use
// end
    opp_string opt_network_name;
    opp_string opt_network_inifilepackage;

    int opt_num_rngs;
    opp_string opt_rng_class;
    int opt_seedset; // which set of seeds to use

    opp_string opt_outputvectormanager_class;
    opp_string opt_outputscalarmanager_class;
    opp_string opt_snapshotmanager_class;
    bool opt_record_eventlog;
    bool opt_fname_append_host;

    bool opt_warnings;
    bool opt_print_undisposed;

    simtime_t opt_simtimelimit;
    long opt_cputimelimit;

    opp_string opt_fingerprint;

// WITH_PARSIM (note: no #ifdef to preserve class layout!)
    cParsimCommunications *parsimcomm;
    cParsimPartition *parsimpartition;
// end

    // The scheduler object
    cScheduler *scheduler;

    // Random number generators. Module RNG's map to these RNG objects.
    int num_rngs;
    cRNG **rngs;

    // Unique "run ID" string (gets written out into output files)
    opp_string runid;

    // Output file managers
    EventlogFileManager *eventlogmgr;  // NULL if no event log is being written
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

    // eventlog callback interface
    virtual void objectDeleted(cObject *object);
    virtual void simulationEvent(cMessage *msg);
    // leave to subclasses: virtual void messageSent_OBSOLETE(cMessage *msg, cGate *directToGate=NULL);
    virtual void messageScheduled(cMessage *msg);
    virtual void messageCancelled(cMessage *msg);
    virtual void beginSend(cMessage *msg);
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay=0, simtime_t transmissionDelay=0);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    virtual void endSend(cMessage *msg);
    virtual void messageDeleted(cMessage *msg);
    virtual void moduleReparented(cModule *module, cModule *oldparent);
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *method);
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
    virtual void readParameter(cPar *parameter);
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index);
    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL);
    // leave to subclasses: virtual unsigned extraStackForEnvir();
    virtual cConfiguration *config();

    // UI functions
    virtual void bubble(cComponent *component, const char *text);
    // leave to subclasses: virtual std::string gets(const char *prompt, const char *defaultreply=NULL);
    // leave to subclasses: virtual cEnvir& flush();

    // RNGs
    virtual int numRNGs();
    virtual cRNG *rng(int k);
    virtual void getRNGMappingFor(cComponent *component);

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname);
    virtual void deregisterOutputVector(void *vechandle);
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value);
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value);

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=NULL);
    virtual void recordScalar(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=NULL);

    // snapshot file
    virtual std::ostream *getStreamForSnapshot();
    virtual void releaseStreamForSnapshot(std::ostream *os);

    // misc
    virtual int argCount();
    virtual char **argVector();
    virtual int getParsimProcId();
    virtual int getParsimNumPartitions();
    virtual const char *getRunId()  {return runid.c_str();}
    virtual unsigned long getUniqueNumber();
    virtual bool idle();
    //@}

  protected:
    // functions added locally
    virtual void setup();
    virtual int run() = 0;
    virtual void shutdown();

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

  public:
    // Utility function: optionally appends host name to fname
    virtual void processFileName(opp_string& fname);

  protected:
    // Utility function: checks simulation fingerprint and displays a message accordingly
    void checkFingerprint();

    /**
     * Original command-line args.
     */
    ArgList *argList()  {return args;}

    /**
     * Display error message.
     */
    virtual void displayError(std::exception& e);

    /**
     * Like displayError(), but for normal termination messages, not errors.
     */
    virtual void displayMessage(std::exception& e);

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

