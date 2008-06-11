//==========================================================================
//   CBASICCHANNEL.H  -  header for
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

#ifndef __CBASICCHANNEL_H
#define __CBASICCHANNEL_H

#include "cchannel.h"
#include "cpar.h"

NAMESPACE_BEGIN


/**
 * Channel that models delay, bit error rate and data rate.
 *
 * @ingroup SimCore
 */
class SIM_API cBasicChannel : public cChannel //implies noncopyable
{
  private:
    enum {
      FL_ISDISABLED = 64,
      FL_DELAY_NONZERO = 128,
      FL_ERROR_NONZERO = 256,
      FL_DATARATE_NONZERO = 512,
    };

    // cached values of parameters (note: parameters are non-volatile)
    simtime_t delayparam; // propagation delay
    double errorparam;    // bit error rate
    double datarateparam; // data rate

    // stores the end of the last transmission; used if there is a datarate
    simtime_t txfinishtime;

  private:
    // internal: checks whether parameters have been set up
    void checkState() const  {if (!areParamsFinalized()) throw cRuntimeError(this, ePARAMSNOTREADY);}

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
    explicit cBasicChannel(const char *name=NULL);

    /**
     * Destructor.
     */
    virtual ~cBasicChannel();
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;
    //@}

    /** @name Setting and getting channel attributes. */
    //@{

    /**
     * Sets the delay parameter of the channel.
     * An alternative way of accessing this values is by par("delay").
     */
    virtual void setDelay(double d);

    /**
     * Sets the bit error rate parameter of the channel.
     * An alternative way of accessing this values is by par("error").
     * When a message sent through the channel suffers at least
     * one bit error, its bit error flag will be set.
     *
     * @see cMessage::hasBitError()
     */
    virtual void setError(double d);

    /**
     * Sets the data rate parameter of the channel.
     * An alternative way of accessing this values is by par("datarate").
     * This value affects the transmission time of messages sent through
     * the channel.
     *
     * @see isBusy(), getTransmissionFinishTime()
     */
    virtual void setDatarate(double d);

    /**
     * Sets the "disabled" parameter of the channel. A disabled channel
     * discards all messages sent on it.
     */
    virtual void setDisabled(bool d);

    /**
     * Returns the delay of the channel.
     */
    virtual simtime_t getDelay() const {checkState(); return delayparam;}

    /**
     * Returns the bit error rate of the channel.
     */
    virtual double getError() const  {checkState(); return errorparam;}

    /**
     * Returns the data rate of the channel.
     */
    virtual double getDatarate() const  {checkState(); return datarateparam;}

    /**
     * Returns the "disabled" parameter of the channel.
     */
    virtual bool isDisabled() const  {checkState(); return flags & FL_ISDISABLED;}
    //@}

    /** @name Transmission state. */
    //@{

    /**
     * Returns whether the sender gate is currently transmitting.
     * Transmission time of a message depends on the message length
     * and the data rate assigned to the channel.
     *
     * If no data rate is assigned to the channel, the result is false.
     */
    virtual bool isBusy() const;

    /**
     * Returns the simulation time the sender gate will finish transmitting.
     * The return value is only meaningful if isBusy() is true.
     *
     * Transmission time of a message depends on the message length
     * and the data rate assigned to the channel.
     */
    virtual simtime_t getTransmissionFinishTime() const {return txfinishtime;}
    //@}

    /** @name Internally used methods. */
    //@{
    /**
     * Called when parameters get set up. Redefined from cComponent.
     */
    virtual void finalizeParameters();

    /**
     * Performs bit error rate, delay and transmission time modelling.
     */
    virtual bool deliver(cMessage *msg, simtime_t at);
    //@}
};

NAMESPACE_END

#endif


