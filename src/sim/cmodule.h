//==========================================================================
//   CMODULE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cModule        : common base for cCompoundModule and cSimpleModule
//    cSimpleModule  : base for simple module objects
//    cCompoundModule: compound module
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMODULE_H
#define __CMODULE_H

#include <time.h>     // time_t, clock_t in cSimulation
#include "cobject.h"
#include "ccor.h"
#include "chead.h"
#include "carray.h"
#include "cqueue.h"
#include "cgate.h"
#include "csimul.h"

//=== module state codes
enum {
       sENDED,    // module terminated
       sREADY     // module is active
};

//=== display string selector
enum {
       dispSUBMOD=0,        // display string: "as submodule"
       dispENCLOSINGMOD=1,  // display string: "as enclosing module"
       dispNUMTYPES         // this one must always be the last element
};

//=== dynamic allocation in module functions
#define NEW             new((___nosuchclass *)NULL)
#define DELETE(x)       memFree((void *&)x)

//=== classes mentioned/declared here:
class  cMessage;
class  cGate;
class  cModulePar;
class  cModule;
class  cSimpleModule;
class  cCompoundModule;
class  cNetMod;
class  cSimulation;
class  cNetworkType;


//=== to connect two gates:
SIM_API void connect(cModule *frm, int frg,
                     cLinkType *linkp,
                     cModule *tom, int tog);

SIM_API void connect(cModule *frm, int frg,
                     cPar *delayp, cPar *errorp, cPar *dataratep,
                     cModule *tom, int tog);

//=== operator new used by the NEW() macro:
class ___nosuchclass;
void *operator new(size_t m,___nosuchclass *);


//==========================================================================
// cModule - common base for cSimpleModule and cCompoundModule
//    type, its cModuleType, gates and parameters, error handling
//  NOTE: 1. no instance of cModule can be created!
//        2. dup() cannot be used! Use mod->moduleType()->create()/buildInside().

typedef TInspector *(InspCreateFunc)(cObject *, int, void *);

class SIM_API cModule : public cObject
{
    friend class cGate;
    friend class cSimulation;

  public:
    static bool pause_in_sendmsg; // if TRUE, split send() with transferToMain()

  protected:
    cModuleType *mod_type;  // type of this module
    int mod_id;             // id (subscript into cSimulation)
    cModule *parentmodp;    // pointer to parent module

  public:

    // The following members are only made public for use by the inspector
    // classes. Do not use them directly from simple modules.
    cArray gatev;           // vector of gates
    cArray paramv;          // vector of parameters
    cArray machinev;        // NET: list of machines --VA
    cHead members;          // list of data members of derived classes

  protected:
    bool warn;              // warnings on/off

    int  idx;               // index if module vector, 0 otherwise
    int  vectsize;          // vector size, -1 if not a vector

    opp_string dispstr[dispNUMTYPES]; // see setDisplayString(..) etc.

    void (*notify_inspector)(int,void*); // to notify inspector disp str changes
    void *data_for_inspector;

  protected:
    virtual void arrived(cMessage *msg,int n) = 0; // internal use

  protected:
    // The following functions are expected to be redefined in concrete
    // module types. They are made protected because they are supposed
    // to be called only via callInitialize() and callFinish().
    virtual void initialize();     // called before simulation starts
    virtual void finish();         // called after end of simulation

  public:
    cModule(cModule& mod);
    cModule(const char *name, cModule *parentmod);
    virtual ~cModule() {}

    // redefined functions
    virtual const char *className()  {return "cModule";}
    virtual cObject *dup();
    virtual void forEach(ForeachFunc f);
    virtual const char *inspectorFactoryName() {return "cModuleIFC";}
    cModule& operator=(cModule& mod);
    virtual const char *fullName();       // "name[12]" (in a static buffer!)
    virtual const char *fullPath();       // "sys.m[3].name[12]" (static buff!)

    // setting up the module
    virtual void setId(int n);              // set module id
    void setIndex(int i, int n);            // if multiple module
    void setModuleType(cModuleType *mtype);

    void addGate(const char *s, char tp);         // add one gate
    void setGateSize(const char *s, int size);    // set gate vector size

    void addPar(const char *s);                   // add parameter

    void addMachinePar(const char *pnam);            // NET: add machine par
    void setMachinePar(const char *pnam, const char *val); // NET: set parameter value

    virtual void buildInside() {}           // build submodules & internal connections

