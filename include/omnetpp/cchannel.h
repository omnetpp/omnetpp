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


/**
 * @brief Base class for channels.
 *
 * @ingroup SimCore
 */
class SIM_API cChannel : public cComponent //implies noncopyable
{
  protected:
    cGate *srcGate; // gate the channel is attached to
    int nedConnectionElementId;  // for cChannel::getProperties(); usually the NED connection element's id

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

  public:
    /**
     * @brief Allows returning multiple values from the processMessage() method.
     *
     * The constructor initializes all fields to zero.
     */
    struct result_t
    {
        result_t() : delay(SIMTIME_ZERO), duration(SIMTIME_ZERO), discard(false) {}
        simtime_t delay;     ///< Propagation delay
        simtime_t duration;  ///< Transmission duration
        bool discard;        ///< Whether the channel has lost the message
    };

    /**
     * @brief Signal value that accompanies the "messageSent" signal.
     */
    class MessageSentSignalValue : public cITimestampedValue, public cObject
    {
      public:
        simtime_t timestamp;
        cMessage *msg;
        result_t *result;
      private:
        void error() const;
      public:
        /** Constructor. */
        MessageSentSignalValue(simtime_t_cref t, cMessage *m, result_t *r) {timestamp=t; msg=m; result=r;}

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
        result_t *getChannelResult() const {return result;}

        /** @name Other (non-cObject) getters throw an exception. */
        //@{
        virtual bool boolValue(simsignal_t signalID) const override {error(); return false;}
        virtual long longValue(simsignal_t signalID) const override {error(); return 0;}
        virtual unsigned long unsignedLongValue(simsignal_t signalID) const override {error(); return 0;}
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

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
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
     * of initialization, and returns <tt>true</tt> if more stages are required.
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
     * to call the <tt>setDuration(duration)</tt> method on the packet; this is
     * done by the simulation kernel. However, the method should call
     * <tt>setBitError(true)</tt> on the packet if error modeling results
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
    virtual void processMessage(cMessage *msg, simtime_t t, result_t& result) = 0;

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
     * For transmission channels: Forcibly overwrites the finish time of the
     * current transmission in the channel (see getTransmissionFinishTime()).
     *
     * This method is a crude device that allows for implementing aborting
     * transmissions; it is not needed for normal packet transmissions.
     * Calling this method with the current simulation time will allow
     * you to immediately send another packet on the channel without the
     * channel reporting error due to its being busy.
     *
     * Note that this call does NOT affect the delivery of the packet being
     * transmitted: the packet object is delivered to the target module
     * at the time it would without the call to this method. The sender
     * needs to inform the target module in some other way that the
     * transmission was aborted and the packet should be treated accordingly
     * (i.e. discarded as incomplete); for example by sending an out-of-band
     * cMessage that the receiver has to understand.
     */
    virtual void forceTransmissionFinishTime(simtime_t t) = 0;
    //@}
};


/**
 * @brief Channel with zero propagation delay, zero transmission delay (infinite
 * datarate), and always enabled.
 *
 * @ingroup SimCore
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
     * <tt>cChannelType::getIdealChannelType()->create(name)</tt>.
     */
    static cIdealChannel *create(const char *name);
    //@}

    /** @name Redefined cChannel member functions. */
    //@{
    /**
     * The cIdealChannel implementation of this method does nothing.
     */
    virtual void processMessage(cMessage *msg, simtime_t t, result_t& result) override {}

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

    /**
     * The cIdealChannel implementation of this method does nothing.
     */
    virtual void forceTransmissionFinishTime(simtime_t t) override {}
    //@}
};

}  // namespace omnetpp


#endif
