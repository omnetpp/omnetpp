//=========================================================================
//  QUANTITYFORMATTER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "quantityformatter.h"
#include "stlutil.h"

using namespace std;

namespace omnetpp {
namespace common {

QuantityFormatter::Output QuantityFormatter::formatQuantity(double value, const char *unit)
{
    const Input input(value, unit);
    checkInput(input);
    checkOptions();
    State state;
    computeValueInOutputUnit(input, state);
    determineSign(state);
    if (std::isfinite(state.valueInOutputUnit) && state.valueInOutputUnit != 0) {
        do {
            clearDigitGroups(state);
            clearStreams(state);
            printAbsoluteValueInOutputUnit(state);
            computeScientificNotationExponent(state);
            determineNotation(state);
            applyScientificNotation(state);
            computeRoundedAbsoluteValueInOutputUnit(state);
            computeIntegralPart(state);
            computeFractionalPart(state);
            formatQuantity(input, state);
            fitIntoAvailableSpace(state);
        }
        while (retryFormat(state));
    }
    else {
        formatQuantity(input, state);
        fitIntoAvailableSpace(state);
    }
    return createOutput(state);
}

void QuantityFormatter::checkInput(const Input& input)
{
    if (input.unit && opp_isempty(input.unit))
        throw opp_runtime_error("Unit must be non-empty if provided");
}

void QuantityFormatter::checkOptions()
{
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
        throw opp_runtime_error("Option decimalSeparator must be specified"); //TODO ???
    if (options.exponentMark.empty())
        throw opp_runtime_error("Option exponentMark must be specified");

    if (options.plusSign.empty())
        throw opp_runtime_error("Option plusSign must be specified");
    if (options.minusSign.empty())
        throw opp_runtime_error("Option minusSign must be specified");
}


void QuantityFormatter::clearDigitGroups(State& state)
{
    state.integralPartDigitGroups.clear();
    state.fractionalPartDigitGroups.clear();
}

void QuantityFormatter::clearStreams(State& state)
{
    state.textStream.str("");
    state.roleStream.str("");
}

void QuantityFormatter::determineSign(State& state)
{
    int sign = int(state.valueInOutputUnit > 0) - int(state.valueInOutputUnit < 0);
    switch (options.signMode) {
        case SignMode::FORCE:
            state.sign = sign < 0 ? options.minusSign.c_str() : (sign > 0 ? options.plusSign.c_str() : nullptr);
            break;
        case SignMode::AUTO:
            state.sign = sign < 0 ? options.minusSign.c_str() : nullptr;
            break;
        default:
            throw opp_runtime_error("Unknown signMode: %d", (int)options.signMode);
    }
}

int64_t QuantityFormatter::shiftScore(int64_t score, int64_t increment, int numDigits)
{
    int64_t scale = std::pow(10, numDigits);
    Assert(0 <= increment && increment < scale);
    return score * scale + increment;
}

void QuantityFormatter::computeValueInOutputUnit(const Input& input, State& state)
{
    switch (options.outputUnitMode) {
        case OutputUnitMode::KEEP:
            state.outputUnit = input.unit;
            state.valueInOutputUnit = input.value;
            break;
        case OutputUnitMode::AUTO:
            state.outputUnit = input.unit ? UnitConversion::getBestUnit(input.value, input.unit, opp_cstrings(options.allowedOutputUnits), options.unitConversionOptions) : nullptr;
            state.valueInOutputUnit = UnitConversion::convertUnit(input.value, input.unit, state.outputUnit);
            break;
        default:
            throw opp_runtime_error("Unknown outputUnitMode: %d", (int)options.outputUnitMode);
    }
}

bool QuantityFormatter::roundDigits(State& state, char *start, int length)
{
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

void QuantityFormatter::printAbsoluteValueInOutputUnit(State& state)
{
    //Assert(state.valueInOutputUnit != 0);  -- WRONG, e.g. 1W = 0dBW
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

void QuantityFormatter::computeRoundedAbsoluteValueInOutputUnit(State& state)
{
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

void QuantityFormatter::computeScientificNotationExponent(State& state)
{
    switch (options.exponentSignMode) {
        case SignMode::FORCE:
            state.exponentSign = state.firstFractionalDigit <= state.firstNonZeroDigit ? options.minusSign.c_str() : options.plusSign.c_str();
            break;
        case SignMode::AUTO:
            state.exponentSign = state.firstFractionalDigit <= state.firstNonZeroDigit ? options.minusSign.c_str() : nullptr;
            break;
        default:
            throw opp_runtime_error("Unknown signMode: %d", (int)options.signMode);
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
            throw opp_runtime_error("Unknown scientificExponentMode: %d", (int)options.scientificExponentMode);
    }
}

void QuantityFormatter::determineNotation(State& state)
{
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
            throw opp_runtime_error("Unknown notationMode: %d", (int)options.notationMode);
    }
}

void QuantityFormatter::applyScientificNotation(State& state)
{
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
                throw opp_runtime_error("Unknown scientificExponentMode: %d", (int)options.scientificExponentMode);
        }
    }
}

void QuantityFormatter::fitIntoAvailableSpace(State& state)
{
    if (options.numAvailableCharacters != std::numeric_limits<int>::max() ||
        options.numAvailablePixels != std::numeric_limits<int>::max())
    {
        auto text = state.textStream.str();
        state.textWidth = options.measureText(text.c_str());
        state.textLength = text.size();
    }
}

bool QuantityFormatter::retryFormat(State& state)
{
    bool fitsIntoAvailableSpace = state.textWidth <= options.numAvailablePixels && state.textLength <= options.numAvailableCharacters;
    if (!fitsIntoAvailableSpace && options.maxSignificantDigits - options.minSignificantDigits> state.numRemovedDigitsDueToInsufficientSpace) {
        state.numRemovedDigitsDueToInsufficientSpace++;
        return true;
    }
    else
        return false;
}

void QuantityFormatter::computeIntegralPart(State& state)
{
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

void QuantityFormatter::computeFractionalPart(State& state)
{
    if (state.firstFractionalDigit - state.firstNonZeroDigit < state.numRoundedValueInOutputUnitDigits) {
        int index = state.firstFractionalDigit - state.firstNonZeroDigit;
        while (index < state.numRoundedValueInOutputUnitDigits) {
            const char *startDigit = state.roundedAbsoluteValueInOutputUnit + index;
            const char *endDigit = state.roundedAbsoluteValueInOutputUnit + std::min((int)(index + 3), state.numRoundedValueInOutputUnitDigits);
            state.fractionalPartDigitGroups.push_back(std::string(startDigit, endDigit - startDigit));
            index += 3;
        }
        // eliminate trailing zeros, TODO could be done above if lastNonZeroDigit would be maintained in roundDigits
        for (int j = state.fractionalPartDigitGroups.size() - 1; j >= 0; j--) {
            auto& x = state.fractionalPartDigitGroups[j];
            for (int i = x.size() - 1; i >= 0; i--) {
                if (x[i] == '0')
                    x.erase(x.begin() + i);
                else
                    return;
            }
            state.fractionalPartDigitGroups.erase(state.fractionalPartDigitGroups.begin() + j);
        }
    }
}

// Quantity = Value Space Unit | Value
void QuantityFormatter::formatQuantity(const Input& input, State& state)
{
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
void QuantityFormatter::formatValue(const Input& input, State& state)
{
    if (state.valueInOutputUnit == 0 || !std::isfinite(state.valueInOutputUnit))
        format(state, state.valueInOutputUnit, 'd');
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
void QuantityFormatter::formatRegularNotation(State& state)
{
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
void QuantityFormatter::formatScientificNotation(State& state)
{
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
void QuantityFormatter::formatUnit(State& state)
{
    if (options.useUnitLongName)
        format(state, UnitConversion::getLongName(state.outputUnit, state.valueInOutputUnit != 1), 'u');
    else
        format(state, state.outputUnit, 'u');
}

void QuantityFormatter::format(State& state, const std::string& str, const char part)
{
    state.textStream << str;
    int len = 0;
    for (char b : str)
        if ((b&0xc0) != 0x80) // utf8: count multibyte unicode characters (more precisely, code points) as one character
            len++;
    state.roleStream << std::string(len, part);
}

void QuantityFormatter::format(State& state, double d, const char part)
{
    auto positionBefore = state.textStream.tellp();
    if (std::isfinite(d))
        state.textStream << d;
    else if (std::isnan(d))
        state.textStream << options.nanText;
    else { // inf
        if (d < 0)
            state.textStream << "-";
        state.textStream << options.infText;
    }
    auto positionAfter = state.textStream.tellp();
    state.roleStream << std::string(positionAfter - positionBefore, part);
}

template <typename T>
void QuantityFormatter::format(State& state, const T& t, const char part)
{
    auto positionBefore = state.textStream.tellp();
    state.textStream << t;
    auto positionAfter = state.textStream.tellp();
    state.roleStream << std::string(positionAfter - positionBefore, part);
}

QuantityFormatter::Output QuantityFormatter::createOutput(const State& state)
{
    bool fitsIntoAvailableSpace = state.textWidth <= options.numAvailablePixels && state.textLength <= options.numAvailableCharacters;
    return Output(state.textStream.str(), state.roleStream.str(), state.outputUnit, fitsIntoAvailableSpace);
}

}  // namespace common
}  // namespace omnetpp

