//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __GEN_H
#define __GEN_H

#include <omnetpp.h>

#define STACKSIZE 16384


/**
 * Generates jobs (messages); see NED file for more info.
 */
class Generator : public cSimpleModule
{
  public:
    Generator() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

#endif
