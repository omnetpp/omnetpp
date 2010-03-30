//==========================================================================
//  RESULTLISTENER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIR_RESULTLISTENER_H
#define __ENVIR_RESULTLISTENER_H

#include "envirdefs.h"
#include "clistener.h"

class ResultFilter;
class cComponent;


/**
 * Common abstract base class for result filters and result recorders.
 */
class ENVIR_API ResultListener : public cIListener
{
    public:
        virtual const char *getClassName() const {return opp_typename(typeid(*this));}
        virtual std::string str() const {return opp_typename(typeid(*this));}

        // simplified API that better supports chaining:
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, long l) = 0;
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l) = 0;
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, double d) = 0;
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v) = 0;
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s) = 0;
        virtual void receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj) = 0;
        virtual void subscribedTo(ResultFilter *prev);
        virtual void unsubscribedFrom(ResultFilter *prev);
        virtual void finish(ResultFilter *prev) {}

        // original listener API delegates to simplified API:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
        virtual void subscribedTo(cComponent *component, simsignal_t signalID);
        virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID);
        virtual void finish(cComponent *component, simsignal_t signalID);
};

/**
 * Data structure used when setting up chains of ResultFilters and ResultRecorders
 */
class SignalSource
{
  private:
    ResultFilter *filter;
    cComponent *component;
    simsignal_t signalID;
  public:
    SignalSource(ResultFilter *prevFilter) {filter=prevFilter; component=NULL; signalID=-1;}
    SignalSource(cComponent *comp, simsignal_t sigID) {filter=NULL; component=comp; signalID=sigID;}
    bool isNull() const {return !filter && !component;}
    void subscribe(ResultListener *listener) const;
};

#endif


