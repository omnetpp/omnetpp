//=========================================================================
//  QUANTITYFORMATTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_QUANTITYFORMATTER_H
#define __OMNETPP_COMMON_QUANTITYFORMATTER_H

#include <algorithm>
#include <functional>
#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <string.h>

#include "stringutil.h"
#include "commondefs.h"
#include "unitconversion.h"

namespace omnetpp {
namespace common {

/**
 * This class formats a number with an optional unit into a human readable string.
 * The algorithm can format using automatically selected regular or scientific
 * notation, it can do automatic unit conversion, rounding based on minimum and
 * maximum number of significant digits, fit the result into the available character
 * and pixel space, and more.
 *
 * Some rules:
 * - the output should be the most accurate output among all possible outputs
 *   having the same length
 * - if the scientific notation would eliminate less allowed digits than the
 *   number of characters needed for the exponent part then scientific notation
 *   is not used
 * - if number of digits in regular notation is smaller than maximum significant
 *   digits then scientific notation is not used
 */
// TODO optimize performance but be careful to keep correctness
class COMMON_API QuantityFormatter
{
  public:
    enum class NotationMode {
        REGULAR,
        SCIENTIFIC,
        AUTO,
    };

    enum class ScientificExponentMode {
        RANGE_01_1,
        RANGE_1_10,
        MOD_3,
    };

// TODO
//    enum class RoundingMode {
//        ROUND,
//        FLOOR,
//        CEIL,
//    };

    enum class OutputUnitMode {
        KEEP,
        AUTO,
    };

    enum class SignMode {
        FORCE,
        AUTO,
    };

    struct Input
    {
        const double value = std::numeric_limits<double>::quiet_NaN();
        const char *unit = nullptr;

        Input(double value, const char *unit) :
            value(value), unit(unit)
        { }
    };

    struct Options
    {
        // available space
        int numAvailableCharacters = std::numeric_limits<int>::max();
        int numAvailablePixels = std::numeric_limits<int>::max();

        // accurate value
        int numAccurateDigits = 15;

        // notation
        NotationMode notationMode = NotationMode::AUTO;
        ScientificExponentMode scientificExponentMode = ScientificExponentMode::RANGE_1_10;
        int scientificNotationExponentLimit = 6;

        // approximation
        int minSignificantDigits = 1;
        int maxSignificantDigits = 3;
        // TODO RoundingMode roundingMode = RoundingMode::ROUND;

        // output unit
        OutputUnitMode outputUnitMode = OutputUnitMode::AUTO;
        std::vector<std::string> allowedOutputUnits;

        // separators and marks
        std::string groupSeparator = ",";
        std::string decimalSeparator = ".";
        std::string approximationMark = "~";
        std::string exponentMark = "e";

        // sign
        SignMode signMode = SignMode::AUTO;
        SignMode exponentSignMode = SignMode::AUTO;
        std::string plusSign = "+";
        std::string minusSign = "-";

        // unit
        bool includeUnit = true;
        std::string unitSeparator = " ";

        // measures text width in pixels
        std::function<int(const char *)> measureText;

        Options() { }
        Options(const Options& other) = default;

        void setMeasureTextFromJava(std::function<int(const char *)> f) {
            measureText = f;
        }
    };

    struct Output
    {
        // formatted number and unit
        const std::string text;
        // role of each character in the formatted output (has the same length)
        //  - ~ approximation mark
        //  - s sign
        //  - d digit
        //  - , group separator
        //  - . decimal separator
        //  - e exponent mark
        //  - S exponent sign
        //  - E exponent digit
        //  - SPACE unit separator
        //  - u unit
        const std::string role;
        const char *unit = nullptr;
        const bool fitsIntoAvailableSpace = false;

        Output(const std::string& text, const std::string& role, const char *unit, bool fitsIntoAvailableSpace) :
            text(text), role(role), unit(unit), fitsIntoAvailableSpace(fitsIntoAvailableSpace)
        { }
    };

  protected:
    struct UnitSpecificOptions
    {
        double lowerLimit = 1;
        double upperLimit = 1000;
        const char *preferredBaseUnit = nullptr;
        std::vector<const char *> allowedOutputUnits;

        UnitSpecificOptions(double lowerLimit, double upperLimit, const char *preferredBaseUnit, std::vector<const char *> allowedOutputUnits) :
            lowerLimit(lowerLimit), upperLimit(upperLimit), preferredBaseUnit(preferredBaseUnit), allowedOutputUnits(allowedOutputUnits)
        { }
    };

    struct State
    {
        // available space
        int numRemovedDigitsDueToInsufficientSpace = 0;

        // determined sign
        const char *sign = nullptr;

        // scientific notation
        bool isScientificNotation = false;
        const char *exponentSign = nullptr;
        int exponent = -1;

