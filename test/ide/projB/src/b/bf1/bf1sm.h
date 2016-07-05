//
// This program is property of its copyright holder. All rights reserved.
//

#ifndef __PROJB_BF1SM_H_
#define __PROJB_BF1SM_H_

#include <omnetpp.h>
#include "a/features.h"
#include "b/features.h"

#ifndef WITH_AF1
#error "'proj B feature 1' requires 'proj A feature 1', please enable it in projA"
#endif

#ifdef WITH_AF1
#include "a/af1/af1sm.h"
#include "a/af1/af1message_m.h"
#include "af1sm-extra.h"
#endif

using namespace omnetpp;

class Bf1sm : public Af1sm
{
  protected:
    Af1message* msg;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
