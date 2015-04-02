/*
    Written by Calum Grant
    Copyright (C) Calum Grant 2007

    Home page: http://calumgrant.net/units
    File location: http://calumgrant.net/units/units.hpp

    Copying permitted under the terms of the Boost software license.

    This library is distributed as a single file for convenience.
    Normally it should be split up!

    Library name:
        units

    Purpose:
        Decorate values with units.
        Provides safety and automatic conversion between values of different
        units.
 */

#ifndef __INET_UNITS_H
#define __INET_UNITS_H

#include <cmath>
#include <iostream>

namespace inet {

namespace units {

namespace internal    // Boost would call this "detail"
{
// Forward
template<typename T1, typename T2> struct convert;

// Allows construction of no units
struct none;

// Forward
template<int Num, int Den, int Div = Num / Den, int Mod = Num % Den>
struct fixed_power;

} // namespace internal

// Construct a unit equivalent to Unit1*Unit2
template<typename Unit1, typename Unit2>
struct compose;

// Constructs a unit equivalent to Unit*Num/Den
template<typename Unit, int Num, int Den = 1>
struct scale;

// Constructs a unit equivalent to Unit+Num/Den
template<typename Unit, int Num, int Den = 1>
struct translate;

// Constructs a unit equivalent to Unit^(Num/Den)
template<typename Unit, int Num, int Den = 1>
struct pow;

// A unit which is effectively no units at all.
typedef pow<internal::none, 0> unit;

// A value with a unit.
//      Value is the type you are storing
//    Units is the units of the value
template<typename Value, typename Units>
class value
{
  public:
    typedef Value value_type;
    typedef Units unit;

    value() : m_rep()
    {
    }

    explicit value(const value_type& v) : m_rep(v)
    {
    }

    template<typename OtherValue, typename OtherUnits>
    value(const value<OtherValue, OtherUnits>& v) :
        m_rep(internal::convert<OtherUnits, Units>::fn(v.get()))
    {
    }

    const value_type& get() const
    {
        return m_rep;
    }

    template<typename OtherValue, typename OtherUnits>
    value& operator=(const value<OtherValue, OtherUnits>& other)
    {
        m_rep = value(other).get();
        return *this;
    }

    template<typename OtherValue, typename OtherUnits>
    value operator+(const value<OtherValue, OtherUnits>& other) const
    {
        return value(get() + value(other).get());
    }

    template<typename OtherValue, typename OtherUnits>
    value& operator+=(const value<OtherValue, OtherUnits>& other)
    {
        m_rep += value(other).get();
        return *this;
    }

    template<typename OtherValue, typename OtherUnits>
    value& operator-=(const value<OtherValue, OtherUnits>& other)
    {
        m_rep -= value(other).get();
        return *this;
    }

    template<typename OtherValue, typename OtherUnits>
    value operator-(const value<OtherValue, OtherUnits>& other) const
    {
        return value(get() - value(other).get());
    }

    value operator-() const
    {
        return value(-get());
    }

    template<typename OtherValue, typename OtherUnits>
    value<Value, compose<Units, OtherUnits> >
    operator*(const value<OtherValue, OtherUnits>& other) const
    {
        return value<Value, compose<Units, OtherUnits> >(get() * other.get());
    }

    value operator*(const value_type& v) const
    {
        return value(get() * v);
    }

    value& operator*=(const value_type& v)
    {
        m_rep *= v;
        return *this;
    }

    template<typename OtherValue, typename OtherUnits>
    value<Value, compose<Units, pow<OtherUnits, -1> > >
    operator/(const value<OtherValue, OtherUnits>& other) const
    {
        return value<Value, compose<Units, pow<OtherUnits, -1> > >(get() / other.get());
    }

    value operator/(const value_type& v) const
    {
        return value(get() / v);
    }

    value& operator/=(const value_type& v)
    {
        m_rep /= v;
        return *this;
    }

    template<typename OtherValue, typename OtherUnits>
    bool operator==(const value<OtherValue, OtherUnits>& other) const
    {
        return get() == value(other).get();
    }

    template<typename OtherValue, typename OtherUnits>
    bool operator!=(const value<OtherValue, OtherUnits>& other) const
    {
        return get() != value(other).get();
    }

    template<typename OtherValue, typename OtherUnits>
    bool operator<(const value<OtherValue, OtherUnits>& other) const
    {
        return get() < value(other).get();
    }

