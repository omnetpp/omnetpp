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
  Copyright (C) 1992,99 Andras Varga
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
// cSimulation  : simulation management class; only one instance

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
     bool warn;                // if TRUE, overrides individual warn flags

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
     explicit cSimulation(const char *name, cHead *h=NULL);
     virtual ~cSimulation();

     // redefined functions
     virtual const char *className()  {return "cSimulation";}
     virtual const char *inspectorFactoryName() {return "cSimulationIFC";}
     virtual void forEach(ForeachFunc f);
     virtual void writeContents(ostream& os);
     virtual const char *fullPath() {return name();}

     // new functions
     void setup();                      // things that cannot be done
                                        // from the constructor which is
                                        // called before main()

     // module vector management
     int add(cModule *mod);             // add a new cModule; returns id
     void del(int id);                  // delete (& destroy) a module
     int lastModuleIndex()              // last module
         {return last_id;}
     bool isUnique(const char *s);      // internal

     // find module and return id (-1 if not found)
     int find(cModule *mod);
     int find(const char *modulename,int index=-1, int parentmod_id=-1);

     // find module
     cModule *moduleByPath(const char *modulepath);
     cModule *module(const char *modulename,int index=-1,int parentmod_id=-1);

     // look up by id
     cModule *module(int id)                  // get module by ID
         {return id>=0 && id<size ? vect[id] : NO(cModule);}
     cModule& operator[](int id)              // act as a vector
         {return id>=0 && id<size ? *vect[id] : *NO(cModule);}

     // selecting system module
     void setSystemModule(cModule *p)
         {systemmodp = p;}
     cModule *systemModule()
         {return systemmodp;}

     // parallel execution support: network interface
     cNetMod *netInterface()
         {return netmodp;}
     void setNetInterface(cNetMod *netif);

     // simulation / physical time
     simtime_t simTime()  // simulation time
         {return sim_time;}
     void resetClock();          //
     void startClock();          //  physical clock
     void stopClock();           //
     void checkTimes();          //  check time limits (phys. AND simul.)

     // starting / stopping simulations
     bool setupNetwork(cNetworkType *net,int run_num); // build new network
     void startRun();            // initialize network
     void callFinish();          // call finish() funcs of simple modules
     void endRun();              // close open files etc.
     void deleteNetwork();       // delete modules, msg queue etc.

     // set...
     void setTimeLimit(long t)         {tmlimit=(time_t)t;}
     void setSimTimeLimit(simtime_t t) {simtmlimit=t;}
     void incEventNumber()             {event_num++;}
     void setNetIfCheckFreq(int f)     {netif_check_freq=f;}

     // get...
     cNetworkType *networkType()     {return networktype;}
     int  runNumber()                {return run_number;}
     long timeLimit()                {return (long)tmlimit;}
     simtime_t simTimeLimit()        {return simtmlimit;}
     long eventNumber()              {return event_num;}

     // central simulation functions:
     cSimpleModule *selectNextModule();  // the scheduler function
     void doOneEvent(cSimpleModule *m);  // execute one event

     // coroutine/context management
     int transferTo(cSimpleModule *p);   // switch to simple module's coroutine
     int transferToMain();               // switch to main coroutine
     void setContextModule(cModule *p);  // set contextmodp and locallistp
     void setGlobalContext()             {contextmodp=NULL;locallistp=&locals;}

     void setLocalList(cHead *p)         {locallistp=p;}
     cSimpleModule *runningModule()      {return runningmodp;}
     cModule       *contextModule()      {return contextmodp;}
     cSimpleModule *contextSimpleModule(); // cannot make inline! would require cmodule.h because of dynamic cast
     cHead *localList()                  {return locallistp==NULL?(&locals):locallistp;}

     // snapshot(): to be called from cSimpleModule::snapshot()
     bool snapshot(cObject *obj, const char *label);

     // record into scalar result file
     void recordScalar(const char *name, double value);
     void recordScalar(const char *name, const char *text);
     void recordStats(const char *name, cStatistic *stats);

     // errors / warnings / messages
     // Note: error() and warning() are supposed to be called through the
     //   opp_error() and opp_warning() global utility functions.
     bool warnings()          {return warn;}
     void setWarnings(bool w) {warn=w;}
     void terminate(int errcode,const char *message); // print message + set error num
     void error(int errcode,const char *message);     // general error handler
     void warning(int errcode,const char *message);   // message + question:continue/abort?
     bool ok()         {return err==eOK;}       // TRUE if sim. can go on
     int errorCode()   {return err;}            // error code
     void setErrorCode(int e) {err=e;}          // set error code without giving error message
     bool normalTermination();                  // examines error code
     void resetError() {err=eOK;}               // reset error code

};

//==========================================================================
//=== operator new used by the NEW() macro:
class ___nosuchclass;
void *operator new(size_t m,___nosuchclass *);

#endif

