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
enum { dispSUBMOD=0,        // display string: "as submodule"
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

//=== global vector of modules:
extern cSimulation simulation;

extern char *modstate[];

//=== to connect two gates:
void connect(cModule *frm, int frg,
             cLinkType *linkp,
             cModule *tom, int tog);

void connect(cModule *frm, int frg,
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

class cModule : public cObject
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

        char *dispstr[dispNUMTYPES]; // see setDisplayString(..) etc.

        void (*notify_inspector)(int,void*); // to notify inspector disp str changes
        void *data_for_inspector;

      protected:
        virtual void arrived(cMessage *msg,int n) = 0; // internal use

      public:
        cModule(cModule& mod);
        cModule(char *namestr, cModule *parentmod);
        virtual ~cModule() {}

        // redefined functions
        virtual char *isA()     {return "cModule";}
        virtual cObject *dup();
        virtual void forEach(ForeachFunc f);
        virtual char *inspectorFactoryName() {return "cModuleIFC";}
        cModule& operator=(cModule& mod);
        virtual char *fullName();       // "namestr[12]" (in a static buffer!)
        virtual char *fullPath();       // "sys.m[3].namestr[12]" (static buff!)

        // setting up the module
        virtual void setId(int n);              // set module id
        void setIndex(int i, int n);            // if multiple module
        void setModuleType(cModuleType *mtype);

        void addGate(char *s, char tp);         // add one gate
        void setGateSize(char *s, int size);    // set gate vector size

        void addPar(char *s);                   // add parameter

        void addMachinePar(char *pnam);            // NET: add machine par
        void setMachinePar(char *pnam, char *val); // NET: set parameter value

        virtual void buildInside() {}           // build submodules & internal connections

        // miscellaneous functions
        virtual bool isSimple() = 0;
        cModuleType *moduleType()   {return mod_type;}
        int id()                    {return mod_id;}
        cModule *parentModule();

        bool isVector()             {return vectsize>=0;}
        int index()                 {return idx;}
        int size()                  {return vectsize<0?1:vectsize;}
        bool isOnLocalMachine();

        // module gates
        int gates() {return gatev.items();}            // total num of gates
        cGate *gate(int g) {return (cGate*)&gatev[g];} // gate by id
        cGate *gate(char *s,int sn=-1);                // gate by name & index
        int findGate(char *s, int sn=-1);              // id of a gate

        bool cModule::checkInternalConnections();  // OK=TRUE; called from NEDC code

        // module parameters
        int params() {return paramv.items();} // no. of pars
        cPar& par(int p);                     // par by index
        cPar& par(char *namestr);             // par by name
        int findPar(char *s);                 // index of a par; -1 of not found
        cPar& ancestorPar(char *namestr);     // search for par in parents;error if not found

        // machine parameters
        int machinePars()  {return machinev.items();}  // NET
        char *machinePar(int i);         // NET
        char *machinePar(char *pname);   // NET

        // dynamic module creation
        virtual void scheduleStart(simtime_t t) = 0; // creates starting msg
        virtual void deleteModule() = 0;             // deletes module

        // enable/disable warnings
        bool warnings()            {return warn;}
        void setWarnings(bool wr)  {warn=wr;}

        // visualization/animation support
        void setDisplayString(int type,char *s);
        char *displayString(int type);

        void setDisplayStringNotify(void (*notify_func)(int,void*), void *data);

};

//==========================================================================
// block on module function's heap
struct sBlock
{
       sBlock       *next;
       sBlock       *prev;
       cSimpleModule *mod;
};

//--------------------------------------------------------------------------
// cSimpleModule - represents a simple module in the simulation
//     cSimpleModule is an abstract base class for user-written modules

class cSimpleModule : public cCoroutine, public cModule
{
        friend class cModule;
        friend class TSimpleModInspector;
      private:
        bool usesactivity;      // uses activity() or handleMessage()
        int state;              // ended/ready/waiting for msg
        opp_string phse;        // a 'phase' string
        sBlock *heap;           // head of modfunc's heap list
        cMessage *timeoutmsg;   // msg used in wait() and receive() with timeout
      protected:
        virtual void arrived(cMessage *msg,int n); // internal use

      public:
        cHead locals;           // list of local variables of module function
        cQueue putAsideQueue;   // put-aside queue

      public:
        cSimpleModule(cSimpleModule& mod);
        cSimpleModule(char *namestr, cModule *parentmod, unsigned stk);
        virtual ~cSimpleModule();

        // redefined functions
        virtual char *isA()     {return "cSimpleModule";}
        virtual cObject *dup()  {return new cSimpleModule(*this);}
        virtual void info(char *buf);
        virtual char *inspectorFactoryName() {return "cSimpleModuleIFC";}
        cSimpleModule& operator=(cSimpleModule& mod);
        virtual void forEach(ForeachFunc f);

