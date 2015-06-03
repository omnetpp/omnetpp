//=========================================================================
//  ENUMS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_ENUMS_H
#define __OMNETPP_SCAVE_ENUMS_H

#include "scavedefs.h"

NAMESPACE_BEGIN
namespace scave {

/**
 * Used by VectorResult as well as filter nodes e.g. Integrate
 */
enum InterpolationMode { UNSPECIFIED, NONE, SAMPLE_HOLD, BACKWARD_SAMPLE_HOLD, LINEAR };

} // namespace scave
NAMESPACE_END

#endif
