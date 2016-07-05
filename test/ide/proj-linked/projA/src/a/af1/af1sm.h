//
// This program is property of its copyright holder. All rights reserved.
//

#ifndef __PROJA_AF1SM_H_
#define __PROJA_AF1SM_H_

#include <omnetpp.h>
#include "a/features.h"

using namespace omnetpp;

class Af1sm : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
