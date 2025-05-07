//==========================================================================
//   SIMTIME.H  - part of
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

#ifndef __OMNETPP_SIMTIME_H
#define __OMNETPP_SIMTIME_H

#include <string>
#include <iostream>
#include <cstdint>
#include "platdep/intlimits.h"
#include "simkerneldefs.h"

namespace omnetpp {

class cPar;

// the most positive int64_t value, represented as double
#define INT64_MAX_DBL  9.22337203685e18

/**
 * @brief Enum for simulation time base-10 exponents.
 * @ingroup SimTime
 */
enum SimTimeUnit
{
    SIMTIME_S  =  0,
    SIMTIME_MS = -3,
    SIMTIME_US = -6,
    SIMTIME_NS = -9,
    SIMTIME_PS = -12,
    SIMTIME_FS = -15,
    SIMTIME_AS = -18
};

/**
 * @brief int64_t-based, base-10 fixed-point simulation time.
 *
 * Precision is determined by a scale exponent, which is global (shared by
 * all SimTime instances), and falls in the range -18..0. For example, a
 * scale exponent of -6 means microsecond precision.
 *
 * Conversions and arithmetic operations are provided specifically for double
 * and cPar, and via template functions for integer types. Performance note:
 * conversion from double uses the scale stored as double, and conversion from
 * integer types uses the scale stored as int64_t; the former eliminates an
 * int64_t-to-double conversion, and the latter allows the compiler
 * to optimize expressions like `if (time>0)` to a trivial integer
 * operation, without floating-point or int64_t multiplication.
 *
 * The underlying raw 64-bit integer is also made accessible.
 *
 * @ingroup SimTime
 */
class SIM_API SimTime
{
  private:
    int64_t t;

    static int scaleexp;     // scale exponent in the range -18..0
    static int64_t dscale;   // 10^-scaleexp, that is 1 or 1000 or 1000000...
    static double fscale;    // 10^-scaleexp, that is 1 or 1000 or 1000000...
    static double invfscale; // 1/fscale; we store it because floating-point multiplication is faster than division
    static int64_t maxseconds; // the largest number in seconds that can be represented with this scaleexp
    static bool checkmul;    // when true, multiplications are checked for integer overflow

    static const int SCALEEXP_UNINITIALIZED = 0xffff;

  public:
    /**
     * Represents the zero simulation time. Using SimTime::ZERO may be faster
     * than writing SimTime() or conversion from 0.0, because the constructor
     * call may be spared.
     */
    static const SimTime ZERO;

  protected:
    static void resetScale() {scaleexp = SCALEEXP_UNINITIALIZED;} // for unit tests only

  private:
    template<typename T> void assertInited(T d) {if (scaleexp==SCALEEXP_UNINITIALIZED) initError(d);}
    [[noreturn]] void initError(double d);

    bool haveSameSign(int64_t a, int64_t b) { return (a^b) >= 0; }

    void fromInt64WithUnit(int64_t d, SimTimeUnit unit);
    void fromUint64WithUnit(uint64_t d, SimTimeUnit unit);

    int64_t toInt64(double d) {
         d = floor(d + 0.5);
         if (fabs(d) <= INT64_MAX_DBL)
             return (int64_t)d;
         else // out of range or NaN
             rangeErrorInt64(d);
    }

    int64_t fromUint64(uint64_t u) {
        if (u > (uint64_t)INT64_MAX)
            rangeErrorInt64(u); //TODO
        return (int64_t)u;
    }

    void setSeconds(int64_t sec) {
        if (sec > maxseconds || sec < -maxseconds)
            rangeErrorSeconds(sec);
        t = dscale * sec;
    }

    void setSecondsU(uint64_t sec) {
        if (sec > (uint64_t)maxseconds)
            rangeErrorSeconds(sec);
        t = dscale * sec;
    }

    void checkedAdd(const SimTime& x) {
        // if operands are the same sign but result has different sign ==> overflow
        bool sameSign = haveSameSign(t, x.t);
        t += x.t;
        if (sameSign && !haveSameSign(t, x.t))
            overflowAdding(x);
    }

