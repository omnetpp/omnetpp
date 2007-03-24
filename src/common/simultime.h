//==========================================================================
//   SIMULTIME.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SIMULTIME_H
#define __SIMULTIME_H

#include <string>
#include <iostream>
#include "commondefs.h"
#include "inttypes.h"

typedef int64 simultime_t;

/**
 * int64-based, fixed-point simulation time. Precision is determined by a scale
 * exponent, which falls in the range -18..0.
 * For example, a scale exponent of -6 means microsecond precision.
 */
//XXX rename to SimTimeUtil (as it doesn't actually store a time value)
class COMMON_API SimulTime
{
  public:

    /**
     * Converts the given string to simulation time. Throws an error if
     * there is an error during conversion.
     */
    static const simultime_t parse(const char *s, int scaleexp);

    /**
     * Converts a prefix of the given string to simulation time, up to the
     * first character that cannot occur in simulation time strings:
     * not (digit or "." or "+" or "-" or whitespace).
     * Throws an error if there is an error during conversion of the prefix.
     * If the prefix is empty (whitespace only), then 0 is returned, and
     * endp is set equal to s; otherwise,  endp is set to point to the
     * first character that was not converted.
     */
    static const simultime_t parse(const char *s, int scaleexp, const char *&endp);

    /**
     * Utility function to convert a 64-bit fixed point number into a string
     * buffer. scaleexp must be in the -18..0 range, and the buffer must be
     * at least 64 bytes long. A pointer to the result string will be
     * returned. A pointer to the terminating '\0' will be returned in endp.
     *
     * ATTENTION: For performance reasons, the returned pointer will point
     * *somewhere* into the buffer, but NOT necessarily at the beginning.
     */
    static char *ttoa(char *buf, simultime_t t, int scaleexp, char *&endp);

    /**
     * Convert a time value to string, using the specified scale.
     */
      static std::string format(simultime_t time, int scale) {
         char *e;
         char buff[64];
         return std::string(ttoa(buff, time, scale, e));
      }

};

#endif
