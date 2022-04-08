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
 * @brief Options for the cSimpleModule::send() and cSimpleModule::sendDirect() calls.
 *
 * This class currently allows specifying send delay, propagation delay, duration,
 * and sending a transmission update.
 *
 * A convenient way to produce an appropriately configured instance for a send() call
 * is by creating a default instance, and chaining mutator methods:
 *
 * <pre>
 * send(msg, SendOptions().updateTx(transmissionId, remainingDuration).duration(duration), "out");
 * </pre>
 *
 * Transmission updates:
 *
 * A transmission update is needed when the transmission of a packet (sending
 * a cPacket over a connection path that contains a transmission channel,
 * i.e. a channel whose isTransmissionChannel() method returns true) needs
 * to be aborted or preempted, extended, or modified in other unforeseen ways
 * while the transmission is in progress.
 *
 * Transmission updates are normal packets (cPacket) sent with the
 * updateTx() or finishTx() options. These methods expect a transmission ID
 * which should be determined when sending the original packet, and specified
 * using the transmissionId() method. A transmission can be updated several
 * times, using always the same transmissionId.
 *
 * The packet in each transmission update conceptually replaces the
 * packet that the peer is going to receive. Updates may be sent
 * while transmission is still ongoing, and, in certain cases,
 * also at the time the packet transmission ends, but not later.
 *
 * As an example, aborting a transmission is done by sending a
 * packet with a truncated content and a remaining duration of zero.
 *
 * Receivers that receive the packet at the end of the reception (default)
 * will only receive the final update, the original packet and
 * intermediate updates are absorbed by the simulation kernel.
 *
 * Receivers that receive the packet at the start of the reception (see
 * cGate::setDeliverImmediately()) should be prepared to receive the
 * original packet and all its updates, and handle them appropriately
 * (i.e. only keep the packet in last update). Transmission updates
 * can be recognized at the receiver end by the cPacket::isUpdate()
 * method returning true. In order to reduce the chance of unprepared
 * modules receiving transmission updates and interpreting them as
 * independent packets leading to erroneous operation, modules will
 * only receive transmission updates if they explicitly declare that
 * they are prepared to handle them by calling
 * cSimpleModule::setTxUpdateSupport(true) beforehand. (If this flag
 * is not set in a module, sending a transmission update to it will
 * result in a runtime error.
 *
 * @see cSimpleModule::send(), cSimpleModule::sendDirect
 * @ingroup Misc
 */
struct SIM_API SendOptions {
    simtime_t sendDelay = SIMTIME_ZERO; // for after()
    simtime_t propagationDelay_ = SIMTIME_ZERO; // for sendDirect()
    simtime_t duration_ = DURATION_UNSPEC; // for packets not using channel datarate
    bool isUpdate = false;
    txid_t transmissionId_ = -1; // if not -1, specifies the transmission id
    simtime_t remainingDuration = DURATION_UNSPEC; // when updating an earlier transmission

    static const simtime_t DURATION_UNSPEC;
    static const SendOptions DEFAULT;

    /**
     * Creates an instance with the default settings.
     */
    SendOptions() {}

    /**
     * Specifies delay for the send operation. Conceptually, this would cause
     * timer to start, and the actual send operation would take place when
     * the timer expires. In practice, for the sake of efficiency, the explicit
     * timer is omitted, and delay is simply applied to the message before
     * it enters the connection path that leads to the destination module.
     * Note that this simplified modeling does not account for changes in the
     * connection path during the delay period, so if modeling those changes are
     * important, you have to use an explicit timer followed by a regular send.
     */
    SendOptions& after(simtime_t delay) {this->sendDelay = delay; return *this;}

    /**
     * Specifies the propagation delay for sendDirect(). It is an error to use
     * this option with send() or sendDelayed().
     */
    SendOptions& propagationDelay(simtime_t delay) {this->propagationDelay_ = delay; return *this;}

    /**
     * Specifies an explicit transmission duration. This is only allowed if the
     * connection path contains a transmission channel (a cChannel whose
     * isTransmissionChannel() method returns true). cDatarateChannel,
     * which is the only built-in transmission channel model in OMNeT++,
     * can compute transmission duration from the packet length in bits and
     * the datarate, but allows it to be overridden with this option.
     */
    SendOptions& duration(simtime_t duration) {this->duration_ = duration; return *this;}

    /**
     * Specifies the transmission ID for a (non-update) packet. A transmission ID
     * is necessary for sending transmission updates (see finishTx() and updateTx()).
     * It is customary to use the packet's message id as transmission ID.
     *
     * @see cMessage::getId(), cPacket::isUpdate()
     */
    SendOptions& transmissionId(txid_t transmissionId) {this->transmissionId_ = transmissionId; return *this;}

    /**
     * Specifies that this is a transmission update, where the remaining duration
     * is zero. See the comment of this class for an explanation of transmission
     * updates.
     *
     * @see cPacket::isUpdate(), cPacket::getRemainingDuration()
     */
    SendOptions& finishTx(txid_t transmissionId) {this->isUpdate = true; this->transmissionId_ = transmissionId; remainingDuration = SIMTIME_ZERO; return *this;}

    /**
     * Specifies that this is a transmission update, where the remaining duration should be
     * computed by the channel (as packet duration minus elapsed transmission time).
     * See the comment of this class for an explanation of transmission updates.
     *
     * @see cPacket::isUpdate(), cPacket::getRemainingDuration()
     */
    SendOptions& updateTx(txid_t transmissionId) {this->isUpdate = true; this->transmissionId_ = transmissionId; remainingDuration = DURATION_UNSPEC; return *this;}

    /**
     * Specifies that this is a transmission update, with the given remaining duration.
     * See the comment of this class for an explanation of transmission
     * updates.@see cPacket::isUpdate(), cPacket::getRemainingDuration()
     */
    SendOptions& updateTx(txid_t transmissionId, simtime_t remainingDuration) {this->isUpdate = true; this->transmissionId_ = transmissionId; this->remainingDuration = remainingDuration; return *this;}

    /**
     * Returns the options in a string form.
     */
    std::string str() const;
};

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
 * @ingroup SimProgr
 */
class SIM_API cSimpleModule : public cModule //implies noncopyable
{
    friend class cModule;
    friend class cSimulation;
    friend class cMessage;

  private:
    enum {
        FL_USESACTIVITY        = 1 << 13, // uses activity() or handleMessage()
        FL_ISTERMINATED        = 1 << 14, // for both activity and handleMessage modules
        FL_STACKALREADYUNWOUND = 1 << 15, // only for activity modules
        FL_SUPPORTSTXUPDATES   = 1 << 16, // whether module is prepared to receive tx updates (see SendOptions)
    };

    cMessage *timeoutMessage;   // msg used in wait() and receive() with timeout
    cCoroutine *coroutine;

    static cMessage *msgForActivity; // helper variable to pass the received message into activity()
    static bool stackCleanupRequested; // 'true' value asks activity() to throw a cStackCleanupException
    static cSimpleModule *afterCleanupTransferTo; // transfer back to this module (or to main)

  private:
    // internal use
    static void activate(void *p);

    // internal utility methods
    cGate *resolveSendDirectGate(cModule *mod, int gateId);
    cGate *resolveSendDirectGate(cModule *mod, const char *gateName, int gateIndex);
    static SendOptions resolveSendDirectOptions(simtime_t propagationDelay, simtime_t duration);
    void deleteObsoletedTransmissionFromFES(txid_t transmissionId, cPacket *updatePkt);
    void throwNotOwnerOfMessage(const char *sendOp, cMessage *msg);

  protected:
    // internal: handle a message event, basically dispatch to handleMessage() or activity(); not supposed to be overridden by models
    virtual void doMessageEvent(cMessage *msg);

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
     * Destructor. Note: it is not allowed delete modules directly, only via
     * the deleteModule() method.
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
     * This method is invoked on the module at the end of the connection path,
     * as part of the send()/sendDirect() protocol. This implementation
     * inserts the message into the FES after some processing.
     */
    virtual void arrived(cMessage *msg, cGate *ongate, const SendOptions& options, simtime_t t) override;
    //@}

    /** @name Information about the module. */
    //@{

    /**
     * Returns the event handling scheme: activity() or handleMessage().
     */
    bool usesActivity() const  {return flags&FL_USESACTIVITY;}

    /**
     * Calling this method with the argument of true indicates that this module
     * is prepared to receive transmission updates, and can handle them properly.
     *
     * This method (and the underlying flag) exists to reduce the chance of
     * unprepared modules receiving transmission updates and interpreting
     * them as independent packets, leading to subtle errors in the simulation.
     * If this flag is not set on a module, then sending a transmission update
     * to it will result in a runtime error.
     *
     * Modules that set the flag are supposed to check whether a received packet
     * is a transmission update by calling cPacket::isUpdate(), and act accordingly.
     *
     * @see supportsTxUpdates(), cPacket::isUpdate(), SendOptions::updateTx()
     */
    void setTxUpdateSupport(bool b) {setFlag(FL_SUPPORTSTXUPDATES, b);}

    /**
     * Returns true if this module is prepared to receive transmission updates.
     *
     * @see setTxUpdateSupport(), cPacket::isUpdate(), SendOptions::updateTx()
     */
    bool supportsTxUpdates() const  {return flags&FL_SUPPORTSTXUPDATES;}

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
     * Qtenv also supports making snapshots interactively.
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
    virtual void send(cMessage *msg, int gateid)  {send(msg, gate(gateid));}

    /**
     * Sends a message through the gate given with its name and index.
     * The index argument is only required if the gate is part of a gate vector.
     */
    virtual void send(cMessage *msg, const char *gatename, int gateindex=-1)  {send(msg, gate(gatename, gateindex));}

    /**
     * Sends a message through the gate given with its pointer.
     */
    virtual void send(cMessage *msg, cGate *outputgate)  {send(msg, SendOptions::DEFAULT, outputgate);}

    /**
     * Sends a message with the given options through the gate given with its ID.
     */
    virtual void send(cMessage *msg, const SendOptions& options, int gateid)  {send(msg, options, gate(gateid));}

    /**
     * Sends a message with the given options through the gate given with its name and index.
     * The index argument is only required if the gate is part of a gate vector.
     */
    virtual void send(cMessage *msg, const SendOptions& options, const char *gatename, int gateindex=-1)  {send(msg, options, gate(gatename, gateindex));}

    /**
     * Sends a message with the given options through the gate given with its pointer.
     */
    virtual void send(cMessage *msg, const SendOptions& options, cGate *outputgate);

    /**
     * Utility function, equivalent to send(msg, SendOptions().after(delay), gateid).
     */
    virtual void sendDelayed(cMessage *msg, simtime_t delay, int gateid)  {send(msg, SendOptions().after(delay), gate(gateid));}

    /**
     * Utility function, equivalent to send(msg, SendOptions().after(delay), gatename, gateindex).
     */
    virtual void sendDelayed(cMessage *msg, simtime_t delay, const char *gatename, int gateindex=-1)  {send(msg, SendOptions().after(delay), gate(gatename, gateindex));}

    /**
     * Utility function, equivalent to send(msg, SendOptions().after(delay), outputgate).
     */
    virtual void sendDelayed(cMessage *msg, simtime_t delay, cGate *outputgate)  {send(msg, SendOptions().after(delay), outputgate);}

    /**
     * Sends a message directly to another module, with zero propagation delay
     * and duration. See sendDirect(cMessage *, const SendOptions&, cGate *)
     * for a more detailed description.
     */
    virtual void sendDirect(cMessage *msg, cModule *mod, const char *inputGateName, int gateIndex=-1)  {sendDirect(msg, SendOptions::DEFAULT, resolveSendDirectGate(mod, inputGateName, gateIndex));}

    /**
     * Sends a message directly to another module, with zero propagation delay
     * and duration. See sendDirect(cMessage *, const SendOptions&, cGate *)
     * for a more detailed description.
     */
    virtual void sendDirect(cMessage *msg, cModule *mod, int inputGateId) {sendDirect(msg, SendOptions::DEFAULT, resolveSendDirectGate(mod, inputGateId));}

    /**
     * Sends a message directly to another module, with zero propagation delay
     * and duration. See sendDirect(cMessage *, const SendOptions&, cGate *)
     * for a more detailed description.
     */
    virtual void sendDirect(cMessage *msg, cGate *inputGate) {sendDirect(msg, SendOptions::DEFAULT, inputGate);}

    /**
     * Sends a message directly to another module, with the given options.
     * See sendDirect(cMessage *, const SendOptions&, cGate *) for a more
     * detailed description.
     */
    virtual void sendDirect(cMessage *msg, const SendOptions& options, cModule *mod, const char *inputGateName, int gateIndex=-1) {sendDirect(msg, options, resolveSendDirectGate(mod, inputGateName, gateIndex));}

    /**
     * Sends a message directly to another module, with the given options.
     * See sendDirect(cMessage *, const SendOptions&, cGate *) for a more
     * detailed description.
     */
    virtual void sendDirect(cMessage *msg, const SendOptions& options, cModule *mod, int inputGateId) {sendDirect(msg, options, resolveSendDirectGate(mod, inputGateId));}

    /**
     * Send a message directly to another module, with the given options.
     *
     * If the target gate is further connected (i.e. getNextGate()!=nullptr),
     * the message will follow the connection path that starts at that gate.
     * Notably, when sending to an input gate of a compound module,
     * the message will follow the connections inside the compound module.
     *
     * It is permitted to send to an output gate, which will also cause
     * the message to follow the connections starting at that gate.
     * This can be useful, for example, when several submodules are sending
     * to a single output gate of their parent module.
     *
     * It is not permitted to send to a gate of a compound module which is not
     * further connected (i.e. getNextGate()==nullptr), as this would cause the
     * message to arrive at a compound module.
     *
     * Also, it is not permitted to send to a gate which is otherwise connected
     * i.e. where getPreviousGate()!=nullptr. This means that modules MUST have
     * dedicated gates for receiving via sendDirect(). You cannot have a gate
     * which receives messages via both connections and sendDirect().
     */
    virtual void sendDirect(cMessage *msg, const SendOptions& options, cGate *inputGate);

    /**
     * Utility function, roughly equivalent to sendDirect(msg, SendOptions().propagationDelay(delay).duration(duration), mod, inputGateName, gateIndex).
     */
    virtual void sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cModule *mod, const char *inputGateName, int gateIndex=-1) {sendDirect(msg, resolveSendDirectOptions(propagationDelay, duration), resolveSendDirectGate(mod, inputGateName, gateIndex));}

    /**
     * Utility function, roughly equivalent to sendDirect(msg, SendOptions().propagationDelay(delay).duration(duration), mod, inputGateId).
     */
    virtual void sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cModule *mod, int inputGateId) {sendDirect(msg, resolveSendDirectOptions(propagationDelay, duration), resolveSendDirectGate(mod, inputGateId));}

    /**
     * Utility function, roughly equivalent to sendDirect(msg, SendOptions().propagationDelay(delay).duration(duration), inputGate).
     */
    virtual void sendDirect(cMessage *msg, simtime_t propagationDelay, simtime_t duration, cGate *inputGate) {sendDirect(msg, resolveSendDirectOptions(propagationDelay, duration), inputGate);}
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
     *
     * @see scheduleAfter()
     */
    virtual void scheduleAt(simtime_t t, cMessage *msg);

    /**
     * Schedules a self-message for the given time delta after the current
     * simulation time. This method is equivalent to scheduleAt(simTime()+delay, msg);
     * See scheduleAt() for more information.
     *
     * @see scheduleAt()
     */
    virtual void scheduleAfter(simtime_t delay, cMessage *msg);

    /**
     * Equivalent to scheduleAt(simtime_t t, cMessage *msg), except that
     * if the message is currently scheduled, the method cancels it before
     * scheduling it again.
     *
     * @see scheduleAt(), cancelEvent(), cMessage::isScheduled()
     */
    virtual void rescheduleAt(simtime_t t, cMessage *msg);

    /**
     * Equivalent to scheduleAfter(simtime_t t, cMessage *msg), except that
     * if the message is currently scheduled, the method cancels it before
     * scheduling it again.
     *
     * @see scheduleAfter(), cancelEvent(), cMessage::isScheduled()
     */
    virtual void rescheduleAfter(simtime_t delay, cMessage *msg);

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
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
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

