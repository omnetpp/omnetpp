//=========================================================================
//  COLORUTIL.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_COLORUTIL_H
#define __OMNETPP_COMMON_COLORUTIL_H

#include "commondefs.h"
#include <stdint.h> // should use cstdint, but that is C++11

NAMESPACE_BEGIN
namespace common {

/**
 * Given a color name, returns the RGB values for it.
 * Accepted formats: "#RRGGBB" "@HHSSBB", X11 and SVG color names.
 * (In case of the 4 common names, SVG has priority.)
 * Use grey instead of gray.
 * Throws a runtime exception (opp_runtime_error) if the name could not be parsed.
 */
COMMON_API void parseColor(const char *s, uint8_t &r, uint8_t &g, uint8_t &b);

} // namespace common
NAMESPACE_END

#endif // __OMNETPP_COMMON_COLORUTIL_H
