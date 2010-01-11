//==========================================================================
//  CLISTENER.H - part of
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

#ifndef __CLISTENER_H
#define __CLISTENER_H

#include <vector>
#include <map>
#include "simtime_t.h"
#include "cobject.h"

NAMESPACE_BEGIN

class cComponent;

/**
 * Signal handle.
 *
 * @see cComponent::subscribe(), cComponent::unsubscribe(), cComponent::emit()
 * and cIListener
 */
typedef int simsignal_t;

#define SIMSIGNAL_NULL   ((simsignal_t)-1)

/**
 * Interface for listeners in a simulation model.
 * @see cComponent::subscribe(), cComponent::unsubscribe()
 */
class SIM_API cIListener
{
  public:
    virtual ~cIListener() {}

    /**
     * Receive an emitted long value. The "source" argument is the channel
     * or module on which the emit() method was invoked, and NOT the one at
     * which this listener is subscribed.
     *
     * It is not allowed inside a receiveSignal() call to unsubscribe from
     * the signal being received -- it will cause an error to be thrown.
     * (This is to prevent interesting cases when the listener list is being
     * modified while the component iterates on it. The alternative would be
     * to make a temporary copy of the listener list each time, but this is
     * not done for performance reasons.)
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) = 0;

    /** Receive an emitted double value. See receiveSignal(cComponent*,simsignal_t,long) for more info */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) = 0;

    /** Receive an emitted simtime_t value. See receiveSignal(cComponent*,simsignal_t,long) for more info */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t) = 0;

    /** Receive an emitted string value. See receiveSignal(cComponent*,simsignal_t,long) for more info */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *obj) = 0;

    /** Receive an emitted cObject value. See receiveSignal(cComponent*,simsignal_t,long) for more info */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) = 0;

    /**
     * Called after the component's finish() method got called. This method
     * can be used to record statistics. This method does not get called if
     * the simulation has terminated with an error. Also, the method may get
     * invoked several times (i.e. if the listener is subscribed to multiple
     * signals).
     */
    virtual void finish(cComponent *component) {}

    /**
     * Called when this object was added to the given component's given signal
     * as a listener. Note that this method will only be called from subscribe()
     * if this listener was not already subscribed.
     */
    virtual void listenerAdded(cComponent *component, simsignal_t signalID) {}

    /**
     * Called when this object was removed from the given component's listener
     * list for the given signal. Note that it will not be called from
     * unsubscribe() if this listener was not actually subscribed before.
     *
     * This method is also called from cComponent's destructor for all
     * listeners, so at this point it is not safe to cast the component pointer
     * to any other type. Also, the method may be called several times (i.e.
     * if the listener was subscribed to multiple signals).
     *
     * It is OK for the listener to delete itself in this method (<tt>delete
     * this</tt>). However, since this method may be called more than once
     * if the listener is subscribed multiple times (see above), one must be
     * careful to prevent double deletion, e.g. by reference counting.
     */
    virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {}
};

/**
 * A do-nothing implementation of cIListener, suitable as a base class
 * for other listeners. The user needs to redefine one or more of the
 * overloaded receiveSignal() methods; the rest will throw a "Data type
 * not supported" error.
 */
class SIM_API cListener : public cIListener
{
  protected:
    int subscribecount;
  protected:
    /** Utility function, throws a "data type not supported" error. */
    virtual void unsupportedType(simsignal_t signalID, const char *dataType);
  public:
    cListener();
    virtual ~cListener();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    virtual void finish(cComponent *component) {}
    virtual void listenerAdded(cComponent *component, simsignal_t signalID);
    virtual void listenerRemoved(cComponent *component, simsignal_t signalID);
};

NAMESPACE_END

#endif

