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

    /**
     * MISSINGDOC: cGate:void(*)(void*)
     */
    void (*notify_inspector)(void*); // to notify inspector about disp str changes
    void *data_for_inspector;

  public:

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

    // redefined functions

    /**
     * Returns pointer to the class name string, "cGate".
     */
    virtual const char *className() const {return "cGate";}

    /**
     * MISSINGDOC: cGate:cObject*dup()
     */
    virtual cObject *dup()  {return new cGate(*this);}

    /**
     * MISSINGDOC: cGate:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cGateIFC";}

    /**
     * MISSINGDOC: cGate:void forEach(ForeachFunc)
     */
    virtual void forEach(ForeachFunc f);

    /**
     * MISSINGDOC: cGate:void info(char*)
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cGate:cGate&operator=(cGate&)
     */
    cGate& operator=(cGate& gate);

    /**
     * MISSINGDOC: cGate:char*fullName()
     */
    virtual const char *fullName() const;

    /**
     * MISSINGDOC: cGate:char*fullPath()
     */
    virtual const char *fullPath() const;

    /**
     * MISSINGDOC: cGate:void writeContents(ostream&)
     */
    virtual void writeContents(ostream& os);

    // new functions

    /**
     * Specifies that the gate is at index sn in a gate array
     * of size vs. This function should not be directly called
     * by the user.
     */
    void setIndex(int sn, int vs);

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
     * Redirect gates. This function will rarely be needed; unless maybe
     * for dynamically created modules.
     */
    void setFrom(cGate *g);

    /**
     * Redirect gates. This function will rarely be needed; unless maybe
     * for dynamically created modules.
     */
    void setTo(cGate *g);

    /**
     * Specifies that the gate is owned by module m, and it
     * is at index g in the gate vector. This function should
     * not be directly called by the user.
     */
    void setOwnerModule(cModule *m, int gid);


    /**
     * MISSINGDOC: cGate:bool isVector()
     */
    bool isVector() const  {return vectsize>=0;}

    /**
     * If the gate is in a gate array, returns the gate's position in
     * it; otherwise, it returns 0.
     */
    int index() const      {return serno;}

    /**
     * If the gate is in a gate array, returns the size of the vector;
     * otherwise, it returns 1.
     */
    int size()  const      {return vectsize<0?1:vectsize;}

    /**
     * Returns the gate's type: 'I' for input and 'O'
     * for output.
     */
    char type() const      {return typ;}

    /**
     * Returns a pointer to the owner module of the gate.
     */
    cModule *ownerModule() const {return omodp;}

    /**
     * Returns the position of the gate in the vector of all gates of
     * the module.
     */
    int id() const         {return gateid;}

    /**
     * Returns the link type of the gate, if it has one.
     */
    cLinkType *link()      {return linkp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    cPar *delay()          {return delayp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    cPar *error()          {return errorp;}

    /**
     * Return pointers to the delay, bit error rate and datarate parameters
     * of the link. Links are one-directional; these parameters are only
     * stored at their starting side.
     */
    cPar *datarate()       {return dataratep;}


    /**
     * For a compound module gate, it returns the previous and the next
     * gate in the series of connections (the route) that contains this
     * gate. For simple module gates, only one of the functions will
     * return non-NULL value.
     */
    cGate *fromGate() const {return fromgatep;}

    /**
     * For a compound module gate, it returns the previous and the next
     * gate in the series of connections (the route) that contains this
     * gate. For simple module gates, only one of the functions will
     * return non-NULL value.
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
     * MISSINGDOC: cGate:bool isBusy()
     */
    bool isBusy();

    /**
     * MISSINGDOC: cGate:simtime_t transmissionFinishes()
     */
    simtime_t transmissionFinishes() {return transm_finishes;}


    /**
     * This function is called internally by the send() functions
     * to deliver the message to its destination.
     */
    void deliver(cMessage *msg);   // called by send() functions


    /**
     * Determines if a given module is in the route that this gate is
     * in.
     */
    int routeContains(cModule *m, int g=-1);

    /**
     * Returns true if the gate is connected.
     */
    bool isConnected() const;

    /**
     * Returns true if the route that this gate is in is complete;
     * i.e., if it starts and arrives at a simple module.
     */
    bool isRouteOK();

    // visualization/animation support

    /**
     * FIXME: visualization/animation support
     */
    void setDisplayString(const char *dispstr);

    /**
     * MISSINGDOC: cGate:char*displayString()
     */
    const char *displayString();

    /**
     * MISSINGDOC: cGate:void setDisplayStringNotify(void(*)(void*),void*)
     */
    void setDisplayStringNotify(void (*notify_func)(void*), void *data);
};

#endif
