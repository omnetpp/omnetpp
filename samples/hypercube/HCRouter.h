//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __HCROUTER_H_
#define __HCROUTER_H_

#include <omnetpp.h>

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * A router node that implements deflection routing; see NED file for more info.
 */
class HCRouter : public cSimpleModule
{
  public:
    HCRouter() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

#endif

