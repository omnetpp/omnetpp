//==========================================================================
//  BIGDECIMAL.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_BIGDECIMAL_H
#define __OMNETPP_COMMON_BIGDECIMAL_H

#include <string>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <climits>
#include "omnetpp/platdep/intlimits.h"
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

// maximum number of digits in an int64_t number, i.e. number of digits in INT64_MAX.
#define INT64_MAX_DIGITS 19

/**
 * BigDecimal stores a decimal value as a 64-bit integer and a scale.
 * Arithmetic operations are performed by converting the values
 * to double and converting the result to BigDecimal, so they
 * may lose precision.
 */
class COMMON_API BigDecimal
{
  private:
    /*
     * The value of the number is intVal * 10^scale.
     * If two decimal is equal then they have the same intVal and scale (see normalize()).
     * Special values (NaN, +-infinity, Nil) use the scale INT_MAX.
     */
    int64_t intVal; // stores digits of the decimal number (up to 18 digits can be stored)
    int scale;    // stores the position of the decimal point, must be in the [0,-18] range

    static const int minScale = -18; // XXX the range length must be <= 18, see buffer allocation in ttoa()
    static const int maxScale = 0;

    void checkScale(int scale) const
    {
        if (scale < minScale || scale > maxScale)
            throw opp_runtime_error("BigDecimal: scale must be between %d and %d", minScale, maxScale);
    }

    /*
     * Sets scale between minScale and maxScale and strips trailing zeros when scale<maxScale.
     * 0.0 always has scale=0.
     * If x and y equal decimals, then x.intVal==y.intVal and x.scale==y.scale after normalization.
     */
    void normalize();

  public:
    /** @name Constants */
    //@{
    static const BigDecimal Zero;
    static const BigDecimal One;
    static const BigDecimal MinusOne;
    static const BigDecimal NaN;
    static const BigDecimal PositiveInfinity;
    static const BigDecimal NegativeInfinity;
    static const BigDecimal Nil;
    //@}

    /** @name Constructors. */
    //@{
    BigDecimal() {intVal=INT64_MAX; scale=INT_MAX;} // == Nil
    explicit BigDecimal(int64_t intVal, int scale=0) : intVal(intVal), scale(scale) { normalize(); }
    BigDecimal(const BigDecimal &x) {operator=(x);}
    BigDecimal(double d) {operator=(d);}
    //@}

    /** @name Testing for special values */
    //@{
    bool isNaN() const { return scale == INT_MAX && intVal == 0; }
    bool isNil() const { return *this == Nil; }
    bool isPositiveInfinity() const {return *this==PositiveInfinity;}
    bool isNegativeInfinity() const {return *this==NegativeInfinity;}
    bool isInfinity() const {return isPositiveInfinity() || isNegativeInfinity();}
    bool isSpecial() const { return scale == INT_MAX; }
    //@}

    /** @name Assignments */
    //@{
    BigDecimal& operator=(double d);
    BigDecimal& operator=(const BigDecimal& x) = default;
    //@}

    /** @name Arithmetic operations */
    //@{
    const BigDecimal& operator+=(const BigDecimal& x) {*this=*this+x; return *this;}
    const BigDecimal& operator-=(const BigDecimal& x) {*this=*this-x; return *this;}
    const BigDecimal& operator*=(double d) {*this=BigDecimal(dbl()*d); return *this;}
    const BigDecimal& operator/=(double d) {*this=BigDecimal(dbl()/d); return *this;}

#define CHK(a,b) { if ((a).isNaN() || (b).isNaN()) return false; }

    bool operator==(const BigDecimal& x) const  {CHK(*this,x); return intVal == x.intVal && scale == x.scale;}
    bool operator!=(const BigDecimal& x) const  {CHK(*this, x); return intVal != x.intVal || scale != x.scale;}
    bool operator< (const BigDecimal& x) const;
    bool operator> (const BigDecimal& x) const  {CHK(*this, x); return x < *this;}
    bool operator<=(const BigDecimal& x) const  {CHK(*this, x); return *this == x || *this < x;}
    bool operator>=(const BigDecimal& x) const  {CHK(*this, x); return *this == x || *this > x;}

    COMMON_API friend const BigDecimal operator+(const BigDecimal& x, const BigDecimal& y);
    COMMON_API friend const BigDecimal operator-(const BigDecimal& x, const BigDecimal& y);