    void checkedSub(const SimTime& x) {
        // if operands are different signs and result has same sign as x ==> overflow
        bool differentSign = !haveSameSign(t, x.t);
        t -= x.t;
        if (differentSign && haveSameSign(t, x.t))
            overflowSubtracting(x);
    }

    void checkedMul(int64_t x);

    [[noreturn]] void rangeErrorInt64(double i64);
    [[noreturn]] void rangeErrorSeconds(int64_t x);
    [[noreturn]] void overflowAdding(const SimTime& x);
    [[noreturn]] void overflowSubtracting(const SimTime& x);
    [[noreturn]] void overflowNegating();

  public:
    // internal: Formats a time string. Use SIMTIME_MS etc constants for prec. Note: it performs TRUNCATION towards zero, not rounding!
    std::string format(int prec=getScaleExp(), const char *decimalSep=".", const char *digitSep="", bool addUnits=false, const char *beforeUnit=nullptr, const char *afterUnit=nullptr) const;

  public:
    /** @name Constructors */
    //@{
    /**
     * Initialize to zero.
     */
    SimTime(): t(0) {}

    /**
     * Initialize simulation time from a double-precision number. The value is
     * understood in seconds. This constructor is recommended if the value is
     * the result of some computation done in `double`. See also the
     * `SimTime(double x, int exponent)` constructor.
     */
    SimTime(double d) {operator=(d);}

    /**
     * This and the subsequent constructors initialize simulation time from an
     * integer number of seconds.
     */
    SimTime(short d) {operator=(d);}
    SimTime(int d) {operator=(d);}
    SimTime(long d) {operator=(d);}
    SimTime(long long d) {operator=(d);}
    SimTime(unsigned short d) {operator=(d);}
    SimTime(unsigned int d) {operator=(d);}
    SimTime(unsigned long d) {operator=(d);}
    SimTime(unsigned long long d) {operator=(d);}

    /**
     * Initialize simulation time from a module or channel parameter via a
     * double or intval_t conversion, depending on the parameter's type. It
     * currently does not check the measurement unit of the parameter (@unit NED
     * property), although this may change in future releases.
     */
    SimTime(const cPar& d) {operator=(d);}

    /**
     * Initialize simulation time from a value specified in the given unit.
     * This constructor allows one to specify non-integer constants more
     * conveniently and possibly more precisely than the plain `double`
     * constructor. By default, i.e. when invoked with allowRounding=false, an
     * effort is made to ensure that the simulation time precisely represents
     * the given value. (If it cannot, an exception will be thrown.) With
     * allowRounding=true, rounding errors are allowed.
     *
     * Note that the unit parameter actually represents a base-10 exponent, so
     * the constructor will also work correctly for values not in the
     * SimTimeUnit enum.
     *
     * Example: SimTime(3.5, SIMTIME_MS) -> 3500us
     */
    SimTime(double value, SimTimeUnit unit, bool allowRounding=false);

    /**
     * This and the subsequent constructors initialize simulation time from an
     * integer specified in the given unit. The intended purpose of this
     * constructor is to allow specifying precise constants in model code. (The
     * functions throws an an exception if the value cannot be precisely
     * represented as simulation time.)
     *
     * Note that the unit parameter actually represents a base-10 exponent, so
     * the constructor also accepts integers not in the SimTimeUnit enum.
     */
    SimTime(short d, SimTimeUnit unit) {fromInt64WithUnit(d, unit);}
    SimTime(int d, SimTimeUnit unit) {fromInt64WithUnit(d, unit);}
    SimTime(long d, SimTimeUnit unit) {fromInt64WithUnit(d, unit);}
    SimTime(long long d, SimTimeUnit unit) {fromInt64WithUnit(d, unit);}
    SimTime(unsigned short d, SimTimeUnit unit) {fromUint64WithUnit(d, unit);}
    SimTime(unsigned int d, SimTimeUnit unit) {fromUint64WithUnit(d, unit);}
    SimTime(unsigned long d, SimTimeUnit unit) {fromUint64WithUnit(d, unit);}
    SimTime(unsigned long long d, SimTimeUnit unit) {fromUint64WithUnit(d, unit);}

