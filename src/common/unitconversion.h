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
 * XXX
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
    static double doParseQuantity(const char *str, const char *expectedUnit, std::string& actualUnit);

  private:
    // all methods are static, no reason to instantiate
    UnitConversion() {}

  public:
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
     * ....
     * XXX todo
     * ....
     * - if the expected unit is a recognized but non-base unit (like ms, Kbps),
     *   the quantity must be given in exactly the same unit (ms, Kbps).
     *   No conversion takes place. The reason is (1) to encourage models to
     *   standardise on expecting parameter values to be in the base units (s, bps);
     *   and (2) to prevent silently changing actual values of parameters if
     *   @unit is accidentally removed from the parameter's NED declaration.
     */
    static double parseQuantity(const char *str, const char *expectedUnit=NULL);

    /**
     * XXX. todo docu. like the one above, but expectedUnit==NULL assumed,
     * and returns actual unit too (if there was one). The expectedUnit param
     * is left out, because if given, outActualUnit would be exactly the
     * same (else exception!), so the two don't make sense together.
     */
    static double parseQuantity(const char *str, std::string& outActualUnit);

    /**
     *
     */
    static std::string formatQuantity(double d, const char *unit=NULL);

    /**
     * Returns a descriptive name of the given unit; in the worst case
     * it returns the input string itself in quotes.
     */
    static std::string unitDescription(const char *unit);
    
    /**
     * Converts the given value with unit into the given target unit.
     * Throws an error if the conversion is not possible. 
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);
};

NAMESPACE_END


#endif

