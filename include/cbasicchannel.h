//==========================================================================
//   CBASICCHANNEL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CBASICCHANNEL_H
#define __CBASICCHANNEL_H

#include "cchannel.h"
#include "cpar.h"


/**
 * Channel that models delay, bit error rate and data rate.
 *
 * @ingroup SimCore
 */
class SIM_API cBasicChannel : public cChannel
{
  private:
    // stores the end of the last transmission; used if there is a datarate
    simtime_t transm_finishes;

    // cached parameters
    enum {
      FL_CONSTDISABLED = 0x10,
      FL_ISDISABLED = 0x20,
      FL_HASDELAY = 0x40,
      FL_CONSTDELAY = 0x80,
      FL_HASERROR = 0x100,
      FL_CONSTERROR = 0x200,
      FL_HASDATARATE = 0x400,
      FL_CONSTDATARATE = 0x800,
      FL_BASICCHANNELFLAGS = 0x0FF0,
    };
    double delaypar;    // propagation delay (if FL_HASDELAY and FL_CONSTDELAY)
    double errorpar;    // bit error rate (if FL_HASERROR and FL_CONSTERROR)
    double dataratepar; // data rate (if FL_HASDATARATE and FL_CONSTDATARATE)

  protected:
    void rereadPars();

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

    /** @name Redefined cObject functions. */
    //@{
    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
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
     * @see isBusy(), transmissionFinishes()
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
    virtual double delay()  {return par("delay").doubleValue();}

    /**
     * Returns the bit error rate of the channel.
     */
    virtual double error()  {return par("error").doubleValue();}

    /**
     * Returns the data rate of the channel.
     */
    virtual double datarate()  {return par("datarate").doubleValue();}

    /**
     * Returns the "disabled" parameter of the channel.
     */
    virtual bool disabled()  {return par("disabled").boolValue();}
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
    virtual simtime_t transmissionFinishes() const {return transm_finishes;}
    //@}

    /** @name Internally used methods. */
    //@{

    /**
     * Performs bit error rate, delay and transmission time modelling.
     */
    virtual bool deliver(cMessage *msg, simtime_t at);
    //@}
};

#endif


