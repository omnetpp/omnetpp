//==========================================================================
//   CSIMPLEMODULE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CSIMPLEMODULE_H
#define __OMNETPP_CSIMPLEMODULE_H

#include "cmodule.h"

namespace omnetpp {

class cQueue;
class cCoroutine;

/**
 * @brief Base class for all simple module classes.
 *
 * cSimpleModule, although packed with simulation-related functionality,
 * does not do anything useful by itself: one has to subclass from it
 * and redefine one or more virtual member functions to make it do useful work.
 * These functions are:
 *
 *    - void initialize()
 *    - void handleMessage(cMessage *msg)
 *    - void activity()
 *    - void finish()
 *
 * initialize() is called after \opp created the module. Multi-stage
 * initialization can be achieved by redefining the initialize(int stage)
 * method instead, and also redefining the numInitStages() const method to
 * return the required number of stages.
 *
 * One has to redefine handleMessage() to contain the internal logic of
 * the module. handleMessage() is called by the simulation kernel when the
 * module receives a message. (An alternative to handleMessage() is
 * activity(), but activity() is not recommended for serious model development
 * because of scalability and debugging issues. activity() also tends to lead
 * to messy module implementations.)
 *
 * You can send() messages to other modules, or use scheduleAt()+cancelEvent()
 * to implement delays, timers or timeouts. Messages sent or scheduled (but
 * not cancelled) are delivered to modules via handleMessage(), or, when using
 * activity(), via receive().
 *
 * The finish() functions are called when the simulation terminates
 * successfully. Typical use of finish() is recording statistics collected
 * during simulation.
 *
 * @ingroup SimCore
 */
class SIM_API cSimpleModule : public cModule //implies noncopyable
{
    friend class cModule;
    friend class cSimulation;

  private:
    enum {
        FL_USESACTIVITY        = 1 << 12, // uses activity() or handleMessage()
        FL_ISTERMINATED        = 1 << 13, // for both activity and handleMessage modules
        FL_STACKALREADYUNWOUND = 1 << 14, // only for activity modules
    };

    cMessage *timeoutMessage;   // msg used in wait() and receive() with timeout
    cCoroutine *coroutine;

    static bool stackCleanupRequested; // 'true' value asks activity() to throw a cStackCleanupException
    static cSimpleModule *afterCleanupTransferTo; // transfer back to this module (or to main)

  private:
    // internal use
    static void activate(void *p);

  protected:
    // internal use
    virtual void arrived(cMessage *msg, cGate *ongate, simtime_t t) override;

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
     * Constructor. Note that module objects should not be created directly,
     * only via their cModuleType objects. See cModule constructor for more info.
     * The stacksize parameter should only be used with simple modules using
     * activity().
     */
    cSimpleModule(unsigned stacksize = 0);

    /**
     * COMPATIBILITY. This constructor is only provided to make it possible
     * to write simple modules that do not use the Module_Class_Members
     * macro and can also be compiled with OMNeT++ versions earlier than
     * 3.2. The first two args are unused in this and later versions.
     */
    cSimpleModule(const char *dummy1, cModule *dummy2, unsigned stacksize);

    /**
     * Destructor.
     */
    virtual ~cSimpleModule();
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;
    //@}

    /** @name Redefined cModule functions. */
    //@{

    /**
     * Creates a starting message for the module.
     */
    virtual void scheduleStart(simtime_t t) override;

    /**
     * Deletes the module and all its (dynamically created) submodules.
     *
     * A running module can also delete itself. When an activity()-based
     * simple module deletes itself from within its activity(), the
     * deleteModule() call will not return (it throws an exception which
     * gets caught by the simulation kernel, and the simulation kernel
     * will delete the module).
     *
     * When a handleMessage()-based module deletes itself, the deleteModule()
     * returns normally -- then, of course, the code should not try to
     * access data members or functions of the deleted module, an should
     * return as soon as possible.
     */
    virtual void deleteModule() override;
    //@}

    /** @name Information about the module. */
    //@{

