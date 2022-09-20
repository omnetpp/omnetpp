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
    void checkInput(const Input& input) {
        if (input.unit && opp_isempty(input.unit))
            throw opp_runtime_error("Unit must be non-empty if provided");
    }

    void checkOptions() {
        if (options.numAvailableCharacters < 1)
            throw opp_runtime_error("Option numAvailableCharacters must be greater than 0");
        if (options.numAvailablePixels < 1)
            throw opp_runtime_error("Option numAvailablePixels must be greater than 0");

        if (options.numAccurateDigits < 1)
            throw opp_runtime_error("Option numAccurateDigits must be greater than 0");

        if (options.scientificNotationExponentLimit < 1)
            throw opp_runtime_error("Option scientificNotationExponentLimit must be greater than 0");

        // approximation
        if (options.minSignificantDigits < 1)
            throw opp_runtime_error("Option minSignificantDigits must be greater than 0");
        if (options.numAccurateDigits < options.minSignificantDigits)
            throw opp_runtime_error("Option minSignificantDigits must be less than numAccurateDigits");
        if (options.maxSignificantDigits < options.minSignificantDigits)
            throw opp_runtime_error("Option maxSignificantDigits must be greater than option minSignificantDigits");
        if (options.numAccurateDigits < options.maxSignificantDigits)
            throw opp_runtime_error("Option maxSignificantDigits must be less than numAccurateDigits");

        if (options.decimalSeparator.empty())
            throw opp_runtime_error("Option decimalSeparator must be specified");
        if (options.exponentMark.empty())
            throw opp_runtime_error("Option exponentMark must be specified");

        if (options.plusSign.empty())
            throw opp_runtime_error("Option plusSign must be specified");
        if (options.minusSign.empty())
            throw opp_runtime_error("Option minusSign must be specified");
    }

    std::vector<const char *> getDefaultAllowedOutputUnits(const char *unit) const {
        std::vector<const char *> units;
        const char *baseUnit = UnitConversion::getBaseUnit(unit);
        for (auto unit : UnitConversion::getAllUnits())
            if (opp_streq(baseUnit, UnitConversion::getBaseUnit(unit)))
                units.push_back(unit);
        return units;
    }

    const UnitSpecificOptions& getUnitSpecificOptions(const char *unit) {
        if (unitSpecificOptions.empty()) {
            unitSpecificOptions = {
               {"s", UnitSpecificOptions(1, std::numeric_limits<int>::max(), "s", getDefaultAllowedOutputUnits("s"))},
               {"b", UnitSpecificOptions(1, 10000, "B", getDefaultAllowedOutputUnits("b"))},
               {"B", UnitSpecificOptions(1, 10000, "B", getDefaultAllowedOutputUnits("b"))},
            };
        }
        auto it = unitSpecificOptions.find(unit);
        if (it != unitSpecificOptions.end())
            return it->second;
        else {
            const char *baseUnit = UnitConversion::getBaseUnit(unit);
            const char *preferredBaseUnit = baseUnit ? baseUnit : unit;
            auto it = unitSpecificOptions.insert({unit, UnitSpecificOptions(1, 1000, preferredBaseUnit, getDefaultAllowedOutputUnits(unit))});
            return it.first->second;
        }
    }

    void clearDigitGroups(State& state) {
        state.integralPartDigitGroups.clear();
        state.fractionalPartDigitGroups.clear();
    }

    void clearStreams(State& state) {
        state.textStream.str("");
        state.roleStream.str("");
    }

    void determineSign(const Input& input, State& state) {
        int inputSign = (input.value > 0) - (input.value < 0);
        switch (options.signMode) {
            case SignMode::FORCE:
                state.sign = inputSign < 0 ? options.minusSign.c_str() : (inputSign > 0 ? options.plusSign.c_str() : nullptr);
                break;
            case SignMode::AUTO:
                state.sign = inputSign < 0 ? options.minusSign.c_str() : nullptr;
                break;
            default:
                throw opp_runtime_error("Unknown signMode: %d", options.signMode);
        }
    }

    int64_t shiftScore(int64_t score, int64_t increment, int numDigits)
    {
        int64_t scale = std::pow(10, numDigits);
        Assert(0 <= increment < scale);
        return score * scale + increment;
    }

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
    int64_t calculateUnitScore(double originalValue, const char *originalUnit, const char *unit)
    {
        originalValue = std::fabs(originalValue);
        bool isOriginalValueInteger = std::floor(originalValue) == originalValue;
        const auto& unitSpecificOptions = getUnitSpecificOptions(unit);
        double multiplier = UnitConversion::convertUnit(1, unit, getUnitSpecificOptions(originalUnit).preferredBaseUnit);
        double multiplierScale = std::log10(multiplier);
        double value = UnitConversion::convertUnit(originalValue, originalUnit, unit);
        double roundedValue = std::round(value);
        // sometimes unit conversion returns non-integers when it should really be an integer value
        bool isValueInteger = roundedValue == value || (roundedValue != 0 && std::abs(roundedValue - value) / value < 1e-12);
        bool isGreaterThanLowerLimit = value >= unitSpecificOptions.lowerLimit;
        bool isSmallerThanUpperLimit = value < unitSpecificOptions.upperLimit;
        bool isDecimalMultiplier = std::fmod(multiplierScale, 1.0) == 0.0;
        bool areBothIntegers = isOriginalValueInteger && isValueInteger;
        int64_t score = 0;
        score = shiftScore(score, 8, 1); // always make the score the same length for visual debugging
        score = shiftScore(score, areBothIntegers && isSmallerThanUpperLimit ? 2 : (!areBothIntegers && isSmallerThanUpperLimit ? 1 : 0), 1);
        score = shiftScore(score, isGreaterThanLowerLimit ? 1 : 0, 1);
        score = shiftScore(score, isDecimalMultiplier ? 1 : 0, 1);
        score = shiftScore(score, isGreaterThanLowerLimit ? std::floor((multiplierScale + 18)) : std::floor(3000 / (multiplierScale + 18)), 3);
        score = shiftScore(score, areBothIntegers && !isSmallerThanUpperLimit ? 2 : (areBothIntegers && isSmallerThanUpperLimit ? 1 : 0), 1);
//        std::cout << "Calculated unit score: value = " << value << ", unit = " << unit << " score = " << score << std::endl;
        return score;
    }

    const char *getBestUnit(double value, const char *unit, const std::vector<const char *>& allowedUnits)
    {
        // We do not touch the unit (return the original unit) in a number of cases:
        // if value is zero, infinite or NaN; if we don't know about the unit.
        if (unit == nullptr)
            return nullptr;
        else if (value == 0 || !std::isfinite(value) || allowedUnits.empty()) {
            const char *preferredBaseUnit = getUnitSpecificOptions(unit).preferredBaseUnit;
            if (std::find(allowedUnits.begin(), allowedUnits.end(), preferredBaseUnit) != allowedUnits.end())
                return preferredBaseUnit;
            else
                return unit;
        }
        else {
            std::vector<int64_t> scores;
            for (auto& allowedUnit : allowedUnits)
                scores.push_back(calculateUnitScore(value, unit, allowedUnit));
            // pick best one
            auto it = std::max_element(scores.begin(), scores.end());
            int64_t bestScore = *it;
            const char *bestUnit = *(allowedUnits.begin() + (it - scores.begin()));
            // give the original unit a chance to win too (so we don't pointlessly convert e.g. 1cm to 10mm);
            // also, do not change the unit if it wouldn't change the value (e.g. don't change 1As to 1C)
            if (!opp_streq(bestUnit, unit) && (calculateUnitScore(value, unit, unit) >= bestScore || UnitConversion::convertUnit(1, unit, bestUnit) == 1))
                return unit;
            else
                return bestUnit;
        }
    }

    void determineOutputUnit(const Input& input, State& state) {
        switch (options.outputUnitMode) {
            case OutputUnitMode::KEEP:
                state.outputUnit = input.unit;
                break;
            case OutputUnitMode::AUTO:
                if (input.unit) {
                    state.allowedOutputUnits = getUnitSpecificOptions(input.unit).allowedOutputUnits;
                    if (!options.allowedOutputUnits.empty()) {
                        auto it = std::remove_if(state.allowedOutputUnits.begin(), state.allowedOutputUnits.end(), [&] (const std::string& unit) {
                            return std::find(options.allowedOutputUnits.begin(), options.allowedOutputUnits.end(), unit) == options.allowedOutputUnits.end();
                        });
                        state.allowedOutputUnits.erase(it, state.allowedOutputUnits.end());
                    }
                    state.outputUnit = getBestUnit(input.value, input.unit, state.allowedOutputUnits);
                }
                else
                    state.outputUnit = nullptr;
                break;
            default:
                throw opp_runtime_error("Unknown outputUnitMode: %d", options.outputUnitMode);
        }
    }

    void computeValueInOutputUnit(const Input& input, State& state) {
        switch (options.outputUnitMode) {
            case OutputUnitMode::KEEP:
                state.valueInOutputUnit = input.value;
                break;
            case OutputUnitMode::AUTO:
                state.valueInOutputUnit = UnitConversion::convertUnit(input.value, input.unit, state.outputUnit);
                break;
            default:
                throw opp_runtime_error("Unknown outputUnitMode: %d", options.outputUnitMode);
        }
    }

    bool roundDigits(State& state, char *start, int length) {
        char *current = start + length;
        if (*current >= '5') {
            current--;
            (*current)++;
            while (current != state.digits) {
                if (*current == '9' + 1) {
                    *current = '0';
                    current--;
                    (*current)++;
                }
                else
                    break;
            }
            Assert(current >= state.digits);
            if (current < start)
                return true;
        }
        return false;
    }

    void printAbsoluteValueInOutputUnit(State& state) {
        Assert(state.valueInOutputUnit != 0);
        // print from offset 1
        *state.digits = '0';
        char *printStart = state.digits + 1;
        const char *printMax = state.digits + MAX_DIGITS;
        double absoluteValueInOutputUnit = std::abs(state.valueInOutputUnit);
        int printFractionalDigits = (int)std::ceil(std::abs(std::log10(absoluteValueInOutputUnit))) + options.numAccurateDigits + 1;
        int printedLength = snprintf(printStart, printMax - printStart, "%.*f", printFractionalDigits, std::abs(absoluteValueInOutputUnit));
        char *printEnd = printStart + printedLength;
        // find first digit after decimal separator
        state.firstFractionalDigit = printStart;
        while (*state.firstFractionalDigit != '.')
            state.firstFractionalDigit++;
        Assert(printStart <= state.firstFractionalDigit && state.firstFractionalDigit <= printEnd);
        // eliminate decimal separator which makes firstFractionalDigit correct
        memmove(state.firstFractionalDigit, state.firstFractionalDigit + 1, printEnd - state.firstFractionalDigit);
        // fix duplicated last digit after move
        *(printEnd - 1) = '0';
        // find first non-zero digits
        state.firstNonZeroDigit = state.digits;
        while (*state.firstNonZeroDigit == '0')
            state.firstNonZeroDigit++;
        // round to accurate number of digits
        if (roundDigits(state, state.firstNonZeroDigit, options.numAccurateDigits))
            state.firstNonZeroDigit--;
        // find last non-zero digits
        state.lastNonZeroDigit = state.firstNonZeroDigit + options.numAccurateDigits - 1;
        while (*state.lastNonZeroDigit == '0')
            state.lastNonZeroDigit--;
    }

    void computeRoundedAbsoluteValueInOutputUnit(State& state) {
        int numSignificantDigits = state.lastNonZeroDigit - state.firstNonZeroDigit + 1;
        if (numSignificantDigits > options.maxSignificantDigits - state.numRemovedDigitsDueToInsufficientSpace) {
            state.roundedAbsoluteValueInOutputUnit = state.firstNonZeroDigit;
            // don't start rounding in the integral part in regular notation
            if (state.isScientificNotation || options.maxSignificantDigits - state.numRemovedDigitsDueToInsufficientSpace > state.firstFractionalDigit - state.firstNonZeroDigit) {
                state.isRoundedValue = true;
                if (roundDigits(state, state.firstNonZeroDigit, options.maxSignificantDigits - state.numRemovedDigitsDueToInsufficientSpace)) {
                    state.firstNonZeroDigit--;
                    state.roundedAbsoluteValueInOutputUnit--;
                }
            }
            else {
                state.isRoundedValue = state.lastNonZeroDigit >= state.firstFractionalDigit;
                if (roundDigits(state, state.digits + 1, state.firstFractionalDigit - state.firstNonZeroDigit)) {
                    state.firstNonZeroDigit--;
                    state.roundedAbsoluteValueInOutputUnit--;
                }
            }
            state.numRoundedValueInOutputUnitDigits = options.maxSignificantDigits - state.numRemovedDigitsDueToInsufficientSpace;
            while (*(state.roundedAbsoluteValueInOutputUnit + state.numRoundedValueInOutputUnitDigits - 1) == '0')
                state.numRoundedValueInOutputUnitDigits--;
        }
        else if (numSignificantDigits < options.minSignificantDigits) {
            state.isRoundedValue = false;
            state.roundedAbsoluteValueInOutputUnit = state.firstNonZeroDigit;
            state.numRoundedValueInOutputUnitDigits = options.minSignificantDigits;
        }
        else {
            state.isRoundedValue = false;
            state.roundedAbsoluteValueInOutputUnit = state.firstNonZeroDigit;
            state.numRoundedValueInOutputUnitDigits = numSignificantDigits;
        }
    }

    void computeScientificNotationExponent(State& state) {
        switch (options.exponentSignMode) {
            case SignMode::FORCE:
                state.exponentSign = state.firstFractionalDigit <= state.firstNonZeroDigit ? options.minusSign.c_str() : options.plusSign.c_str();
                break;
            case SignMode::AUTO:
                state.exponentSign = state.firstFractionalDigit <= state.firstNonZeroDigit ? options.minusSign.c_str() : nullptr;
                break;
            default:
                throw opp_runtime_error("Unknown signMode: %d", options.signMode);
        }
        switch (options.scientificExponentMode) {
            case ScientificExponentMode::RANGE_01_1:
                state.exponent = std::abs(state.firstFractionalDigit - state.firstNonZeroDigit);
                break;
            case ScientificExponentMode::RANGE_1_10:
                state.exponent = std::abs(state.firstFractionalDigit - state.firstNonZeroDigit - 1);
                break;
            case ScientificExponentMode::MOD_3:
                state.exponent = std::abs(std::floor((state.firstFractionalDigit - state.firstNonZeroDigit - 1) / 3.0) * 3);
                break;
            default:
                throw opp_runtime_error("Unknown scientificExponentMode: %d", options.scientificExponentMode);
        }
    }

    void determineNotation(State& state) {
        switch (options.notationMode) {
            case NotationMode::REGULAR:
                state.isScientificNotation = false;
                break;
            case NotationMode::SCIENTIFIC:
                state.isScientificNotation = true;
                break;
            case NotationMode::AUTO: {
                int numExponentCharacters = options.exponentMark.size() + opp_strlen(state.exponentSign) + std::ceil(std::log10(state.exponent + 1));
                int numEliminatedZeros = state.firstFractionalDigit <= state.firstNonZeroDigit ?
                                             std::min(state.exponent, (int)(state.firstNonZeroDigit - state.firstFractionalDigit)) :
                                             std::min(state.exponent, (int)(state.firstFractionalDigit - std::min(state.firstNonZeroDigit + options.maxSignificantDigits - state.numRemovedDigitsDueToInsufficientSpace, state.lastNonZeroDigit)));
                int numRegularNotationDigits =  std::max(state.firstFractionalDigit - 1, state.lastNonZeroDigit) - state.digits - 1;
                bool smallScaleValue = state.firstFractionalDigit - state.firstNonZeroDigit < 2 - options.scientificNotationExponentLimit;
                bool largeScaleValue = state.firstFractionalDigit - state.firstNonZeroDigit > options.scientificNotationExponentLimit;
                state.isScientificNotation = state.exponent != 0 && numEliminatedZeros >= numExponentCharacters &&
                                             numRegularNotationDigits > options.maxSignificantDigits - state.numRemovedDigitsDueToInsufficientSpace &&
                                             (smallScaleValue || largeScaleValue);
                break;
            }
            default:
                throw opp_runtime_error("Unknown notationMode: %d", options.notationMode);
        }
    }

    void applyScientificNotation(State& state) {
        if (state.isScientificNotation) {
            switch (options.scientificExponentMode) {
                case ScientificExponentMode::RANGE_01_1:
                    state.firstFractionalDigit = state.firstNonZeroDigit;
                    break;
                case ScientificExponentMode::RANGE_1_10:
                    state.firstFractionalDigit = state.firstNonZeroDigit + 1;
                    break;
                case ScientificExponentMode::MOD_3:
                    state.firstFractionalDigit += state.firstFractionalDigit <= state.firstNonZeroDigit ? state.exponent : -state.exponent;
                    break;
                default:
                    throw opp_runtime_error("Unknown scientificExponentMode: %d", options.scientificExponentMode);
            }
        }
    }

    void fitIntoAvailableSpace(State& state) {
        if (options.numAvailableCharacters != std::numeric_limits<int>::max() ||
            options.numAvailablePixels != std::numeric_limits<int>::max())
        {
            auto text = state.textStream.str();
            state.textWidth = options.measureText(text.c_str());
            state.textLength = text.size();
        }
    }

    bool retryFormat(State& state) {
        bool fitsIntoAvailableSpace = state.textWidth <= options.numAvailablePixels && state.textLength <= options.numAvailableCharacters;
        if (!fitsIntoAvailableSpace && options.maxSignificantDigits - options.minSignificantDigits> state.numRemovedDigitsDueToInsufficientSpace) {
            state.numRemovedDigitsDueToInsufficientSpace++;
            return true;
        }
        else
            return false;
    }

    void computeIntegralPart(State& state) {
        if (state.firstFractionalDigit <= state.firstNonZeroDigit)
            state.integralPartDigitGroups.push_back("0");
        else {
            int index = state.firstFractionalDigit - state.firstNonZeroDigit;
            while (index > 0) {
                const char *startDigit = state.roundedAbsoluteValueInOutputUnit + std::max(0, index - 3);
                const char *endDigit = state.roundedAbsoluteValueInOutputUnit + index;
                state.integralPartDigitGroups.push_back(std::string(startDigit, endDigit - startDigit));
                index -= 3;
            }
            std::reverse(state.integralPartDigitGroups.begin(), state.integralPartDigitGroups.end());
        }
    }

    void computeFractionalPart(State& state) {
        if (state.firstFractionalDigit - state.firstNonZeroDigit < state.numRoundedValueInOutputUnitDigits) {
            int index = state.firstFractionalDigit - state.firstNonZeroDigit;
            while (index < state.numRoundedValueInOutputUnitDigits) {
                const char *startDigit = state.roundedAbsoluteValueInOutputUnit + index;
                const char *endDigit = state.roundedAbsoluteValueInOutputUnit + std::min((int)(index + 3), state.numRoundedValueInOutputUnitDigits);
                state.fractionalPartDigitGroups.push_back(std::string(startDigit, endDigit - startDigit));
                index += 3;
            }
        }
    }

    // Quantity = Value Space Unit | Value
    void formatQuantity(const Input& input, State& state) {
        if (options.includeUnit) {
            formatValue(input, state);
            if (state.outputUnit) {
                format(state, options.unitSeparator, ' ');
                formatUnit(state);
            }
        }
        else
            formatValue(input, state);
    }

    // Value = ApproximationMark? Sign? (RegularNotation | ScientificNotation)
    void formatValue(const Input& input, State& state) {
        if (input.value == 0 || !std::isfinite(input.value))
            format(state, input.value, 'd');
        else {
            if (state.isRoundedValue)
                format(state, options.approximationMark, '~');
            if (state.sign)
                format(state, state.sign, 's');
            if (state.isScientificNotation)
                formatScientificNotation(state);
            else
                formatRegularNotation(state);
        }
    }

    // RegularNotation = [DigitGroup GroupSeparator]* DigitGroup [DecimalSeparator [DigitGroup GroupSeparator]* DigitGroup]?
    void formatRegularNotation(State& state) {
        for (int i = 0; i < state.integralPartDigitGroups.size(); i++) {
            auto& digitGroup = state.integralPartDigitGroups[i];
            if (i != 0)
                format(state, options.groupSeparator, ',');
            format(state, digitGroup, 'd');
        }
        if (state.fractionalPartDigitGroups.size() != 0)
            format(state, options.decimalSeparator, '.');
        for (int i = 0; i < state.fractionalPartDigitGroups.size(); i++) {
            auto& digitGroup = state.fractionalPartDigitGroups[i];
            if (i != 0)
                format(state, options.groupSeparator, ',');
            format(state, digitGroup, 'd');
        }
    }

    // ScientificNotation = [DecimalSeparator [DigitGroup GroupSeparator]* DigitGroup]? ExponentMark Sign Digit+
    void formatScientificNotation(State& state) {
        for (int i = 0; i < state.integralPartDigitGroups.size(); i++) {
            auto& digitGroup = state.integralPartDigitGroups[i];
            if (i != 0)
                format(state, options.groupSeparator, ',');
            format(state, digitGroup, 'd');
        }
        if (state.fractionalPartDigitGroups.size() != 0)
            format(state, options.decimalSeparator, '.');
        for (int i = 0; i < state.fractionalPartDigitGroups.size(); i++) {
            auto& digitGroup = state.fractionalPartDigitGroups[i];
            if (i != 0)
                format(state, options.groupSeparator, ',');
            format(state, digitGroup, 'd');
        }
        format(state, options.exponentMark, 'e');
        format(state, state.exponentSign == nullptr ? "" : state.exponentSign, 'S');
        format(state, state.exponent, 'D');
    }

    // Unit = [a-zA-Z]+
    void formatUnit(State& state) {
        format(state, state.outputUnit, 'u');
    }

    template <typename T>
    void format(State& state, const T& t, const char part) {
        auto positionBefore = state.textStream.tellp();
        state.textStream << t;
        auto positionAfter = state.textStream.tellp();
        state.roleStream << std::string(positionAfter - positionBefore, part);
    }

    Output createOutput(const State& state) {
        bool fitsIntoAvailableSpace = state.textWidth <= options.numAvailablePixels && state.textLength <= options.numAvailableCharacters;
        return Output(state.textStream.str(), state.roleStream.str(), state.outputUnit, fitsIntoAvailableSpace);
    }
};

