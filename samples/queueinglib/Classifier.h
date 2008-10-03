//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __CLASSIFIER_H
#define __CLASSIFIER_H

#include <omnetpp.h>

namespace queueing {

/**
 * See the NED declaration for more info.
 */
class Classifier : public cSimpleModule
{
    private:
        const char *dispatchField;   // the message's field or parameter we are dispatching on
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

}; //namespace

#endif
