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
    int  serno;         // index if gate vector, 0 otherwise
    int  vectsize;      // gate vector size (-1 if not vector)
    char typ;           // type of gate: 'I' or 'O'

    cLinkType *linkp;   // link prototype or NULL
    cPar *delayp;       // propagation delay or NULL
    cPar *errorp;       // bit error rate or NULL
    cPar *dataratep;    // data rate or NULL

    cModule *omodp;     // owner module
    int gateid;         // gate number within the module

    cGate *fromgatep;   // previous and next gate
    cGate *togatep;     //   in the route

    simtime_t transm_finishes; // end of transmission; used if dataratep!=NULL

    opp_string dispstr;      // the display string

    void (*notify_inspector)(void*); // to notify inspector about disp str changes
    void *data_for_inspector;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor.
     */
    cGate(cGate& gate);

    /**
     * Constructor.
     */
    explicit cGate(const char *name, char tp);

    /**
     * Destructor.
     */
    virtual ~cGate() {}

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cGate& operator=(cGate& gate);
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
    virtual cObject *dup()  {return new cGate(*this);}

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
     * MISSINGDOC: cGate:char*fullName()
     */
    virtual const char *fullName() const;

    /**
     * MISSINGDOC: cGate:char*fullPath()
     */
    virtual const char *fullPath() const;

    /**
     * Writes textual information about this object to the stream.
     * See cObject for more details.
     */
    virtual void writeContents(ostream& os);
    //@}

    /**
     * FIXME: This function is called internally by the send() functions
     * to deliver the message to its destination. Why's this public???
     */
    void deliver(cMessage *msg);

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
     * Redirect gates. This function will rarely be needed; unless maybe
     * for dynamically created modules.
     */
    void setFrom(cGate *g);

    /**
     * Redirect gates. This function will rarely be needed; unless maybe
     * for dynamically created modules.
     */
    void setTo(cGate *g);
    //@}

    /** @name Setting and getting link attributes. */
    //@{

    /**
     * Sets the parameters of the link to those specified by the link
     * type.
     */
    void setLink(cLinkType *l);

    /**
     * Set the parameters of the link. Ownership of cPar objects
     * are handled according to the ownership flag (that is set by takeOwnership()).
     */
    void setDelay(cPar *p);

    /**
     * Set the parameters of the link. Ownership of cPar objects
     * are handled according to the ownership flag (that is set by takeOwnership()).
     */
    void setError(cPar *p);

    /**
     * Set the parameters of the link. Ownership of cPar objects
     * are handled according to the ownership flag (that is set by takeOwnership()).
     */
    void setDataRate(cPar *p);

    /**
     * Returns the link type of the gate, if it has one.
     */
    cLinkType *link() _CONST {return linkp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    cPar *delay() _CONST     {return delayp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    cPar *error() _CONST     {return errorp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    cPar *datarate() _CONST  {return dataratep;}
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

    /** @name Transmission state. */
    //@{

    /**
     * Returns whether the gate is currently transmitting.
     */
    bool isBusy();

    /**
     * Returns the simulation time the gate is expected to finish transmitting.
     * Note that additonal messages send on the gate may prolong the time the gate
     * will actually finish.
     */
    simtime_t transmissionFinishes() _CONST {return transm_finishes;}
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
    cGate *sourceGate();

    /**
     * Return the ultimate source and destination of the series of connections
     * (the route) that contains this gate.
     */
    cGate *destinationGate();

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
    bool isRouteOK();
    //@}

    /** @name Display string. */
    //@{

    /**
     * Sets the display string for the gate.
     */
    void setDisplayString(const char *dispstr);

    /**
     * Returns the display string for the gate.
     */
    const char *displayString();

    /**
     * Sets up a notification function which is called every time the display
     * string changes.
     */
    void setDisplayStringNotify(void (*notify_func)(void*), void *data);
    //@}
};

#endif

