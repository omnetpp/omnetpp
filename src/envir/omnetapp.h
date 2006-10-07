//==========================================================================
//  OMNETAPP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    TOmnetApp:  abstract base class for simulation applications
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETAPP_H
#define __OMNETAPP_H

#include "carray.h"
#include "globals.h"
#include "cenvir.h"
#include "cexception.h"
#include "args.h"
#include "envdefs.h"
#include "envirext.h"
#include "cconfig.h"
#include "platdep/time.h"


class cXMLDocCache;
class cScheduler;
// WITH_PARSIM:
class cParsimCommunications;
class cParsimPartition;
class cParsimSynchronizer;
// endif


/**
 * Abstract base class for the user interface.
 *
 * Concrete user interface implementations (Cmdenv, Tvenv,
 * Tkenv's app classes) should be derived from this class.
 */
class ENVIR_API TOmnetApp
{
  protected:
    bool initialized;
    cConfiguration *config;
    cXMLDocCache *xmlcache;
    ArgList *args;

    int opt_total_stack_kb;

    bool opt_ini_warnings;
    opp_string opt_scheduler_class;
    bool opt_parsim;
// WITH_PARSIM (note: no #ifdef to preserve class layout!)
    opp_string opt_parsimcomm_class; // if opt_parsim: cParsimCommunications class to use
    opp_string opt_parsimsynch_class; // if opt_parsim: cParsimSynchronizer class to use
// end
    opp_string opt_load_libs;
    opp_string opt_network_name;

    int opt_num_rngs;
    opp_string opt_rng_class;

    opp_string opt_outputvectormanager_class;
    opp_string opt_outputscalarmanager_class;
    opp_string opt_snapshotmanager_class;
    bool opt_fname_append_host;

    bool opt_pause_in_sendmsg;
    bool opt_warnings;
    bool opt_print_undisposed;

    simtime_t opt_simtimelimit;
    long opt_cputimelimit;

    int opt_netifcheckfreq;

// WITH_PARSIM (note: no #ifdef to preserve class layout!)
    cParsimCommunications *parsimcomm;
    cParsimPartition *parsimpartition;
// end

    cScheduler *scheduler;

    int num_rngs;
    cRNG **rngs;

    cOutputVectorManager *outvectmgr;
    cOutputScalarManager *outscalarmgr;
    cSnapshotManager *snapshotmgr;

    unsigned long nextuniquenumber;

    timeval simbegtime;  // real time when sim. started
    timeval simendtime;  // real time when sim. ended
    timeval laststarted; // real time from where sim. was last cont'd
    timeval elapsedtime; // time spent simulating
    simtime_t simulatedtime;  // sim. time after finishing simulation

  protected:
    // internal
    const char *getRunSectionName(int runnumber);

  public:
    /**
     * Constructor takes command-line args and ini file instance.
     */
    TOmnetApp(ArgList *args, cConfiguration *config);

    /**
     * Destructor.
     */
    virtual ~TOmnetApp();

    /** @name Functions called from cEnvir's similar functions */
    //@{

    virtual void setup();
    virtual int run() = 0;
    virtual void shutdown();

    virtual void startRun();
    virtual void endRun();

