//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __PLACE_H__
#define __PLACE_H__

#include <omnetpp.h>
#include "IPlace.h"
#include "ITransition.h"

using namespace omnetpp;

/**
 * Implements a Petri Net place
 */
class Place : public cSimpleModule, public IPlace
{
  protected:
    int numTokens;
    static simsignal_t numTokensSignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    // helpers
    virtual void numTokensChanged();
    virtual ITransition *getOutputTransition(int i);

  public:
    // IPlace methods:
    virtual int getNumTokens() override;
    virtual void addTokens(int n) override;
    virtual void removeTokens(int n) override;
};

#endif
