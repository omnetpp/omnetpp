//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "SinkExt.h"

Define_Module(SinkExt);


void SinkExt::initialize()
{
    Sink::initialize();
    numJobsReceived = 0;
}

void SinkExt::handleMessage(cMessage *msg)
{
    Sink::handleMessage(msg);

    char buf[80];
    sprintf(buf, "received jobs: %d", ++numJobsReceived);
    getDisplayString().setTagArg("t",0, buf);
}

