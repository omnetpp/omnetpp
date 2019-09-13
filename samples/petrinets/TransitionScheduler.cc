//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <algorithm>
#include "TransitionScheduler.h"

Define_Module(TransitionScheduler);

void TransitionScheduler::registerTransition(ITransition *t)
{
    transitions.push_back(t);
}

void TransitionScheduler::deregisterTransition(ITransition *t)
{
    std::vector<ITransition*>::iterator it = std::find(transitions.begin(), transitions.end(), t);
    ASSERT(it != transitions.end());
    transitions.erase(it);
}

void TransitionScheduler::initialize(int stage)
{
    if (stage == 2)
        scheduleNextFiring();
}

bool TransitionScheduler::scheduleNextFiring()
{
    Enter_Method_Silent("scheduleNextFiring");

    // choose one randomly among the ones that can fire
    std::vector<ITransition*> armedList;
    for (auto transition : transitions)
        if (transition->canFire())
            armedList.push_back(transition);

    if (armedList.empty()) {
        EV << "No transition can fire\n";
        return false;
    }

    int k = intrand(armedList.size());
    EV << "Selected transition '" << check_and_cast<cModule*>(armedList[k])->getFullName() << "'\n";
    armedList[k]->triggerFiring();
    return true;
}
