//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __QUEUEING_CLASSIFIER_H
#define __QUEUEING_CLASSIFIER_H

#include "QueueingDefs.h"

namespace queueing {

/**
 * See the NED declaration for more info.
 */
class QUEUEING_API Classifier : public cSimpleModule
{
    private:
        const char *dispatchField;   // the message's field or parameter we are dispatching on
    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};

}; //namespace

#endif