    friend const BigDecimal operator*(const BigDecimal& x, double d);
    friend const BigDecimal operator*(double d, const BigDecimal& x);
    friend const BigDecimal operator/(const BigDecimal& x, double d);
    friend const BigDecimal operator/(const BigDecimal& x, const BigDecimal& y);
    //@}

    /** @name Conversions */
    //@{
    /**
     * Converts big decimal to double. Note that conversion to and from
     * double may lose precision.
     */
    double dbl() const;

    /**
     * Converts to string.
     */
    std::string str() const;

    /**
     * Converts to a string. Use this variant over str() where performance is
     * critical. The result is placed somewhere in the given buffer (pointer
     * is returned), but for performance reasons, not necessarily at the buffer's
     * beginning. Please read the documentation of ttoa() for the minimum
     * required buffer size.
     */
    char *str(char *buf) const {char *endp; return BigDecimal::ttoa(buf, *this, endp);}

    /**
     * Returns the underlying 64-bit integer.
     */
    int64_t getIntValue() const  {return intVal;}

    /**
     * Directly sets the underlying 64-bit integer.
     */
    const BigDecimal& setIntValue(int64_t l) {intVal = l; return *this;}

    /**
     * Returns the scale exponent.
     */
    int getScale() const {return scale;}

    /**
     * Sets the scale exponent.
     */
    void setScale(int s) { checkScale(s); scale = s; normalize(); };

    /**
     * Returns the mantissa as if scale was the given value;
     */
    int64_t getMantissaForScale(int scale) const;

    /**
     * Converts the given string to big decimal. Throws an error if
     * there is an error during conversion.
     */
    static const BigDecimal parse(const char *s);

    /**
     * Converts a prefix of the given string to big decimal, up to the
     * first character that cannot occur in big decimal strings:
     * not (digit or letter or "." or "+" or "-" or whitespace).
     */
    static const BigDecimal parse(const char *s, const char *&endp);

    /**
     * Utility function to convert a big decimal into a string
     * buffer. scaleexp must be in the -18..0 range, and the buffer must be
     * at least 64 bytes long. A pointer to the result string will be
     * returned. A pointer to the terminating '\0' will be returned in endp.
     *
     * ATTENTION: For performance reasons, the returned pointer will point
     * *somewhere* into the buffer, but NOT necessarily at the beginning.
     */
    static char *ttoa(char *buf, const BigDecimal &x, char *&endp);
    //@}
};

inline bool haveSameSign(int64_t a, int64_t b) { return (a^b) >= 0; }

inline const BigDecimal operator*(const BigDecimal& x, double d)
{
    return BigDecimal(x.dbl()*d);
}

inline const BigDecimal operator*(double d, const BigDecimal& x)
{
    return BigDecimal(d*x.dbl());
}

inline const BigDecimal operator/(const BigDecimal& x, double d)
{
    return BigDecimal(x.dbl()/d);
}

inline const BigDecimal operator/(const BigDecimal& x, const BigDecimal& y)
{
    return BigDecimal(x.dbl()/y.dbl());
}

inline std::ostream& operator<<(std::ostream& os, const BigDecimal& x)
{
    char buf[64]; char *endp;
    return os << BigDecimal::ttoa(buf, x, endp);
}

#ifndef SWIG

/**
 * BigDecimal version of floor(double) from math.h.
 */
inline const BigDecimal floor(const BigDecimal& x)
{
    return BigDecimal(std::floor(x.dbl()));
}

/**
 * BigDecimal version of ceil(double) from math.h.
 */
inline const BigDecimal ceil(const BigDecimal& x)
{
    return BigDecimal(std::ceil(x.dbl()));
}

/**
 * BigDecimal version of fabs(double) from math.h.
 */
inline const BigDecimal fabs(const BigDecimal& x)
{
    return x.getIntValue()<0 ? BigDecimal(x).setIntValue(-x.getIntValue()) : x;
}

/**
 * BigDecimal version of fmod(double,double) from math.h.
 */
inline const BigDecimal fmod(const BigDecimal& x, const BigDecimal& y)
{
    return BigDecimal(std::fmod(x.dbl(), y.dbl()));
}

/**
 * Returns the greater of the two arguments.
 */
inline const BigDecimal max(const BigDecimal& x, const BigDecimal& y)
{
    return x > y ? x : y;
}

/**
 * Returns the smaller of the two arguments.
 */
inline const BigDecimal min(const BigDecimal& x, const BigDecimal& y)
{
    return x < y ? x : y;
}

#endif // SWIG

}  // namespace common
}  // namespace omnetpp

#endif