    template<typename OtherValue, typename OtherUnits>
    bool operator<=(const value<OtherValue, OtherUnits>& other) const
    {
        return get() <= value(other).get();
    }

    template<typename OtherValue, typename OtherUnits>
    bool operator>(const value<OtherValue, OtherUnits>& other) const
    {
        return get() > value(other).get();
    }

    template<typename OtherValue, typename OtherUnits>
    bool operator>=(const value<OtherValue, OtherUnits>& other) const
    {
        return get() >= value(other).get();
    }

    value& operator++()
    {
        ++m_rep;
        return *this;
    }

    value operator++(int)
    {
        value v = *this;
        ++m_rep;
        return v;
    }

    value& operator--()
    {
        --m_rep;
        return *this;
    }

    value operator--(int)
    {
        value v = *this;
        --m_rep;
        return v;
    }

  private:
    value_type m_rep;
};

template<typename Value, typename Unit>
value<Value, pow<Unit, -1> > operator/(const Value& a, const value<Value, Unit>& b)
{
    return value<Value, pow<Unit, -1> >(a / b.get());
}

template<typename Value, typename Unit>
value<Value, Unit> operator*(const Value& a, const value<Value, Unit>& b)
{
    return value<Value, Unit>(a * b.get());
}

template<typename Value, typename Unit>
value<Value, pow<Unit, 1, 2> > sqrt(const value<Value, Unit>& a)
{
    return value<Value, pow<Unit, 1, 2> >(std::sqrt(a.get()));
}

template<int Num, int Den, typename Value, typename Unit>
value<Value, pow<Unit, Num, Den> > raise(const value<Value, Unit>& a)
{
    return value<Value, pow<Unit, Num, Den> >(internal::fixed_power<Num, Den>::pow(a.get()));
}

} // namespace units

/*****************************************************************************/
// Implementation

namespace units {

namespace internal {

// Ensures (at compile-time) that the template argument is true.
template<bool> struct legacy_static_assert;
template<> struct legacy_static_assert<true> {};

// Forward
template<typename T1, typename T2>
struct convertible;

// Forward
template<typename U>
struct scaling_factor;

// Converts T1 to T2.
// Stage 3 - performed after Stage 1 and Stage 2.
// The reason we perform convert in stages is so that the compiler
// can resolve templates in the order we want it to.
template<typename T1, typename T2>
struct convert3
{
    // The default implementation assumes that the two quantities are in compatible
    // units up to some scaling factor.  Find the scaling factor and apply it.
    template<typename V>
    static V fn(const V& v)
    {
        return v * scaling_factor<T2>::template fn<V>() / scaling_factor<T1>::template fn<V>();
    }
};

// Converts T1 to T2.
// Template matches the first argument (T1),
// this is the fall-through to convert3.
template<typename T1, typename T2>
struct convert2
{
    template<typename V>
    static V fn(const V& v)
    {
        return convert3<T1, T2>::fn(v);
    }
};

// Converts T1 to T2.
// If you really want to implement your own conversion routine,
// specialise this template.
// The default implementation falls through to convert2.
template<typename T1, typename T2>
struct convert
{
    // If this fails, then T1 is not convertible to T2:
    legacy_static_assert<convertible<T1, T2>::value> check_convertible;

