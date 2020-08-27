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

#include <vector>
#include <utility>
#include "simtime_t.h"
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
    SIMSIGNAL_INT, // intval_t
    SIMSIGNAL_UINT, // uintval_t
    SIMSIGNAL_DOUBLE,
    SIMSIGNAL_SIMTIME,
    SIMSIGNAL_STRING,
    SIMSIGNAL_OBJECT,
    SIMSIGNAL_LONG [[deprecated]] = SIMSIGNAL_INT,
    SIMSIGNAL_ULONG [[deprecated]] = SIMSIGNAL_UINT
};

/**
 * @brief Interface for listeners in a simulation model.
 *
 * Note that the destructor automatically unsubscribes the listener from all
 * signals it is subscribed to, so it is not necessary to do it manually.
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
    std::vector<std::pair<cComponent*,simsignal_t>> subscriptions;

  protected:
    /**
     * Unsubscribes from all components and signals this listener is
     * subscribed to.
     */
    virtual void unsubscribeAll();

  public:
    /**
     * Constructor.
     */
    cIListener() {}

    /**
     * Copy constructor.
     */
    cIListener(const cIListener& other) {}

    /**
     * The destructor automatically unsubscribes from all places
     * this listener is subscribed to.
     *
     * Note that due to the way C++ destructors work, unsubscribedFrom()
     * won't be called in the way one might expect it. See its documentation
     * for details.
     */
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
     * to make a temporary copy of the listener list each time, but that is
     * not desirable for performance reasons.)
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) = 0;

    /** Receive an emitted signed integer value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t i, cObject *details) = 0;

    /** Receive an emitted unsigned integer value. See receiveSignal(cComponent*,simsignal_t,bool,cObject*) for more info. */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, uintval_t i, cObject *details) = 0;

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
     * as a listener. (It won't be called from subscribe() if this listener
     * was already subscribed.)
     */
    virtual void subscribedTo(cComponent *component, simsignal_t signalID) {}

    /**
     * Called when this object was removed from the given component's listener
     * list for the given signal. (It won't be called from unsubscribe() if this
     * listener was already unsubscribed.)
     *
     * It is OK for the listener to delete itself in this method (<tt>delete
     * this</tt>). However, in that case, since this method is called more than once
     * if the listener is subscribed multiple times, one must be
     * careful to prevent double deletion.
     *
     * Note that due to the way C++ destructors work with regard to inheritance,
     * overrides of this method won't be called if the unsubscription takes
     * place from within the destructor of the super class (usually cIListener).
     * A workaround is to redefine the destructor in your listener class,
     * and already call unsubscribeAll() from there.
     */
    virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID) {}

    /**
     * Returns the number of listener lists that contain this listener.
     */
    virtual int getSubscribeCount() const  {return subscriptions.size();}
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

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, intval_t i, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, uintval_t i, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s, cObject *details) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

#ifdef _WIN32
    [[deprecated]] virtual void receiveSignal(cComponent *source, simsignal_t signalID, long i, cObject *details);
    [[deprecated]] virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long i, cObject *details);
#endif

};

}  // namespace omnetpp

#endif

