//
// This program is property of its copyright holder. All rights reserved.
// 

#ifndef __PROJC_C1SM_H_
#define __PROJC_C1SM_H_

#include <omnetpp.h>
#include "b/features.h"

#ifndef WITH_BF1
#error "'proj C' requires 'proj B feature 1', please enable it in projB"
#endif

#include "b/bf1/bf1sm.h"
// NOTE: header files from the projA should be accessible as projB re-exports the include folders it is using
#include "a/af1/af1message_m.h"

using namespace omnetpp;


class C1sm : public Bf1sm
{
  protected:
    Af1message* msg;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
