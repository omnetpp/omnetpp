//==========================================================================
//  CRESULTLISTENER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CRESULTLISTENER_H
#define __OMNETPP_CRESULTLISTENER_H

#include "simkerneldefs.h"
#include "clistener.h"

NAMESPACE_BEGIN

class cResultFilter;
class cComponent;


/**
 * Common abstract base class for result filters and result recorders.
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cResultListener : public cIListener
{
    public:
        virtual const char *getClassName() const {return opp_typename(typeid(*this));}
        virtual std::string str() const {return opp_typename(typeid(*this));}
        static const char *getPooled(const char *s);

        // simplified API that better supports chaining:
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b) = 0;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, long l) = 0;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l) = 0;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, double d) = 0;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v) = 0;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s) = 0;
        virtual void receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj) = 0;
        virtual void subscribedTo(cResultFilter *prev);
        virtual void unsubscribedFrom(cResultFilter *prev);
        virtual void callFinish(cResultFilter *prev) {finish(prev);}
        virtual void finish(cResultFilter *prev) {}

        // original listener API delegates to simplified API:
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b) override;
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) override;
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l) override;
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) override;
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& v) override;
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s) override;
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override;
        virtual void subscribedTo(cComponent *component, simsignal_t signalID) override;
        virtual void unsubscribedFrom(cComponent *component, simsignal_t signalID) override;
        virtual void finish(cComponent *component, simsignal_t signalID) override;
};

NAMESPACE_END

#endif


