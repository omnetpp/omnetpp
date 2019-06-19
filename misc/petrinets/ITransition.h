//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
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
         * A transition gets notified when number of tokens changes on its input places
         */
        virtual void numTokensChanged(IPlace *inputPlace) = 0;

        virtual bool canFire() = 0;

        virtual void scheduleFiring() = 0;

};

#endif

