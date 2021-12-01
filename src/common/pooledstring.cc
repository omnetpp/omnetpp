//=========================================================================
//  POOLEDSTRING.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "pooledstring.h"

namespace omnetpp {
namespace common {

COMMON_API StaticStringPool opp_staticpooledstring::pool;
COMMON_API StringPool opp_pooledstring::pool;

}  // namespace common
}  // namespace omnetpp

