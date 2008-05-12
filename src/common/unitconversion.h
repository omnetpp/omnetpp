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

#include "commondefs.h"
#include "exception.h"

NAMESPACE_BEGIN

/**
 * Unit conversion utilities. This class has built-in knowledge of some 
 * physical units (seconds, watts, etc); see internal unitTable[].
 */
class COMMON_API UnitConversion
{
  protected:
    struct UnitDesc { const char *unit; double mult; const char *baseUnit; const char *longName; };
    static UnitDesc unitTable[];

  protected:
    static UnitDesc *lookupUnit(const char *unit);
    static bool readNumber(const char *&s, double& number);
    static bool readUnit(const char *&s, std::string& unit);

  private:
    // all methods are static, no reason to instantiate
    UnitConversion() {}

  public:
    /**
     * Invokes parseQuantity(), and converts the result into the given unit.
     * If conversion is not possible (unrelated or unknown units), and error
     * is thrown.
     */
    static double parseQuantity(const char *str, const char *expectedUnit=NULL);

    /**
     * Converts a quantity given as string to a double, and returns it, together
     * with the unit it was given in. If there are several numbers and units 
     * (see syntax), everything is converted into the last unit.
     * 
     * Syntax: <number> | (<number> <unit>)+ 
     * 
     * If there's a syntax error, or if unit mismatch is found (i.e. distance 
     * is given instead of time), the method throws an exception.
     */
    static double parseQuantity(const char *str, std::string& outActualUnit);

    /**
     * Parse a quantity and return its unit. This method simply delegates to
     * parseQuantity(), and it only exists to make it possible to get the unit
     * from Java code.
     */
    static std::string parseQuantityForUnit(const char *str) {std::string u; parseQuantity(str, u); return u;}

    /**
     * Returns a concatenation of the number and the unit.
     */
    static std::string formatQuantity(double d, const char *unit=NULL);

    /**
     * Returns a descriptive name of the given unit; in the worst case
     * it returns the input string itself in quotes.
     */
    static std::string unitDescription(const char *unit);

    /**
     * Returns 0.0 if conversion is not possible (unrelated or unrecognized units).
     */
    static double getConversionFactor(const char *sourceUnit, const char *targetUnit);

    /**
     * Converts the given value with unit into the given target unit.
     * Throws an error if the conversion is not possible. 
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);
};

NAMESPACE_END

#endif

