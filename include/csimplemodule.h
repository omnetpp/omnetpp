//==========================================================================
//   CSIMPLEMODULE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cSimpleModule  : base for simple module objects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSIMPLEMODULE_H
#define __CSIMPLEMODULE_H

#include "cmodule.h"

// classes mentioned here
class cStatistic;


//=== dynamic allocation in module functions
#define opp_new             new((___nosuchclass *)NULL)
#define opp_delete          memFree((void *&)x)


/**
 * cSimpleModule is the base class for all simple module classes.
 * cSimpleModule, although stuffed with simulation-related functionality,
 * doesn't do anything useful by itself: one has to redefine
 * one or more virtual member functions to make it do useful work:
 *
 *    - void initialize()
 *    - void activity()
 *    - void handleMessage(cMessage *msg)
 *    - void finish()
 *
 * initialize() is called after OMNeT++ created the module.
 *
 * One has to redefine either activity() or handleMessage() to contain
 * the internal logic of the module. activity() and handleMessage() implement
 * different event processing strategies: activity() is a coroutine-based
 * solution which implements the process-interaction approach (coroutines are
 * non-preemptive [cooperative] threads). handleMessage() is a method that is
 * called by the simulation kernel when the module receives a message.
 *
 * The finish() functions are called when the simulation terminates
 * successfully. Typical use of finish() is recording statistics
 * collected during simulation.
 *
 * The activity() functions run as coroutines during simulation.
 * Coroutines are brought to cSimpleModule from the cCoroutine base class.
 *
 * @ingroup SimCore
 */
class SIM_API cSimpleModule : public cModule
{
    friend class cModule;
    friend class cSimulation;
    friend class TSimpleModInspector;

    // internal struct: block on module function's heap
    struct sBlock
    {
        sBlock *next;
        sBlock *prev;
        cSimpleModule *mod;
    };

  private:
    bool usesactivity;      // uses activity() or handleMessage()
    int state;              // ended/ready/waiting for msg
    opp_string phasestr;    // a 'phase' string
    sBlock *heap;           // head of modfunc's heap list
    cMessage *timeoutmsg;   // msg used in wait() and receive() with timeout
    cCoroutine *coroutine;

