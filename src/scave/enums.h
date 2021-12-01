//=========================================================================
//  ENUMS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_ENUMS_H
#define __OMNETPP_SCAVE_ENUMS_H

#include "scavedefs.h"

namespace omnetpp {
namespace scave {

/**
 * Used by VectorResult as well as filter nodes e.g. Integrate
 */
enum InterpolationMode { UNSPECIFIED, NONE, SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };

}  // namespace scave
}  // namespace omnetpp

#endif
