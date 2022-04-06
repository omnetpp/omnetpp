//==========================================================================
//  CHARTTICKDECIMAL.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "charttickdecimal.h"

#include <iostream>
#include <cinttypes>
#include "common/exception.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "omnetpp/opp_string.h"

namespace omnetpp {
namespace qtenv {

using namespace omnetpp::common;

int64_t ChartTickDecimal::pow10(int exponent)
{
    Assert(exponent >= 0);
    Assert(exponent <= 19);

    int64_t result = 1;

    while (exponent-- > 0)
        result *= 10;

    return result;
}

void ChartTickDecimal::normalize()
{
    if (mantissa == 0)
        exponent = 0;
    else
        while (mantissa % 10 == 0) {
            mantissa /= 10;
            exponent += 1;
        }
}

bool ChartTickDecimal::overnormalize(int factor)
{
    if (factor == 0)
        return false;

    Assert(factor > 0);

    if (mantissa == 0) {
        exponent = 0;
        return false;
    }

    bool loss = false;
    for (int steps = 0; steps < factor; ++steps) {
        loss |= ((mantissa % 10) != 0);

        mantissa /= 10;
        exponent += 1;

        if (mantissa == 0) {
            exponent = 0;
            break;
        }
    }

    return loss;
}

int ChartTickDecimal::denormalize(int factor)
{
    if (factor == 0)
        return 0;

    Assert(factor > 0);

    int steps;
    for (steps = 0; steps < factor && std::abs(mantissa) <= maxDenormalizableMantissa; ++steps) {
        mantissa *= 10;
        exponent -= 1;
    }

    return steps;
}

void ChartTickDecimal::setExponent(int newExp)
{
    if (newExp == exponent)
        return;

    if (newExp < exponent)
        denormalize(exponent - newExp);

    if (newExp > exponent)
        overnormalize(newExp - exponent);
}

bool ChartTickDecimal::match(ChartTickDecimal& a, ChartTickDecimal& b)
{
    if (a.exponent == b.exponent)
        return false;

    a.normalize();
    b.normalize();

    // they might have been unnormalized before
    if (a.exponent == b.exponent)
        return false;

    bool lossy = false;

    if (a.exponent < b.exponent) {
        b.denormalize(b.exponent - a.exponent);
        // If b's mantissa couldn't be made large enough, we
        // must make a's smaller, losing precision.
        lossy |= a.overnormalize(b.exponent - a.exponent);
    }
    else {  // (b.exponent < a.exponent)
        a.denormalize(a.exponent - b.exponent);
        // If a's mantissa couldn't be made large enough, we
        // must make b's smaller, losing precision.
        lossy |= b.overnormalize(a.exponent - b.exponent);
    }

    return lossy;
}

ChartTickDecimal::ChartTickDecimal(int64_t man, int exp)
    : mantissa(man), exponent(exp)
{
    normalize();
}

ChartTickDecimal::ChartTickDecimal(double val)
{
    Assert(std::isfinite(val));
    if (val == 0) {
        mantissa = 0;
        exponent = 0;
    }
    else {
        int magnitude = std::ceil(std::log10(std::abs(val)));  // note: if val!=0, magnitude will be within -308..+309 (fits in an int)
        const int maxMagnitude = 14; // we are limited by the poor accuracy of std::pow on Windows (double type in itself would have 15 digit accuracy)
        exponent = magnitude - maxMagnitude;
        mantissa = val * std::pow(10, -exponent);
        normalize();
    }
}

int ChartTickDecimal::magnitudeOf(int64_t x)
{
    int count = 0;
    while (x != 0) {
        x = x / 10;  // not too efficient, too many divisions; could use lookup table
        count++;
    }
    return count;
}

std::string ChartTickDecimal::str() const
{
    // Note: This function decides based on the magnitude of the number alone
    // and ignores the number of trailing zeroes in the mantissa.
    // This is intentionally different from the "choose the shorter representation
    // of the two". The current behavior is more suite to chart ticks, where we
    // don't want tick labels on the same axis to mix notations.
    int magnitude = magnitudeOf(mantissa) + exponent;
    return (magnitude <= -6 || magnitude > 19) ? strE() : strF();
}

std::string ChartTickDecimal::strE() const
{
    if (mantissa == 0)
        return "0e0";

    char buf[32];  // covers sign, max 18 digits, "E" + sign plus 3-digit exponent plus NUL

    int64_t intVal = mantissa;

    // prepare for conversion
    char *endp = buf + sizeof(buf) - 1;
    char *s = endp;

    s -= 6; // leave room for exponent part
    char *expPos = s;
    *expPos = 0;

    // check if negative
    bool negative = intVal < 0;
    if (!negative)
        intVal = -intVal;  // make t negative, otherwise we can't handle INT64_MIN which has no positive equivalent

    // convert digits
    bool skipZeros = true;
    int numDigits = 0;
    do {
        int64_t res = intVal / 10;
        int digit = 10*res - intVal;  // note: intVal is negative!
        if (skipZeros && digit != 0)
            skipZeros = false;
        if (!skipZeros)
            *--s = '0' + digit;
        intVal = res;
        numDigits++;
    } while (intVal);

    // insert decimal point between first and second digit (if it exists)
    if (*(s+1)) {
        char firstDigit = *s;
        *s = '.';
        *--s = firstDigit;
    }

    if (negative)
        *--s = '-';

    // print exponent at the end
    int scale = exponent + numDigits - 1;
    *expPos++ = 'e';
    opp_itoa(expPos, scale);

    return s;
}

std::string ChartTickDecimal::strF() const
{
    int bufSize = abs(exponent) + 24;  // 24: max number of digits in an int64, plus sign, NUL and some slack
    opp_string tmp(nullptr, bufSize);

    int64_t intVal = mantissa;
    int scale = exponent;

    // prepare for conversion
    char *endp = tmp.buffer() + bufSize - 1;
    *endp = '\0';
    char *s = endp;
    if (intVal == 0) {
        *--s = '0';
        return s;
    }

    // check if negative
    bool negative = intVal < 0;
    if (!negative)
        intVal = -intVal;  // make t negative, otherwise we can't handle INT64_MIN which has no positive equivalent

    // add trailing zeroes
    for (int i = 0; i < scale; ++i)
        *--s = '0';

    // convert digits
    bool skipZeros = true;
    int decimalPlace = scale;
    do {
        int64_t res = intVal / 10;
        int digit = 10*res - intVal;  // note: intVal is negative!
        if (skipZeros && (digit != 0 || decimalPlace >= 0))
            skipZeros = false;
        if (decimalPlace++ == 0 && s != endp)
            *--s = '.';
        if (!skipZeros)
            *--s = '0' + digit;
        intVal = res;
    } while (intVal);

    // add leading zeros, decimal point, etc if needed
    if (decimalPlace <= 0) {
        while (decimalPlace++ < 0)
            *--s = '0';
        *--s = '.';
        *--s = '0';
    }

    if (negative)
        *--s = '-';

    return s;
}

std::string ChartTickDecimal::strR() const
{
    return opp_stringf("%" PRIi64 "e%d", mantissa, exponent);
}

ChartTickDecimal ChartTickDecimal::over2() const
{
    if (mantissa % 2 == 0)
        return ChartTickDecimal(mantissa / 2, exponent);
    else {
        ChartTickDecimal result(mantissa, exponent);

        result.denormalize(1);  // this might not take place if mantissa is too big
        result.mantissa /= 2;  // and in that case, this will truncate, losing precision

        // might be necessary if loss occurred
        // (the mantissa was so big that denormalize couldn't do anything)
        result.normalize();

        return result;
    }
}

ChartTickDecimal ChartTickDecimal::over5() const
{
    if (mantissa % 5 == 0)
        return ChartTickDecimal(mantissa / 5, exponent);
    else {
        ChartTickDecimal result(mantissa, exponent);

        result.denormalize(1);  // this might not take place if mantissa is too big
        result.mantissa /= 5;  // and in that case, this will truncate, losing precision

        // might be necessary if loss occurred
        // (the mantissa was so big that denormalize couldn't do anything)
        result.normalize();

        return result;
    }
}

ChartTickDecimal ChartTickDecimal::over10() const
{
    return ChartTickDecimal(mantissa, exponent - 1);
}

bool ChartTickDecimal::multipleOf(ChartTickDecimal base) const
{
    if (base.mantissa == 0)
        return mantissa == 0;

    ChartTickDecimal clone(*this), baseClone(base);

    bool lossy = match(clone, baseClone);

    if (!lossy) // this is the easy case
        return (clone.mantissa % baseClone.mantissa) == 0;

    // even if we couldn't bring them to the _same_ exponent
    // in a lossless way, if this has a bigger exponent than
    // base, and the mantissa is divisible by base's mantissa, we win
    ChartTickDecimal clone2(*this);

    bool mantissaMultiple = (clone2.mantissa % base.mantissa) == 0;
    if (mantissaMultiple && (clone2.exponent >= base.exponent))
        return true;

    // and just as a last chance, try a different ratio of mantissas and exponents...
    // (this could be done one step at a time, in some other smarter way
    // to increase the chance of success)
    clone2.denormalize(100); // inflate the mantissa as much as we can
    base.normalize();

    mantissaMultiple = (clone2.mantissa % base.mantissa) == 0;
    if (mantissaMultiple && (clone2.exponent >= base.exponent))
        return true;

    // if nothing worked so far, let's give up...
    throw opp_runtime_error("ChartTickDecimal::multipleOf: Unhandled case. Numbers cannot be brought to the same exponent.");

    return false;
}

ChartTickDecimal ChartTickDecimal::floor(int precision) const
{
    int exponentDiff = -precision - exponent;

    // we don't even have that many digits stored to need adjustment
    if (exponentDiff <= 0)
        return *this;

    int64_t newMantissa;
    bool lossOccurred;

    if (exponentDiff <= 19) {
        int64_t scale = pow10(exponentDiff);
        newMantissa = mantissa / scale;

        lossOccurred = newMantissa * scale != mantissa;
    } else {
        newMantissa = 0;
        lossOccurred = true;
    }

    if (lossOccurred && mantissa < 0)
        newMantissa -= 1;

    return ChartTickDecimal(newMantissa, -precision);
}

ChartTickDecimal ChartTickDecimal::ceil(int precision) const
{
    int exponentDiff = -precision - exponent;

    // we don't even have that many digits stored to need adjustment
    if (exponentDiff <= 0)
        return *this;

    int64_t newMantissa;
    bool lossOccurred;

    if (exponentDiff <= 19) {
        int64_t scale = pow10(exponentDiff);
        newMantissa = mantissa / scale;

        lossOccurred = newMantissa * scale != mantissa;
    } else {
        newMantissa = 0;
        lossOccurred = true;
    }

    if (lossOccurred && mantissa > 0)
        newMantissa += 1;

    return ChartTickDecimal(newMantissa, -precision);
}

ChartTickDecimal& ChartTickDecimal::operator+=(ChartTickDecimal inc)
{
    if (inc.mantissa == 0)
        return *this;

    if (mantissa == 0) {
        mantissa = inc.mantissa;
        exponent = inc.exponent;
        normalize();
        return *this;
    }

    match(*this, inc);
    mantissa += inc.mantissa;
    normalize();
    return *this;
}

bool ChartTickDecimal::operator ==(const ChartTickDecimal& other) const
{
    ChartTickDecimal thisClone = *this;
    ChartTickDecimal otherClone = other;

    thisClone.normalize();
    otherClone.normalize();

    return (thisClone.mantissa == otherClone.mantissa) && (thisClone.exponent == otherClone.exponent);
}

bool ChartTickDecimal::operator <(const ChartTickDecimal& other) const
{
    ChartTickDecimal thisClone = *this;
    ChartTickDecimal otherClone = other;

    match(thisClone, otherClone);

    return thisClone.mantissa < otherClone.mantissa;
}

bool ChartTickDecimal::operator >(const ChartTickDecimal &other) const
{
    ChartTickDecimal thisClone = *this;
    ChartTickDecimal otherClone = other;

    match(thisClone, otherClone);

    return thisClone.mantissa > otherClone.mantissa;
}

bool ChartTickDecimal::operator <(double other) const
{
    if (std::isfinite(other))
        return *this < ChartTickDecimal(other);
    else {
        Assert(std::isinf(other)); // NaN is not accepted
        return other > 0;
    }
}

bool ChartTickDecimal::operator <=(double other) const
{
    if (std::isfinite(other))
        return *this <= ChartTickDecimal(other);
    else {
        Assert(std::isinf(other)); // NaN is not accepted
        return other > 0;
    }
}

}  // namespace qtenv
}  // namespace omnetpp
