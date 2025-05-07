//==========================================================================
//   CDATARATECHANNEL.H  -  header for
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

#ifndef __OMNETPP_CDATARATECHANNEL_H
#define __OMNETPP_CDATARATECHANNEL_H

#include "cchannel.h"
#include "csimulation.h"

namespace omnetpp {


/**
 * @brief A transmission channel model that supports propagation delay,
 * transmission duration computed from a data rate or given explicitly,
 * and a simple error model with based on bit error rate (BER) and
 * packet error rate (PER). The above parameters can be supplied via
 * NED or explicit setter methods.
 *
 * This class is a transmission channel, i.e. its isTransmissionChannel()
 * method returns true. There can be at most one transmission channel
 * in a connection path.
 *
 * The treatment of packets depends on the mode setting. In mode=SINGLE,
 * at most one transmission can be active at any given time, and overlapping
 * transmissions are not permitted. In mode=MULTI, multiple concurrent
 * transmissions are allowed, with consistency checking (transmission updates
 * must refer to existing and active transmissions). The UNCHECKED mode is the
 * low-overhead version of MULTI: the channel does not keep track of ongoing
 * transmissions, so there are no consistency checks, and certain functionality
 * is unavailable (e.g. the channel does not emit channelBusy signals).
 *
 * Non-packet messages are let through at any time, after applying propagation
 * delay only. They do not interfere with ongoing transmissions in any way.
 *
 * By default, transmission duration is computed from the packet bit length
 * and the channel datarate. The channel datarate is optional; if it is missing,
 * the packet duration needs to be specified explicitly in the send() call
 * (see SendOptions). If both datarate and explicit duration are present,
 * the explicit duration takes precedence.
 *
 * Allowing to override the duration is useful for a few cases: to account for
 * certain parts in the frame (preamble, padding, PHY-layer header with different
 * datarate, etc.) and for aborting at arbitrary time (for example at a
 * fractional bit time).
 *
 * The channel supports updating the ongoing transmission (see SendOptions).
 * Duration and remainingDuration are optional, as long as the missing one(s)
 * can be computed from the others. For example, duration can be computed as
 * the elapsed transmission time plus remainingDuration (unless in UNCHECKED
 * mode, when the channel doesn't keep the needed information around). Sanity
 * checks (e.g. duration/remainingDuration cannot be negative, duration >=
 * remainingDuration, etc.) are included.
 *
 * @ingroup ModelComponents
 */
class SIM_API cDatarateChannel : public cChannel //implies noncopyable
{
  protected:
    static simsignal_t channelBusySignal;
    static simsignal_t messageSentSignal;
    static simsignal_t messageDiscardedSignal;

  public:
    /**
     * Channel operating mode
     */
    enum Mode {
        /**
         * Allow a single transmission at a time.
         */
        SINGLE,

        /**
         * Allow multiple concurrent transmissions, with consistency checks.
         */
        MULTI,

        /**
         * Allow multiple concurrent transmissions, with low overhead. In this mode,
         * the channel does not keep track of ongoing transmissions, so there are
         * no consistency checks, the isBusy() and getTransmissionFinishTime()
         * methods are not supported, and the channel does not emit channelBusy
         * signals.
         */
        UNCHECKED
    };

  private:
    enum {
      FL_ISDISABLED =       1 << 10,
      FL_DELAY_NONZERO =    1 << 11,
      FL_DATARATE_PRESENT = 1 << 12,
      FL_BER_NONZERO =      1 << 13,
      FL_PER_NONZERO =      1 << 14,
    };

    // cached values of parameters (note: parameters are non-volatile)
    simtime_t delay = 0; // propagation delay
    double datarate = 0; // data rate
    double ber = 0;      // bit error rate
    double per = 0;      // packet error rate

    Mode mode = SINGLE;

    // data of the last/ongoing transmission (only for mode=SINGLE)
    struct Tx {
        simtime_t startTime;
        simtime_t finishTime;
        txid_t transmissionId = -1;
    };
    Tx singleTx; // for SINGLE
    std::vector<Tx> txList; // for MULTI
    simtime_t channelFinishTime; // for both SINGLE and MULTI

  private:
    // internal: checks whether parameters have been set up
    void checkState() const  {if (!parametersFinalized()) throw cRuntimeError(this, E_PARAMSNOTREADY);}

  protected:
    // internal: update cached copies of parameters
    void rereadPars();

    // internal: react to parameter changes
    virtual void handleParameterChange(const char *parname) override;

    // internal: helper to processMessage()
    virtual void processPacket(cPacket *pkt, const SendOptions& options, simtime_t t, Result& inoutResult);

    // internal: emits last busy signal
    virtual void finish() override;

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor. This is only for internal purposes, and should not
     * be used when creating channels dynamically; use the create()
     * factory method instead.
     */
    explicit cDatarateChannel(const char *name=nullptr);

    /**
     * Destructor.
     */
    virtual ~cDatarateChannel() {}

    /**
     * Utility function for dynamic channel creation. Equivalent to
     * `cChannelType::getDatarateChannelType()->create(name)`.
     */
    static cDatarateChannel *create(const char *name);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name Setting and getting channel parameters. */
    //@{
    /**
     * Returns true.
     */
    virtual bool isTransmissionChannel() const override {return true;}

    /**
     * Sets the operating mode. Switching operating mode during simulation
     * (when the channel is in use) is not allowed.
     */
    virtual void setMode(Mode mode);

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
     * Returns the operating mode.
     */
    virtual Mode getMode() const {return mode;}

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
    virtual bool isDisabled() const override  {checkState(); return flags & FL_ISDISABLED;}
    //@}

    /** @name Transmission state. */
    //@{
    /**
     * Same as getDatarate().
     */
    virtual double getNominalDatarate() const override {return getDatarate();}

    /**
     * Returns the message length in bits divided by the datarate.
     *
     * Note that processMessage() does NOT call this method, so in order to
     * change the duration computation algorithm via subclassing you need
     * to redefine both this and processMessage().
     */
    virtual simtime_t calculateDuration(cMessage *msg) const override;

    /**
     * Returns the simulation time all transmissions will finish on the
     * (sender side) of the channel. If there is no ongoing transmission,
     * the result is undefined but less or equal the current simulation time.
     *
     * This method is unsupported (throws exception) in the UNCHECKED mode.
     */
    virtual simtime_t getTransmissionFinishTime() const override;

    /**
     * Returns whether the sender gate is currently transmitting, ie. whether
     * transmissionFinishTime() is greater than the current simulation time.
     *
     * This method is unsupported (throws exception) in the UNCHECKED mode.
     */
    virtual bool isBusy() const override;

    /**
     * DEPRECATED METHOD. It is only provided for backward compatibility with
     * OMNeT++ 5.x.
     *
     * This method forcibly overwrites the finish time of the current
     * transmission in the channel. It is a crude device that allows
     * implementing aborting transmissions. Models using this method
     * should be migrated to the transmission update API.
     *
     * @see cSimpleModule::SendOptions, getTransmissionFinishTime(), isBusy()
     */
    [[deprecated]] virtual void forceTransmissionFinishTime(simtime_t t) override;
    //@}

    /** @name Implementation methods */
    //@{
    /**
     * Initialization.
     */
    virtual void initialize() override;

    /**
     * Performs bit error rate, delay and transmission time modelling.
     */
    virtual Result processMessage(cMessage *msg, const SendOptions& options, simtime_t t) override;
    //@}
};

}  // namespace omnetpp

#endif


