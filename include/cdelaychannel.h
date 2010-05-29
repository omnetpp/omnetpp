//==========================================================================
//   CDELAYCHANNEL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDelayChannel : channel class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDELAYCHANNEL_H
#define __CDELAYCHANNEL_H

#include "cchannel.h"
#include "csimulation.h"

NAMESPACE_BEGIN

/**
 * Channel with propagation delay.
 *
 * @ingroup SimCore
 */
class SIM_API cDelayChannel : public cChannel //implies noncopyable
{
  protected:
    static simsignal_t messageSentSignal;
    static simsignal_t messageDiscardedSignal;

  private:
    enum {
      FL_ISDISABLED = 128,
      FL_DELAY_NONZERO = 256,
    };

    simtime_t delay; // cached value of propagation delay parameter

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
    explicit cDelayChannel(const char *name=NULL) : cChannel(name) {}

    /**
     * Destructor.
     */
    virtual ~cDelayChannel() {}

    /**
     * Utility function for dynamic channel creation. Equivalent to
     * <tt>cChannelType::getDelayChannelType()->create(name)</tt>.
     */
    static cDelayChannel *create(const char *name);
    //@}

    /** @name Redefined cChannel member functions. */
    //@{
    /**
     * The cDelayChannel implementation of this method always returns false.
     */
    virtual bool isTransmissionChannel() const {return false;}

    /**
     * The cDelayChannel implementation of this method always returns zero.
     */
    virtual double getNominalDatarate() const {return 0;}

    /**
     * The cDelayChannel implementation of this method always returns zero.
     */
    virtual simtime_t calculateDuration(cMessage *msg) const {return 0;}

    /**
     * The cDelayChannel implementation of this method always returns zero.
     */
    virtual simtime_t getTransmissionFinishTime() const {return SIMTIME_ZERO;}

    /**
     * The cDelayChannel implementation of this method always returns false.
     */
    virtual bool isBusy() const {return false;}

    /**
     * The cDelayChannel implementation of this method does nothing.
     */
    virtual void forceTransmissionFinishTime(simtime_t t) {}
    //@}

    /** @name Setting and getting channel parameters. */
    //@{
    /**
     * Sets the propagation delay of the channel, in seconds.
     */
    virtual void setDelay(double d);

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
     * Returns whether the channel is disabled. This method is equivalent
     * to reading the "disabled" parameter, via par("disabled").
     * A disabled channel discards all messages sent on it.
     */
    virtual bool isDisabled() const  {checkState(); return flags & FL_ISDISABLED;}
    //@}

    /** @name Implementation methods */
    //@{
    /**
     * Initialization.
     */
    virtual void initialize();

    /**
     * This implementation delivers the message to the opposite gate
     * with a delay.
     */
    virtual void processMessage(cMessage *msg, simtime_t t, result_t& result);
    //@}
};

NAMESPACE_END


#endif


