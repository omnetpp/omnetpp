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
  Copyright (C) 1992-2001 Andras Varga
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
#define opp_new             new((___nosuchclass *)NULL)
#define opp_delete          memFree((void *&)x)

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

// Function to notify inspector about display string changes
//  args: (sub)module which changed; immediate refresh wanted or not; inspector's data
typedef void (*DisplayStringNotifyFunc)(cModule*,bool,void*);

//==========================================================================

/**
 * Common base for cSimpleModule and cCompoundModule. Provides gates,
 * parameters, error handling.
 *
 * NOTE: dup() cannot be use with modules. Use mod->moduleType()->create()/buildInside()
 * instead.
 */
class SIM_API cModule : public cObject
{
    friend class cGate;
    friend class cSimulation;

  public:
    static bool pause_in_sendmsg; // if true, split send() with transferToMain()

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

    DisplayStringNotifyFunc notify_inspector;
    void *data_for_inspector;

  protected:

    /**
     * FIXME: The following members are only made public for use by the inspector
     * classes. Do not use them directly from simple modules.
     */
    virtual void arrived(cMessage *msg,int n) = 0; // internal use

  protected:
    // The following functions are expected to be redefined in concrete
    // module types. They are made protected because they are supposed
    // to be called only via callInitialize() and callFinish().

    // multi-stage init functions are called before simulation starts

    /**
     * TBD
     */
    virtual void initialize(int stage) {initialize();}  // multi-stage init: defaults to single-stage init

    /**
     * TBD
     */
    virtual int  numInitStages()       {return 1;}      // multi-stage init: defaults to single-stage init

    /**
     * TBD
     */
    virtual void initialize();                          // single-stage init

    // finish() is called after end of simulation (if it terminated without error)

    /**
     * TBD
     */
    virtual void finish();

  public:

    /**
     * Constructor.
     */
    cModule(cModule& mod);

    /**
     * Constructor.
     */
    cModule(const char *name, cModule *parentmod);

    /**
     * Destructor.
     */
    virtual ~cModule() {}

    // redefined functions

    /**
     * Returns pointer to the class name string, "cModule".
     */
    virtual const char *className() const {return "cModule";}

    /**
     * MISSINGDOC: cModule:cObject*dup()
     */
    virtual cObject *dup();

    /**
     * MISSINGDOC: cModule:void forEach(ForeachFunc)
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Redefined.
     */
    virtual const char *inspectorFactoryName() const {return "cModuleIFC";}

    /**
     * MISSINGDOC: cModule:cModule&operator=(cModule&)
     */
    cModule& operator=(cModule& mod);

    /**
     * Returns full name of the module in a static buffer, in the form
     * "name" or "name[index]".
     */
    virtual const char *fullName() const;       // "name[12]" (in a static buffer!)

    /**
     * MISSINGDOC: cModule:char*fullPath()
     */
    virtual const char *fullPath() const;       // "sys.m[3].name[12]" (static buff!)

    // setting up the module

    /**
     * FIXME: setting up the module
     */
    virtual void setId(int n);              // set module id

    /**
     * MISSINGDOC: cModule:void setIndex(int,int)
     */
    void setIndex(int i, int n);            // if multiple module

    /**
     * MISSINGDOC: cModule:void setModuleType(cModuleType*)
     */
    void setModuleType(cModuleType *mtype);


    /**
     * MISSINGDOC: cModule:void addGate(char*,char)
     */
    void addGate(const char *s, char tp);         // add one gate

    /**
     * MISSINGDOC: cModule:void setGateSize(char*,int)
     */
    void setGateSize(const char *s, int size);    // set gate vector size


    /**
     * MISSINGDOC: cModule:void addPar(char*)
     */
    void addPar(const char *s);                   // add parameter


    /**
     * MISSINGDOC: cModule:void addMachinePar(char*)
     */
    void addMachinePar(const char *pnam);            // NET: add machine par

    /**
     * MISSINGDOC: cModule:void setMachinePar(char*,char*)
     */
    void setMachinePar(const char *pnam, const char *val); // NET: set parameter value


    /**
     * MISSINGDOC: cModule:void buildInside()
     */
    virtual void buildInside() {}           // build submodules & internal connections

    // miscellaneous functions

    /**
     * Pure virtual function. It is redefined in cSimpleModule
     * to return true and in cCompoundModule to return
     * false.
     */
    virtual bool isSimple() = 0;

