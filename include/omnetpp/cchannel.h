//==========================================================================
//   CCHANNEL.H  -  header for
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

#ifndef __OMNETPP_CCHANNEL_H
#define __OMNETPP_CCHANNEL_H

#include "ccomponent.h"
#include "ccomponenttype.h"
#include "cmessage.h"
#include "ctimestampedvalue.h"

namespace omnetpp {

class cGate;
class cModule;
struct SendOptions;

/**
 * @brief Allows returning multiple values from the processMessage() method.
 */
struct SIM_API ChannelResult
{
    bool discard = false;              ///< Whether the channel has lost the message
    simtime_t delay = SIMTIME_ZERO;    ///< Propagation delay
    simtime_t duration = SIMTIME_ZERO; ///< Transmission duration
    simtime_t remainingDuration = SIMTIME_ZERO; ///< Remaining transmission duration (for tx update)
};

/**
 * @brief Base class for channels.
 *
 * @ingroup ModelComponents
 */
class SIM_API cChannel : public cComponent //implies noncopyable
{
    friend class cModule; // for setting prevSibling/nextSibling
  protected:
    cGate *srcGate = nullptr; // gate the channel is attached to
    int nedConnectionElementId = -1;  // for cChannel::getProperties(); usually the NED connection element's id

  public:
    // internal: called from cGate
    void setSourceGate(cGate *g) {srcGate=g;}

    // internal: sets/gets nedConnectionElementId
    void setNedConnectionElementId(int id) {nedConnectionElementId = id;}
    int getNedConnectionElementId() {return nedConnectionElementId;}

    // internal: called from callInitialize(). Does one stage for this
    // channel, and returns true if there are more stages to do
    virtual bool initializeChannel(int stage);

    // internal: overridden to perform additional checks
    virtual void finalizeParameters() override;

    // internal: calls refreshDisplay() recursively
    virtual void callRefreshDisplay() override;

    // internal: calls preDelete() recursively
    virtual void callPreDelete(cComponent *root) override;

  protected:
    virtual cModule *doFindModuleByPath(const char *s) const override;

  public:
    typedef ChannelResult Result;

    /**
     * @brief Signal value that accompanies the "messageSent" signal.
     */
    class MessageSentSignalValue : public cITimestampedValue, public cObject
    {
      public:
        simtime_t timestamp;
        cMessage *msg;
        Result *result;
      private:
        void error() const;
      public:
        /** Constructor. */
        MessageSentSignalValue(simtime_t_cref t, cMessage *m, Result *r) {timestamp=t; msg=m; result=r;}

        /** @name cITimestampedValue methods */
        //@{
        /** Returns the timestamp; it represents the start of the transmission. */
        virtual simtime_t_cref getTimestamp(simsignal_t signalID) const override {return timestamp;}

        /** Returns SIMSIGNAL_OBJECT. */
        virtual SimsignalType getValueType(simsignal_t signalID) const override {return SIMSIGNAL_OBJECT;}

        /** Returns the message (packet) as the stored object. */
        virtual cObject *objectValue(simsignal_t signalID) const override {return msg;}
        //@}

        /** Returns the message (packet). */
        cMessage *getMessage() const {return msg;}

        /** Returns the channel result. */
        Result *getChannelResult() const {return result;}

        /** @name Other (non-cObject) getters throw an exception. */
        //@{
        virtual bool boolValue(simsignal_t signalID) const override {error(); return false;}
        virtual intval_t intValue(simsignal_t signalID) const override {error(); return 0;}
        virtual uintval_t uintValue(simsignal_t signalID) const override {error(); return 0;}
        virtual double doubleValue(simsignal_t signalID) const override {error(); return 0;}
        virtual SimTime simtimeValue(simsignal_t signalID) const override {error(); return timestamp;}
        virtual const char *stringValue(simsignal_t signalID) const override {error(); return nullptr;}
        //@}
    };

  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Constructor.
     */
    explicit cChannel(const char *name=nullptr);

