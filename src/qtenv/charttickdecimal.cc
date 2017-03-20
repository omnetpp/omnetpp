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
#include "common/exception.h"
#include "common/commonutil.h"
#include "omnetpp/opp_string.h"

namespace omnetpp {
namespace qtenv {

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
    int magnitude = std::ceil(std::log10(std::abs(val)));
    const int maxMagnitude = 15; // the double type has this many digits of precision
    exponent = magnitude - maxMagnitude;
    mantissa = val * std::pow(10, -exponent);
    normalize();
}

std::string ChartTickDecimal::str() const
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

    // convert digits
    bool negative = intVal < 0;
    if (negative)
        intVal = -intVal;

    bool skipzeros = true;
    int decimalplace = scale;

    for (int i = 0; i < decimalplace; ++i)
        *--s = '0';

    do {
        int64_t res = intVal / 10;
        int digit = intVal - (10*res);

        if (skipzeros && (digit != 0 || decimalplace >= 0))
            skipzeros = false;
        if (decimalplace++ == 0 && s != endp)
            *--s = '.';
        if (!skipzeros)
            *--s = '0'+digit;
        intVal = res;
    } while (intVal);

    // add leading zeros, decimal point, etc if needed
    if (decimalplace <= 0) {
        while (decimalplace++ < 0)
            *--s = '0';
        *--s = '.';
        *--s = '0';
    }

    if (negative)
        *--s = '-';

    return s;
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

    bool mantissaMultiple = (mantissa % base.mantissa) == 0;

    if (mantissaMultiple && (exponent >= base.exponent))
        return true;

    ChartTickDecimal clone;
    clone.mantissa = mantissa;  // not using the ctor to avoid one unnecessary
    clone.exponent = exponent;  // (and potentially harmful (to perf only)) call to normalize

    bool lossy = match(clone, base);

    if (lossy)
        return false;

    if (base.mantissa == 0)
        return clone.mantissa == 0;

    return (clone.mantissa % base.mantissa) == 0;
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
    match(*this, inc);
    mantissa += inc.mantissa;
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

}  // namespace qtenv
}  // namespace omnetpp