    /**
     * Copy constructor.
     */
    SimTime(const SimTime& x) {t=x.t;}
    //@}

    /** @name Arithmetic operations */
    //@{
    SimTime& operator=(const SimTime& x) = default;
    SimTime& operator=(const cPar& d);
    SimTime& operator=(double d) {assertInited(d); t=toInt64(fscale*d); return *this;}
    SimTime& operator=(short d) {assertInited(d); setSeconds(d); return *this;}
    SimTime& operator=(int d) {assertInited(d); setSeconds(d); return *this;}
    SimTime& operator=(long d) {assertInited(d); setSeconds(d); return *this;}
    SimTime& operator=(long long d) {assertInited(d); setSeconds(d); return *this;}
    SimTime& operator=(unsigned short d) {assertInited(d); setSecondsU(d); return *this;}
    SimTime& operator=(unsigned int d) {assertInited(d); setSecondsU(d); return *this;}
    SimTime& operator=(unsigned long d) {assertInited(d); setSecondsU(d); return *this;}
    SimTime& operator=(unsigned long long d) {assertInited(d); setSecondsU(d); return *this;}

    bool operator==(const SimTime& x) const  {return t==x.t;}
    bool operator!=(const SimTime& x) const  {return t!=x.t;}
    bool operator< (const SimTime& x) const  {return t<x.t;}
    bool operator> (const SimTime& x) const  {return t>x.t;}
    bool operator<=(const SimTime& x) const  {return t<=x.t;}
    bool operator>=(const SimTime& x) const  {return t>=x.t;}

    SimTime operator-() const {SimTime x; x.t = -t; if (x.t==INT64_MIN) x.overflowNegating(); return x;}

    const SimTime& operator+=(const SimTime& x) {checkedAdd(x); return *this;}
    const SimTime& operator-=(const SimTime& x) {checkedSub(x); return *this;}
    friend const SimTime operator+(const SimTime& x, const SimTime& y)  { return SimTime(x)+=y; }
    friend const SimTime operator-(const SimTime& x, const SimTime& y) { return SimTime(x)-=y; }

