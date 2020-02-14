//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <algorithm>
#include <omnetpp.h>
#include "Transition.h"
#include "TransitionScheduler.h"

using namespace omnetpp;

/**
 * Implements an arc.
 */
class Arc : public cIdealChannel
{
  protected:
    virtual void refreshDisplay() const override;
};

Define_Channel(Arc);

void Arc::refreshDisplay() const
{
    int multiplicity = par("multiplicity");

    bool dashed = multiplicity < 0;
    int width = std::min(5, std::abs(multiplicity));
    getDisplayString().setTagArg("ls", 1, width);
    getDisplayString().setTagArg("ls", 2, dashed ? "dashed" : "");

    if (multiplicity == 1)
        getDisplayString().setTagArg("t", 0, "");
    else
        getDisplayString().setTagArg("t", 0, multiplicity);

}


