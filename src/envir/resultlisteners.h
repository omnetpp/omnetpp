//==========================================================================
//  RESULTLISTENERS.H - part of
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

#ifndef __ENVIR_RESULTLISTENERS_H
#define __ENVIR_RESULTLISTENERS_H

#include "envirdefs.h"

/**
 * Listener for recording a signal to an output vector
 */
class ENVIR_API VectorRecordingListener : public cIListener
{
    protected:
        void *handle;        // identifies output vector for the output vector manager
        simtime_t last_t;    // last timestamp written, needed to ensure increasing timestamp order (TODO move into manager)
    protected:
        /** Utility function, throws a "data type not supported" error. */
        virtual void unsupportedType(simsignal_t signalID, const char *dataType);
    public:
        VectorRecordingListener(const char *name); //TODO accept vector attributes somehow: unit, data type, interpolation mode, enum etc
        virtual ~VectorRecordingListener();
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s);
        virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
        virtual void finish(cComponent *component, simsignal_t signalID) {}
        virtual void listenerAdded(cComponent *component, simsignal_t signalID) {}
        virtual void listenerRemoved(cComponent *component, simsignal_t signalID) {}
};

#endif

