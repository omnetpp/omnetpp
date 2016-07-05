//
// This program is property of its copyright holder. All rights reserved.
//

#ifndef __PROJB_BF2SM_H_
#define __PROJB_BF2SM_H_

#include <omnetpp.h>
#include "a/features.h"

#ifdef WITH_AF2
#include "a/af2/af2sm.h"
#endif

using namespace omnetpp;

class Bf2sm : public Af2sm
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
