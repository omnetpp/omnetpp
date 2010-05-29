//==========================================================================
//   CDATARATECHANNEL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDATARATECHANNEL_H
#define __CDATARATECHANNEL_H

#include "cchannel.h"
#include "csimulation.h"

NAMESPACE_BEGIN


/**
 * Channel that models delay, bit error rate and data rate.
 *
 * @ingroup SimCore
 */
class SIM_API cDatarateChannel : public cChannel //implies noncopyable
{
  protected:
    static simsignal_t channelBusySignal;
    static simsignal_t messageSentSignal;
    static simsignal_t messageDiscardedSignal;

  private:
    enum {
      FL_ISDISABLED = 128,
      FL_DELAY_NONZERO = 256,
      FL_DATARATE_NONZERO = 512,
      FL_BER_NONZERO = 1024,
      FL_PER_NONZERO = 2048,
    };

    // cached values of parameters (note: parameters are non-volatile)
    simtime_t delay; // propagation delay
    double datarate; // data rate
    double ber;      // bit error rate
    double per;      // packet error rate

    // stores the end of the last transmission; used if there is a datarate
    simtime_t txfinishtime;

  private:
    // internal: checks whether parameters have been set up
    void checkState() const  {if (!parametersFinalized()) throw cRuntimeError(this, ePARAMSNOTREADY);}

  protected:
    // internal: update cached copies of parameters
    void rereadPars();

    /**
     * Called back when a parameter changes. Redefined from cComponent.
     */
    virtual void handleParameterChange(const char *parname);

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor. This is only for internal purposes, and should not
     * be used when creating channels dynamically; use the create()
     * factory method instead.
     */
    explicit cDatarateChannel(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cDatarateChannel();

    /**
     * Utility function for dynamic channel creation. Equivalent to
     * <tt>cChannelType::getDatarateChannelType()->create(name)</tt>.
     */
    static cDatarateChannel *create(const char *name);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name Setting and getting channel parameters. */
    //@{
    /**
     * Returns true.
     */
    virtual bool isTransmissionChannel() const {return true;}

    /**
     * Sets the propagation delay of the channel, in seconds.
     */
    virtual void setDelay(double d);

    /**
     * Sets the data rate of the channel, in bit/second.
     *
     * @see isBusy(), getTransmissionFinishTime()
     */
    virtual void setDatarate(double d);

    /**
     * Sets the bit error rate (BER) of the channel.
     *
     * @see cPacket::hasBitError()
     */
    virtual void setBitErrorRate(double d);

    /**
     * Sets the packet error rate (PER) of the channel.
     *
     * @see cPacket::hasBitError()
     */
    virtual void setPacketErrorRate(double d);

    /**
     * Disables or enables the channel.
     */
    virtual void setDisabled(bool d);

    /**
     * Returns the propagation delay of the channel, in seconds.
     * This method is equivalent to reading the "delay" parameter, via par("delay").
     */
    virtual simtime_t getDelay() const {checkState(); return delay;}

    /**
     * Returns the data rate of the channel, in bit/second.
     * This method is equivalent to reading the "datarate" parameter, via par("datarate").
     * This value affects the transmission time of messages sent through
     * the channel.
     */
    virtual double getDatarate() const  {checkState(); return datarate;}

    /**
     * Returns the bit error rate (BER) of the channel.
     * This method is equivalent to reading the "ber" parameter, via par("ber").
     * When a message sent through the channel suffers at least
     * one bit error, its bit error flag will be set.
     */
    virtual double getBitErrorRate() const  {checkState(); return ber;}

    /**
     * Returns the packet error rate (PER) of the channel.
     * This method is equivalent to reading the "per" parameter, via par("per").
     * When a message is sent through the channel, its bit error flag
     * will be set with this probability.
     */
    virtual double getPacketErrorRate() const  {checkState(); return per;}

    /**
     * Returns whether the channel is disabled.
     * This method is equivalent to reading the "disabled" parameter, via par("disabled").
     * A disabled channel discards all messages sent on it.
     */
    virtual bool isDisabled() const  {checkState(); return flags & FL_ISDISABLED;}
    //@}

    /** @name Transmission state. */
    //@{
    /**
     * Same as getDatarate().
     */
    virtual double getNominalDatarate() const {return getDatarate();}

    /**
     * Returns the message length in bits divided by the datarate.
     *
     * Note that processMessage() does NOT call this method, so in order to
     * change the duration computation algorithm via subclassing you need
     * to redefine both this and processMessage().
     */
    virtual simtime_t calculateDuration(cMessage *msg) const;

    /**
     * Returns the simulation time the sender gate will finish transmitting.
     * If the gate is not currently transmitting, the result is undefined but
     * less or equal the current simulation time.
     *
     * The transmission duration of a message depends on the message length
     * and the data rate assigned to the channel.
     */
    virtual simtime_t getTransmissionFinishTime() const {return txfinishtime;}

    /**
     * Returns whether the sender gate is currently transmitting, ie. whether
     * transmissionFinishTime() is greater than the current simulation time.
     */
    virtual bool isBusy() const {return simTime() < txfinishtime;}

    /**
     * Forcibly overwrites the finish time of the current transmission in the
     * channel (see getTransmissionFinishTime()). This method can be used for
     * implementing transmission abortion; see declaration in base class
     * (cChannel::forceTransmissionFinishTime()) for more information.
     */
    virtual void forceTransmissionFinishTime(simtime_t t);
    //@}

    /** @name Implementation methods */
    //@{
    /**
     * Initialization.
     */
    virtual void initialize();

    /**
     * Performs bit error rate, delay and transmission time modelling.
     */
    virtual void processMessage(cMessage *msg, simtime_t t, result_t& result);
    //@}
};

NAMESPACE_END

#endif