    const SimTime& operator*=(double d) {t=toInt64(t*d); return *this;}
    const SimTime& operator*=(short d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(int d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(long d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(long long d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(unsigned short d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(unsigned int d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(unsigned long d) {if (checkmul) checkedMul(d); else t*=d; return *this;}
    const SimTime& operator*=(unsigned long long d) {if (checkmul) checkedMul(d); else t*=d; return *this;} //TODO check overflow from unsigned->signed conversion
    const SimTime& operator*=(const cPar& p);

    const SimTime& operator/=(double d) {t=toInt64(t/d); return *this;}
    const SimTime& operator/=(short d) {t/=d; return *this;}
    const SimTime& operator/=(int d) {t/=d; return *this;}
    const SimTime& operator/=(long d) {t/=d; return *this;}
    const SimTime& operator/=(long long d) {t/=d; return *this;}
    const SimTime& operator/=(unsigned short d) {t/=d; return *this;}
    const SimTime& operator/=(unsigned int d) {t/=d; return *this;}
    const SimTime& operator/=(unsigned long d) {t/=d; return *this;}
    const SimTime& operator/=(unsigned long long d) {t/=d; return *this;}
    const SimTime& operator/=(const cPar& p);

    friend const SimTime operator*(const SimTime& x, double d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, short d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, int d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, long d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, long long d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, unsigned short d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, unsigned int d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, unsigned long d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, unsigned long long d) { return SimTime(x)*=d; }
    friend const SimTime operator*(const SimTime& x, const cPar& p) { return SimTime(x)*=p; }

    friend const SimTime operator*(double d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(short d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(int d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(long d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(long long d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(unsigned short d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(unsigned int d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(unsigned long d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(unsigned long long d, const SimTime& x) { return SimTime(x)*=d; }
    friend const SimTime operator*(const cPar& p, const SimTime& x) { return SimTime(x)*=p; }

    friend const SimTime operator/(const SimTime& x, double d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, short d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, int d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, long d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, long long d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, unsigned short d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, unsigned int d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, unsigned long d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, unsigned long long d) { return SimTime(x)/=d; }
    friend const SimTime operator/(const SimTime& x, const cPar& p) { return SimTime(x)/=p; }

    friend double operator/(const SimTime& x, const SimTime& y) { return (double)x.raw() / (double)y.raw(); }

    friend double operator/(double x, const SimTime& y) { return x / y.dbl(); }
    friend double operator/(short x, const SimTime& y) { return (long long)x / y; }
    friend double operator/(int x, const SimTime& y) { return (long long)x / y; }
    friend double operator/(long x, const SimTime& y) { return (long long)x / y; }
    friend SIM_API double operator/(long long x, const SimTime& y);
    friend double operator/(unsigned short x, const SimTime& y) { return (unsigned long long)x / y; }
    friend double operator/(unsigned int x, const SimTime& y) { return (unsigned long long)x / y; }
    friend double operator/(unsigned long x, const SimTime& y) { return (unsigned long long)x / y; }
    friend SIM_API double operator/(unsigned long long x, const SimTime& y);
    friend SIM_API double operator/(const cPar& p, const SimTime& x);
    //@}

    /** @name Misc operations and utilities */
    //@{

    /**
     * Returns true if this simulation time is zero, false otherwise. This is
     * more efficient than comparing the variable to a (double) 0.0, and shorter
     * than comparing against SimTime::ZERO.
     */
    bool isZero() const {return t==0;}

    /**
     * Converts simulation time (in seconds) to a double. Note that conversion to
     * and from double may lose precision. We do not provide implicit conversion
     * to double as it would conflict with other overloaded operators, and would
     * cause ambiguities during compilation.
     */
    double dbl() const  {return t*invfscale;}

    /**
     * Converts the simulation time to the given time unit, discarding the
     * fractional part (i.e. rounding towards zero). If the return type is
     * not wide enough to hold the result, an error will be thrown.
     *
     * Examples:
     *   1.7ms in us --> 1700;
     *   3.8ms in ms --> 3;
     *   -3.8ms in ms --> -3;
     *   999ms in s --> 0
     */
    int64_t inUnit(SimTimeUnit unit) const;

    /**
     * Returns a new simulation time that is truncated (rounded towards zero)
     * to the precision of the specified time unit.
     *
     * Examples:
     *   3750ms truncated to s --> 3;
     *   -3750ms truncated to s --> -3
     */
    SimTime trunc(SimTimeUnit unit) const  {return SimTime(inUnit(unit), unit);}

    /**
     * Returns a simtime that is the difference between the simulation time and
     * its truncated value (see trunc()).
     *
     * That is, t == t.trunc(unit) + t.remainderforUnit(unit) for any unit.
     */
    SimTime remainderForUnit(SimTimeUnit unit) const  {return (*this) - trunc(unit);}

    /**
     * Convenience method: splits the simulation time into a whole and a
     * fractional part with regard to a time unit.
     * `t.split(exponent, outValue, outRemainder)` is equivalent to:
     *
     * ```
     * outValue = t.inUnit(unit);
     * outRemainder = t.remainderForUnit(unit);
     * ```
     */
    void split(SimTimeUnit unit, int64_t& outValue, SimTime& outRemainder) const;

    /**
     * Converts the time to a numeric string. The number expresses the simulation
     * time precisely (including all significant digits), in seconds.
     * The measurement unit (seconds) is not part of the string.
     */
    std::string str() const {char buf[64]; return str(buf);}

    /**
     * Converts to a string in the same way as str() does. Use this variant
     * over str() where performance is critical. The result is placed somewhere
     * in the given buffer (pointer is returned), but for performance reasons,
     * not necessarily at the buffer's beginning. Please read the documentation
     * of ttoa() for the minimum required buffer size.
     */
    char *str(char *buf) const {char *endp; return SimTime::ttoa(buf, t, getScaleExp(), endp);}

    /**
     * Converts the time to a numeric string with unit in the same format as
     * ustr(SimTimeUnit) does, but chooses the time unit automatically.
     * The function tries to choose the "natural" time unit, e.g. 0.0033 is
     * returned as "3.3ms".
     */
    std::string ustr() const;

    /**
     * Converts the time to a numeric string in the given time unit. The unit can
     * be "s", "ms", "us", "ns", "ps", "fs", or "as". The result represents
     * the simulation time precisely (includes all significant digits), and the
     * unit is appended.
     */
    std::string ustr(SimTimeUnit unit) const;

    /**
     * Returns the underlying 64-bit integer.
     */
    int64_t raw() const  {return t;}

    /**
     * Creates a SimTime by setting its underlying 64-bit integer.
     */
    static SimTime fromRaw(int64_t l) {SimTime tmp; tmp.t = l; return tmp;}

    /**
     * Directly sets the underlying 64-bit integer.
     */
    const SimTime& setRaw(int64_t l) {t = l; return *this;}

    /**
     * Returns the largest simulation time that can be represented using the
     * present scale exponent.
     */
    static const SimTime getMaxTime() {return SimTime().setRaw(INT64_MAX);}

    /**
     * Returns the time resolution as the number of units per second,
     * e.g. for microsecond resolution it returns 1000000.
     */
    static int64_t getScale()  {return dscale;}

    /**
     * Returns the scale exponent, which is an integer in the range -18..0.
     * For example, for microsecond resolution it returns -6.
     */
    static int getScaleExp() {return scaleexp;}

    /**
     * Sets the scale exponent, and thus the resolution of time. Accepted
     * values are -18..0; for example, setScaleExp(-6) selects microsecond
     * resolution. Normally, the scale exponent is set from the configuration
     * file (omnetpp.ini) on simulation startup.
     *
     * The scale exponent can only be set ONCE, and cannot be changed
     * afterwards. Any attempt to change it after it got initialized
     * will result in a runtime error. Reason: the simtime exponent is
     * a global variable, and changing it would silently change the
     * value of existing SimTime variables.
     */
    static void setScaleExp(int e);

    /**
     * Converts the given string to simulation time. Throws an error if
     * there is an error during conversion. Accepted format is: \<number\>
     * or (\<number\>\<unit\>)+.
     */
    static const SimTime parse(const char *s);

    /**
     * Converts a prefix of the given string to simulation time, up to the
     * first character that cannot occur in simulation time strings:
     * not (digit or letter or "." or "+" or "-" or whitespace).
     * Throws an error if there is an error during conversion of the prefix.
     * If the prefix is empty (whitespace only), then 0 is returned, and
     * endp is set equal to s; otherwise,  endp is set to point to the
     * first character that was not converted.
     */
    static const SimTime parse(const char *s, const char *&endp);

    /**
     * Utility function to convert a 64-bit fixed point number into a string
     * buffer. scaleexp must be in the -18..0 range, and the buffer must be
     * at least 64 bytes long. A pointer to the result string will be
     * returned. A pointer to the terminating '\\0' will be returned in endp.
     *
     * ATTENTION: For performance reasons, the returned pointer will point
     * *somewhere* into the buffer, but NOT necessarily at the beginning.
     */
    static char *ttoa(char *buf, int64_t t, int scaleexp, char *&endp);
    //@}
};

inline std::ostream& operator<<(std::ostream& os, const SimTime& x)
{
    char buf[64]; char *endp;
    return os << SimTime::ttoa(buf, x.raw(), SimTime::getScaleExp(), endp);
}

}  // namespace omnetpp

#endif