    /**
     * MISSINGDOC: cModule:cModuleType*moduleType()
     */
    cModuleType *moduleType()   {return mod_type;}

    /**
     * Returns the index of the module in the module vector (cSimulation
     * simulation).
     */
    int id()                    {return mod_id;}

    /**
     * Returns reference to the module's parent.
     */
    cModule *parentModule()     {return parentmodp;}

    /**
     * Used with parallel execution: determines if the module is on the
     * local machine. See the user manual for more info.
     */
    bool isOnLocalMachine();

    // if this module is in a module vector:

    /**
     * FIXME: if this module is in a module vector:
     */
    bool isVector() const       {return vectsize>=0;}

    /**
     * Returns the index of the module if it is multiple, otherwise 0.
     */
    int index() const           {return idx;}

    /**
     * Returns the size of the multiple module, or 1.
     */
    int size() const            {return vectsize<0?1:vectsize;}

    // submodule access:

    /**
     * Finds an immediate submodule with the given name and (optional)
     * index, and returns its module ID. If the submodule was not found,
     * returns -1.
     */
    int findSubmodule(const char *submodname, int idx=-1);  // returns module id (-1 on error)

    /**
     * Finds an immediate submodule with the given name and (optional)
     * index, and returns its pointer. If the submodule was not found,
     * returns NULL.
     */
    cModule *submodule(const char *submodname, int idx=-1); // returns module ptr

    /**
     * Finds a submodule potentially several levels deeper, given with
     * its relative path from this module. (The path is a string of module
     * full names separated by dots). If the submodule was not found,
     * returns NULL.
     */
    cModule *moduleByRelativePath(const char *path);        // find sub-sub-...-modules

    // module gates

    /**
     * Returns total number of module gates.
     */
    int gates() const {return gatev.items();}             // total num of gates

    /**
     * MISSINGDOC: cModule:cGate*gate(int)
     */
    cGate *gate(int g) {return (cGate*)gatev[g];}   // gate by id

    /**
     * MISSINGDOC: cModule:cGate*gate(int)
     */
    const cGate *gate(int g) const {return (const cGate*)gatev[g];}   // gate by id

    /**
     * MISSINGDOC: cModule:cGate*gate(char*,int)
     */
    cGate *gate(const char *gatename,int sn=-1);    // gate by name & index

    /**
     * MISSINGDOC: cModule:cGate*gate(char*,int)
     */
    const cGate *gate(const char *gatename,int sn=-1) const;    // gate by name & index

    /**
     * Returns index of the gate specified by name and index (if multiple
     * gate). Returns -1 if the gate doesn't exist.
     */
    int findGate(const char *gatename, int sn=-1) const;  // id of a gate (-1 on error)

    /**
     * MISSINGDOC: cModule:bool hasGate(char*,int,)>}
     */
    bool hasGate(const char *gatename, int sn=-1) const {return findGate(gatename,sn)>=0;} // check if gate exists


    /**
     * For compound modules, it checks if all gates are connected inside
     * the module (it returns true if they are OK); for simple
     * modules, it returns true.
     */
    bool checkInternalConnections();      // true means OK; called from NEDC code

    // module parameters

    /**
     * Returns total number of the module's parameters.
     */
    int params() {return paramv.items();}    // no. of pars

    /**
     * Returns reference to the module parameter identified with its
     * index p. Returns *NULL if the object doesn't
     * exist.
     */
    cPar& par(int p);                        // par by index

    /**
     * Returns reference to the module parameter specified with its name.
     * Returns *NULL if the object doesn't exist.
     */
    cPar& par(const char *parname);          // par by name

    /**
     * Returns index of the module parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    int findPar(const char *parname);        // index of a par (-1 of not found)

    /**
     * Searches for the parameter in the parent modules, up to the system
     * module. It the parameter is not found, an error message is generated.
     */
    cPar& ancestorPar(const char *parname);  // search for par in parents;error if not found

    /**
     * MISSINGDOC: cModule:bool hasPar(char*)
     */
    bool hasPar(const char *s) {return findPar(s)>=0;}  // check if parameter exists

    // machine parameters

    /**
     * FIXME: machine parameters
     */
    int machinePars()  {return machinev.items();}    // NET

    /**
     * MISSINGDOC: cModule:char*machinePar(int)
     */
    const char *machinePar(int i);                   // NET

    /**
     * MISSINGDOC: cModule:char*machinePar(char*)
     */
    const char *machinePar(const char *machinename); // NET