    // miscellaneous functions
    virtual bool isSimple() = 0;
    cModuleType *moduleType()   {return mod_type;}
    int id()                    {return mod_id;}
    cModule *parentModule();
    bool isOnLocalMachine();

    // if this module is in a module vector:
    bool isVector()             {return vectsize>=0;}
    int index()                 {return idx;}
    int size()                  {return vectsize<0?1:vectsize;}

    // module gates
    int gates() {return gatev.items();}             // total num of gates
    cGate *gate(int g) {return (cGate*)gatev[g];}   // gate by id
    cGate *gate(const char *gatename,int sn=-1);    // gate by name & index
    int findGate(const char *gatename, int sn=-1);  // id of a gate
    bool hasGate(const char *gatename, int sn=-1) {return findGate(gatename,sn)>=0;} // check if gate exists

    bool checkInternalConnections();      // TRUE means OK; called from NEDC code

    // module parameters
    int params() {return paramv.items();}    // no. of pars
    cPar& par(int p);                        // par by index
    cPar& par(const char *parname);          // par by name
    int findPar(const char *parname);        // index of a par; -1 of not found
    cPar& ancestorPar(const char *parname);  // search for par in parents;error if not found
    bool hasPar(const char *s) {return findPar(s)>=0;}  // check if parameter exists

    // machine parameters
    int machinePars()  {return machinev.items();}    // NET
    const char *machinePar(int i);                   // NET
    const char *machinePar(const char *machinename); // NET

    // interface for calling initialize() and finish() from outside
    virtual void callInitialize() = 0;           // calls initialize()
    virtual void callFinish() = 0;               // calls finish()

    // dynamic module creation
    virtual void scheduleStart(simtime_t t) = 0; // creates starting msg
    virtual void deleteModule() = 0;             // deletes module

    // enable/disable warnings
    bool warnings()            {return warn;}
    void setWarnings(bool wr)  {warn=wr;}

    // visualization/animation support
    void setDisplayString(int type, const char *dispstr);
    const char *displayString(int type);

    void setDisplayStringNotify(void (*notify_func)(int,void*), void *data);

};

//==========================================================================
// block on module function's heap
struct sBlock
{
    sBlock *next;
    sBlock *prev;
    cSimpleModule *mod;
};

//--------------------------------------------------------------------------
// cSimpleModule - represents a simple module in the simulation
//     cSimpleModule is an abstract base class for user-written modules

class SIM_API cSimpleModule : public cCoroutine, public cModule
{
    friend class cModule;
    friend class cSimulation;
    friend class TSimpleModInspector;
  private:
    bool usesactivity;      // uses activity() or handleMessage()
    int state;              // ended/ready/waiting for msg
    opp_string phasestr;    // a 'phase' string
    sBlock *heap;           // head of modfunc's heap list
    cMessage *timeoutmsg;   // msg used in wait() and receive() with timeout

  private:
    static void activate(void *p);  // internal use

  protected:
    virtual void arrived(cMessage *msg,int n);  // internal use

  public:
    cHead locals;           // list of local variables of module function
    cQueue putAsideQueue;   // put-aside queue

  protected:
    // The following functions are expected to be redefined in concrete
    // simple module types. They are made protected because they shouldn't
    // be called directly from outside.
    virtual void activity();                   // coroutine function
    virtual void handleMessage(cMessage *msg); // alternative to activity()

  public:
    cSimpleModule(cSimpleModule& mod);
    cSimpleModule(const char *name, cModule *parentmod, unsigned stk);
    virtual ~cSimpleModule();

