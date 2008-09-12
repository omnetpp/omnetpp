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
  private:
    enum {
      FL_ISDISABLED = 128,
      FL_DELAY_NONZERO = 256,
    };

    simtime_t delayparam; // cached value of propagation delay parameter

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
     * Constructor.
     */
    explicit cDelayChannel(const char *name=NULL) : cChannel(name) {}

    /**
     * Destructor.
     */
    virtual ~cDelayChannel() {}
    //@}

    /** @name Redefined cChannel member functions. */
    //@{
    /**
     * Returns false.
     */
    virtual bool isTransmissionChannel() const {return false;}

    /**
     * Returns zero.
     */
    virtual simtime_t calculateDuration(cMessage *msg) const {return 0;}

    /**
     * Returns the current simulation time.
     */
    virtual simtime_t getTransmissionFinishTime() const {return simTime();}

    /**
     * Returns false.
     */
    virtual bool isBusy() const {return false;}
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
    virtual simtime_t getDelay() const {checkState(); return delayparam;}

    /**
     * Returns whether the channel is disabled. This method is equivalent
     * to reading the "disabled" parameter, via par("disabled").
     * A disabled channel discards all messages sent on it.
     */
    virtual bool isDisabled() const  {checkState(); return flags & FL_ISDISABLED;}
    //@}

    /** @name Internally used methods. */
    //@{
    /**
     * Called when parameters get set up. Redefined from cComponent.
     */
    virtual void finalizeParameters();

    /**
     * This implementation delivers the message to the opposite gate
     * with a delay.
     */
    virtual bool deliver(cMessage *msg, simtime_t at);
    //@}
};

NAMESPACE_END


#endif


