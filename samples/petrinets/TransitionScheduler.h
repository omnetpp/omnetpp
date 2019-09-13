//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __TRANSITIONSCHEDULER_H__
#define __TRANSITIONSCHEDULER_H__

#include <omnetpp.h>
#include "ITransition.h"

using namespace omnetpp;

class TransitionScheduler : public cSimpleModule
{
  protected:
    std::vector<ITransition*> transitions;
  public:
    virtual int numInitStages() const override {return 3;}
    virtual void initialize(int stage) override;
    void registerTransition(ITransition *t);
    void deregisterTransition(ITransition *t);
    bool scheduleNextFiring();
};

#endif


