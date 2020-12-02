//==========================================================================
//  CHARTTICKDECIMAL.H - part of
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

#ifndef __OMNETPP_QTENV_CHARTTICKDECIMAL_H
#define __OMNETPP_QTENV_CHARTTICKDECIMAL_H

#include <string>
#include <cstdint>
#include <cmath>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class QTENV_API ChartTickDecimal
{
    // value == mantissa * pow(10, exponent)
    int64_t mantissa = 0;
    int exponent = 0;

    // This is floor((2^63 - 1) / 10), the largest number that can
    // be multiplied by 10 and still have the result fit in an int64.
    static const int64_t maxDenormalizableMantissa = 922337203685477580L;

    static int64_t pow10(int exponent);
    static int magnitudeOf(int64_t x);  // floor(log10(abs(x))), i.e. number of digits (but zero for 0)

    // Makes the mantissa as small as possible by increasing the exponent.
    // This is a harmless, lossless operation, it can't fail.
    void normalize();

    // Makes the mantissa smaller, most likely truncating it, increasing the exponent by factor.
    // Precision can be lost, the number might even be reduced to zero (with 0 as exponent).
    // If loss occurs, the value is always rounded toward zero (truncated).
    // Returns true if precision really was lost (which will happen in most cases,
    // if the number was normalized before, and factor is positive).
    // The factor parameter can't be negative.
    bool overnormalize(int factor);

    // Makes the mantissa unnecessarily big, decreasing the exponent by factor if possible.
    // Stops early if the absolute value of the mantissa would become too big to
    // represent in int64, so overflowing can't occur.
    // Precision is never lost, since this is a best-effort operation.
    // Returns the actual change to the exponent, which is always non-negative.
    // The factor parameter can't be negative.
    int denormalize(int factor);

    void setExponent(int newExp);

    // Will (de|over)?normalize the parameters so that they have the same exponent.
    // Precision loss can occur, and if they are many (close to 20 or more) orders
    // of magnitude apart, the smaller one (in absolute value) might diminish to zero.
    static bool match(ChartTickDecimal& a, ChartTickDecimal& b);

  public:

    ChartTickDecimal() {}
    ChartTickDecimal(const ChartTickDecimal& other) = default;
    ChartTickDecimal(int64_t man, int exp);
    explicit ChartTickDecimal(double val); // val must be finite

    int64_t getMantissa() const { return mantissa; }
    int64_t getExponent() const { return exponent; }

    double dbl() const { return mantissa * std::pow(10, exponent); }

    std::string str() const;  // choose either strE() or strF(), based on exponent of the data
    std::string strE() const; // scientific notation
    std::string strF() const; // common notation
    std::string strR() const; // raw form of stored integers

    ChartTickDecimal over2() const;
    ChartTickDecimal over5() const;
    ChartTickDecimal over10() const;

    bool multipleOf(ChartTickDecimal base) const;

    // Rounding to -3 will round to thousands,
    // rounding to 6 will round to 6 fractional digits, etc.
    // Note that this is inverse compared to the exponent.
    ChartTickDecimal floor(int precision) const;
    ChartTickDecimal ceil(int precision) const;

    ChartTickDecimal& operator+=(ChartTickDecimal inc);

    bool operator ==(const ChartTickDecimal& other) const;
    bool operator !=(const ChartTickDecimal& other) const { return !(*this == other); }

    bool operator <(const ChartTickDecimal& other) const;
    bool operator <=(const ChartTickDecimal& other) const { return (*this == other) || (*this < other); }

    bool operator >(const ChartTickDecimal& other) const;
    bool operator >=(const ChartTickDecimal& other) const  { return (*this == other) || (*this > other); }

    bool operator <(double other) const;
    bool operator <=(double other) const;
    bool operator >(double other) const { return !(*this <= other); }
    bool operator >=(double other) const { return !(*this < other); }
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
