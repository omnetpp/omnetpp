//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __HCGENERATOR_H_
#define __HCGENERATOR_H_

#include <omnetpp.h>

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Packet generator; see NED file for more info.
 */
class HCGenerator : public cSimpleModule
{
  public:
    HCGenerator() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

#endif