    // interface for calling initialize() and finish() from outside

    /**
     * FIXME: interface for calling initialize() and finish() from outside
     */
    virtual void callInitialize();               // full multi-stage init for this module

    /**
     * MISSINGDOC: cModule:bool callInitialize(int)
     */
    virtual bool callInitialize(int stage) = 0;  // does single stage of initialization; returns true if more stages are required

    /**
     * MISSINGDOC: cModule:void callFinish()
     */
    virtual void callFinish() = 0;               // calls finish()

    // dynamic module creation

    /**
     * Pure virtual function; it is redefined in both cCompoundModule
     * and cSimpleModule. It creates starting message for dynamically
     * created module (or recursively for its submodules). See the user
     * manual for explanation how to use dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t) = 0; // creates starting msg

    /**
     * Pure virtual function; it is redefined in both cCompoundModule
     * and cSimpleModule. Deletes a dynamically created module
     * and recursively all its submodules.
     */
    virtual void deleteModule() = 0;             // deletes module

    // enable/disable warnings

    /**
     * Warning messages can be enabled/disabled individually for each
     * module. This function returns the warning status for this module:
     * true=enabled, false=disabled.
     */
    bool warnings()            {return warn;}

    /**
     * Enables or disables warnings for this module: true=enable,
     * false=disable.
     */
    void setWarnings(bool wr)  {warn=wr;}

    // visualization/animation support

    /**
     * TBD
     */
    void setDisplayString(int type, const char *dispstr, bool immediate=true);

    /**
     * TBD
     */
    const char *displayString(int type);


    /**
     * MISSINGDOC: cModule:void setDisplayStringNotify(DisplayStringNotifyFunc,void*)
     */
    void setDisplayStringNotify(DisplayStringNotifyFunc notify_func, void *data);

};

//==========================================================================

//
// internal struct: block on module function's heap
//
struct sBlock
{
    sBlock *next;
    sBlock *prev;
    cSimpleModule *mod;
};

//--------------------------------------------------------------------------

