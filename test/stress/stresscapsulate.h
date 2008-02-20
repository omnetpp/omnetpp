//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSCAPSULATE_H
#define __STRESSCAPSULATE_H

#include <omnetpp.h>

class StressCapsulate : public cSimpleModule
{
    protected:
        void handleMessage(cMessage *msg);
};

#endif