    /**
     * Called by cPar::read() to get param value from the ini file
     */
    virtual std::string getParameter(int run_nr, const char *parname);
    virtual bool getParameterUseDefault(int run_no, const char *parname);

    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index);

    virtual void getOutVectorConfig(int run_no, const char *modname, /*input*/
                                    const char *vecname,
                                    bool& enabled, /*output*/
                                    double& starttime, double& stoptime);

    virtual cXMLElement *getXMLDocument(const char *filename, const char *path=NULL);
    //@}

    /** @name Internal methods */
    //@{
    /**
     * Prints help text.
     */
    void printHelp();

    /**
     * To be redefined to print Cmdenv or Tkenv-specific help on available
     * command-line options. Invoked from printHelp().
     */
    virtual void printUISpecificHelp() = 0;

    /**
     * Provides access to the configuration
     */
    virtual cConfiguration *getConfig();

    /**
     * Used internally to read opt_xxxxx setting from ini file.
     * Can be overloaded in subclasses, to support new options.
     */
    virtual void readOptions();
    virtual void readPerRunOptions(int run_nr);

    // Utility function: optionally appends host name to fname
    virtual void processFileName(opp_string& fname);

    // Utility function: NED file loading and list file handling
    virtual void globAndLoadNedFile(const char *fnamepattern);

    // Utility function: NED file loading and list file handling
    virtual void globAndLoadListFile(const char *fnamepattern, bool istemplistfile);

    // Utility function: NED file loading and list file handling
    virtual void processListFile(const char *listfilename, bool istemplistfile);

    /**
     * Partition Id when parallel simulation is active.
     */
    virtual int getParsimProcId();
    /**
     * Number of partitions when parallel simulation is active, otherwise 0.
     */
    virtual int getParsimNumPartitions();

    //@}

    /** @name Functions called from the objects of the simulation kernel
     * to notify the application about certain events.
     * For documentation see corresponding methods in cEnvir.
     */
    //@{
    virtual void objectDeleted(cObject *object) {}
    virtual void messageSent(cMessage *msg, cGate *directToGate) {}
    virtual void messageDelivered(cMessage *msg) {}
    virtual void breakpointHit(const char *lbl, cSimpleModule *mod) {}
    virtual void moduleMethodCalled(cModule *from, cModule *to, const char *method) {}
    virtual void moduleCreated(cModule *newmodule) {}
    virtual void moduleDeleted(cModule *module) {}
    virtual void moduleReparented(cModule *module, cModule *oldparent) {}
    virtual void connectionCreated(cGate *srcgate) {}
    virtual void connectionRemoved(cGate *srcgate) {}
    virtual void displayStringChanged(cGate *gate) {}
    virtual void displayStringChanged(cModule *submodule) {}
    virtual void backgroundDisplayStringChanged(cModule *parentmodule) {}
    virtual void undisposedObject(cObject *obj);
    //@}

    /** @name Functions called by cEnvir's similar functions.
     * They provide I/O for simple module activity functions and the sim. kernel.
     * Default versions use standard I/O.
     */
    //@{
    virtual bool isGUI() = 0;
    virtual void bubble(cModule *mod, const char *text) {}

    virtual void putmsg(const char *s) = 0;
    virtual void sputn(const char *s, int n) = 0;
    virtual void flush() = 0;
    virtual bool gets(const char *promptstr, char *buf, int len=255) = 0;  // 0==OK 1==CANCEL
    virtual int  askYesNo(const char *question) = 0; //0==NO 1==YES -1==CANCEL
    //@}

    /** @name Methods for accessing RNGs; called by cEnvir's similar functions */
    //@{
    int numRNGs();
    cRNG *rng(int k);
    void getRNGMappingFor(cModule *mod);
    //@}

    /** @name Methods for recording data from output vectors; called by cEnvir's similar functions */
    //@{
    void *registerOutputVector(const char *modulename, const char *vectorname, int tuple);
    void deregisterOutputVector(void *vechandle);
    bool recordInOutputVector(void *vechandle, simtime_t t, double value);
    bool recordInOutputVector(void *vechandle, simtime_t t, double value1, double value2);
    //@}

    /** @name Methods for output scalars; called by cEnvir's similar functions */
    //@{
    void recordScalar(cModule *module, const char *name, double value);
    //@}

    /* @name Methods for snapshot management; called by cEnvir's similar functions. */
    //@{
    std::ostream *getStreamForSnapshot();
    void releaseStreamForSnapshot(std::ostream *os);
    //@}

    /**
     * Returns how much extra stack space the user interface recommends
     * for the simple modules; called by cEnvir's similar function.
     */
    virtual unsigned extraStackForEnvir() {return 0;}

    /** Called from cEnvir's similar function */
    virtual unsigned long getUniqueNumber();

    /** @name Utility functions. */
    //@{

    /**
     * Interface to memory manager.
     */
    virtual bool memoryIsLow();

    /**
     * Original command-line args.
     */
    ArgList *argList()  {return args;}

    /**
     * Display error message.
     */
    virtual void displayError(cException *e);

    /**
     * Like displayError(), but for normal termination messages, not errors.
     */
    virtual void displayMessage(cException *e);

    /**
     * Called from cEnvir::idle().
     */
    virtual bool idle();
    //@}

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
    void stoppedWithTerminationException(cTerminationException *e);

    /**
     * Hook called when the simulation is stopped with an error.
     * Its current use is to notify parallel simulation part.
     */
    void stoppedWithException(cException *e);
    //@}
};

#endif