/**
 * cSimpleModule is a base class for all simple module classes.
 * Most important, cSimpleModule has the virtual member functions
 * activity() and handleMessage(), one of which has to be redefined in
 * the user's simple modules.
 *
 * All basic functions associated with the simulation such as sending
 * and receiving messages are implemented as cSimpleModule's member
 * functions.
 *
 * The activity() functions run as coroutines during simulation.
 * Coroutines are brought to cSimpleModule from the cCoroutine base class.
 */
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

    /**
     * MISSINGDOC: cSimpleModule:static void activate(void*)
     */
    static void activate(void *p);  // internal use

  protected:

    /**
     * MISSINGDOC: cSimpleModule:void arrived(cMessage*,int)
     */
    virtual void arrived(cMessage *msg,int n);  // internal use

  public:
    cHead locals;           // list of local variables of module function
    cQueue putAsideQueue;   // put-aside queue

  protected:
    // The following functions are expected to be redefined in concrete
    // simple module types. They are made protected because they shouldn't
    // be called directly from outside.

    /**
     * Contains the module function. It is empty in cSimpleModule
     * and should be redefined in descendants.
     */
    virtual void activity();                   // coroutine function

    /**
     * MISSINGDOC: cSimpleModule:void handleMessage(cMessage*)
     */
    virtual void handleMessage(cMessage *msg); // alternative to activity()

  public:

    /**
     * Copy constructor.
     */
    cSimpleModule(cSimpleModule& mod);

    /**
     * Constructor.
     */
    cSimpleModule(const char *name, cModule *parentmod, unsigned stk);

    /**
     * Destructor.
     */
    virtual ~cSimpleModule();

    // redefined functions

    /**
     * Returns pointer to the class name string, "cSimpleModule".
     */
    virtual const char *className() const {return "cSimpleModule";}

    /**
     * MISSINGDOC: cSimpleModule:cObject*dup()
     */
    virtual cObject *dup()  {return new cSimpleModule(*this);}

    /**
     * Redefined.
     */
    virtual void info(char *buf);

    /**
     * Redefined.
     */
    virtual const char *inspectorFactoryName() const {return "cSimpleModuleIFC";}

    /**
     * MISSINGDOC: cSimpleModule:cSimpleModule&operator=(cSimpleModule&)
     */
    cSimpleModule& operator=(cSimpleModule& mod);

    /**
     * MISSINGDOC: cSimpleModule:void forEach(ForeachFunc)
     */
    virtual void forEach(ForeachFunc f);

    // redefined cModule functions:

    /**
     * FIXME: redefined cModule functions:
     */
    virtual void setId(int n);

    /**
     * Returns true.
     */
    virtual bool isSimple() {return true;}

  public:
    // return event handling scheme

    /**
     * FIXME: return event handling scheme
     */
    bool usesActivity()  {return usesactivity;}

    // interface for calling initialize() and finish() from outside
    //virtual void callInitialize();

    /**
     * FIXME: interface for calling initialize() and finish() from outside
     *     //virtual void callInitialize();
     */
    virtual bool callInitialize(int stage);

    /**
     * MISSINGDOC: cSimpleModule:void callFinish()
     */
    virtual void callFinish();

    // dynamic module creation

    /**
     * Creates a starting message for the module.
     */
    virtual void scheduleStart(simtime_t t); // creates starting msg

    /**
     * Deletes a dynamically created module.
     */
    virtual void deleteModule();             // deletes module

    // module time

    /**
     * Returns the current simulation time (that is, the arrival time
     * of the last message returned by a receiveNew() call).
     */
    simtime_t simTime();   // (cannot make inline because of declaration order)

    // user-settable phase string

    /**
     * Sets the phase string (the function creates a copy of the string).
     * The string can be displayed in user interfaces which support tracing
     * / debugging (currently only Tkenv) and the string can contain
     * information that tells the user what the module is currently doing.
     */
    void setPhase(const char *phase)  {phasestr=phase;}

    /**
     * Returns pointer to the current phase string.
     */
    const char *phase()  {return correct(phasestr);}

    // debugging aids

    /**
     * To be called from module functions. Outputs textual information
     * about all objects of the simulation (including the objects created
     * in module functions by the user!) into the snapshot file. The
     * output is detailed enough to be used for debugging the simulation:
     * by regularly calling snapshot(), one can trace how the
     * values of variables, objects changed over the simulation. The
     * arguments: label is a string that will appear in the
     * output file; obj is the object whose inside is of interest.
     * By default, the whole simulation (all modules etc) will be written
     * out.
     *
     * Tkenv also supports making snapshots manually, from menu.
     *
     * See also class cWatch and the WATCH() macro.
     */
    bool snapshot(cObject *obj=&simulation, const char *label=NULL); // write snapshot file

    /**
     * Specifies a breakpoint. During simulation, if execution gets to
     * a breakpoint() call (and breakpoints are active etc.),
     * the simulation will be stopped, if the user interface can handle
     * breakpoints.
     */
    void breakpoint(const char *label);     // user breakpoint

    /**
     * If the user interface supports step-by-step execution, one can
     * stop execution at each receive() call of the module function
     * and examine the objects, variables, etc. If the state of simulation
     * between receive() calls is also of interest, one can
     * use pause() calls. The string argument (if given) sets
     * the phase string, so pause("here") is the same
     * as setPhase("here"); pause().
     */
    void pause(const char *phase=NULL);     // set phase & temporarily yield control to main

    // sending a message through an output gate

    /**
     * Sends a message through the gate given with its ID.
     */
    int send(cMessage *msg, int gateid);   // send a message thru gate id

    /**
     * Sends a message through the gate given with its name and index
     * (if multiple gate).
     */
    int send(cMessage *msg, const char *gatename, int sn=-1); // s:gate name, sn:index

    /**
     * Sends a message through the gate given with its pointer.
     */
    int send(cMessage *msg, cGate *outputgate);

    // delayed sending through an output gate

    /**
     * Sends a message through the gate given with its index as if it
     * was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, int gateid);

    /**
     * Sends a message through the gate given with its name and index
     * (if multiple gate) as if it was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, const char *gatename, int sn=-1);

    /**
     * Sends a message through the gate given with its pointer as if
     * it was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, cGate *outputgate);

    // sending messages directly to another module (to an input gate)

    /**
     * FIXME: sending messages directly to another module (to an input gate)
     */
    int sendDirect(cMessage *msg, double delay, cModule *mod, int inputgateid);

    /**
     * MISSINGDOC: cSimpleModule:int sendDirect(cMessage*,double,cModule*,char*,int)
     */
    int sendDirect(cMessage *msg, double delay, cModule *mod, const char *inputgatename, int sn=-1);

    /**
     * TBD (also to include other sendDirect() funcs!)
     */
    int sendDirect(cMessage *msg, double delay, cGate *inputgate);

    // self-messages

    /**
     * FIXME: self-messages
     */
    int scheduleAt(simtime_t t, cMessage *msg); // receive() will return msg at t

    /**
     * Removes the given message from the message queue. The message
     * needs to have been sent using the scheduleAt() function.
     * This function can be used to cancel a timer implemented with scheduleAt().
     */
    cMessage *cancelEvent(cMessage *msg);       // remove cMessage sent by scheduleAt() from FES

    // parallel execution
    //   sync with receiver segment (blocks receiver at t until msg arrives)

    /**
     * FIXME: parallel execution
     *   sync with receiver segment (blocks receiver at t until msg arrives)
     */
    int syncpoint(simtime_t t, int gateid);

    /**
     * MISSINGDOC: cSimpleModule:int syncpoint(simtime_t,char*,int)
     */
    int syncpoint(simtime_t t, const char *gatename, int sn=-1);

    /**
     * MISSINGDOC: cSimpleModule:int cancelSyncpoint(simtime_t,int)
     */
    int cancelSyncpoint(simtime_t t, int gateid);

    /**
     * MISSINGDOC: cSimpleModule:int cancelSyncpoint(simtime_t,char*,int)
     */
    int cancelSyncpoint(simtime_t t, const char *gatename, int sn=-1);

    // see if there's a message scheduled for this time

    /**
     * Tells if the next message in the event queue is for the same module
     * and has the same arrival time. (Returns true only if
     * two or more messages arrived to the module at the same time.)
     */
    bool isThereMessage();

    // get message from the put-aside queue OR the FES

    /**
     * FIXME: get message from the put-aside queue OR the FES
     */
    cMessage *receive();

    /**
     * Returns the first message from the put-aside queue or, if it is
     * empty, calls receiveNew() to return a message from the
     * event queue with the given timeout. Note that the arrival time
     * of the message returned by receive() can be earlier than
     * the current simulation time.
     */
    cMessage *receive(simtime_t timeout);

    /**
     * Scans the put-aside queue for the first message that has arrived
     * on the gate specified with its name and index. If there is no
     * such message in the put-aside queue, calls receiveNew()
     * to return a message from the event queue with the given timeout.
     * Note that the arrival time of the message returned by receive()
     * can be earlier than the current simulation time.
     */
    cMessage *receiveOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     */
    cMessage *receiveOn(int gateid, simtime_t timeout=MAXTIME);

    // get message from the FES

    /**
     * FIXME: get message from the FES
     */
    cMessage *receiveNew();

    /**
     * Removes the next message from the event queue and returns a pointer
     * to it. If there is no message in the event queue, the function
     * waits with t timeout until a message will be available. If the
     * timeout expires and there is still no message in the queue, the
     * function returns NULL.
     */
    cMessage *receiveNew(simtime_t timeout);

    /**
     * The same as receiveNew(), except that it returns the
     * next message that arrives on the gate specified with its name
     * and index. All messages received meanwhile are inserted into the
     * put-aside queue. If the timeout expires and there is still no
     * such message in the queue, the function returns NULL.
     *
     * In order to process messages that may have been put in the put-aside
     * queue, the user is expected to call receive() or receiveOn(),
     * or to examine the put-aside queue directly sometime.
     */
    cMessage *receiveNewOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     */
    cMessage *receiveNewOn(int gateid, simtime_t timeout=MAXTIME);

    // simulation control

    /**
     * Wait for given interval. The messages received meanwhile are inserted
     * into the put-aside queue.
     */
    void wait(simtime_t time);        // waits 'time' units

    /**
     * Ends the run of the simple module. The simulation is not stopped
     * (unless this is the last running module.)
     */
    void end();                       // ends this module

    /**
     * Causes the whole simulation to stop.
     */
    void endSimulation();             // finish the whole simulation

    /**
     * MISSINGDOC: cSimpleModule:void error(char*,...)
     */
    void error(const char *fmt,...) const;  // user error message

    //// access putaside-queue -- soon!
    //virtual cQueue& putAsideQueue();

    // record into scalar result file

    /**
     * FIXME:     //access putaside-queue -- soon!
     *     //virtual cQueue& putAsideQueue();
     * record into scalar result file
     */
    void recordScalar(const char *name, double value);

    /**
     * MISSINGDOC: cSimpleModule:void recordScalar(char*,char*)
     */
    void recordScalar(const char *name, const char *text);

    /**
     * MISSINGDOC: cSimpleModule:void recordStats(char*,cStatistic*)
     */
    void recordStats(const char *name, cStatistic *stats);

    // value-added 'raw' heap allocation for module functions:

    /**
     * Dynamic memory allocation. This function should be used instead
     * of the global ::malloc() from inside the module function
     * (activity()), if deallocation by the simple module constructor
     * is not provided.
     *
     * Dynamic allocations are discouraged in general unless you put
     * the pointer into the class declaration of the simple module class
     * and provide a proper destructor. Or, you can use container classes
     * (cArray, cQueue)!
     */
    void *memAlloc(size_t m);          // allocate m bytes

    /**
     * Frees a memory block reserved with the malloc() described
     * above and NULLs the pointer.
     */
    void memFree(void *&p);            // free block & NULL pointer

    // for internal use ONLY:

    /**
     * FIXME: for internal use ONLY:
     */
    void clearHeap();                  // free module's local allocations

    /**
     * MISSINGDOC: cSimpleModule:int moduleState()
     */
    int moduleState() {return state;}  // return module state
};