    /**
     * Destructor. 
     * 
     * Note: It is not allowed delete channel objects directly,
     * only via cGate's disconnect() and reconnectWith() methods.
     */
    virtual ~cChannel();
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name Public methods for invoking initialize()/finish(), redefined from cComponent.
     * initialize(), numInitStages(), and finish() are themselves also declared in
     * cComponent, and can be redefined in channel classes by the user to perform
     * initialization and finalization (result recording, etc) tasks.
     */
    //@{
    /**
     * Interface for calling initialize() from outside. Implementation
     * performs multi-stage initialization for this channel object.
     */
    virtual void callInitialize() override;

    /**
     * Interface for calling initialize() from outside. It does a single stage
     * of initialization, and returns `true` if more stages are required.
     */
    virtual bool callInitialize(int stage) override;

    /**
     * Interface for calling finish() from outside.
     */
    virtual void callFinish() override;
    //@}

    /** @name Channel information. */
    //@{
    /**
     * Redefined from cComponent to return KIND_CHANNEL.
     */
    virtual ComponentKind getComponentKind() const override  {return KIND_CHANNEL;}

    /**
     * Returns the compound module containing this channel. That is,
     * the channel is either between two submodules of getParentModule(),
     * or between getParentModule() and one of its submodules.
     * (For completeness, it may also connect two gates of getParentModule()
     * on the inside).
     */
    virtual cModule *getParentModule() const override;

    /**
     * Convenience method: casts the return value of getComponentType() to cChannelType.
     */
    cChannelType *getChannelType() const  {return (cChannelType *)getComponentType();}

    /**
     * Return the properties for this channel. Properties cannot be changed
     * at runtime. Redefined from cComponent.
     */
    virtual cProperties *getProperties() const override;

    /**
     * Returns the gate this channel is attached to.
     */
    virtual cGate *getSourceGate() const  {return srcGate;}

    /**
     * Returns true if the channel models a nonzero-duration transmission,
     * that is, sets the duration field of cPacket. Only one transmission
     * channel is allowed per connection path (see cGate methods getPreviousGate(),
     * getNextGate(), getPathStartGate(), getPathEndGate()).
     */
    virtual bool isTransmissionChannel() const = 0;
    //@}

    /** @name Channel functionality */
    //@{
    /**
     * This method encapsulates the channel's functionality. The method should
     * model the transmission of the given message starting at the given t time,
     * and store the results (propagation delay, transmission duration,
     * discard flag) in the result object. Only the relevant fields
     * in the result object need to be changed, others can be left untouched.
     *
     * Transmission duration and bit error modeling only applies to packets
     * (i.e. to instances of cPacket, where cMessage's isPacket() returns true),
     * it should be skipped for non-packet messages. The method does not need
     * to call the `setDuration(duration)` method on the packet; this is
     * done by the simulation kernel. However, the method should call
     * `setBitError(true)` on the packet if error modeling results
     * in bit errors.
     *
     * If the method sets the discard flag in the result object, it means
     * that the message object should be deleted by the simulation kernel;
     * this facility can be used to model that the message gets lost in the
     * channel.
     *
     * The method does not need to throw error on overlapping transmissions,
     * or if the packet's duration field is already set; these checks are
     * done by the simulation kernel before processMessage() is called.
     */
    virtual Result processMessage(cMessage *msg, const SendOptions& options, simtime_t t) = 0;

    /**
     * For transmission channels: Returns the nominal data rate of the channel.
     * The number returned from this method should be treated as informative;
     * there is no strict requirement that the channel calculates packet
     * duration by dividing the packet length by the nominal data rate.
     * For example, specialized channels may add the length of a lead-in
     * signal to the duration.
     *
     * @see isTransmissionChannel(), cDatarateChannel
     */
    virtual double getNominalDatarate() const = 0;

