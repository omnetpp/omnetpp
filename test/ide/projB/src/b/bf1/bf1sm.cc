//
// This program is property of its copyright holder. All rights reserved.
//

#include "bf1sm.h"
#include "af1sm-extra.h"

#ifndef WITH_BF1
#error "WITH_BF1 macro should be defined in .oppfeatures should be present in b/features.h file"
#endif

#ifndef MAKEMAKE_DEFINE
#error "MAKEMAKE_DEFINE macro should be defined on the Preview page of the Makemake option dialog and it should be passed to the compiler"
// NOTE: the presence of this macro may not be recognized by CDT indexer so the IDE may report this as a warning in the text editor
#endif

#ifndef CDT_DEFINE
#error "CDT_DEFINE macro should be defined on CDT Path and Symbols page and it should be passed to the compiler"
#endif

#ifndef EXTRA_DEFINE
#error "EXTRA_DEFINE macro should be present and correctly included from the af1sm-extra.h file"
#endif

Define_Module(Bf1sm);

void Bf1sm::initialize()
{
}

void Bf1sm::handleMessage(cMessage *msg)
{
}
