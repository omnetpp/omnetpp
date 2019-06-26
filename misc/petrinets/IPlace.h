//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __IPLACE_H
#define __IPLACE_H

#include <omnetpp.h>

using namespace omnetpp;

/**
 * The following interface must be implemented by a Petri Net place.
 */
class IPlace
{
    public:
        virtual ~IPlace() {}

        /**
         * Number of tokens at the queue
         */
        virtual int getNumTokens() = 0;

        /**
         * Add n tokens to this place.
         */
        virtual void addTokens(int n) = 0;

        /**
         * Take n tokens to this place. An error is thrown if the place
         * has less than n tokens.
         */
        virtual void removeTokens(int n) = 0;
};

#endif

