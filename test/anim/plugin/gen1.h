//
// file: gen1.h
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#ifndef __GEN_H
#define __GEN_H

#include <omnetpp.h>

USING_NAMESPACE

class FF1Generator : public cSimpleModule
{
  public:
    FF1Generator() : cSimpleModule(16384) {}
    virtual void activity() override;
    virtual void finish() override;
};

#endif
