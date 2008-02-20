//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __STRESSSINK_H
#define __STRESSSINK_H

#include <omnetpp.h>

class StressSink : public cSimpleModule
{
    protected:
        void handleMessage(cMessage *msg);
};

#endif
