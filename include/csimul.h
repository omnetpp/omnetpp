//==========================================================================
//   CSIMUL.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cSimulation  : simulation management class; only one instance
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSIMUL_H
#define __CSIMUL_H

#include <time.h>     // time_t, clock_t in cSimulation
#include "defs.h"
#include "util.h"
#include "errmsg.h"
#include "chead.h"
#include "cmsgheap.h"
#include "coutvect.h"
#include "ccor.h"

//=== classes mentioned:
class  cMessage;
class  cGate;
class  cModulePar;
class  cModule;
class  cSimpleModule;
class  cCompoundModule;
class  cNetMod;
class  cSimulation;
class  cNetworkType;
class  TModInspector;
class  cStatistic;

//=== global vector of modules:
SIM_API extern cSimulation simulation;

//==========================================================================

/**
 * Manages the simulation. It has only one instance.
 */
class SIM_API cSimulation : public cObject
{
    friend class cSimpleModule;

  private:
    // variables of the module vector
    int size;                 // size of vector
    int delta;                // if needed, grows by delta
    cModule **vect;           // vector of modules, vect[0] is not used
    int last_id;              // index of last used pos. in vect[]

    // simulation global vars
    cModule *systemmodp;      // pointer to system module
    cSimpleModule *runningmodp; // the currently executing module (NULL if in main)
    cModule *contextmodp;     // module in context (or NULL)
    cHead *locallistp;        // owner of newly created objects
    cHead locals;             // "global" local objects list

    cNetMod *netmodp;         // if runs distributed; communications
                              //      (network interface) module

    int err;                  // error code, 0 (== eOK) if no error
    bool warn;                // if true, overrides individual warn flags

    simtime_t sim_time;       // simulation time (time of current event)
    long event_num;           // sequence number of current event

    int netif_check_freq;     // (distributed execution:) frequency of processing
    int netif_check_cntr;     // msgs from other segments

  private:
    cNetworkType *networktype; // ptr to network creator object
    int run_number;            // which simulation run
    time_t simbegtime;         // real time when sim. started
    time_t simendtime;         // real time when sim. ended
    time_t laststarted;        // real time from where sim. was last cont'd
    time_t elapsedtime;        // in seconds
    time_t tmlimit;            // real time limit in seconds
    simtime_t simulatedtime;   // sim.time at after finishing simulation
    simtime_t simtmlimit;      // simulation time limit
    cSimpleModule *backtomod;  // used in cSimpleModule::wait/sendmsg
    cCoroutine runningmod_deleter; // used when a simple module deletes itself

  public:
    cMessageHeap msgQueue;            // future messages (FES)

    cOutFileMgr outvectfilemgr;   // output vector file manager
    cOutFileMgr scalarfilemgr;    // output scalar file manager
    cOutFileMgr parchangefilemgr; // parameter change log file manager
    cOutFileMgr snapshotfilemgr;  // snapshot file manager
    bool logparchanges;           // module parameter change logging on/off
    bool scalarfile_header_written;

  public:
    /**
     * Constructor.
     */
    explicit cSimulation(const char *name, cHead *h=NULL);

    /**
     * Destructor.
     */
    virtual ~cSimulation();

     // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cSimulation";}

    /**
     * MISSINGDOC: cSimulation:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cSimulationIFC";}

    /**
     * MISSINGDOC: cSimulation:void forEach(ForeachFunc)
     */
    virtual void forEach(ForeachFunc f);

    /**
     * MISSINGDOC: cSimulation:void writeContents(ostream&)
     */
    virtual void writeContents(ostream& os);

    /**
     * MISSINGDOC: cSimulation:char*fullPath()
     */
    virtual const char *fullPath() const {return name();}

     // new functions

    /**
     * FIXME: new functions
     */
    void setup();                      // things that cannot be done
                                        // from the constructor (which is
                                        // called before main())

     // module vector management

    /**
     * FIXME: from the constructor (which is
     * called before main())
     * module vector management
     */
    int add(cModule *mod);             // add a new cModule; returns id