    template<typename V>
    static V fn(const V& v)
    {
        return convert2<T1, T2>::fn(v);
    }
};

// If we convert to the same type, the conversion function
// is trivial.
template<typename T>
struct convert<T, T>
{
    template<typename U>
    static const U& fn(const U& u) { return u; }
};

// Convert to same type.
template<typename T>
struct convert3<T, T>
{
    template<typename U>
    static const U& fn(const U& u) { return u; }
};

// Convert from a scaled unit
template<typename T, typename U, int Num, int Den>
struct convert2<scale<T, Num, Den>, U>
{
    template<typename V>
    static V fn(const V& v)
    {
        return convert<T, U>::fn((v * Den) / Num);
    }
};

// Convert to a scaled unit
template<typename T, typename U, int Num, int Den>
struct convert3<T, scale<U, Num, Den> >
{
    template<typename V>
    static V fn(const V& v)
    {
        return (convert<T, U>::fn(v) * Num) / Den;
    }
};

// Convert from a translated unit
template<typename T, typename U, int Num, int Den>
struct convert2<translate<T, Num, Den>, U>
{
    template<typename V>
    static V fn(const V& v)
    {
        return convert<T, U>::fn(v - static_cast<V>(Num) / static_cast<V>(Den));
    }
};

// Convert to a translated unit
template<typename T, typename U, int Num, int Den>
struct convert3<T, translate<U, Num, Den> >
{
    template<typename V>
    static V fn(const V& v)
    {
        return convert<T, U>::fn(v) + static_cast<V>(Num) / static_cast<V>(Den);
    }
};

// Count the power to which unit Term is raised in the unit List.
// Returns a rational num/den of the power of term Term in List.
// The default assumes that Term is not found (num/den=0)
template<typename Term, typename List>
struct count_terms
{
    static const int num = 0;
    static const int den = 1;
};

template<typename Term>
struct count_terms<Term, Term>
{
    static const int num = 1;
    static const int den = 1;
};

// count_terms ignores scaling factors - that is taken care of by scaling_factor.
template<typename Term, typename Unit, int N, int D>
struct count_terms<Term, scale<Unit, N, D> >
{
    typedef count_terms<Term, Unit> result;
    static const int num = result::num;
    static const int den = result::den;
};

// count_terms ignores translation.
template<typename Term, typename Unit, int N, int D>
struct count_terms<Term, translate<Unit, N, D> >
{
    typedef count_terms<Term, Unit> result;
    static const int num = result::num;
    static const int den = result::den;
};

// Addition of fractions.
template<typename Term, typename T1, typename T2>
struct count_terms<Term, compose<T1, T2> >
{
    typedef count_terms<Term, T1> result1;
    typedef count_terms<Term, T2> result2;
    static const int num =
        result1::num * result2::den + result1::den * result2::num;
    static const int den =
        result1::den * result2::den;
};

// Multiplication of fractions.
template<typename Term, typename Unit, int N, int D>
struct count_terms<Term, pow<Unit, N, D> >
{
    typedef count_terms<Term, Unit> result;
    static const int num = N * result::num;
    static const int den = D * result::den;
};

// Counts the power of the unit Term in units T1 and T2.
// Reports if they are equal, using equality of fractions.
// Does a depth-first search of the unit "Term",
// or counts the terms in the default case.
template<typename Term, typename T1, typename T2>
struct check_terms_equal
{
    typedef count_terms<Term, T1> count1;
    typedef count_terms<Term, T2> count2;