//==========================================================================

/**
 * Represents a compound module in the simulation.
 *
 * NOTE: dup() cannot be used. Use moduleType()->create() instead.
 */
class SIM_API cCompoundModule : public cModule
{
    friend class TCompoundModInspector;

  protected:
    /**
     * MISSINGDOC: cCompoundModule:void arrived(cMessage*,int)
     */
    virtual void arrived(cMessage *msg,int n); //internal use

  public:
    /**
     * Copy constructor.
     */
    cCompoundModule(cCompoundModule& mod);

    /**
     * Constructor.
     */
    cCompoundModule(const char *name, cModule *parentmod);

    /**
     * Destructor.
     */
    virtual ~cCompoundModule();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cCompoundModule";}

    /**
     * MISSINGDOC: cCompoundModule:cObject*dup()
     */
    virtual cObject *dup()   {return new cCompoundModule(*this);}

    /**
     * MISSINGDOC: cCompoundModule:void info(char*)
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cCompoundModule:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cCompoundModuleIFC";}

    /**
     * MISSINGDOC: cCompoundModule:cCompoundModule&operator=(cCompoundModule&)
     */
    cCompoundModule& operator=(cCompoundModule& mod);

    // redefined cModule functions

    /**
     * Returns false.
     */
    virtual bool isSimple() {return false;}

