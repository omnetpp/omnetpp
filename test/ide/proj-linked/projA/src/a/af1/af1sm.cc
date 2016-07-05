//
// This program is property of its copyright holder. All rights reserved.
//

#include "af1sm-extra.h" // this is in the extra directory and cannot be accessed unless the feature's compile flags are correctly applied
#include "af1sm.h"

#ifndef EXTRA_DEFINE
#error "EXTRA_DEFINE macro should be present and correctly included from the af1sm-extra.h file"
#endif

Define_Module(Af1sm);

void Af1sm::initialize()
{
}

void Af1sm::handleMessage(cMessage *msg)
{
}
