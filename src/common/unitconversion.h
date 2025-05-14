//=========================================================================
//  UNITCONVERSION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_UNITCONVERSION_H
#define __OMNETPP_COMMON_UNITCONVERSION_H

#include <vector>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

/**
 * Unit conversion utilities. This class has built-in knowledge of some
 * physical units (seconds, watts, etc); see internal unitTable[].
 */
class COMMON_API UnitConversion
{
  protected:
    enum Mapping { LINEAR, LOG10 };
    enum PrefixType { DECIMAL, BINARY, TRADITIONAL };
    struct Unit {
        const char *name;
        double mult;
        Mapping mapping;
        const char *baseUnitName;
        const char *longName;
        const char *bestUnitCandidatesList="";
        // ---- computed fields: ----
        PrefixType prefixType;
        bool isByteBased;
        const Unit *baseUnit;
        std::vector<const char *> bestUnitCandidateNames;
        std::vector<const char *> compatibleUnitNames;
        std::vector<const Unit*> bestUnitCandidates;
        std::vector<const Unit*> compatibleUnits;
    };
    static Unit unitTable[];

    static const Unit *bitsUnit;
    static const Unit *bytesUnit;
    static const Unit *secondsUnit;

    static const int HASHTABLESIZE = 2048; // must be power of 2
    static const Unit *hashTable[HASHTABLESIZE];
    static int numCollisions;

  public:
    enum Preference { PREFER, AVOID, KEEP };

    /**
     * Options for getBestUnit(). It contains options for the most commonly
     * used preferences; more control over which units to choose as best unit,
     * such as degrees or radians for angle, or m/s or km/h for speed, can be
     * achieved via the allowedCompatibleUnits parameter of getBestUnit().
     */
    struct COMMON_API Options {
        bool convertZeroToBaseUnit = false; // as opposed to keeping it
        bool allowOriginalUnit = true; // i.e. implicitly add to allowedCompatibleUnits
        bool allowNonmetricTimeUnits = false;
        Preference logarithmicUnitsPolicy = KEEP;
        Preference bitBasedUnitsPolicy = KEEP;
        Preference binaryPrefixPolicy = KEEP;
        bool preferSmallWholeNumbersForBitByte = true; // note: may beat kilobyteThreshold
        double kilobyteThreshold = 10240;  // don't use kilobytes/kibibytes under this many bytes; ditto for bits
        std::string str() const;
    };

  public:
    // internal
    static void init();

  protected:
    static unsigned hashCode(const char *unitName);
    static bool matches(const Unit *unit, const char *unitName);
    static void insert(const char *key, const Unit *unit);
    static void fillHashtable();
    static void fillUnitData();

    static const Unit *lookupUnit(const char *unit);
    static const Unit *getUnit(const char *unit);
    static std::vector<const Unit *> lookupUnits(const std::vector<const char *>& unitNames);
    static bool readNumber(const char *&s, double& number);
    static bool readUnit(const char *&s, std::string& unit);
    static double convertToBase(double value, const Unit *unit);
    static double convertFromBase(double value, const Unit *unit);
    static double tryConvert(double d, const Unit *unit, const Unit *targetUnit);
    static void cannotConvert(const char *unit, const char *targetUnit);
    static bool areCompatibleUnits(const Unit *unit1, const Unit *unit2);
    static double tryGetConversionFactor(const Unit *unit, const Unit *targetUnit);
    static const char *getBestUnit(double value, const Unit *unit, const std::vector<const Unit *>& allowedCompatibleUnits, const Options& options, bool considerBestUnitCandidates);
    static double calculateUnitScore(double originalValue, const Unit *originalUnit, const Unit *unit, const Options& options, bool considerBestUnitCandidates);

  private:
    // all methods are static, no reason to instantiate
    UnitConversion() {}