    // redefined functions
    virtual const char *className()  {return "cSimpleModule";}
    virtual cObject *dup()  {return new cSimpleModule(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName() {return "cSimpleModuleIFC";}
    cSimpleModule& operator=(cSimpleModule& mod);
    virtual void forEach(ForeachFunc f);

    // redefined cModule functions:
    virtual void setId(int n);
    virtual bool isSimple() {return TRUE;}

  public:
    // return event handling scheme
    bool usesActivity()  {return usesactivity;}

    // interface for calling initialize() and finish() from outside
    virtual void callInitialize();
    virtual void callFinish();

    // dynamic module creation
    virtual void scheduleStart(simtime_t t); // creates starting msg
    virtual void deleteModule();             // deletes module

    // module time
    simtime_t simTime();   // (cannot make inline because of declaration order)

    // user-settable phase string
    void setPhase(const char *phase)  {phasestr=phase;}
    const char *phase()  {return correct(phasestr);}

    // debugging aids
    bool snapshot(cObject *obj=&simulation, const char *label=NULL); // write snapshot file
    void breakpoint(const char *label);     // user breakpoint
    void pause(const char *phase=NULL);     // set phase & temporarily yield control to main

    // sending a message through a gate
    int send(cMessage *msg, int gateid);        // send a message thru gate id
    int send(cMessage *msg, const char *gatename, int sn=-1); // s:gate name, sn:index

    // sending messages directly to another module
    int sendDirect(cMessage *msg, double delay, cModule *mod, int gateid);
    int sendDirect(cMessage *msg, double delay, cModule *mod, const char *gatename, int sn=-1);

    // delayed sending
    int sendDelayed(cMessage *msg, double delay, int gateid);
    int sendDelayed(cMessage *msg, double delay, const char *gatename, int sn=-1);

    // self-messages
    int scheduleAt(simtime_t t, cMessage *msg); // receive() will return msg at t
    cMessage *cancelEvent(cMessage *msg);       // remove cMessage sent by scheduleAt() from FES

    // parallel execution
    //   sync with receiver segment (blocks receiver at t until msg arrives)
    int syncpoint(simtime_t t, int gateid);
    int syncpoint(simtime_t t, const char *gatename, int sn=-1);
    int cancelSyncpoint(simtime_t t, int gateid);
    int cancelSyncpoint(simtime_t t, const char *gatename, int sn=-1);

    // see if there's a message scheduled for this time
    bool isThereMessage();

    // get message from the put-aside queue OR the FES
    cMessage *receive();
    cMessage *receive(simtime_t timeout);
    cMessage *receiveOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);
    cMessage *receiveOn(int gateid, simtime_t timeout=MAXTIME);

    // get message from the FES
    cMessage *receiveNew();
    cMessage *receiveNew(simtime_t timeout);
    cMessage *receiveNewOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);
    cMessage *receiveNewOn(int gateid, simtime_t timeout=MAXTIME);

    // simulation control
    void wait(simtime_t time);        // waits 'time' units
    void end();                       // ends this module
    void endSimulation();             // finish the whole simulation
    void error(const char *fmt,...);  // user error message

    // record into scalar result file
    void recordScalar(const char *name, double value);
    void recordScalar(const char *name, const char *text);
    void recordStats(const char *name, cStatistic *stats);

    // value-added 'raw' heap allocation for module functions:
    void *memAlloc(size_t m);          // allocate m bytes
    void memFree(void *&p);            // free block & NULL pointer

    // for internal use ONLY:
    void clearHeap();                  // free module's local allocations
    int moduleState() {return state;}  // return module state
};

//==========================================================================
// cCompoundModule - represents a compound module in the simulation
//   NOTE: dup() cannot be used. Use moduleType()->create() instead.

class SIM_API cCompoundModule : public cModule
{
    friend class TCompoundModInspector;
  protected:
    virtual void arrived(cMessage *msg,int n); //internal use

  public:
    cCompoundModule(cCompoundModule& mod);
    cCompoundModule(const char *name, cModule *parentmod);
    virtual ~cCompoundModule();

    // redefined functions
    virtual const char *className()  {return "cCompoundModule";}
    virtual cObject *dup()   {return new cCompoundModule(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName()  {return "cCompoundModuleIFC";}
    cCompoundModule& operator=(cCompoundModule& mod);

    // redefined cModule functions
    virtual bool isSimple() {return FALSE;}

    // interface for calling initialize() and finish() from outside
    virtual void callInitialize();   // first for this module, then for submods
    virtual void callFinish();       // first for submods, then for itself

    // dynamic module creation
    virtual void scheduleStart(simtime_t t); // starting msg for submodules
    virtual void deleteModule();             // deletes module & submodules
};

//==========================================================================
//  cSubModIterator : iterates through submodules of a compound module
//   NOTE: not a cObject descendant!

class SIM_API cSubModIterator
{
  private:
    cModule *parent;
    int i;
  public:
    cSubModIterator(cModule& p)  {parent=&p;i=0;operator++(0);}
    void init(cModule& p)        {parent=&p;i=0;operator++(0);}
    cModule& operator[](int)     {return simulation[i];} //OBSOLETE
    cModule *operator()()        {return &simulation[i];}
    bool end()                   {return (i==-1);}
    cModule *operator++(int);    // sets i to -1 if end was reached
};

#endif