    /**
     * Returns the event handling scheme: activity() or handleMessage().
     */
    bool usesActivity() const  {return flags&FL_USESACTIVITY;}

    /**
     * Returns true if the module has already terminated, by having called end()
     * or returning from the activity() method.
     */
    bool isTerminated() const {return flags&FL_ISTERMINATED;}
    //@}

    /** @name Debugging aids. */
    //@{
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
    virtual void snapshot(cObject *obj=nullptr, const char *label=nullptr);
    //@}

    /** @name Message sending. */
    //@{

    /**
     * Sends a message through the gate given with its ID.
     */
    virtual void send(cMessage *msg, int gateid)  {return sendDelayed(msg, SIMTIME_ZERO, gateid);}

    /**
     * Sends a message through the gate given with its name and index
     * (if multiple gate).
     */
    virtual void send(cMessage *msg, const char *gatename, int gateindex=-1)  {return sendDelayed(msg, SIMTIME_ZERO, gatename, gateindex);}

    /**
     * Sends a message through the gate given with its pointer.
     */
    virtual void send(cMessage *msg, cGate *outputgate)  {return sendDelayed(msg, SIMTIME_ZERO, outputgate);}

    /**
     * Delayed sending. Sends a message through the gate given with
     * its index as if it was sent delay seconds later.
     */
    virtual void sendDelayed(cMessage *msg, simtime_t delay, int gateid);

    /**
     * Delayed sending. Sends a message through the gate given with
     * its name and index (if multiple gate) as if it was sent delay
     * seconds later.
     */
    virtual void sendDelayed(cMessage *msg, simtime_t delay, const char *gatename, int gateindex=-1);

    /**
     * Sends a message through the gate given with its pointer as if
     * it was sent delay seconds later.
     */
    virtual void sendDelayed(cMessage *msg, simtime_t delay, cGate *outputgate);

    /**
     * Sends a message directly to another module, with zero propagation delay
     * and duration. See sendDirect(cMessage *, simtime_t, simtime_t, cGate *)
     * for a more detailed description.
     */
    virtual void sendDirect(cMessage *msg, cModule *mod, const char *inputGateName, int gateIndex=-1);

    /**
     * Sends a message directly to another module, with zero propagation delay
     * and duration. See sendDirect(cMessage *, simtime_t, simtime_t, cGate *)
     * for a more detailed description.
     */
    virtual void sendDirect(cMessage *msg, cModule *mod, int inputGateId);

    /**
     * Sends a message directly to another module, with zero propagation delay
     * and duration. See sendDirect(cMessage *, simtime_t, simtime_t, cGate *)
     * for a more detailed description.
     */
    virtual void sendDirect(cMessage *msg, cGate *inputGate);

    /**
     * Sends a message directly to another module.
     * See sendDirect(cMessage *, simtime_t, simtime_t, cGate *) for a more
     * detailed description.
     */
    virtual void sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cModule *mod, const char *inputGateName, int gateIndex=-1);

