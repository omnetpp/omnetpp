//
// This program is property of its copyright holder. All rights reserved.
//

#ifndef __PROJB_BF3SM_H_
#define __PROJB_BF3SM_H_

#include <omnetpp.h>
#include "b/features.h"

#ifdef WITH_BF1
#include "b/bf1/bf1sm.h"
#endif

using namespace omnetpp;

class Bf3sm : public Bf1sm
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
