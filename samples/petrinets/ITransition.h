//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __ITRANSITION_H
#define __ITRANSITION_H

#include <omnetpp.h>

using namespace omnetpp;

class IPlace;

/**
 * The following interface must be implemented by a Petri Net transition.
 */
class ITransition
{
    public:
        virtual ~ITransition() {}

        /**
         * Return true if the transition can fire (is armed).
         */
        virtual bool canFire() = 0;

        /**
         * Fire, or schedule immediate (zero-delay) firing of the transition.
         */
        virtual void triggerFiring() = 0;
};

#endif

