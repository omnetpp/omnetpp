//==========================================================================
//   CGATE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cGate       : module gate
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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
class  cLinkType;
class  cChannel;

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
    int  vectsize;      // gate vector size (-1 if not vector)
    char typ;           // type of gate: 'I' or 'O'

    cChannel *channelp; // channel object (if exists)

    cModule *omodp;     // owner module
    int gateid;         // gate number within the module

    cGate *fromgatep;   // previous and next gate
    cGate *togatep;     //   in the route

    opp_string dispstr; // the display string

    void (*notify_inspector)(cGate*,bool,void*); // to notify inspector about display string changes
    void *data_for_inspector;

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
     * Returns pointer to a string containing the class name, "cGate".
     */
    virtual const char *className() const {return "cGate";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cGate(*this);}

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cGateIFC";}

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the full name of the gate, which is name() plus the
     * index in square brackets (e.g. "out[4]"). Redefined to add the
     * index.
     */
    virtual const char *fullName() const;

    /**
     * Redefined. (Reason: a C++ rule that overloaded virtual methods must be redefined together.)
     */
    virtual const char *fullPath() const;

    /**
     * The original fullPath() method is redefined to hide the
     * internal array (a cArray) used to store the gate objects.
     */
    virtual const char *fullPath(char *buffer, int bufsize) const;

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);
    //@}

    /**
     * This function is called internally by the send() functions
     * to deliver the message to its destination.
     */
    // FIXME: why public?
    void deliver(cMessage *msg, simtime_t at);

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
     * Connect the a gate to another gate, optionally using the given
     * channel object. This methods can e.g. be used to manually create
     * connections for dynamically created modules.
     */
    void connectTo(cGate *g, cChannel *chan=NULL);

    /**
     * DEPRECATED - use connectTo() instead!
     *
     * Redirect gates. This method is mostly used internally during
     * network setup to create the connections.
     */
    void setFrom(cGate *g);

    /**
     * DEPRECATED - use connectTo() instead!
     *
     * Redirect gates. This method is mostly used internally during
     * network setup to create the connections.
     */
    void setTo(cGate *g);
    //@}

    /** @name Accessing the channel object. */
    //@{

    /**
     * Assigns the given channel to this gate.
     */
    void cGate::setChannel(cChannel *ch);

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
     * Otherwise, it returns 1.
     */
    int size()  const      {return vectsize<0?1:vectsize;}
    //@}

    /** @name Setting and getting link attributes. DEPRECATED methods. */
    //@{

    /**
     * DEPRECATED! Use cChannel and setChannel() instead.
     *
     * Sets the parameters of the link to those specified by the link
     * type.
     */
    void setLink(cLinkType *l);

    /**
     * DEPRECATED! Use cChannel and setChannel() instead.
     *
     * Creates a channel (of class cSimpleChannel) if the gate does not have
     * one, and calls setDelay() on it. If the gate already has a channel, it must
     * be of class cSimpleChannel or one subclassed from it.
     */
    void setDelay(cPar *p);

    /**
     * DEPRECATED! Use cChannel and setChannel() instead.
     *
     * Creates a channel (of class cSimpleChannel) if the gate does not have
     * one, and calls setError() on it. If the gate already has a channel, it must
     * be of class cSimpleChannel or one subclassed from it.
     */
    void setError(cPar *p);

    /**
     * DEPRECATED! Use cChannel and setChannel() instead.
     *
     * Creates a channel (of class cSimpleChannel) if the gate does not have
     * one, and calls setDataRate() on it. If the gate already has a channel, it must
     * be of class cSimpleChannel or one subclassed from it.
     */
    void setDataRate(cPar *p);

    /**
     * DEPRECATED! Use cChannel and channel() instead.
     *
     * Returns the link type of the gate, if it has one.
     */
    cLinkType *link() const;

    /**
     * DEPRECATED! Use cChannel and channel() instead.
     *
     * If the gate has a channel, calls delay() on it. The channel must
     * be of class cSimpleChannel or one subclassed from it. If the gate
     * has no channel, the method returns NULL.
     */
    cPar *delay() const;

    /**
     * DEPRECATED! Use cChannel and channel() instead.
     *
     * If the gate has a channel, calls error() on it. The channel must
     * be of class cSimpleChannel or one subclassed from it. If the gate
     * has no channel, the method returns NULL.
     */
    cPar *error() const;

    /**
     * DEPRECATED! Use cChannel and channel() instead.
     *
     * If the gate has a channel, calls datarate() on it. The channel must
     * be of class cSimpleChannel or one subclassed from it. If the gate
     * has no channel, the method returns NULL.
     */
    cPar *datarate() const;
    //@}

    /** @name Transmission state. DEPRECATED methods. */
    //@{

    /**
     * DEPRECATED! Use channel() and cChannel methods instead.
     *
     * If the gate has a channel, calls isBusy() on it. The channel must
     * be of class cSimpleChannel or one subclassed from it. If the gate
     * has no channel, the method returns false.
     */
    bool isBusy() const;

    /**
     * DEPRECATED! Use channel() and cChannel methods instead.
     *
     * If the gate has a channel, calls transmissionFinishes() on it. The channel
     * must be of class cSimpleChannel or one subclassed from it. If the gate has
     * no channel, the method returns the current simulation time.
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
     * Return the ultimate source and destination of the series of connections
     * (the route) that contains this gate.
     */
    cGate *sourceGate() const;

    /**
     * Return the ultimate source and destination of the series of connections
     * (the route) that contains this gate.
     */
    cGate *destinationGate() const;

    /**
     * Determines if a given module is in the route containing this gate.
     */
    int routeContains(cModule *m, int g=-1);

    /**
     * Returns true if the gate is connected. A simple module output gate is
     * connected if toGate() is not NULL, and a simple module input gate is
     * connected if fromGate() is not NULL. For a compound module gate to be
     * connected, it should be connected both from "inside" and "outside",
     * that is, both fromGate() and toGate() should be non-NULL.
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
     * Sets the display string for the gate, which in practice affects the
     * appearance of the connection for which this gate is the source.
     *
     * The immediate flag selects whether the change should become effective
     * right now or later (after finishing the current event).
     *
     * If several display string changes are going to be done within one event,
     * then immediate=false is useful because it reduces the number of necessary
     * redraws. Immediate=false also uses less stack. But its drawback is that
     * a setDisplayString() followed by a send() would actually be displayed
     * in reverse order (message animation first), because message animations
     * are always performed immediately (actually within the send() call).
     */
    void setDisplayString(const char *dispstr, bool immediate=true);

    /**
     * Returns the display string for the gate, which in practice affects the
     * apprearance of the connection for which this gate is the source.
     */
    const char *displayString() const;

    /**
     * Sets up a notification function which is called every time the display
     * string changes.
     */
    void setDisplayStringNotify(void (*notify_func)(cGate*,bool,void*), void *data);
    //@}
};

#endif