        #define MAX_DIGITS 1500 // TODO this could probably be smaller
        // contains the sequence of digits without any separator (decimal or grouping)
        // the actual number can start anywhere in the buffer and it is not null terminated
        char digits[MAX_DIGITS]; // 1500 >> log2(10) * 308 + 17 (largest number)
        // first fractional digit right after the decimal separator
        char *firstFractionalDigit = nullptr;
        // first non-zero digit, the number may not start here (small fractional numbers)
        char *firstNonZeroDigit = nullptr;
        // last non-zero digit, the number may not end here (large integer numbers)
        char *lastNonZeroDigit = nullptr;

        // value converted to output unit
        std::vector<const char *> allowedOutputUnits;
        double valueInOutputUnit = std::numeric_limits<double>::quiet_NaN();
        const char *outputUnit = nullptr;

        // rounding state
        bool isRoundedValue = false;
        char *roundedAbsoluteValueInOutputUnit; // points to the first digit in the buffer
        int numRoundedValueInOutputUnitDigits = -1; // length of significant digits in the buffer

        // grouped digits of integral and fractional parts
        std::vector<std::string> integralPartDigitGroups;
        std::vector<std::string> fractionalPartDigitGroups;

        // formatted state
        std::stringstream textStream;
        std::stringstream roleStream;

        // measurement
        int textWidth = -1;
        int textLength = -1;
    };

  protected:
    const Options options;
    std::map<std::string, UnitSpecificOptions> unitSpecificOptions;

  public:
    QuantityFormatter() { }
    QuantityFormatter(const Options& options) : options(options) { }

    const Options& getOptions() const { return options; }

    /**
     * Quantity = Value UnitSeparator Unit | Value
     * Value = ApproximationMark? Sign? (RegularNotation | ScientificNotation)
     * RegularNotation = [DigitGroup GroupSeparator]* DigitGroup [DecimalSeparator [DigitGroup GroupSeparator]* DigitGroup]?
     * ScientificNotation = [DecimalSeparator [DigitGroup GroupSeparator]* DigitGroup]? ExponentMark Sign Digit+
     * DigitGroup = Digit Digit Digit | Digit Digit | Digit
     * Digit = [0-9]
     * Unit = [a-zA-Z]+
     * ApproximationMark = "~"
     * GroupSeparator = ","
     * Sign = "+"|"-"
     * ExponentMark = "e"
     * UnitSeparator = " "
     */
    Output formatQuantity(double value, const char *unit);

    //Output formatQuantity(uint64_t value, int scale, const char *unit);

  protected:
    void checkInput(const Input& input);
    void checkOptions();
    std::vector<const char *> getDefaultAllowedOutputUnits(const char *unit) const;
    const UnitSpecificOptions& getUnitSpecificOptions(const char *unit);
    void clearDigitGroups(State& state);
    void clearStreams(State& state);
    void determineSign(const Input& input, State& state);
    int64_t shiftScore(int64_t score, int64_t increment, int numDigits);

    /**
     * Calculate score for a given unit conversion.
     *
     * Parameters for each candidate unit:
     *   - base unit (e.g. b)
     *   - preferred base unit for human consumption (e.g. B)
     *   - preferred lower limit for human consumption (e.g. 1)
     *   - preferred upper limit for human consumption (e.g. 1000)
     *
     * For each candidate unit calculate:
     *   - value
     *   - is integer (e.g. 123)
     *   - greater than or equal to lower limit (e.g. >=1)
     *   - smaller than upper limit (e.g. <1000)
     *   - unit multiplier to preferred base unit (e.g. 1000)
     *   - is decimal multiplier (e.g. 1000000 vs 8192)
     *   - score based on the above
     */
    int64_t calculateUnitScore(double originalValue, const char *originalUnit, const char *unit);

    const char *getBestUnit(double value, const char *unit, const std::vector<const char *>& allowedUnits);
    void determineOutputUnit(const Input& input, State& state);
    void computeValueInOutputUnit(const Input& input, State& state);
    bool roundDigits(State& state, char *start, int length);
    void printAbsoluteValueInOutputUnit(State& state);
    void computeRoundedAbsoluteValueInOutputUnit(State& state);
    void computeScientificNotationExponent(State& state);
    void determineNotation(State& state);
    void applyScientificNotation(State& state);
    void fitIntoAvailableSpace(State& state);
    bool retryFormat(State& state);
    void computeIntegralPart(State& state);
    void computeFractionalPart(State& state);
    void formatQuantity(const Input& input, State& state);
    void formatValue(const Input& input, State& state);
    void formatRegularNotation(State& state);
    void formatScientificNotation(State& state);
    void formatUnit(State& state);

    void format(State& state, const std::string& str, const char part);

    template <typename T>
    void format(State& state, const T& t, const char part);

    Output createOutput(const State& state);
};

}  // namespace common
}  // namespace omnetpp

#endif