static std::string digits123etc(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << (i + 1);
    return stream.str();
}

static std::string digits987etc(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << (9 - i);
    return stream.str();
}

static std::string digits9etc(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << 9;
    return stream.str();
}

static std::string digits10etc1(int numDigits) {
    std::stringstream stream;
    stream << 1;
    for (int i = 0; i < numDigits; i++)
        stream << 0;
    stream << 1;
    return stream.str();
}

static std::string digits4etc5(int numDigits) {
    std::stringstream stream;
    for (int i = 0; i < numDigits; i++)
        stream << (i == numDigits - 1 ? 5 : 4);
    return stream.str();
}

static void testQuantityFormatter(QuantityFormatter& formatter, std::string text, const char *unit) {
    double value = atof(text.c_str());
    auto output = formatter.formatQuantity(value, unit);
    auto simplified = output.text;
    simplified = opp_replacesubstring(simplified, "~", "", true);
    simplified = opp_replacesubstring(simplified, ",", "", true);
    double parsed = std::numeric_limits<double>::quiet_NaN();
    try {
        parsed = UnitConversion::parseQuantity(simplified.c_str(), unit);
    }
    catch (...) {
    }
    double absoluteError = value - parsed;
    double relativeError = value == 0 && absoluteError == 0 ? 0 : std::abs(absoluteError / value);
    std::stringstream valuestream;
    valuestream << std::fixed << std::setprecision(24) << value;
    if (unit) {
        text += " " + std::string(unit);
        valuestream << " " << unit;
    }
    double maxRelativeError = std::pow(10, 1 - formatter.getOptions().maxSignificantDigits) / 2;
    int numDigits = 0;
    int numLeadingZeros = 0;
    int numTrailingZeros = 0;
    bool leadingZeros = true;
    bool integer = true;
    for (int i = 0; i < output.role.size(); i++) {
        if (output.role[i] == 'd') {
            if (output.text[i] == '0') {
                if (leadingZeros)
                    numLeadingZeros++;
                else
                    numTrailingZeros++;
            }
            else {
                leadingZeros = false;
                numTrailingZeros = 0;
            }
            numDigits++;
        }
        else if (output.role[i] == '.')
            integer = false;
        else if (output.role[i] == 'e')
            break;
    }
    int numSignificantDigits = numDigits - numLeadingZeros - numTrailingZeros;
    const char *errorMessage = nullptr;
    if (relativeError > maxRelativeError)
        errorMessage = "Relative error too large";
    else if (value != 0 && numSignificantDigits < formatter.getOptions().minSignificantDigits)
        errorMessage = "Too few significant digits";
    else if (!integer && numSignificantDigits > formatter.getOptions().maxSignificantDigits)
        errorMessage = "Too many significant digits";
    std::cout << std::setw(24) << std::left << text << std::setw(40) << std::left << valuestream.str() << std::setw(18) << std::left << output.text
              << std::setw(18) << std::left << parsed << std::setw(18) << std::left << absoluteError << std::setw(18) << std::left << relativeError << (errorMessage ? "!!! " : "") << (errorMessage ? errorMessage : "") << std::endl;
}