    static const bool value =
        count1::num * count2::den ==
        count1::den * count2::num;
};

template<typename Unit, int N, int D, typename T1, typename T2>
struct check_terms_equal<pow<Unit, N, D>, T1, T2>
{
    static const bool value = check_terms_equal<Unit, T1, T2>::value;
};

template<typename Unit, int N, int D, typename T1, typename T2>
struct check_terms_equal<scale<Unit, N, D>, T1, T2>
{
    static const bool value = check_terms_equal<Unit, T1, T2>::value;
};

template<typename Unit, int N, int D, typename T1, typename T2>
struct check_terms_equal<translate<Unit, N, D>, T1, T2>
{
    static const bool value = check_terms_equal<Unit, T1, T2>::value;
};

template<typename T1, typename T2, typename T3, typename T4>
struct check_terms_equal<compose<T1, T2>, T3, T4>
{
    static const bool value =
        check_terms_equal<T1, T3, T4>::value &&
        check_terms_equal<T2, T3, T4>::value;
};

// Determines whether two types are convertible
// Counts the powers in the LHS and RHS and ensures they are equal.
template<typename T1, typename T2>
struct convertible
{
    static const bool value =
        check_terms_equal<T1, T1, T2>::value &&
        check_terms_equal<T2, T1, T2>::value;
};

// A functor that raises a value to the power Num/Den.
// The template is specialised for efficiency
// so that we don't always have to call the std::pow function.
template<int Num, int Den, int Div, int Mod>
struct fixed_power
{
    template<typename T> static T pow(const T& t)
    {
        return std::pow(t, static_cast<T>(Num) / static_cast<T>(Den));
    }
};

template<int N, int D>
struct fixed_power<N, D, 1, 0>
{
    template<typename T> static const T& pow(const T& t)
    {
        return t;
    }
};

template<int N, int D>
struct fixed_power<N, D, 2, 0>
{
    template<typename T> static T pow(const T& t)
    {
        return t * t;
    }
};

template<int N, int D>
struct fixed_power<N, D, 3, 0>
{
    template<typename T> static T pow(const T& t)
    {
        return t * t * t;
    }
};

template<int N, int D>
struct fixed_power<N, D, 4, 0>
{
    template<typename T> static const T& pow(const T& t)
    {
        T u = t * t;
        return u * u;
    }
};

template<int N, int D>
struct fixed_power<N, D, -1, 0>
{
    template<typename T> static T pow(const T& t)
    {
        return 1 / t;
    }
};

template<int N, int D>
struct fixed_power<N, D, -2, 0>
{
    template<typename T> static T pow(const T& t)
    {
        return 1 / (t * t);
    }
};

template<int N, int D>
struct fixed_power<N, D, 0, 0>
{
    template<typename T> static T pow(const T& t)
    {
        return 1;
    }
};

// Determine the scaling factor of a unit in relation to its "base" units.
// Default is that U is a primitive unit and is not scaled.
template<typename U>
struct scaling_factor
{
    template<typename T>
    static T fn() { return 1; }
};

template<typename U1, typename U2>
struct scaling_factor<compose<U1, U2> >
{
    template<typename T>
    static T fn()
    {
        return scaling_factor<U1>::template fn<T>()
               * scaling_factor<U2>::template fn<T>();
    }
};

template<typename U, int N, int D>
struct scaling_factor<scale<U, N, D> >
{
    template<typename T>
    static T fn()
    {
        return scaling_factor<U>::template fn<T>()
               * static_cast<T>(N) / static_cast<T>(D);
    }
};

template<typename U, int N, int D>
struct scaling_factor<pow<U, N, D> >
{
    template<typename T>
    static T fn()
    {
        return fixed_power<N, D>::pow(scaling_factor<U>::template fn<T>());
    }
};

template<typename U, int N, int D>
struct scaling_factor<translate<U, N, D> >
{
    template<typename T>
    static T fn()
    {
        return scaling_factor<U>::template fn<T>();
    }
};

} // namespace internal

} // namespace units

/*****************************************************************************/
// Display

#define UNIT_DISPLAY_NAME(unit, string) \
    template<> struct output_unit<unit> { \
        template<typename Stream> static void fn(Stream & os) { os << string; } \
    };

#define UNITS_DISPLAY_NAME(unit, string) \
    namespace units { UNIT_DISPLAY_NAME(unit, string) }

namespace units {

namespace internal {

// This is the default unit formatting mechanism
template<typename Unit>
struct output_unit2
{
    template<typename Stream>
    static void fn(Stream& os) { os << "units"; }
};

} // namespace internal

// Functor to write unit text to stream
template<typename Unit>
struct output_unit
{
    template<typename Stream>
    static void fn(Stream& os) { internal::output_unit2<Unit>::fn(os); }
};

UNIT_DISPLAY_NAME(unit, "1");

namespace internal {

template<typename U1, typename U2>
struct output_unit2<compose<U1, U2> >
{
    template<typename Stream>
    static void fn(Stream& os)
    {
        output_unit<U1>::fn(os);
        os << '.';
        output_unit<U2>::fn(os);
    }
};

template<typename Unit, int Num, int Den>
struct output_unit2<pow<Unit, Num, Den> >
{
    template<typename Stream>
    static void fn(Stream& os)
    {
        if (Num != Den) os << '(';
        output_unit<Unit>::fn(os);
        if (Num != Den) {
            os << ')';
            os << '^' << Num;
            if (Num % Den) {
                os << '/' << Den;
            }
        }
    }
};

template<typename Unit, int Num, int Den>
struct output_unit2<translate<Unit, Num, Den> >
{
    template<typename Stream>
    static void fn(Stream& os)
    {
        os << '(';
        output_unit<Unit>::fn(os);
        os << '+' << Num;
        if (Den != 1) os << '/' << Den;
        os << ')';
    }
};

template<typename Unit, int Num, int Den>
struct output_unit2<scale<Unit, Num, Den> >
{
    template<typename Stream>
    static void fn(Stream& os)
    {
        os << Den;
        if (Num != 1)
            os << '/' << Num;
        os << '.';
        output_unit<Unit>::fn(os);
    }
};

} // namespace internal

template<typename Value, typename Unit>
std::ostream& operator<<(std::ostream& os, const value<Value, Unit>& value)
{
    os << value.get(); // << ' ';
    output_unit<Unit>::fn(os);
    return os;
}

} // namespace units

/*****************************************************************************/
// Additional units

namespace units {

namespace units {

typedef ::inet::units::unit unit;

// SI base units:

struct m;    // meter
struct kg;    // kilogram
struct s;    // second
struct K;    // Kelvin
struct A;    // Ampere
struct mol;    // mole
struct cd;    // candela
struct b;    // bit

} // namespace units

UNIT_DISPLAY_NAME(units::m, "m");
UNIT_DISPLAY_NAME(units::kg, "kg");
UNIT_DISPLAY_NAME(units::s, "s");
UNIT_DISPLAY_NAME(units::K, "K");
UNIT_DISPLAY_NAME(units::A, "A");
UNIT_DISPLAY_NAME(units::mol, "mol");
UNIT_DISPLAY_NAME(units::cd, "cd");

namespace units {

// SI derived units:
typedef compose<m, pow<m, -1> > rad;
typedef compose<pow<m, 2>, pow<m, -2> > sr;
typedef pow<s, -1> Hz;
typedef compose<m, compose<kg, pow<s, -2> > > N;
typedef compose<N, pow<m, -2> > Pa;
typedef compose<N, m> J;
typedef compose<J, pow<s, -1> > W;
typedef compose<s, A> C;
typedef compose<W, pow<A, -1> > V;
typedef compose<C, pow<V, -1> > F;
typedef compose<V, pow<A, -1> > Ohm;
typedef compose<Ohm, m> Ohmm;
typedef compose<A, pow<V, -1> > S;
typedef compose<S, pow<m, -1> > Spm;
typedef compose<V, s> Wb;
typedef compose<Wb, pow<m, -2> > T;
typedef compose<Wb, pow<A, -1> > H;
typedef cd lm;
typedef compose<lm, pow<m, -2> > lx;
typedef pow<s, -1> Bq;
typedef compose<J, pow<kg, -1> > Gy;
typedef Gy Sv;
typedef compose<pow<s, -1>, mol> kat;

} // namespace units

UNIT_DISPLAY_NAME(units::rad, "rad");
UNIT_DISPLAY_NAME(units::sr, "sr");
UNIT_DISPLAY_NAME(units::Hz, "Hz");    // Too problematic
UNIT_DISPLAY_NAME(units::N, "N");
UNIT_DISPLAY_NAME(units::Pa, "Pa");
UNIT_DISPLAY_NAME(units::J, "J");
UNIT_DISPLAY_NAME(units::W, "W");
UNIT_DISPLAY_NAME(units::C, "C");
UNIT_DISPLAY_NAME(units::V, "V");
UNIT_DISPLAY_NAME(units::F, "F");
UNIT_DISPLAY_NAME(units::Ohm, "Ohm");
UNIT_DISPLAY_NAME(units::S, "S");
UNIT_DISPLAY_NAME(units::Wb, "Wb");
UNIT_DISPLAY_NAME(units::T, "T");
UNIT_DISPLAY_NAME(units::H, "H");
UNIT_DISPLAY_NAME(units::lx, "lx");
UNIT_DISPLAY_NAME(units::Gy, "Gy");
UNIT_DISPLAY_NAME(units::kat, "kat");

namespace units {

// SI prefixes:
template<typename Unit> struct deca
{
    typedef scale<Unit, 1, 10> type;
};
template<typename Unit> struct hecto
{
    typedef scale<Unit, 1, 100> type;
};
template<typename Unit> struct kilo
{
    typedef scale<Unit, 1, 1000> type;
};
template<typename Unit> struct mega
{
    typedef scale<typename kilo<Unit>::type, 1, 1000> type;
};
template<typename Unit> struct giga
{
    typedef scale<typename mega<Unit>::type, 1, 1000> type;
};
template<typename Unit> struct tera
{
    typedef scale<typename giga<Unit>::type, 1, 1000> type;
};
template<typename Unit> struct peta
{
    typedef scale<typename tera<Unit>::type, 1, 1000> type;
};
template<typename Unit> struct exa
{
    typedef scale<typename peta<Unit>::type, 1, 1000> type;
};
template<typename Unit> struct zetta
{
    typedef scale<typename exa<Unit>::type, 1, 1000> type;
};
template<typename Unit> struct yotta
{
    typedef scale<typename zetta<Unit>::type, 1, 1000> type;
};

template<typename Unit> struct deci
{
    typedef scale<Unit, 10> type;
};
template<typename Unit> struct centi
{
    typedef scale<Unit, 100> type;
};
template<typename Unit> struct milli
{
    typedef scale<Unit, 1000> type;
};
template<typename Unit> struct micro
{
    typedef scale<typename milli<Unit>::type, 1000> type;
};
template<typename Unit> struct nano
{
    typedef scale<typename micro<Unit>::type, 1000> type;
};
template<typename Unit> struct pico
{
    typedef scale<typename nano<Unit>::type, 1000> type;
};
template<typename Unit> struct femto
{
    typedef scale<typename pico<Unit>::type, 1000> type;
};
template<typename Unit> struct atto
{
    typedef scale<typename femto<Unit>::type, 1000> type;
};
template<typename Unit> struct zepto
{
    typedef scale<typename atto<Unit>::type, 1000> type;
};
template<typename Unit> struct yocto
{
    typedef scale<typename zepto<Unit>::type, 1000> type;
};

// Some prefixed SI units:
typedef centi<m>::type cm;
typedef milli<m>::type mm;
typedef kilo<m>::type km;
typedef milli<kg>::type g;
typedef milli<g>::type mg;
typedef milli<s>::type ms;
typedef milli<W>::type mW;
typedef kilo<Hz>::type kHz;
typedef mega<Hz>::type MHz;
typedef giga<Hz>::type GHz;

} // namespace units

UNIT_DISPLAY_NAME(units::cm, "cm");
UNIT_DISPLAY_NAME(units::mm, "mm");
UNIT_DISPLAY_NAME(units::km, "km");
UNIT_DISPLAY_NAME(units::g, "g");
UNIT_DISPLAY_NAME(units::mg, "mg");
UNIT_DISPLAY_NAME(units::ms, "ms");
UNIT_DISPLAY_NAME(units::mW, "mW");
UNIT_DISPLAY_NAME(units::kHz, "kHz");
UNIT_DISPLAY_NAME(units::MHz, "MHz");
UNIT_DISPLAY_NAME(units::GHz, "GHz");

namespace units {

// Non-SI mass
typedef scale<kg, 22046223, 10000000> lb;
typedef scale<lb, 16> oz;
typedef scale<kg, 1, 1000> tonne;

// Non-SI temperature
typedef translate<K, -27315, 100> Celsius;
typedef translate<scale<Celsius, 9, 5>, 32> Fahrenheit;

// Non-SI time
typedef scale<s, 1, 60> minute;
typedef scale<minute, 1, 60> hour;
typedef scale<hour, 1, 24> day;
typedef scale<day, 1, 7> week;
struct month;    // No fixed ratio with week
typedef scale<month, 1, 12> year;
typedef scale<year, 1, 100> century;
typedef scale<year, 1, 1000> millennium;

// Non-SI length
typedef scale<cm, 100, 254> inch;
typedef scale<inch, 1, 12> foot;
typedef scale<inch, 1, 36> yard;
typedef scale<yard, 1, 1760> mile;
typedef scale<m, 1, 1852> nautical_mile;

// Non-SI area
typedef pow<m, 2> m2;
typedef scale<m2, 1, 10000> hectare;
typedef scale<m2, 1, 100> are;
typedef pow<inch, 2> inch2;
typedef scale<hectare, 24710538, 10000000> acre;

// Non-SI volume
typedef pow<cm, 3> cm3;
typedef cm3 ml;
typedef scale<ml, 1, 1000> liter;
typedef scale<liter, 10> dl;
typedef scale<liter, 100> cl;
typedef pow<m, 3> m3;

// Non-SI velocity
typedef compose<mile, pow<hour, -1> > mph;
typedef compose<km, pow<hour, -1> > kph;
typedef compose<m, pow<s, -1> > mps;
typedef compose<s, pow<m, -1> > spm;
typedef compose<nautical_mile, pow<hour, -1> > knot;
typedef scale<mps, 100, 34029> mach;

// Angles
typedef scale<rad, 180000000, 3141593> degree;
typedef scale<rad, 200000000, 3141593> grad;
typedef scale<degree, 60> degree_minute;
typedef scale<degree_minute, 60> degree_second;

// Pressure
typedef scale<Pa, 1, 1000> kPa;
typedef scale<kPa, 1450377, 10000000> psi;
typedef scale<kPa, 10> millibar;

// Informatics
typedef compose<b, pow<s, -1> > bps;
typedef scale<bps, 1, 1000> kbps;
typedef scale<bps, 1, 1000000> Mbps;

// Other
typedef scale<Hz, 60> rpm;
typedef scale<unit, 100> percent;
typedef scale<unit, 1, 12> dozen;
typedef scale<unit, 1, 13> bakers_dozen;

} // namespace units

UNIT_DISPLAY_NAME(units::lb, "lb");
UNIT_DISPLAY_NAME(units::oz, "oz");
UNIT_DISPLAY_NAME(units::tonne, "tonnes");
UNIT_DISPLAY_NAME(units::Celsius, "'C");
UNIT_DISPLAY_NAME(units::Fahrenheit, "'F");
UNIT_DISPLAY_NAME(units::minute, "minutes");
UNIT_DISPLAY_NAME(units::hour, "hours");
UNIT_DISPLAY_NAME(units::day, "days");
UNIT_DISPLAY_NAME(units::week, "weeks");
UNIT_DISPLAY_NAME(units::month, "months");
UNIT_DISPLAY_NAME(units::year, "years");
UNIT_DISPLAY_NAME(units::century, "centuries");
UNIT_DISPLAY_NAME(units::millennium, "millennia");
UNIT_DISPLAY_NAME(units::inch, "inches");
UNIT_DISPLAY_NAME(units::foot, "foot");
UNIT_DISPLAY_NAME(units::yard, "yards");
UNIT_DISPLAY_NAME(units::mile, "miles");
UNIT_DISPLAY_NAME(units::nautical_mile, "nautical miles");
UNIT_DISPLAY_NAME(units::hectare, "ha");
UNIT_DISPLAY_NAME(units::are, "are");
UNIT_DISPLAY_NAME(units::acre, "acres");
UNIT_DISPLAY_NAME(units::ml, "ml");
UNIT_DISPLAY_NAME(units::liter, "l");
UNIT_DISPLAY_NAME(units::dl, "dl");
UNIT_DISPLAY_NAME(units::cl, "cl");
UNIT_DISPLAY_NAME(units::mph, "mph");
UNIT_DISPLAY_NAME(units::kph, "km/h");
UNIT_DISPLAY_NAME(units::knot, "knots");
UNIT_DISPLAY_NAME(units::mach, "mach");
UNIT_DISPLAY_NAME(units::degree, "deg");
UNIT_DISPLAY_NAME(units::grad, "grad");
UNIT_DISPLAY_NAME(units::degree_minute, "'");
UNIT_DISPLAY_NAME(units::degree_second, "\"");
UNIT_DISPLAY_NAME(units::kPa, "kPa");
UNIT_DISPLAY_NAME(units::psi, "PSI");
UNIT_DISPLAY_NAME(units::millibar, "millibars");
UNIT_DISPLAY_NAME(units::percent, "%");
UNIT_DISPLAY_NAME(units::rpm, "rpm");
UNIT_DISPLAY_NAME(units::dozen, "dozen");
UNIT_DISPLAY_NAME(units::bakers_dozen, "bakers dozen");

namespace values {

typedef value<double, units::unit> unit;

// SI units
typedef value<double, units::m> m;
typedef value<double, units::kg> kg;
typedef value<double, units::s> s;
typedef value<double, units::K> K;
typedef value<double, units::A> A;
typedef value<double, units::mol> mol;
typedef value<double, units::cd> cd;
typedef value<double, units::m> b;

// SI derived
typedef value<double, units::rad> rad;
typedef value<double, units::sr> sr;
typedef value<double, units::Hz> Hz;
typedef value<double, units::N> N;
typedef value<double, units::Pa> Pa;
typedef value<double, units::J> J;
typedef value<double, units::W> W;
typedef value<double, units::C> C;
typedef value<double, units::V> V;
typedef value<double, units::F> F;
typedef value<double, units::Ohm> Ohm;
typedef value<double, units::Ohmm> Ohmm;
typedef value<double, units::S> S;
typedef value<double, units::S> Spm;
typedef value<double, units::Wb> Wb;
typedef value<double, units::T> T;
typedef value<double, units::H> H;
typedef value<double, units::lm> lm;
typedef value<double, units::lx> lx;
typedef value<double, units::Bq> Bq;
typedef value<double, units::Gy> Gy;
typedef value<double, units::Sv> Sv;
typedef value<double, units::kat> kat;

// Prefixed units
typedef value<double, units::cm> cm;
typedef value<double, units::mm> mm;
typedef value<double, units::km> km;
typedef value<double, units::g> g;
typedef value<double, units::mg> mg;
typedef value<double, units::ms> ms;
typedef value<double, units::mW> mW;
typedef value<double, units::kHz> kHz;
typedef value<double, units::MHz> MHz;
typedef value<double, units::GHz> GHz;

// Non-SI
typedef value<double, units::lb> lb;
typedef value<double, units::oz> oz;
typedef value<double, units::tonne> tonne;

typedef value<double, units::Celsius> Celsius;
typedef value<double, units::Fahrenheit> Fahrenheit;

typedef value<double, units::minute> minute;
typedef value<double, units::hour> hour;
typedef value<double, units::day> day;
typedef value<double, units::week> week;
typedef value<double, units::month> month;
typedef value<double, units::year> year;
typedef value<double, units::century> century;
typedef value<double, units::millennium> millennium;

typedef value<double, units::inch> inch;
typedef value<double, units::foot> foot;
typedef value<double, units::yard> yard;
typedef value<double, units::mile> mile;
typedef value<double, units::nautical_mile> nautical_mile;

typedef value<double, units::m2> m2;
typedef value<double, units::hectare> hectare;
typedef value<double, units::are> are;
typedef value<double, units::inch2> inch2;
typedef value<double, units::acre> acre;

typedef value<double, units::cm3> cm3;
typedef value<double, units::ml> ml;
typedef value<double, units::cl> cl;
typedef value<double, units::liter> liter;
typedef value<double, units::dl> dl;
typedef value<double, units::m3> m3;

typedef value<double, units::mph> mph;
typedef value<double, units::kph> kph;
typedef value<double, units::mps> mps;
typedef value<double, units::spm> spm;
typedef value<double, units::knot> knot;
typedef value<double, units::mach> mach;

typedef value<double, units::degree> degree;
typedef value<double, units::grad> grad;
typedef value<double, units::degree_minute> degree_minute;
typedef value<double, units::degree_second> degree_second;

typedef value<double, units::kPa> kPa;
typedef value<double, units::psi> psi;
typedef value<double, units::millibar> millibar;

typedef value<double, units::bps> bps;
typedef value<double, units::kbps> kbps;
typedef value<double, units::Mbps> Mbps;

typedef value<double, units::percent> percent;
typedef value<double, units::rpm> rpm;
typedef value<double, units::dozen> dozen;
typedef value<double, units::bakers_dozen> bakers_dozen;

} // namespace values

namespace constants {

// Physical constants:
const value<double, compose<units::J, pow<units::K, -1> > > k(1.3806504e-23);
const value<double, units::kg> mu(1.660538782e-27);
const value<double, pow<units::mol, -1> > NA(6.02214179e23);
const value<double, units::s> G0(7.7480917004e-5);
const value<double, compose<units::F, pow<units::m, -1> > > e0(8.854187817e-12);
const value<double, units::kg> me(9.10938215e-31);
const value<double, units::J> eV(1.602176487e-19);
const value<double, units::C> e(1.602176487e-19);
const value<double, units::F> F(96485.3399);
const value<double, units::unit> alpha(7.2973525376e-3);
const value<double, units::unit> inv_alpha(137.035999679);
const value<double, compose<units::N, pow<units::A, -2> > > u0(12.566370614e-7);
const value<double, units::Wb> phi0(2.067833667e-15);    // ??
const value<double, compose<units::J, compose<pow<units::mol, -1>, pow<units::kg, -1> > > > R(8.314472);
const value<double, compose<pow<units::m, 3>, compose<pow<units::kg, -1>, pow<units::s, -2> > > > G(6.67428e-11);
const value<double, compose<units::J, units::s> > h(6.62606896e-34);
const value<double, compose<units::J, units::s> > h_bar(1.054571628e-34);
const value<double, units::kg> mp(1.672621637e-27);
const value<double, unit> mpme(1836.15267247);
const value<double, pow<units::m, -1> > Rinf(10973731.568527);
const value<double, compose<units::m, pow<units::s, -1> > > c(299792458);
const value<double, compose<units::W, compose<pow<units::m, -1>, pow<units::K, -4> > > > rho(5.6704e-8);

// Other constants:
const value<double, units::rad> pi(3.141592653589793);
const value<double, units::m> lightyear(9.4605284e15);
const value<double, compose<units::m, pow<units::s, -2> > > g(9.80665);

} // namespace constants

// Trigonometry

template<typename Value, typename Unit>
Value sin(const value<Value, Unit>& angle)
{
    return std::sin(value<Value, units::rad>(angle).get());
}

template<typename Value, typename Unit>
Value cos(const value<Value, Unit>& angle)
{
    return std::cos(value<Value, units::rad>(angle).get());
}

template<typename Value, typename Unit>
Value tan(const value<Value, Unit>& angle)
{
    return std::tan(value<Value, units::rad>(angle).get());
}

} // namespace units

} // namespace inet

#endif // ifndef __INET_UNITS_H

