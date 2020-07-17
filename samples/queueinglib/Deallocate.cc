//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Deallocate.h"

namespace queueing {

Define_Module(Deallocate);

void Deallocate::initialize()
{
    resourceAmount = par("resourceAmount");

    const char *resourceName = par("resourceModuleName");
    cModule *mod = getParentModule()->getModuleByPath(resourceName);
    resourcePool = check_and_cast<IResourcePool *>(mod);
}

void Deallocate::handleMessage(cMessage *msg)
{
    resourcePool->release(resourceAmount);
    send(msg, "out");
}

}; // namespace