  public:
    /**
     * Invokes parseQuantity(), and converts the result into the given unit.
     * nullptr or empty string for expectedUnit means no unit (dimensionless
     * number expected). If conversion is not possible (unrelated or unknown units),
     * an exception is thrown.
     */
    static double parseQuantity(const char *str, const char *expectedUnit=nullptr);

    /**
     * Converts a quantity given as string to a double, and returns it, together
     * with the unit it was given in. If there are several numbers and units
     * (see syntax), everything is converted into the last unit.
     *
     * Syntax: <number> | (<number> <unit>)+
     *
     * To reduce the chance of confusion, units must be in strictly decreasing order
     * of magnitude, and negative numbers may only occur at the start of the quantity.
     * The potential minus sign refers to the whole quantity. For example, "-1s200ms"
     * means -1.2s and not -0.8s; and "1s-100ms" is illegal.
     *
     * If there is a syntax error, or if unit mismatch is found (i.e. distance
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
    static std::string formatQuantity(double d, const char *unit=nullptr);

    /**
     * Returns a descriptive name of the given unit; in the worst case
     * it returns the input string itself in quotes.
     */
    static std::string getUnitDescription(const char *unit);

    /**
     * Returns a short string describing the conversion between the unit and its base unit.
     */
    static std::string getConversionDescription(const char *unit);

    /**
     * Returns 0.0 if conversion is not possible (unrelated or unrecognized units)
     * or if it the relationship is nonlinear (such as between dBW and W).
     */
    static double getConversionFactor(const char *sourceUnit, const char *targetUnit);

    /**
     * Converts the given value with unit into the given target unit. nullptr or empty
     * string for either argument means no unit (dimensionless number).
     * Throws an error if the conversion is not possible.
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);

    /**
     * Returns the list of known units that the given one may be converted into, i.e.
     * the ones for which convertUnit() will not raise an error. For unknown
     * units it returns an empty list.
     */
    static const std::vector<const char *>& getCompatibleKnownUnits(const char *unit);

    /**
     * Returns the best unit for human consumption for the given quantity. Returns
     * the unit in which the value is closest to 1.0 but >= 1.0 if at all possible.
     */
    static const char *getBestUnit(double d, const char *unit);

    /**
     * Returns the best unit for human consumption for the given quantity, chosen from
     * the list passed in via the allowedUnits parameter. If allowedUnits is empty or
     * does not contain any compatible units, the set of all known compatible units
     * will be used instead. Further preferences can be passed in via the Options structure.
     */
    static const char *getBestUnit(double value, const char *unit, const std::vector<const char *>& allowedUnits, const Options& options);

    /**
     * Returns the short name for the given unit, or the same pointer if the unit is unrecognized.
     * See getKnownUnits().
     */
    static const char *getShortName(const char *unit);

    /**
     * Returns the long name for the given unit, or the same pointer if the unit is unrecognized.
     * See getKnownUnits().
     */
    static const char *getLongName(const char *unit);

    /**
     * Returns the long name for the given unit, either in singular or in plural form.
     * Returns the unit itself (in best-effort plural form if plural=true is requested)
     * if it is unrecognized. See getKnownUnits().
     */
    static std::string getLongName(const char *unitName, bool plural);

    /**
     * Returns the base unit for the given unit, or nullptr if the unit is unrecognized.
     * See getKnownUnits().
     */
    static const char *getBaseUnit(const char *unit);

    /**
     * Returns true if the given string is a recognized measurement unit.
     */
    static bool isKnownUnit(const char *unit) {return lookupUnit(unit) != nullptr;}

    /**
     * Returns true if the given unit is linear (e.g. W and KiB), and false
     * if not (dBm, dbW). Unknown (custom) units are assumed to be linear.
     */
    static bool isLinearUnit(const char *unit);

    /**
     * Returns true if the two units can be converted into each other.
     */
    static bool areCompatibleUnits(const char *unit1, const char *unit2);

    /**
     * Produces the list of all recognized units, with their short names.
     */
    static std::vector<const char *> getKnownUnits();
};

}  // namespace common
}  // namespace omnetpp

#endif

