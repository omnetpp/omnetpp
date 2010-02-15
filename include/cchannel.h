//==========================================================================
//   CCHANNEL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cChannel : channel class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCHANNEL_H
#define __CCHANNEL_H

#include "ccomponent.h"
#include "ccomponenttype.h"

NAMESPACE_BEGIN

class cGate;
class cMessage;


/**
 * Base class for channels.
 *
 * @ingroup SimCore
 */
class SIM_API cChannel : public cComponent //implies noncopyable
{
  protected:
    cGate *srcgatep; // gate the channel is attached to
    int connId;      // for cChannel::getProperties(); usually the NED connection element's id

  public:
    // internal: called from cGate
    void setSourceGate(cGate *g) {srcgatep=g;}

    // internal: sets/gets connId
    void setConnectionId(int id) {connId = id;}
    int getConnectionId() {return connId;}

    // internal: called from callInitialize(). Does one stage for this
    // channel, and returns true if there's more stages to do
    virtual bool initializeChannel(int stage);

    // internal: overridden to perform additional checks
    virtual void finalizeParameters();

  public:
    /**
     * Allows for returning multiple values from the process() method.
     * The constructor initializes all fields to zero.
     */
    struct result_t {
        result_t() : delay(SIMTIME_ZERO), duration(SIMTIME_ZERO), deleteMessage(false) {}
        simtime_t delay;     //< propagation delay
        simtime_t duration;  //< transmission duration
        bool deleteMessage;  //< whether the channel is losing the message
    };

  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Constructor.
     */
    explicit cChannel(const char *name=NULL);

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
    virtual std::string info() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
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
    virtual void callInitialize();

    /**
     * Interface for calling initialize() from outside. It does a single stage
     * of initialization, and returns <tt>true</tt> if more stages are required.
     */
    virtual bool callInitialize(int stage);

    /**
     * Interface for calling finish() from outside.
     */
    virtual void callFinish();
    //@}

    /** @name Channel information. */
    //@{
    /**
     * Returns the compound module containing this channel. That is,
     * the channel is either between two submodules of getParentModule(),
     * or between getParentModule() and one of its submodules.
     * (For completeness, it may also connect two gates of getParentModule()
     * on the inside).
     */
    virtual cModule *getParentModule() const;

    /**
     * Convenience method: casts the return value of getComponentType() to cChannelType.
     */
    cChannelType *getChannelType() const  {return (cChannelType *)getComponentType();}

    /**
     * Return the properties for this channel. Properties cannot be changed
     * at runtime. Redefined from cComponent.
     */
    virtual cProperties *getProperties() const;

    /**
     * Returns the gate this channel is attached to.
     */
    cGate *getSourceGate() const  {return srcgatep;}

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
     * deleteMessage flag) in the result object. Only the relevant fields
     * in the result object need to be changed, others can be left untouched.
     *
     * Transmission duration and bit error modeling only applies to packets
     * (i.e. to instances of cPacket, where cMessage's isPacket() returns true),
     * it should be skipped for non-packet messages.
     *
     * If the transmission duration is nonzero, it is the method's responsibility
     * to call <tt>setDuration(duration)</tt> on the packet. As a result of bit
     * error modeling, the method may set the packet's bit error flag.
     *
     * If the method sets the deleteMessage flag in the result object, that
     * means that the message object should be deleted by the caller; this
     * facility can be used to model that the message gets lost in the channel.
     */
    virtual void process(cMessage *msg, simtime_t t, result_t& result) = 0;

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
     * For channels that support datarate: Returns whether the sender gate
     * is currently transmitting, ie. whether transmissionFinishTime()
     * is greater than the current simulation time.
     *
     * @see isTransmissionChannel(), getTransmissionFinishTime(), cDatarateChannel
     */
    virtual bool isBusy() const;
    //@}
};


/**
 * Channel with zero propagation delay, zero transmission delay (infinite
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
    explicit cIdealChannel(const char *name=NULL) : cChannel(name) {}

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
    virtual void process(cMessage *msg, simtime_t t, result_t& result) {}

    /**
     * The cIdealChannel implementation of this method always returns false.
     */
    virtual bool isTransmissionChannel() const {return false;}

    /**
     * The cIdealChannel implementation of this method always returns zero.
     */
    virtual simtime_t getTransmissionFinishTime() const {return SIMTIME_ZERO;}

    /**
     * The cIdealChannel implementation of this method always returns false.
     */
    virtual bool isBusy() const {return false;}
    //@}
};

NAMESPACE_END


#endif


