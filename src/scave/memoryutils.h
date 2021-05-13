//=========================================================================
//  MEMORYUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2019 Andras Varga
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_MEMORYUTILS_H
#define __OMNETPP_SCAVE_MEMORYUTILS_H

#include "scavedefs.h"

namespace omnetpp {
namespace scave {

// return 1 if successful, else 0
SCAVE_API int malloc_trim();

// returns -1 if could not be determined
SCAVE_API int64_t getAvailableMemoryBytes();

}  // namespace scave
}  // namespace omnetpp

#endif // __OMNETPP_SCAVE_MEMORYUTILS_H