    /**
     * MISSINGDOC: cSimulation:void del(int)
     */
    void del(int id);                  // delete (& destroy) a module

    /**
     * MISSINGDOC: cSimulation:int lastModuleIndex()
     */
    int lastModuleIndex()              // last module
         {return last_id;}

     // find modules by path

    /**
     * FIXME: find modules by path
     */
    cModule *moduleByPath(const char *modulepath);

     // look up modules by id

    /**
     * FIXME: look up modules by id
     */
    cModule *module(int id)                  // get module by ID
         {return id>=0 && id<size ? vect[id] : NO(cModule);}

    /**
     * MISSINGDOC: cSimulation:cModule&operator[](int)
     */
    cModule& operator[](int id)              // act as a vector
         {return id>=0 && id<size ? *vect[id] : *NO(cModule);}

     // set/get system module

    /**
     * FIXME: set/get system module
     */
    void setSystemModule(cModule *p)
         {systemmodp = p;}

    /**
     * MISSINGDOC: cSimulation:cModule*systemModule()
     */
    cModule *systemModule()
         {return systemmodp;}

     // parallel execution support: network interface

    /**
     * FIXME: parallel execution support: network interface
     */
    cNetMod *netInterface()
         {return netmodp;}

    /**
     * MISSINGDOC: cSimulation:void setNetInterface(cNetMod*)
     */
    void setNetInterface(cNetMod *netif);

     // simulation / physical time

    /**
     * FIXME: simulation / physical time
     */
    simtime_t simTime()  // simulation time
         {return sim_time;}

    /**
     * MISSINGDOC: cSimulation:void resetClock()
     */
    void resetClock();          //

    /**
     * MISSINGDOC: cSimulation:void startClock()
     */
    void startClock();          //  physical clock

    /**
     * MISSINGDOC: cSimulation:void stopClock()
     */
    void stopClock();           //

    /**
     * MISSINGDOC: cSimulation:void checkTimes()
     */
    void checkTimes();          //  check time limits (phys. AND simul.)

     // starting / stopping simulations

    /**
     * FIXME: starting / stopping simulations
     */
    bool setupNetwork(cNetworkType *net,int run_num); // build new network

    /**
     * MISSINGDOC: cSimulation:void startRun()
     */
    void startRun();            // initialize network

    /**
     * MISSINGDOC: cSimulation:void callFinish()
     */
    void callFinish();          // call finish() funcs of simple modules

    /**
     * MISSINGDOC: cSimulation:void endRun()
     */
    void endRun();              // close open files etc.

    /**
     * MISSINGDOC: cSimulation:void deleteNetwork()
     */
    void deleteNetwork();       // delete modules, msg queue etc.

     // set...

    /**
     * FIXME: set...
     */
    void setTimeLimit(long t)         {tmlimit=(time_t)t;}

    /**
     * MISSINGDOC: cSimulation:void setSimTimeLimit(simtime_t)
     */
    void setSimTimeLimit(simtime_t t) {simtmlimit=t;}

    /**
     * MISSINGDOC: cSimulation:void incEventNumber()
     */
    void incEventNumber()             {event_num++;}

    /**
     * MISSINGDOC: cSimulation:void setNetIfCheckFreq(int)
     */
    void setNetIfCheckFreq(int f)     {netif_check_freq=f;}

     // get...

    /**
     * FIXME: get...
     */
    cNetworkType *networkType()     {return networktype;}

    /**
     * MISSINGDOC: cSimulation:int runNumber()
     */
    int  runNumber()                {return run_number;}

    /**
     * MISSINGDOC: cSimulation:long timeLimit()
     */
    long timeLimit()                {return (long)tmlimit;}

    /**
     * MISSINGDOC: cSimulation:simtime_t simTimeLimit()
     */
    simtime_t simTimeLimit()        {return simtmlimit;}

    /**
     * MISSINGDOC: cSimulation:long eventNumber()
     */
    long eventNumber()              {return event_num;}

     // central simulation functions:

    /**
     * FIXME: central simulation functions:
     */
    cSimpleModule *selectNextModule();  // the scheduler function