    /**
     * See sendDirect(cMessage *, simtime_t, simtime_t, cGate *) for a more
     * detailed description.
     */
    virtual void sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cModule *mod, int inputGateId);

    /**
     * Send a message directly to another module.
     *
     * If the target gate is further connected (i.e. getNextGate()!=nullptr),
     * the message will follow the connections that start at that gate.
     * For example, when sending to an input gate of a compound module,
     * the message will follow the connections inside the compound module.
     *
     * It is permitted to send to an output gate, which will also cause
     * the message to follow the connections starting at that gate.
     * This can be useful, for example, when several submodules are sending
     * to a single output gate of their parent module.
     *
     * It is not permitted to send to a gate of a compound module which is not
     * further connected (i.e. getNextGate()==nullptr), as this would cause the message
     * to arrive at a compound module.
     *
     * Also, it is not permitted to send to a gate which is otherwise connected
     * i.e. where getPreviousGate()!=nullptr. This means that modules MUST have
     * dedicated gates for receiving via sendDirect(). You cannot have a gate
     * which receives messages via both connections and sendDirect().
     *
     * When a nonzero duration is given, that signifies the duration of the packet
     * transmission, that is, the time difference between the transmission (or
     * reception) of the start of the packet and that of the end of the packet.
     * The destination module can choose whether it wants the simulation kernel
     * to deliver the packet object to it at the start or at the end of the
     * reception. The default is the latter; the module can change it by calling
     * setDeliverOnReceptionStart() on the final input gate (that is, on
     * inputGate->getPathEndGate()). setDeliverOnReceptionStart() needs to be
     * called in advance, for example in the initialize() method of the module.
     * When a module receives a packet, it can call the isReceptionStart() and
     * getDuration() methods on the packet to find out whether it represents
     * the start or the end of the reception, and the duration of the
     * transmission.
     *
     * For messages that are not packets (i.e. not subclassed from cPacket),
     * the duration parameter must be zero.
     */
    virtual void sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cGate *inputGate);
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
     * from other modules. <tt>msg->getKind()</tt> can be used to further
     * classify it, or of you need to manage an unbounded number of timers,
     * you can set <tt>msg->getContextPointer()</tt> before scheduling to
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
    virtual void scheduleAt(simtime_t t, cMessage *msg);

    /**
     * Removes the given message from the future events. The message
     * needs to have been sent using the scheduleAt() function.
     * This function can be used to cancel a timer implemented with scheduleAt().
     * If the message is not currently scheduled, nothing happens.
     */
    virtual cMessage *cancelEvent(cMessage *msg);

    /**
     * Invokes cancelEvent() on the message (in case it is scheduled), then
     * deletes it. nullptr is also accepted, then the method does nothing.
     * This method is especially useful in simple module destructors, to dispose
     * of self-messages that the module has allocated.
     */
    virtual void cancelAndDelete(cMessage *msg);
    //@}

    /** @name Receiving messages.
     *
     * These methods may only be invoked from activity()-based simple modules.
     */
    //@{

    /**
     * Remove the next message from the event queue and return a pointer
     * to it.
     */
    virtual cMessage *receive();

    /**
     * Removes the next message from the event queue and returns a pointer
     * to it. If there is no message in the event
     * queue, the function waits with t timeout until a message will be
     * available. If the timeout expires and there is still no message
     * in the queue, the function returns nullptr.
     */
    virtual cMessage *receive(simtime_t timeout);
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
    virtual void wait(simtime_t time);

    /**
     * Waits for the given interval. The messages received during the wait
     * period are inserted into the queue passed as argument.
     *
     * This function can only be used with activity(), but not with
     * handleMessage().
     */
    virtual void waitAndEnqueue(simtime_t time, cQueue *queue);
    //@}

    /** @name Stopping the module or the simulation. */
    //@{
    /**
     * Causes the whole simulation to stop. The implementation simply
     * throws a cTerminationException.
     */
    virtual void endSimulation();

    /**
     * May only be invoked from activity()-based simple modules.
     * Execution of the simple module stops in this call, and any further
     * messages sent to module will cause a runtime error.
     */
    virtual void halt();

    /**
     * Equivalent to <tt>throw cRuntimeError(<i>same argument list</i>)</tt>.
     */
    virtual void error(const char *format,...) const;
    //@}

    /** @name Coroutine stack info. Useful only if module uses activity(). */
    //@{

    /**
     * Returns true if there was a stack overflow during execution of the
     * coroutine. (Not implemented for every coroutine package - see cCoroutine
     * documentation for more info.) If the module uses handleMessage(),
     * this method always returns false.
     *
     * @see cCoroutine
     */
    virtual bool hasStackOverflow() const;

    /**
     * Returns the stack size of the coroutine. If the module uses handleMessage(),
     * this method always returns 0.
     */
    virtual unsigned getStackSize() const;

    /**
     * Returns the amount of stack actually used by the coroutine.
     * (Not implemented for every coroutine package - see cCoroutine
     * documentation for more info.) If the module uses handleMessage(),
     * this method always returns 0.
     *
     * @see cCoroutine
     */
    virtual unsigned getStackUsage() const;
    //@}
};

}  // namespace omnetpp


#endif