    /**
     * For transmission channels: Calculates the transmission duration
     * of the message with the current channel configuration (datarate, etc);
     * it does not check or modify channel state. For non-transmission channels
     * this method returns zero.
     *
     * This method is useful for transmitter modules that need to determine
     * the transmission time of a packet without actually sending the packet.
     *
     * Caveats: this method is "best-effort" -- there is no guarantee that
     * transmission time when the packet is actually sent will be the same as
     * the value returned by this method. The difference may be caused by
     * changed channel parameters (i.e. "datarate" being overwritten), or by
     * a non-time-invariant transmission algorithm.
     *
     * Note that there is no requirement that processMessage() relies on this
     * method to calculated the packet duration. That is, to change the
     * duration computation algorithm via subclassing you need to redefine
     * BOTH the processMessage() and calculateDuration() methods.
     *
     * @see isTransmissionChannel(), cDatarateChannel
     */
    virtual simtime_t calculateDuration(cMessage *msg) const = 0;

    /**
     * For transmission channels: Returns the simulation time
     * the sender gate will finish transmitting. If the gate is not
     * currently transmitting, the result is unspecified but less or equal
     * the current simulation time.
     *
     * @see isTransmissionChannel(), isBusy(), cDatarateChannel
     */
    virtual simtime_t getTransmissionFinishTime() const = 0;

    /**
     * For transmission channels: Returns whether the sender gate
     * is currently transmitting, ie. whether getTransmissionFinishTime()
     * is greater than the current simulation time.
     *
     * @see isTransmissionChannel(), getTransmissionFinishTime(), cDatarateChannel
     */
    virtual bool isBusy() const;

    /**
     * DEPRECATED METHOD. It is only provided for backward compatibility with
     * OMNeT++ 5.x.
     *
     * For transmission channels, this method forcibly overwrites the finish
     * time of the current transmission in the channel. It is a crude device
     * that allows implementing aborting transmissions. Models using this method
     * should be migrated to the transmission update API.
     *
     * @see cSimpleModule::SendOptions, getTransmissionFinishTime(), isBusy()
     */
    [[deprecated]] virtual void forceTransmissionFinishTime(simtime_t t) {}

    /**
     * Returns whether the channel is disabled.
     * A disabled channel discards all messages sent on it.
     */
    virtual bool isDisabled() const  { return false; }
    //@}
};


/**
 * @brief Channel with zero propagation delay, zero transmission delay (infinite
 * datarate), and always enabled.
 *
 * @ingroup ModelComponents
 */
class SIM_API cIdealChannel : public cChannel //implies noncopyable
{
  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Constructor. This is only for internal purposes, and should not
     * be used when creating channels dynamically; use the create()
     * factory method instead.
     */
    explicit cIdealChannel(const char *name=nullptr) : cChannel(name) {}

    /**
     * Destructor.
     */
    virtual ~cIdealChannel() {}

    /**
     * Utility function for dynamic channel creation. Equivalent to
     * `cChannelType::getIdealChannelType()->create(name)`.
     */
    static cIdealChannel *create(const char *name);
    //@}

    /** @name Redefined cChannel member functions. */
    //@{
    /**
     * The cIdealChannel implementation of this method does nothing.
     */
    virtual Result processMessage(cMessage *msg, const SendOptions& options, simtime_t t) override {return Result();}

    /**
     * The cIdealChannel implementation of this method does nothing.
     */
    virtual double getNominalDatarate() const override {return 0;}

    /**
     * The cIdealChannel implementation of this method always returns false.
     */
    virtual bool isTransmissionChannel() const override {return false;}

    /**
     * The cIdealChannel implementation of this method always returns zero.
     */
    virtual simtime_t calculateDuration(cMessage *msg) const override {return SIMTIME_ZERO;}

    /**
     * The cIdealChannel implementation of this method always returns zero.
     */
    virtual simtime_t getTransmissionFinishTime() const override {return SIMTIME_ZERO;}

    /**
     * The cIdealChannel implementation of this method always returns false.
     */
    virtual bool isBusy() const override {return false;}
    //@}
};

}  // namespace omnetpp


#endif
