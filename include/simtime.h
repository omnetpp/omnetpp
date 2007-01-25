//==========================================================================
//   SIMTIME.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SIMTIME_H
#define __SIMTIME_H

#include <string>
#include <iostream>
#include "simkerneldefs.h"
#include "platdep/inttypes.h"

/**
 * int64-based simulation time.
 */
class SIM_API SimTime
{
  private:
    int64 t;

    static int scaleexp;     // scale exponent, >=0
    static int64 iscale;     // 10^scaleexp, >=1      //XXX not needed then...
    static double scale_;    // 10^scaleexp, >=1
    static double invscale_; // 1/scale_; we store it because floating-point multiplication is faster than division
    static double maxtime;   // also precalculated

  public:
    static const int SCALEEXP_S  = 0;
    static const int SCALEEXP_MS = 3;
    static const int SCALEEXP_US = 6;
    static const int SCALEEXP_NS = 9;
    static const int SCALEEXP_PS = 12;
    static const int SCALEEXP_FS = 15;

    SimTime() {t=0;}
//XXX these integer ctors are no win -- looks like performance-wise int*int64 ~ double*int64
//XXX another idea that does NOT work for speeding up conversion from double: t = d==0 ? 0 : (int64)(scale_*d);
//    SimTime(char d)  {operator=(d);}
//    SimTime(short d) {operator=(d);}
//    SimTime(int d)   {operator=(d);}
//    SimTime(long d)  {operator=(d);}
//    SimTime(unsigned short d) {operator=(d);}
//    SimTime(unsigned int d)   {operator=(d);}
//    SimTime(unsigned long d)  {operator=(d);}

//    SimTime(double d) {operator=(d);}
    SimTime(double d) {t = (int64)(scale_*d);}
    SimTime(const SimTime& x) {t=x.t;}

    bool operator==(const SimTime& x) const  {return t==x.t;}
    bool operator!=(const SimTime& x) const  {return t!=x.t;}
    bool operator< (const SimTime& x) const  {return t<x.t;}
    bool operator> (const SimTime& x) const  {return t>x.t;}
    bool operator<=(const SimTime& x) const  {return t<=x.t;}
    bool operator>=(const SimTime& x) const  {return t>=x.t;}

//    const SimTime& operator=(char d)  {t=iscale*d; return *this;}
//    const SimTime& operator=(short d) {t=iscale*d; return *this;}
//    const SimTime& operator=(int d)   {t=iscale*d; return *this;}
//    const SimTime& operator=(long d)  {t=iscale*d; return *this;}
//    const SimTime& operator=(unsigned short d) {t=iscale*d; return *this;}
//    const SimTime& operator=(unsigned int d)   {t=iscale*d; return *this;}
//    const SimTime& operator=(unsigned long d)  {t=iscale*d; return *this;}

    const SimTime& operator=(double d) {t = (int64)(scale_*d); return *this;}
    const SimTime& operator=(const SimTime& x) {t=x.t; return *this;}

    const SimTime& operator+=(const SimTime& x) {t+=x.t; return *this;}
    const SimTime& operator-=(const SimTime& x) {t-=x.t; return *this;}

    const SimTime& operator*=(double d) {t*=d; return *this;} //XXX to be checked on Linux, see below
    const SimTime& operator/=(double d) {t/=d; return *this;} //XXX to be checked on Linux, see below

    friend const SimTime operator+(const SimTime& x, const SimTime& y);
    friend const SimTime operator-(const SimTime& x, const SimTime& y);
    friend const SimTime operator*(const SimTime& x, double d);
    friend const SimTime operator*(double d, const SimTime& x);
    friend const SimTime operator/(const SimTime& x, double d);
    friend double operator/(const SimTime& x, const SimTime& y);

    double dbl() const  {return t * invscale_;} //XXX operator double? perhaps not a good idea

    std::string str() const;

    int64 raw() const    {return t;}
    void setRaw(int64 l) {t = l;}

    static int64 scale()  {return (int64)scale_;}
    static int scaleExp() {return scaleexp;}
    static double maxTime() {return maxtime;}

    /**
     * IMPORTANT: This function has a global effect, and therefore
     * should NEVER be called during simulation.
     */
    static void setScaleExp(int e);

};

/*XXX
 for *= and /=, SystemC uses the following code:
    // linux bug workaround; don't change next two lines
    volatile double tmp = uint64_to_double( m_value ) * d;
    m_value = static_cast<int64>( tmp );
    return *this;
*/

inline const SimTime operator+(const SimTime& x, const SimTime& y)
{
    return SimTime(x)+=y;
}

inline const SimTime operator-(const SimTime& x, const SimTime& y)
{
    return SimTime(x)-=y;
}

inline const SimTime operator*(const SimTime& x, double d)
{
    return SimTime(x)*=d;
}

inline const SimTime operator*(double d, const SimTime& x)
{
    return SimTime(x)*=d;
}

inline const SimTime operator/(const SimTime& x, double d)
{
    return SimTime(x)/=d;
}

inline double operator/(const SimTime& x, const SimTime& y)
{
    return (double)x.t / (double)y.t;
}

inline std::ostream& operator<<(std::ostream& os, const SimTime& x)
{
    return os << x.dbl() << " (raw:" << x.raw() << ")";   //XXX refine
}

#endif
