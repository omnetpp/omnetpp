//==========================================================================
//   CGATE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cGate       : module gate
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CGATE_H
#define __CGATE_H

#include "cobject.h"

//=== classes mentioned here:
class  cGate;
class  cModule;
class  cPar;
class  cMessage;
class  cChannelType;
class  cChannel;
class  cDisplayString;

//==========================================================================

/**
 * Represents a module gate. cGate object are created and managed by modules;
 * the user typically does not want to directly create or destroy cGate
 * objects. However, they are important if a simple module algorithm
 * needs to know about its surroundings.
 *
 * @ingroup SimCore
 */
class SIM_API cGate : public cObject
{
    friend class cModule;
  protected:
    mutable char *fullname; // buffer to store full name of object
    int  serno;         // index if gate vector, 0 otherwise
    int  vectsize;      // gate vector size (-1 if not vector, 0 if zero-sized gate vector)
    char typ;           // type of gate: 'I' or 'O'

    cChannel *channelp; // channel object (if exists)

    cModule *omodp;     // owner module
    int gateid;         // gate number within the module

    cGate *fromgatep;   // previous and next gate
    cGate *togatep;     //   in the route

    cDisplayString *dispstr; // the display string (created on demand)

  public:
    // internal: used from Tkenv: find out if cGate has a display string.
    // displayString() would create the object immediately which we want to avoid.
    bool hasDisplayString() {return dispstr!=NULL;}
    // internal
    void _setTo(cGate *g) {togatep = g;}
    // internal
    void _setFrom(cGate *g) {fromgatep = g;}
    
  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cGate(const cGate& gate);

    /**
     * Constructor.
     */
    explicit cGate(const char *name, char tp);

    /**
     * Destructor.
     */
    virtual ~cGate();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cGate& operator=(const cGate& gate);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPolymorphic *dup() const  {return new cGate(*this);}

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Returns the full name of the gate, which is name() plus the
     * index in square brackets (e.g. "out[4]"). Redefined to add the
     * index.
     */
    virtual const char *fullName() const;

    /**
     * Sets object's name. Redefined to update the stored fullName string.
     */
    virtual void setName(const char *s);

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual std::string fullPath() const;

    /**
     * The original fullPath() method is redefined to hide the
     * internal array (a cArray) used to store the gate objects.
     */
    virtual const char *fullPath(char *buffer, int bufsize) const;

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(std::ostream& os);
    //@}

    /**
     * This function is called internally by the send() functions and
     * channel classes' deliver() to deliver the message to its destination.
     * A false return value means that the message object should be deleted
     * by the caller. (This is used e.g. with parallel simulation, for
     * messages leaving the partition.)
     */
    virtual bool deliver(cMessage *msg, simtime_t at);

    /** @name Setting up the gate. */
    //@{

    /**
     * Specifies that the gate is owned by module m, and it
     * is at index g in the gate vector. This function should
     * not be directly called by the user.
     */
    void setOwnerModule(cModule *m, int gid);

    /**
     * Specifies that the gate is at index sn in a gate array
     * of size vs. This function should not be directly called
     * by the user.
     */
    void setIndex(int sn, int vs);
    //@}

    /** @name Connecting the gate. */
    //@{

    /**
     * Connects the gate to another gate, optionally using the given
     * channel object. This method can be used to manually create
     * connections for dynamically created modules. If no channel object
     * is specified (or NULL pointer is passed), the existing channel
     * object (assigned via setChannel()) is preserved.
     *
     * If the gate is already connected, an error will occur.
     * The g argument cannot be NULL, that is, you cannot use
     * this function to disconnect a gate. Use disconnect() instead.
     */
    void connectTo(cGate *g, cChannel *chan=NULL);

    /**
     * Disconnects the gate. It also destroys the associated channel object
     * if one has been set (see setChannel()). disconnect() must be invoked
     * on the source gate ("from" side) of the connection.
     *
     * The method has no effect if the gate is not connected.
     */
    void disconnect();

    /**
     * DEPRECATED - use connectTo() instead!
     *
     * Redirect gates. This method is mostly used internally during
     * network setup to create the connections.
     */
    void setFrom(cGate *g) _OPPDEPRECATED;

    /**
     * DEPRECATED - use connectTo() instead!
     *
     * Redirect gates. This method is mostly used internally during
     * network setup to create the connections.
     */
    void setTo(cGate *g) _OPPDEPRECATED;
    //@}

    /** @name Accessing the channel object. */
    //@{

    /**
     * Assigns a channel object to this gate. The channel object stores
     * connection attributes such as delay, bit error rate or data rate.
     *
     * See also connectTo().
     */
    void setChannel(cChannel *ch);

    /**
     * Returns the channel object attached to this gate, or NULL if there's no channel.
     */
    cChannel *channel() const {return channelp;}
    //@}

    /** @name Information about the gate. */
    //@{

    /**
     * Returns the gate's type: 'I' for input and 'O' for output.
     */
    char type() const      {return typ;}

    /**
     * Returns a pointer to the owner module of the gate.
     */
    cModule *ownerModule() const {return omodp;}

    /**
     * Returns gate ID, the position of the gate in the array of all gates of
     * the module.
     */
    int id() const         {return gateid;}

    /**
     * Returns true if the gate is part of a gate vector.
     */
    bool isVector() const  {return vectsize>=0;}

    /**
     * If the gate is part of a gate vector, returns the gate's index in the vector.
     * Otherwise, it returns 0.
     */
    int index() const      {return serno;}

