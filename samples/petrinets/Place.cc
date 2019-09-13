//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Place.h"

Define_Module(Place);

simsignal_t Place::numTokensSignal = cComponent::registerSignal("numTokens");

void Place::initialize()
{
    numTokens = par("numInitialTokens");
    EV << "Setting up with " << numTokens << " tokens.\n";
    WATCH(numTokens);
}

void Place::handleMessage(cMessage *msg)
{
    error("this module does not process messages");
}

void Place::refreshDisplay() const
{
    getDisplayString().setTagArg("t", 0, numTokens);

    static char buf[] = "placeX";
    const char *icon = numTokens <= 4 ? (buf[5]='0'+numTokens, buf) : "placemany";
    getDisplayString().setTagArg("i", 0, icon);
}

int Place::getNumTokens()
{
    return numTokens;
}

void Place::addTokens(int n)
{
    Enter_Method_Silent();
    ASSERT(n>0);
    numTokens += n;
    numTokensChanged();
}

void Place::removeTokens(int n)
{
    Enter_Method_Silent();
    ASSERT(n>0);
    ASSERT(numTokens>=n);
    numTokens -= n;
    numTokensChanged();
}

void Place::numTokensChanged()
{
    emit(numTokensSignal, numTokens);
}

ITransition *Place::getOutputTransition(int i)
{
    cGate *g = gate("out", i);
    if (g->getNextGate() == nullptr)
        return nullptr; // not connected
    return check_and_cast<ITransition *>(g->getPathEndGate()->getOwnerModule());
}