static void testQuantityFormatter(QuantityFormatter& formatter, int maxDigits, int minScale, int maxScale, const char *unit, std::vector<std::function<std::string(int)>> computeDigits) {
    std::cout << "Text input, printf-ed double, formatted quantity, parsed formatted quantity, absolute error, relative error" << std::endl;
    for (auto sign : {"", "-"}) {
        for (int numDigits = 1; numDigits <= maxDigits; numDigits++) {
            for (int scale = minScale; scale <= maxScale; scale++) {
                for (auto f : computeDigits) {
                    auto digits = f(numDigits);
                    int shift = scale - numDigits + 1;
                    if (shift > 0)
                        for (int i = 0; i < shift; i++)
                            digits = digits + "0";
                    else if (shift < 0) {
                        int c = -shift - digits.size() + 1;
                        for (int i = 0; i < c; i++)
                            digits = "0" + digits;
                        digits = digits.substr(0, digits.size() + shift) + "." + digits.substr(digits.size() + shift);
                    }
                    digits = sign + digits;
                    testQuantityFormatter(formatter, digits.c_str(), unit);
                }
            }
        }
    }
}

static void testQuantityFormatter(QuantityFormatter& formatter, int size, const char *unit, std::vector<std::function<std::string(int)>> computeDigits) {
    testQuantityFormatter(formatter, size, -size, size, unit, computeDigits);
}

}  // namespace common
}  // namespace omnetpp

#endif