  private:
    // internal use
    static void activate(void *p);
    void discardLocals();

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n,simtime_t t);

    // internal: sets the module id.
    virtual void setId(int n);

  public:
    cHead locals;           // list of local variables of module function

    /**
     * DEPRECATED. putAsideQueue will be removed at some point in the
     * future, and this will affect the message receiving functions.
     * Details below.
     *
     * putAsideQueue is used implicitly by the methods wait(), receiveOn()
     * and receiveNewOn() to insert messages that arrive during the wait
     * period or not on the specified gate. The receive() functions
     * looked first on the putAsideQueue and only then at the future events.
     *
     * As practice has shown, the purpose of the putAsideQueue was very
     * often misunderstood, and its implicit use by wait() and the message
     * receiving functions was highly error-prone. It will be removed
     * at some point in the future. Be prepared: use wait() and receiveOn()
     * if you do not expect other messages to arrive at the module;
     * use waitAndEnqueue() where you mean it; do not use the receiveNew..()
     * functions as they will be made redundant when putAsideQueue goes away.
     */
    cQueue putAsideQueue;   // put-aside queue

  protected:
    /** @name Hooks for defining module behavior.
     *
     * Exactly one of activity() and handleMessage() must be redefined
     * by the user to add functionality to the simple module. See the
     * manual for detailed guidance on how use to these two methods.
     *
     * These methods are made protected because they shouldn't be called
     * directly from outside.
     */
    //@{

    /**
     * Should be redefined to contain the module activity function.
     * This default implementation issues an error message (throws cException).
     */
    virtual void activity();

    /**
     * Should be redefined to contain the module's message handling function.
     * This default implementation issues an error message (throws cException).
     */
    virtual void handleMessage(cMessage *msg);
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cSimpleModule(const cSimpleModule& mod);

    /**
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     */
    cSimpleModule(const char *name, cModule *parentmod, unsigned stk);

    /**
     * Destructor.
     */
    virtual ~cSimpleModule();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cSimpleModule& operator=(const cSimpleModule& mod);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cSimpleModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Calls initialize(int stage) in the context of this module.
     * It does a single stage of initialization, and returns <tt>true</tt>
     * if more stages are required.
     */
    virtual bool callInitialize(int stage);

    /**
     * Calls finish() in the context of this module.
     */
    virtual void callFinish();

    /**
     * Creates a starting message for the module.
     */
    virtual void scheduleStart(simtime_t t);

    /**
     * Deletes a dynamically created module. A running module can also
     * delete itself.
     */
    virtual void deleteModule();
    //@}

    /** @name Information about the module. */
    //@{

    /**
     * Returns event handling scheme.
     */
    bool usesActivity() const  {return usesactivity;}
    //@}

    /** @name Simulation time. */
    //@{

    /**
     * Returns the current simulation time (that is, the arrival time
     * of the last message returned by a receiveNew() call).
     */
    simtime_t simTime() const;   // cannot make inline because of declaration order!
    //@}

    /** @name Debugging aids. */
    //@{

    /**
     * DEPRECATED.
     * Sets the phase string which can be displayed as some kind of status.
     * The module creates its own copy of the string.
     */
    void setPhase(const char *phase)  {phasestr=phase;}

    /**
     * DEPRECATED.
     * Returns pointer to the current phase string.
     */
    const char *phase() const  {return correct(phasestr);}

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
     * Sets phase string and temporarily yield control to the user interface.
     * If the user interface supports step-by-step execution, one can
     * stop execution at each receive() call of the module function
     * and examine the objects, variables, etc. If the state of simulation
     * between receive() calls is also of interest, one can
     * use pause() calls. The string argument (if given) sets
     * the phase string, so pause("here") is the same
     * as setPhase("here"); pause().
     */
    void pause(const char *phase=NULL);
    //@}

    /** @name Message sending. */
    //@{

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

    /**
     * Delayed sending. Sends a message through the gate given with
     * its index as if it was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, int gateid);

    /**
     * Delayed sending. Sends a message through the gate given with
     * its name and index (if multiple gate) as if it was sent delay
     * seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, const char *gatename, int sn=-1);

    /**
     * Sends a message through the gate given with its pointer as if
     * it was sent delay seconds later.
     */
    int sendDelayed(cMessage *msg, double delay, cGate *outputgate);

    /**
     * Sends a message directly to another module. 
     * See sendDirect(cMessage *, double, cModule *, const char *, in) for a  
     * more detailed description.
     */
    int sendDirect(cMessage *msg, double delay, cModule *mod, int inputgateid);

    /**
     * Send a message directly to another module. 
     *
     * If the gate is further connected (i.e. toGate()!=NULL), the 
     * message will follow the connections that start at that gate.
     * For example, when sending to an input gate of a compound module,
     * the message will follow the connections to the inside of the compound module.
     *
     * It is permitted to send to an output gate, which will also cause the
     * message to follow the connections starting at that gate.
     * This can be useful, for example, when several submodules are sending 
     * to a single output gate of their parent module.
     * 
     * It is not permitted to send to a gate of a compound module which is not
     * further connected (i.e. toGate()==NULL), as this would cause the message
     * to arrive at a compound module.
     * 
     * Also, it is not permitted to send to a gate which is otherwise connected
     * i.e. where fromGate()!=NULL. This means that modules MUST have 
     * dedicated gates for receiving via sendDirect(). You cannot have a gate 
     * which receives messages via both connections and sendDirect().
     */
    int sendDirect(cMessage *msg, double delay, cModule *mod, const char *inputgatename, int sn=-1);

    /**
     * Sends a message directly to another module.
     * See sendDirect(cMessage *, double, cModule *, const char *, in) for a  
     * more detailed description.
     */
    int sendDirect(cMessage *msg, double delay, cGate *inputgate);
    //@}

    /** @name Self-messages. */
    //@{

    /**
     * Schedules a self-message. Receive() will return the message at
     * t simulation time.
     */
    int scheduleAt(simtime_t t, cMessage *msg);

    /**
     * Removes the given message from the future events. The message
     * needs to have been sent using the scheduleAt() function.
     * This function can be used to cancel a timer implemented with scheduleAt().
     */
    cMessage *cancelEvent(cMessage *msg);
    //@}

    /** @name Receiving messages.
     *
     * These functions can only be used with activity(), but not with
     * handleMessage().
     */
    //@{

    /**
     * DEPRECATED.
     *
     * Tells if the next message in the event queue is for the same module
     * and has the same arrival time. (Returns true only if
     * two or more messages arrived to the module at the same time.)
     */
    bool isThereMessage() const;

    /**
     * Receives a message from the put-aside queue or the FES.
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
     *
     * <b>IMPORTANT</b>: The put-aside queue has been deprecated,
     * and the semantics of this function will be changed in the future.
     * It will be intended for use only if you do not expect other messages
     * to arrive at the module on other gates than the specified one.
     * To assert this, it will throw an exception if an inappropriate message
     * arrives, as it will probably signal a logic error in the model.
     * On the other hand, if you <i>do</i> expect to receive other messages
     * during the call, you should not use receiveOn() but implement
     * similar functionality using a loop (for() or while()) and receive(),
     * which will make your intent more conspicuous to the reader of your
     * source code.
     * See the API-doc on putAsideQueue for more explanation.
     */
    cMessage *receiveOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     *
     * <b>IMPORTANT</b>: The put-aside queue has been deprecated,
     * and the semantics of this function will be changed in the future.
     * It will be intended for use only if you do not expect other messages
     * to arrive at the module on other gates than the specified one.
     * To assert this, it will throw an exception if an inappropriate message
     * arrives, as it will probably signal a logic error in the model.
     * On the other hand, if you <i>do</i> expect to receive other messages
     * during the call, you should not use receiveOn() but implement
     * similar functionality using a loop (for() or while()) and receive(),
     * which will make your intent more conspicuous to the reader of your
     * source code.
     * See the API-doc on putAsideQueue for more explanation.
     */
    cMessage *receiveOn(int gateid, simtime_t timeout=MAXTIME);

    /**
     * Remove the next message from the event queue and return a pointer
     * to it. Ignores put-aside queue.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receive() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNew();

    /**
     * Removes the next message from the event queue and returns a pointer
     * to it. Ignores put-aside queue. If there is no message in the event
     * queue, the function waits with t timeout until a message will be
     * available. If the timeout expires and there is still no message
     * in the queue, the function returns NULL.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receive() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
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
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receiveOn() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNewOn(const char *gatename, int sn=-1, simtime_t timeout=MAXTIME);

    /**
     * Same as the previous function except that the gate must be specified
     * with its index in the gate array. Using this function instead
     * the previous one may speed up the simulation if the function is
     * called frequently.
     *
     * <b>IMPORTANT</b>: This function should not be used. As putAsideQueue is
     * deprecated, it will be removed at some point in the future.
     * This means that then the receiveOn() method will act exactly as this one,
     * and this method will no longer be needed.
     * See the API-doc on putAsideQueue, wait() and waitAndEnqueue() for more explanation.
     */
    cMessage *receiveNewOn(int gateid, simtime_t timeout=MAXTIME);
    //@}

    /** @name Waiting. */
    //@{

    /**
     * Waits for the given interval. (Some other simulators call this
     * functionality hold()). The messages received meanwhile are inserted
     * into the put-aside queue, but if you expect to receive messages
     * during the call, you should use the waitAndEnqueue() method
     * instead (see the following note).
     *
     * This function can only be used with activity(), but not with
     * handleMessage().
     *
     * <b>IMPORTANT</b>: The put-aside queue has been deprecated,
     * and the semantics of this function will be changed in the future.
     * It will be intended for use only if you do not expect other messages
     * to arrive at the module during the wait period. To assert this,
     * it will throw an exception if a message arrives during the wait,
     * as it will probably signal a logic error in the model.
     * On the other hand, if you <i>do</i> expect to receive messages
     * during the call, you should use waitAndEnqueue(), which makes
     * this assumption much more conspicuous to the reader of your source
     * code than the old wait() method with its implicit putAsideQueue
     * would. See the API-doc on putAsideQueue for more explanation.
     */
    void wait(simtime_t time);

    /**
     * Waits for the given interval. The messages received during the wait
     * period are inserted into the queue passed as argument.
     *
     * This function can only be used with activity(), but not with
     * handleMessage().
     */
    void waitAndEnqueue(simtime_t time, cQueue *queue);
    //@}

    /** @name Stopping the module or the simulation. */
    //@{

    /**
     * Ends the run of the simple module. The simulation is not stopped
     * (unless this is the last running module.) The implementation simply
     * throws a cEndModuleException.
     *
     * Note that end() does NOT delete the module; its state is simply
     * set to Ended and it won't take part in further simulation.
     * If you also want to delete the module, use deleteModule(); however,
     * this implies that the module's finish() method won't be called
     * at the end of the simulation.
     */
    void end();

    /**
     * Causes the whole simulation to stop. The implementation simply
     * throws a cTerminationException.
     */
    void endSimulation();

    /**
     * DEPRECATED. Equivalent to <code>throw new cException(
     * ...<i>same argument list</i>...)</code>.
     */
    void error(const char *fmt,...) const;
    //@}

    /** @name Statistics collection */
    //@{

    /**
     * Records a double into the scalar result file.
     */
    void recordScalar(const char *name, double value);

    /**
     * Records a string into the scalar result file.
     */
    void recordScalar(const char *name, const char *text);

    /**
     * Records a statistics object into the scalar result file.
     */
    void recordStats(const char *name, cStatistic *stats);
    //@}

    /** Coroutine stack info. Useful only if module uses activity(). */
    //@{

    /**
     * Returns true if there was a stack overflow during execution of the
     * coroutine. (Not implemented for every coroutine package - see cCoroutine
     * documentation for more info.) If the module uses handleMessage(),
     * this method always returns false.
     *
     * @see cCoroutine
     */
    virtual bool stackOverflow() const;

    /**
     * Returns the stack size of the coroutine. If the module uses handleMessage(),
     * this method always returns 0.
     */
    virtual unsigned stackSize() const;

    /**
     * Returns the amount of stack actually used by the coroutine.
     * (Not implemented for every coroutine package - see cCoroutine
     * documentation for more info.) If the module uses handleMessage(),
     * this method always returns 0.
     *
     * @see cCoroutine
     */
    virtual unsigned stackUsage() const;
    //@}

    /** @name Miscellaneous. */
    //@{

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
    void *memAlloc(size_t m);

    /**
     * Frees a memory block reserved with the malloc() described
     * above and NULLs the pointer.
     */
    void memFree(void *&p);

    // INTERNAL: free module's local allocations
    void clearHeap();

    /**
     * Returns module state.
     */
    int moduleState() const {return state;}
    //@}
};

#endif

