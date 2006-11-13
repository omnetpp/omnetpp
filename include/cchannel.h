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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCHANNEL_H
#define __CCHANNEL_H

#include "cobject.h"
#include "cpar.h"
#include "carray.h"

//=== classes mentioned:
class cPar;
class cGate;
class cChannelType;
class cArray;
class cMessage;


/**
 * Base class for channels. This channel doesn't delay or change messages.
 * One can attach parameters (e.g. "cost").
 *
 * @ingroup SimCore
 */
class SIM_API cChannel : public cObject
{
    friend class cGate; // to call setFromGate()
  protected:
    cChannelType *channeltypep;  // channel type object or NULL
    cArray *parlistp;            // ptr to list of all parameters
    cGate *fromgatep;            // gate the channel is attached to
    // TBD: add on/off modelling? or to cGate?

  protected:
    // helper functions
    void _createparlist();
    cArray& _parList() {if (!parlistp) _createparlist(); return *parlistp;}
    void setFromGate(cGate *g) {fromgatep=g;}

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cChannel(const cChannel& ch);

    /**
     * Constructor.
     */
    explicit cChannel(const char *name=NULL, cChannelType *l=NULL);

    /**
     * Destructor.
     */
    virtual ~cChannel();

    /**
     * Assignment operator. Duplication and the assignment operator work all right with cChannel.
     * The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cChannel& operator=(const cChannel& msg);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cChannel(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(std::ostream& os);

    /**
     * Serializes the object into a PVM or MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Channel information. */
    //@{

    /**
     * Returns the gate this channel is attached to.
     */
    cGate *fromGate() const {return fromgatep;}

    /**
     * Returns the channel type of the channel, if it has one.
     */
    cChannelType *channelType() const {return channeltypep;}
    //@}

    /** @name Parameter list. */
    //@{
    /**
     *  Returns the total number of the channel's parameters.
     */
    virtual int params() const  {return const_cast<cChannel *>(this)->_parList().items();}

    /**
     * Adds a parameter to the channel's parameter list.
     */
    virtual cPar& addPar(const char *s)  {cPar *p=new cPar(s);_parList().add(p);return *p;}

    /**
     * Adds a parameter to the channel's parameter list.
     */
    virtual cPar& addPar(cPar *p)  {_parList().add(p); return *p;}

    /**
     * Returns the nth channel parameter.
     */
    virtual cPar& par(int n);

    /**
     * Returns the channel parameter with the given name.
     */
    virtual cPar& par(const char *s);

    /**
     * Returns the index of the parameter with the given name in the channel's
     * parameter list, or -1 if it could not be found.
     */
    virtual int findPar(const char *s) const;

    /**
     * Check if a parameter exists.
     */
    virtual bool hasPar(const char *s) const {return findPar(s)>=0;}

    /**
     * DEPRECATED. Manipulating a channel's parameters directly will not be 
     * compatible with future OMNeT++ releases.
     */
    virtual cArray& parList() _OPPDEPRECATED;
    //@}

    /** @name Internally used methods. */
    //@{

    /**
     * This function is called internally by the simulation kernel for transmission modelling.
     * A false return value means that the message object should be deleted by the caller;
     * this can be used to model that the message was lost in the channel.
     */
    virtual bool deliver(cMessage *msg, simtime_t at);
    //@}
};


/**
 * Channel that models delay, bit error rate and data rate.
 *
 * @ingroup SimCore
 */
class SIM_API cBasicChannel : public cChannel
{
  private:
    cPar *disabledp;    // if not NULL and value is nonzero, channel is down (will delete all packets). Points to an object in the parlist.
    cPar *delayp;       // propagation delay or NULL. Points to an object in the parlist.
    cPar *errorp;       // bit error rate or NULL. Points to an object in the parlist.
    cPar *dataratep;    // data rate or NULL. Points to an object in the parlist.
    simtime_t transm_finishes; // end of transmission; used if dataratep!=NULL

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    explicit cBasicChannel(const char *name=NULL);

    /**
     * Copy constructor.
     */
    cBasicChannel(const cBasicChannel& ch);

    /**
     * Destructor.
     */
    virtual ~cBasicChannel();

    /**
     * Assignment operator. Duplication and the assignment operator work all right with cBasicChannel.
     * The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cBasicChannel& operator=(const cBasicChannel& msg);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cBasicChannel(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(std::ostream& os);

    /**
     * Serializes the object into a PVM or MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
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
     * DEPRECATED. Use setDelay(double) instead.
     */
    virtual void setDelay(cPar *p) _OPPDEPRECATED;

    /**
     * DEPRECATED. Use setError(double) instead.
     */
    virtual void setError(cPar *p) _OPPDEPRECATED;

    /**
     * DEPRECATED. Use setDatarate(double) instead.
     */
    virtual void setDatarate(cPar *p) _OPPDEPRECATED;

    /**
     * Returns the delay of the channel.
     */
    virtual double delay() const     {return !delayp ? 0.0 : delayp->doubleValue();}

    /**
     * Returns the bit error rate of the channel.
     */
    virtual double error() const     {return !errorp ? 0.0 : errorp->doubleValue();}

    /**
     * Returns the data rate of the channel.
     */
    virtual double datarate() const  {return !dataratep ? 0.0 : dataratep->doubleValue();}

    /**
     * Returns the "disabled" parameter of the channel.
     */
    virtual bool disabled() const  {return !disabledp ? false : disabledp->boolValue();}
    //@}

    /** @name Redefined cChannel methods. */
    //@{

    /**
     * Redefined to specially handle "delay", "error" and "datarate".
     */
    virtual cPar& addPar(const char *s);

    /**
     * Redefined to specially handle "delay", "error" and "datarate".
     */
    virtual cPar& addPar(cPar *p);
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