    /**
     * MISSINGDOC: cSimulation:void doOneEvent(cSimpleModule*)
     */
    void doOneEvent(cSimpleModule *m);  // execute one event

     // coroutine/context management

    /**
     * FIXME: coroutine/context management
     */
    int transferTo(cSimpleModule *p);   // switch to simple module's coroutine

    /**
     * MISSINGDOC: cSimulation:int transferToMain()
     */
    int transferToMain();               // switch to main coroutine

    /**
     * MISSINGDOC: cSimulation:void setContextModule(cModule*)
     */
    void setContextModule(cModule *p);  // set contextmodp and locallistp

    /**
     * MISSINGDOC: cSimulation:void setGlobalContext()
     */
    void setGlobalContext()             {contextmodp=NULL;locallistp=&locals;}


    /**
     * MISSINGDOC: cSimulation:void setLocalList(cHead*)
     */
    void setLocalList(cHead *p)         {locallistp=p;}

    /**
     * MISSINGDOC: cSimulation:cSimpleModule*runningModule()
     */
    cSimpleModule *runningModule()      {return runningmodp;}

    /**
     * MISSINGDOC: cSimulation:cModule*contextModule()
     */
    cModule       *contextModule()      {return contextmodp;}

    /**
     * MISSINGDOC: cSimulation:cSimpleModule*contextSimpleModule()
     */
    cSimpleModule *contextSimpleModule(); // cannot make inline! would require cmodule.h because of dynamic cast

    /**
     * MISSINGDOC: cSimulation:cHead*localList()
     */
    cHead *localList()                  {return locallistp==NULL?(&locals):locallistp;}

     // snapshot(): to be called from cSimpleModule::snapshot()

    /**
     * FIXME: snapshot(): to be called from cSimpleModule::snapshot()
     */
    bool snapshot(cObject *obj, const char *label);

     // record into scalar result file

    /**
     * FIXME: record into scalar result file
     */
    void recordScalar(const char *name, double value);

    /**
     * MISSINGDOC: cSimulation:void recordScalar(char*,char*)
     */
    void recordScalar(const char *name, const char *text);

    /**
     * MISSINGDOC: cSimulation:void recordStats(char*,cStatistic*)
     */
    void recordStats(const char *name, cStatistic *stats);

     // errors / warnings / messages
     // NOTE: error() and warning() are supposed to be called through the
     //   opp_error() and opp_warning() global utility functions.

    /**
     * FIXME: errors / warnings / messages
     * NOTE: error() and warning() are supposed to be called through the
     *   opp_error() and opp_warning() global utility functions.
     */
    bool warnings()          {return warn;}

    /**
     * MISSINGDOC: cSimulation:void setWarnings(bool)
     */
    void setWarnings(bool w) {warn=w;}

    /**
     * MISSINGDOC: cSimulation:void terminate(int,char*)
     */
    void terminate(int errcode,const char *message); // print message + set error num

    /**
     * MISSINGDOC: cSimulation:void error(int,char*)
     */
    void error(int errcode,const char *message);     // general error handler

    /**
     * MISSINGDOC: cSimulation:void warning(int,char*)
     */
    void warning(int errcode,const char *message);   // message + question:continue/abort?

    /**
     * MISSINGDOC: cSimulation:bool ok()
     */
    bool ok()         {return err==eOK;}       // true if sim. can go on

    /**
     * MISSINGDOC: cSimulation:int errorCode()
     */
    int errorCode()   {return err;}            // error code

    /**
     * MISSINGDOC: cSimulation:void setErrorCode(int)
     */
    void setErrorCode(int e) {err=e;}          // set error code without giving error message

    /**
     * MISSINGDOC: cSimulation:bool normalTermination()
     */
    bool normalTermination();                  // examines error code

    /**
     * MISSINGDOC: cSimulation:void resetError()
     */
    void resetError() {err=eOK;}               // reset error code

};

//==========================================================================
//=== operator new used by the NEW() macro:
class ___nosuchclass;
void *operator new(size_t m,___nosuchclass *);

#endif
