//==========================================================================
//  CLISTENER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CLISTENER_H
#define __OMNETPP_CLISTENER_H

#include "simtime_t.h"
#include "cobject.h"

namespace omnetpp {

class cComponent;

/**
 * @brief Signal handle.
 *
 * @see cComponent::subscribe(), cComponent::unsubscribe(), cComponent::emit()
 * and cIListener
 *
 * @ingroup Signals
 */
typedef int simsignal_t;

#define SIMSIGNAL_NULL   ((omnetpp::simsignal_t)-1)

/**
 * @brief Signal data types.
 *
 * @see simsignal_t, cIListener
 * @ingroup Signals
 */
enum SimsignalType
{
    SIMSIGNAL_UNDEF,
    SIMSIGNAL_BOOL,
    SIMSIGNAL_LONG,
    SIMSIGNAL_ULONG,
    SIMSIGNAL_DOUBLE,
    SIMSIGNAL_SIMTIME,
    SIMSIGNAL_STRING,
    SIMSIGNAL_OBJECT
};

/**
 * @brief Interface for listeners in a simulation model.
 *
 * This class performs subscription counting, in order to make sure that
 * when the destructor runs, the object is no longer subscribed anywhere.
 *
 * @see cComponent::subscribe(), cComponent::unsubscribe()
 *
 * @ingroup Signals
 */
class SIM_API cIListener
{
    friend class cComponent; // for subscribecount
    friend class cResultFilter; // for subscribecount
  private:
    int subscribeCount;
  public:
    cIListener();
    virtual ~cIListener();

    /**
     * Receive an emitted signal. Note that cIListener declares several
     * overloaded receiveSignal() methods, one for each data type (except
     * that all integer types are merged to the long and unsigned long
     * types). The "source" argument is the channel or module on which
     * the emit() method was invoked, and NOT the one at which this listener
     * is subscribed.
     *
     * It is not allowed inside a receiveSignal() call to unsubscribe from
     * the signal being received -- it will cause an error to be thrown.
     * (This is to prevent interesting cases when the listener list is being
     * modified while the component iterates on it. The alternative would be
     * to make a temporary copy of the listener list each time, but this is
     * not desirable for performance reasons.)
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) = 0;

    /** Receive an emitted long value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l, cObject *details) = 0;

    /** Receive an emitted unsigned long value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l, cObject *details) = 0;

    /** Receive an emitted double value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) = 0;

    /** Receive an emitted simtime_t value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) = 0;

    /** Receive an emitted string value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) = 0;

    /** Receive an emitted cObject value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) = 0;

    /**
     * Called by a component on its local listeners after the component's
     * finish() method was called. If the listener is subscribed to multiple
     * signals or at multiple components, the method will be called multiple times.
     * Note that finish() methods in general are not invoked if the simulation
     * terminates with an error.
     */
    virtual void finish(cComponent *component, simsignal_t signalID) {}

    /**
     * Called when this object was added to the given component's given signal
     * as a listener. Note that this method will only be called from subscribe()
     * if this listener was not already subscribed.
     */
    virtual void subscribedTo(cComponent *component, simsignal_t signalID) {}

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
    virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID) {}

    /**
     * Returns the number of listener lists that contain this listener.
     */
    int getSubscribeCount() const  { return subscribeCount; }
};

/**
 * @brief A do-nothing implementation of cIListener, suitable as a base class
 * for other listeners.
 *
 * The user needs to redefine one or more of the overloaded receiveSignal()
 * methods; the rest will throw a "Data type not supported" error.
 *
 * @ingroup Signals
 */
class SIM_API cListener : public cIListener
{
  protected:
    /** Utility function, throws a "data type not supported" error. */
    virtual void unsupportedType(simsignal_t signalID, const char *dataType);

#ifdef WITH_OMNETPP4x_LISTENER_SUPPORT
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
#endif

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
};

}  // namespace omnetpp

#endif

