//==========================================================================
//   CSIMPLEMODULE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cSimpleModule  : base for simple module objects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSIMPLEMODULE_H
#define __CSIMPLEMODULE_H

#include "cmodule.h"


/**
 * Base class for all simple module classes.
 * cSimpleModule, although stuffed with simulation-related functionality,
 * doesn't do anything useful by itself: one has to subclass from it
 * and redefine one or more virtual member functions to make it do useful work:
 *
 *    - void initialize()
 *    - void handleMessage(cMessage *msg)
 *    - void activity()
 *    - void finish()
 *
 * initialize() is called after \opp created the module.
 *
 * One has to redefine handleMessage() to contain
 * the internal logic of the module. handleMessage() is called
 * by the simulation kernel when the module receives a message.
 * (An alternative to handleMessage() is activity(), but activity()
 * is not recommended for serious model development because of
 * scalability and debuggability problems. It also tends to lead
 * to messy simple module implementations.)
 *
 * You can send() messages to other modules, or use scheduleAt() +
 * cancelEvent() to implement delays, timers or timeouts.
 * Messages sent or scheduled (but not cancelled) are delivered
 * to modules via handleMessage(), or (if using activity()) via
 * receive().
 *
 * The finish() functions are called when the simulation terminates
 * successfully. Typical use of finish() is recording statistics
 * collected during simulation.
 *
 * @ingroup SimCore
 */
class SIM_API cSimpleModule : public cModule
{
    friend class cModule;
    friend class cSimulation;
    friend class TSimpleModInspector;

  private:
    bool usesactivity;      // uses activity() or handleMessage()
    int state;              // ended/ready/waiting for msg
    opp_string phasestr;    // a 'phase' string
    cMessage *timeoutmsg;   // msg used in wait() and receive() with timeout
    cCoroutine *coroutine;

    static bool stack_cleanup_requested; // 'true' value asks activity() to throw a cStackCleanupException
    static cSimpleModule *after_cleanup_transfer_to; // transfer back to this module (or to main)

  private:
    // internal use
    static void activate(void *p);

  protected:
    // internal use
    virtual void arrived(cMessage *msg,int n,simtime_t t);

    // internal: sets the module id.
    virtual void setId(int n);

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
     * For several good reasons, you should prefer handleMessage() to activity().
     * This default implementation issues an error message (throws cRuntimeError).
     */
    virtual void activity();

    /**
     * Should be redefined to contain the module's message handling function.
     * This default implementation issues an error message (throws cRuntimeError).
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
     * The stacksize parameter should only be used with simple modules using
     * activity().
     */
    cSimpleModule(unsigned stacksize = 0);

    /**
     * COMPATIBILITY. This constructor is only provided to make it possible
     * to write simple modules that don't use the Module_Class_Members
     * macro and can also be compiled with OMNeT++ versions earlier than
     * 3.2. The first two args are unused in this and later versions.
     */
    cSimpleModule(const char *name, cModule *parent, unsigned stacksize);

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
    virtual cPolymorphic *dup() const  {return new cSimpleModule(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Creates a starting message for the module.
     */
    virtual void scheduleStart(simtime_t t);

    /**
     * Deletes the module and all its (dynamically created) submodules.
     * A running module can also delete itself.
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
    void setPhase(const char *phase) _OPPDEPRECATED  {phasestr=phase;}

    /**
     * DEPRECATED.
     * Returns pointer to the current phase string.
     */
    const char *phase() const _OPPDEPRECATED  {return phasestr.c_str();}

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
     * Schedules a self-message. It will be delivered back to the module
     * via receive() or handleMessage() at simulation time t. This method
     * is the way you can implement timers or timeouts. Timers can also
     * be cancelled via cancelEvent() (See below.)
     *
     * When the message is delivered at the module, you can call
     * <tt>msg->isSelfMessage()</tt> to tell it apart from messages arriving
     * from other modules. <tt>msg->kind()</tt> can be used to further
     * classify it, or of you need to manage an unbounded number of timers,
     * you can set <tt>msg->contextPointer()</tt> before scheduling to
     * point to the data structure the message belongs to -- this way
     * you can avoid having to search through lists or other data structures
     * to find out where a just-arrived self-message belongs.
     *
     * cancelEvent() can be used to cancel the self-message before it arrives.
     * This is useful for implementing timeouts: if the event occurs "in time"
     * (before timeout), the scheduled self-message can be cancelled.
     *
     * Given a cMessage pointer, you can check whether it is currently
     * scheduled by calling <tt>msg->isScheduled()</tt>. If it is scheduled,
     * you cannot schedule it again without calling cancelEvent() first.
     * However, after the message was delivered to the module or cancelled,
     * you can schedule it again -- so you can reuse the same message
     * object for timeouts over and over during the whole simulation.
     */
    int scheduleAt(simtime_t t, cMessage *msg);

    /**
     * Removes the given message from the future events. The message
     * needs to have been sent using the scheduleAt() function.
     * This function can be used to cancel a timer implemented with scheduleAt().
     * If the message is not currently scheduled, nothing happens.
     */
    cMessage *cancelEvent(cMessage *msg);

    /**
     * Invokes cancelEvent() on the message (in case it is scheduled), then
     * deletes it. A NULL pointer is also accepted, then the method does nothing.
     * This method is especially useful in simple module destructors, to dispose
     * of self-messages that the module has allocated.
     */
    void cancelAndDelete(cMessage *msg);
    //@}

    /** @name Receiving messages.
     *
     * These functions can only be used with activity(), but not with
     * handleMessage().
     */
    //@{

    /**
     * Remove the next message from the event queue and return a pointer
     * to it.
     */
    cMessage *receive();

    /**
     * Removes the next message from the event queue and returns a pointer
     * to it. If there is no message in the event
     * queue, the function waits with t timeout until a message will be
     * available. If the timeout expires and there is still no message
     * in the queue, the function returns NULL.
     */
    cMessage *receive(simtime_t timeout);
    //@}

    /** @name Waiting. */
    //@{

    /**
     * Waits for the given interval. (Some other simulators call this
     * functionality hold()).
     * It is intended for use only if you do not expect other messages
     * to arrive at the module during the wait period. To assert this,
     * it throws an exception if a message arrives during the wait.
     *
     * If you expect to receive messages during the call, you should use
     * waitAndEnqueue() instead.
     *
     * This function can only be used with activity(), but not with
     * handleMessage().
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
     * DEPRECATED. Equivalent to <tt>throw new cRuntimeError(
     * ...<i>same argument list</i>...)</tt>.
     */
    void error(const char *fmt,...) const;
    //@}

    /** @name Statistics collection */
    //@{

    /**
     * Records a double into the scalar result file. Statistics objects
     * (ones subclassed from cStatistic, e.g. cStdDev) can be recorded
     * by calling their recordScalar() methods).
     *
     * @see cStatistic::recordScalar()
     */
    void recordScalar(const char *name, double value);
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
     * Returns module state.
     */
    int moduleState() const {return state;}
    //@}
};

#endif