        // redefined cModule functions:
        virtual void setId(int n);
        virtual bool isSimple() {return TRUE;}

        // return event handling scheme
        bool usesActivity()  {return usesactivity;}

        // The following two functions are expected to be redefined in
        // concrete applications:
        virtual void initialize() {}   // called before simulation starts
        virtual void activity();       // module function
        virtual void handleMessage(cMessage *msg); // alternative to activity()
        virtual void finish() {}       // called after end of simulation

        // module time
        simtime_t simTime(); // (cannot make inline because of declaration order)

        // user-settable phase string
        void setPhase(char *s) {phse=s;}
        char *phase()    {return correct(phse);}

        // debugging aids
        bool snapshot(cObject *obj=&simulation,char *label=NULL); // write snapshot file
        void breakpoint(char *label);     // user bkpt
        void pause(char *s=NULL);         // set phase & temporarily yield control to main

        // sending a message through a gate
        int send(cMessage *msg, int id);        // send a message thru gate id
        int send(cMessage *msg, char *s, int sn=-1); // s:gate name, sn:index

        // sending messages directly to another module
        int sendDirect(cMessage *msg, double delay, cModule *mod, int id);
        int sendDirect(cMessage *msg, double delay, cModule *mod, char *s, int sn=-1);

        // delayed sending
        int sendDelayed(cMessage *msg, double delay, int id);
        int sendDelayed(cMessage *msg, double delay, char *s, int sn=-1);

        // self-messages
        int scheduleAt(simtime_t t, cMessage *msg); // receive() will return msg at t
        cMessage *cancelEvent(cMessage *msg);        // remove cMessage sent by scheduleAt() from FES

        // parallel execution
        //   sync with receiver segment (blocks receiver at t until msg arrives)
        int syncpoint(simtime_t t, int id);
        int syncpoint(simtime_t t, char *s, int sn=-1);
        int cancelSyncpoint(simtime_t t, int id);
        int cancelSyncpoint(simtime_t t, char *s, int sn=-1);

        // see if there's a message scheduled for this time
        bool isThereMessage();

        // get message from the put-aside queue OR the FES
        cMessage *receive();             // get a message from queue
        cMessage *receive(simtime_t t);  // get a message from queue, t timeout
        cMessage *receiveOn(char *s, int sn=-1, simtime_t t=MAXTIME);
        cMessage *receiveOn(int id, simtime_t t=MAXTIME);             // t timeout, on specified gate

        // get message from the FES
        cMessage *receiveNew();             // get a message from queue
        cMessage *receiveNew(simtime_t t);  // get a message from queue, t timeout
        cMessage *receiveNewOn(char *s, int sn=-1, simtime_t t=MAXTIME); // getmsg/peekmsg
        cMessage *receiveNewOn(int id, simtime_t t=MAXTIME);             // t timeout, on specified gate

        // simulation control
        void wait(simtime_t time);      // waits 'time' units
        void end();                     // ends this module
        void endSimulation();           // finish the whole simulation
        void error(char *fmt,...);      // user error message

        // dynamic module creation
        virtual void scheduleStart(simtime_t t); // creates starting msg
        virtual void deleteModule();             // deletes module

        // record into scalar result file
        void recordScalar(char *name, double value);
        void recordScalar(char *name, char *text);
        void recordStats(char *name, cStatistic *stats);

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

class cCompoundModule : public cModule
{
        friend class TCompoundModInspector;
      protected:
        virtual void arrived(cMessage *msg,int n); //internal use

      public:
        cCompoundModule(cCompoundModule& mod);
        cCompoundModule(char *namestr, cModule *parentmod);
        virtual ~cCompoundModule();

        // redefined functions
        virtual char *isA()     {return "cCompoundModule";}
        virtual cObject *dup()     {return new cCompoundModule(*this);}
        virtual void info(char *buf);
        virtual char *inspectorFactoryName() {return "cCompoundModuleIFC";}
        cCompoundModule& operator=(cCompoundModule& mod);

        // redefined cModule functions
        virtual bool isSimple() {return FALSE;}
        virtual void scheduleStart(simtime_t t); // starting msg for submodules
        virtual void deleteModule();             // deletes module & submodules
};

//==========================================================================
//  cSubModIterator : iterates through submodules of a compound module
//   NOTE: not a cObject descendant!

class cSubModIterator
{
   private:
      cModule *parent;
      int i;
   public:
      cSubModIterator(cModule& p)  {parent=&p;i=0;operator++(0);}
      void init(cModule& p)    {parent=&p;i=0;operator++(0);}
      cModule& operator[](int) {return simulation[i];} //OBSOLETE
      cModule *operator()()    {return &simulation[i];}
      bool end()            {return (i==-1);}
      cModule *operator++(int); // sets i to -1 if end was reached
};

#endif

