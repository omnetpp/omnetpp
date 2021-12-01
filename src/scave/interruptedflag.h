//=========================================================================
//  INTERRUPTEDFLAG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INTERRUPTEDFLAG_H
#define __OMNETPP_SCAVE_INTERRUPTEDFLAG_H

#include "scavedefs.h"

namespace omnetpp {
namespace scave {

class SCAVE_API InterruptedFlag {
  public:
    bool flag = false;
};

class SCAVE_API InterruptedException : public std::runtime_error {
  public:
    InterruptedException(const char *msg="Interrupted") : std::runtime_error(msg) {}
};

}  // namespace scave
}  // namespace omnetpp


#endif