    /**
     * If the gate is part of a gate vector, returns the size of the vector.
     * For non-vector gates it returns 1. Zero-size vectors are represented
     * by a single gate whose size() returns 0.
     *
     * The gate vector size can also be obtained by calling the cModule::gateSize().
     */
    int size()  const      {return vectsize<0?1:vectsize;}
    //@}

    /** @name Setting and getting link attributes. DEPRECATED methods. */
    //@{

    /**
     * DEPRECATED! Use cChannel/cBasicChannel and setChannel() instead.
     *
     * Sets the parameters of the link to those specified by the link type.
     */
    void setLink(cChannelType *l) _OPPDEPRECATED;

    /**
     * DEPRECATED! Use cBasicChannel and setChannel() instead.
     *
     * Creates a channel (of class cBasicChannel) if the gate does not have
     * one, and calls setDelay() on it. If the gate already has a channel, it must
     * be of class cBasicChannel or one subclassed from it.
     */
    void setDelay(cPar *p) _OPPDEPRECATED;

    /**
     * DEPRECATED! Use cBasicChannel and setChannel() instead.
     *
     * Creates a channel (of class cBasicChannel) if the gate does not have
     * one, and calls setError() on it. If the gate already has a channel, it must
     * be of class cBasicChannel or one subclassed from it.
     */
    void setError(cPar *p) _OPPDEPRECATED;

    /**
     * DEPRECATED! Use cBasicChannel and setChannel() instead.
     *
     * Creates a channel (of class cBasicChannel) if the gate does not have
     * one, and calls setDataRate() on it. If the gate already has a channel, it must
     * be of class cBasicChannel or one subclassed from it.
     */
    void setDataRate(cPar *p) _OPPDEPRECATED;

    /**
     * DEPRECATED! Use channel() and cBasicChannel::delay() instead.
     *
     * If the gate has a channel, calls delay() on it. The channel must
     * be of class cBasicChannel or one subclassed from it. If the gate
     * has no channel, the method returns NULL.
     */
    cPar *delay() const _OPPDEPRECATED;

    /**
     * DEPRECATED! Use channel() and cBasicChannel::error() instead.
     *
     * If the gate has a channel, calls error() on it. The channel must
     * be of class cBasicChannel or one subclassed from it. If the gate
     * has no channel, the method returns NULL.
     */
    cPar *error() const _OPPDEPRECATED;

    /**
     * DEPRECATED! Use channel() and cBasicChannel::datarate() instead.
     *
     * If the gate has a channel, calls datarate() on it. The channel must
     * be of class cBasicChannel or one subclassed from it. If the gate
     * has no channel, the method returns NULL.
     */
    cPar *datarate() const _OPPDEPRECATED;
    //@}

    /** @name Transmission state. */
    //@{

    /**
     * If the gate has a channel subclassed from cBasicChannel,
     * the methods calls isBusy() on it and returns the result.
     * Otherwise, it returns false.
     */
    bool isBusy() const;

    /**
     * If the gate has a channel subclassed from cBasicChannel,
     * the methods calls transmissionFinishes() on it and returns
     * the result. Otherwise, it returns 0.0.
     */
    simtime_t transmissionFinishes() const;
    //@}

    /** @name Gate connectivity. */
    //@{

    /**
     * Returns the previous gate in the series of connections (the route) that
     * contains this gate, or a NULL pointer if this gate is the first one in the route.
     * (E.g. for a simple module output gate, this function will return NULL.)
     */
    cGate *fromGate() const {return fromgatep;}

    /**
     * Returns the next gate in the series of connections (the route) that
     * contains this gate, or a NULL pointer if this gate is the last one in the route.
     * (E.g. for a simple module input gate, this function will return NULL.)
     */
    cGate *toGate() const   {return togatep;}

    /**
     * Return the ultimate source of the series of connections
     * (the route) that contains this gate.
     */
    cGate *sourceGate() const;

    /**
     * Return the ultimate destination of the series of connections
     * (the route) that contains this gate.
     */
    cGate *destinationGate() const;

    /**
     * Determines if a given module is in the route containing this gate.
     */
    int routeContains(cModule *m, int g=-1);

    /**
     * Returns true if the gate is connected outside (i.e. to one of its
     * sibling modules or to the parent module).
     *
     * This means that for an input gate, fromGate() must be non-NULL; for an output
     * gate, toGate() must be non-NULL.
     */
    bool isConnectedOutside() const;

    /**
     * Returns true if the gate (of a compound module) is connected inside
     * (i.e. to one of its submodules).
     *
     * This means that for an input gate, toGate() must be non-NULL; for an output
     * gate, fromGate() must be non-NULL.
     */
    bool isConnectedInside() const;

    /**
     * Returns true if the gate fully connected. For a compound module gate
     * this means both isConnectedInside() and isConnectedOutside() are true;
     * for a simple module, only isConnectedOutside() is checked.
     */
    bool isConnected() const;

    /**
     * Returns true if the route that this gate is in is complete, that is,
     * if it starts and arrives at a simple module.
     */
    bool isRouteOK() const;
    //@}

    /** @name Display string. */
    //@{
    /**
     * Returns the display string for the gate, which in practice affects the
     * apprearance of the connection for which this gate is the source.
     */
    cDisplayString& displayString();

    /**
     * DEPRECATED. Use displayString() and cDisplayString methods instead.
     */
    void setDisplayString(const char *dispstr, bool immediate=true);
    //@}
};

#endif

