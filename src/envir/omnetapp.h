//==========================================================================
//   OMNETAPP.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    TOmnetApp:  abstract base class for simulation applications
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETAPP_H
#define __OMNETAPP_H

#include <time.h>     // time_t, clock_t
#include "carray.h"
#include "chead.h"
#include "cenvir.h"
#include "cexception.h"
#include "args.h"
#include "envdefs.h"
#include "envirext.h"


class cIniFile;

#define NUM_STARTINGSEEDS  256
extern long starting_seeds[NUM_STARTINGSEEDS];  // they are in seeds.cc

//==========================================================================
// TOmnetApp: abstract base class for the user interface.
//      Concrete user interface implementations (Cmdenv, Tvenv,
//      Tkenv's app classes) should be derived from this class, and
//      the create() static function must be written to create an object
//      of the desired type (ie.TOmnetCmdApp, TOmnetWindowsApp etc.)
//      The createSlave() function is used when doing distributed simulation:
//      it should create a "slave" application that can work with the "master"
//      app that runs on the console.
//
class ENVIR_API TOmnetApp
{
  protected:
    bool initialized;
    cIniFile *ini_file;
    ArgList *args;

    // options common for all simulation applications
    opp_string opt_inifile_name;

    int opt_total_stack_kb;

    bool opt_ini_warnings;
    bool opt_distributed; // NET
    opp_string opt_parallel_env; // PVM, MPI or something else?
    opp_string opt_load_libs;
    opp_string opt_network_name;
    opp_string opt_outputvectormanager_class;
    opp_string opt_outputscalarmanager_class;
    opp_string opt_snapshotmanager_class;

    bool opt_pause_in_sendmsg;
    bool opt_warnings;

    long *opt_genk_randomseed;

    simtime_t opt_simtimelimit;
    long opt_cputimelimit;

    int opt_netifcheckfreq;

    int next_startingseed;  // index of next seed to use

    cOutputVectorManager *outvectmgr;
    cOutputScalarManager *outscalarmgr;
    cSnapshotManager *snapshotmgr;

    time_t simbegtime;         // real time when sim. started
    time_t simendtime;         // real time when sim. ended
    time_t laststarted;        // real time from where sim. was last cont'd
    time_t elapsedtime;        // in seconds
    simtime_t simulatedtime;   // sim.time at after finishing simulation

  public:
    /**
     * Constructor takes command-line args and ini file instance.
     */
    TOmnetApp(ArgList *args, cIniFile *inifile);

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

    virtual cIniFile *getIniFile();

    /**
     * Used internally to read opt_xxxxx setting from ini file
     */
    virtual void readOptions();
    virtual void readPerRunOptions(int run_nr);

    /**
     * Used internally to make options effective in cSimulation
     * and other places
     */
    virtual void makeOptionsEffective();

    /**
     * Called by cPar::read() to get param value from the ini file
     */
    virtual const char *getParameter(int run_nr, const char *parname);

    /**
     * Called from nedc-generated network setup function to get
     * logical machine --> physical machine mapping
     */
    virtual const char *getPhysicalMachineFor(const char *logical_mach);
    virtual void getOutVectorConfig(int run_no, const char *modname, /*input*/
                                    const char *vecname,
                                    bool& enabled, /*output*/
                                    double& starttime, double& stoptime);
    virtual const char *getDisplayString(int run_no,const char *name);
    //@}

    /** @name Functions called from the objects of the simulation kernel
     *  to notify the application about certain events.
     */
    //@{
    virtual void objectDeleted(cObject *object) {}
    virtual void messageSent(cMessage *msg) {}
    virtual void messageDelivered(cMessage *msg) {}
    virtual void breakpointHit(const char *lbl, cSimpleModule *mod) {}
    //@}

    /** @name Functions called by cEnvir's similar functions.
     * They provide I/O for simple module activity functions and the sim. kernel.
     * Default versions use standard I/O.
     */
    //@{
    virtual void putmsg(const char *s);
    virtual void puts(const char *s);
    virtual void flush();
    virtual bool gets(const char *promptstr, char *buf, int len=255);  // 0==OK 1==CANCEL
    virtual int  askYesNo(const char *question); //0==NO 1==YES -1==CANCEL
    virtual void foreignPuts(const char *hostname, const char *mod, const char *str);
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
    void recordScalar(cModule *module, const char *name, const char *text);
    void recordScalar(cModule *module, const char *name, cStatistic *stats);
    //@}

    /* @name Methods for snapshot management; called by cEnvir's similar functions. */
    //@{
    ostream *getStreamForSnapshot();
    void releaseStreamForSnapshot(ostream *os);
    //@}

    /**
     * Returns how much extra stack space the user interface recommends
     * for the simple modules; called by cEnvir's similar function.
     */
    virtual unsigned extraStackForEnvir() {return 0;}

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
    time_t totalElapsed();

    //@}
};

#endif