    // interface for calling initialize() and finish() from outside
    //virtual void callInitialize();           // full multi-stage init

    /**
     * FIXME: interface for calling initialize() and finish() from outside
     *     //virtual void callInitialize();full multi-stage init
     */
    virtual bool callInitialize(int stage);  // first for this module, then for submods

    /**
     * MISSINGDOC: cCompoundModule:void callFinish()
     */
    virtual void callFinish();               // first for submods, then for itself

    // dynamic module creation

    /**
     * Calls scheduleStart() recursively for all its (immediate)
     * submodules. This is used with dynamically created modules.
     */
    virtual void scheduleStart(simtime_t t); // starting msg for submodules

    /**
     * Calls deleteModule() for all its submodules and then
     * deletes itself.
     */
    virtual void deleteModule();             // deletes module & submodules
};

//==========================================================================

/**
 * Iterates through submodules of a compound module.
 */
class SIM_API cSubModIterator
{
  private:
    cModule *parent;
    int i;
  public:

    /**
     * Constructor.
     */
    cSubModIterator(cModule& p)  {parent=&p;i=0;operator++(0);}

    /**
     * MISSINGDOC: cSubModIterator:void init(cModule&)
     */
    void init(cModule& p)        {parent=&p;i=0;operator++(0);}

    /**
     * MISSINGDOC: cSubModIterator:cModule&operator[]()
     */
    cModule& operator[](int)     {return simulation[i];} //OBSOLETE

    /**
     * MISSINGDOC: cSubModIterator:cModule*operator()()
     */
    cModule *operator()()        {return &simulation[i];}

    /**
     * MISSINGDOC: cSubModIterator:bool end()
     */
    bool end()                   {return (i==-1);}

    /**
     * MISSINGDOC: cSubModIterator:cModule*operator++()
     */
    cModule *operator++(int);    // sets i to -1 if end was reached
};

#endif

