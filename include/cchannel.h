//==========================================================================
//   CCHANNEL.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cChannel : channel class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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
class cLinkType;
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
    cLinkType *linkp;   // link prototype or NULL
    cArray *parlistp;   // ptr to list of all parameters
    cGate *fromgatep;   // gate the channel is attached to
    // FIXME: add on/off modelling? or to cGate?

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
    explicit cChannel(const char *name=NULL, cLinkType *l=NULL);

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
    virtual cObject *dup() const  {return new cChannel(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cChannelIFC";}

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach( ForeachFunc do_fn );

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Serializes the object into a PVM or MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Channel information. */
    //@{

    /**
     * Returns the gate this channel is attached to.
     */
    cGate *fromGate() const {return fromgatep;}

    /**
     * Returns the link type of the gate, if it has one.
     */
    cLinkType *link() const {return linkp;}
    //@}

    /** @name Parameter list. FIXME needs remove, etc. operators too! */
    //@{

    /**
     * Add a parameter to the channel's parameter list.
     */
    virtual cPar& addPar(const char *s)  {cPar *p=new cPar(s);_parList().add(p);return *p;}

    /**
     * Add a parameter to the channel's parameter list.
     */
    virtual cPar& addPar(cPar *p)  {_parList().add(p); return *p;}

    /**
     * Returns the nth object in the channel's parameter list,
     * converting it to a cPar.
     */
    virtual cPar& par(int n);

    /**
     * Returns the object with the given name in the channel's parameter list,
     * converting it to a cPar.
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
     * Returns the cArray member of the message which holds
     * the parameters and other attached objects. Parameters can be inserted,
     * retrieved, looked up or deleted through cArray's member
     * functions.
     */
    virtual cArray& parList();
    //@}

    /** @name Internally used methods. */
    //@{

    /**
     * This function is called internally by the simulation kernel for transmission
     * modelling.
     */
    virtual void deliver(cMessage *msg, simtime_t at);
    //@}
};


/**
 * Channel that models delay, bit error rate and data rate.
 *
 * @ingroup SimCore
 */
class SIM_API cSimpleChannel : public cChannel
{
  private:
    cPar *disabledp;    // FIXME implement! if not NULL and value is nonzero, channel is down (will delete all packets). Points to an object in the parlist.
    cPar *delayp;       // propagation delay or NULL. Points to an object in the parlist.
    cPar *errorp;       // bit error rate or NULL. Points to an object in the parlist.
    cPar *dataratep;    // data rate or NULL. Points to an object in the parlist.
    simtime_t transm_finishes; // end of transmission; used if dataratep!=NULL

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cSimpleChannel(const cSimpleChannel& ch);

    /**
     * Constructor.
     */
    explicit cSimpleChannel(const char *name=NULL, cLinkType *l=NULL);

    /**
     * Destructor.
     */
    virtual ~cSimpleChannel();

    /**
     * Assignment operator. Duplication and the assignment operator work all right with cSimpleChannel.
     * The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cSimpleChannel& operator=(const cSimpleChannel& msg);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cSimpleChannel(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cSimpleChannelIFC";}

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach( ForeachFunc do_fn );

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);

    /**
     * Serializes the object into a PVM or MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Setting and getting link attributes. */
    //@{

    /**
     * Set the parameters of the link. Ownership of cPar objects
     * are handled according to the ownership flag (that is set by takeOwnership()).
     */
    virtual void setDelay(cPar *p);

    /**
     * Set the parameters of the link. Ownership of cPar objects
     * are handled according to the ownership flag (that is set by takeOwnership()).
     */
    virtual void setError(cPar *p);

    /**
     * Set the parameters of the link. Ownership of cPar objects
     * are handled according to the ownership flag (that is set by takeOwnership()).
     */
    virtual void setDatarate(cPar *p);

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    virtual cPar *delay() const     {return delayp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    virtual cPar *error() const     {return errorp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    virtual cPar *datarate() const  {return dataratep;}
    //@}

    /** @name Redefined cChannel methods. */
    //@{

    /**
     * Redefined to specially handle "disabled", "delay", "error" and "datarate".
     */
    virtual cPar& addPar(const char *s);

    /**
     * Redefined to specially handle "disabled", "delay", "error" and "datarate".
     */
    virtual cPar& addPar(cPar *p);
    //@}

    /** @name Transmission state. */
    //@{

    /**
     * Returns whether the gate is currently transmitting.
     */
    virtual bool isBusy() const;

    /**
     * Returns the simulation time the gate is expected to finish transmitting.
     * Note that additional messages send on the gate may prolong the time the gate
     * will actually finish.
     */
    virtual simtime_t transmissionFinishes() const {return transm_finishes;}
    //@}

    /** @name Internally used methods. */
    //@{

    /**
     * Performs bit error rate, delay and transmission time modelling.
     */
    virtual void deliver(cMessage *msg, simtime_t at);
    //@}
};

#endif


