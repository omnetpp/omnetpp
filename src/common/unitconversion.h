//=========================================================================
//  UNITCONVERSION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _UNITCONVERSION_H_
#define _UNITCONVERSION_H_

#include "exception.h"

/**
 * XXX
 */
class UnitConversion
{
  public:
    struct UnitDesc { const char *unit; double mult; const char *baseunit; const char *longname; };
  protected:
    UnitDesc unitTable[];

  protected:
    UnitDesc *lookupUnit(const char *unit);

  public:
    UnitConversion() {}

    /**
     * Converts a quantity given as string to a double, and returns it.
     * Performs unit conversion and unit checking as needed.
     * Ths class has knowledge about some units (minutes, milliseconds,
     * microseconds, kilobit/sec, megabit/sec, etc) and how to convert them
     * to a base unit (seconds, bit/sec, etc.) If there's a syntax error,
     * or if unit mismatch is found (i.e. distance is given instead of time),
     * the method throws an exception.
     *
     * Operation:
     * - if string does not contain a unit (just a number), return the number
     *   without conversion.
     * - if string contains a single unit (e.g. "5.1
     * Operation:
     */
    double parseQuantity(const char *str, const char *requiredUnit=NULL);

    /**
     *
     */
    std::string formatQuantity(double d, const char *requiredUnit=NULL);
};

#endif

