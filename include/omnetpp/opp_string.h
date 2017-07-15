//==========================================================================
//   OPP_STRING.H - part of
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

#ifndef __OMNETPP_OPP_STRING_H
#define __OMNETPP_OPP_STRING_H

#include <vector>
#include <map>
#include <ostream>
#include "simkerneldefs.h"
#include "simutil.h"

namespace omnetpp {

/**
 * @brief Lightweight string class, used internally in some parts of \opp.
 *
 * In simulation models it is better to use std::string or const char *
 * instead.
 *
 * opp_string has only one data member, a char* pointer. Allocation and
 * deallocation of the contents takes place via opp_strdup() and operator
 * delete.
 *
 * @ingroup Utilities
 */
class SIM_API opp_string
{
  private:
    char *buf;

  public:
    /**
     * Constructor.
     */
    opp_string()  {buf = nullptr;}

    /**
     * Constructor.
     */
    opp_string(const char *s)  {buf = opp_strdup(s);}

    /**
     * Constructor.
     */
    opp_string(const char *s, int n)  {buf = new char[n+1]; strncpy(buf, s?s:"", n); buf[n] = '\0';}

    /**
     * Constructor.
     */
    opp_string(const std::string& s)  {buf = opp_strdup(s.c_str());}

    /**
     * Copy constructor.
     */
    opp_string(const opp_string& s)  {buf = opp_strdup(s.buf);}

    /**
     * Destructor.
     */
    ~opp_string()  {delete [] buf;}

    /**
     * Return pointer to the string.
     */
    const char *c_str() const  {return buf ? buf : "";}

    /**
     * Convert to std::string.
     */
    std::string str() const  {return buf ? buf : "";}

    /**
     * Null (empty) string or not.
     */
    bool empty() const  {return !buf || !buf[0];}

    /**
     * Returns pointer to the internal buffer where the string is stored.
     * It is allowed to write into the string via this pointer, but the
     * length of the string should not be exceeded.
     */
    char *buffer()  {return buf;}

    /**
     * Returns the length of the string.
     */
    int size() const {return buf ? strlen(buf) : 0;}

    /**
     * Allocates a buffer of the given size.
     */
    char *reserve(unsigned size)  {delete[] buf;buf=new char[size];return buf;}

    /**
     * Deletes the old value and opp_strdup()'s the new value
     * to create the object's own copy.
     */
    const char *operator=(const char *s)  {delete[] buf;buf=opp_strdup(s);return buf;}

    /**
     * Assignment.
     */
    opp_string& operator=(const opp_string& s)  {operator=(s.buf); return *this;}

    /**
     * Assignment.
     */
    opp_string& operator=(const std::string& s)  {operator=(s.c_str()); return *this;}

    /**
     * Comparison.
     */
    bool operator<(const opp_string& s) const  {return opp_strcmp(buf,s.buf) < 0;}

    /**
     * Comparison.
     */
    bool operator==(const opp_string& s) const  {return opp_strcmp(buf,s.buf) == 0;}

    /**
     * Comparison.
     */
    bool operator!=(const opp_string& s) const  {return opp_strcmp(buf,s.buf) != 0;}

    /**
     * Concatenation
     */
    opp_string& operator+=(const char *s) {return operator=(std::string(buf).append(s));}

    /**
     * Concatenation
     */
    opp_string& operator+=(const opp_string& s) {operator+=(s.buf); return *this;}

    /**
     * Concatenation
     */
    opp_string& operator+=(const std::string& s) {operator+=(s.c_str()); return *this;}

    /**
     * Concatenation
     */
    opp_string operator+(const char *s) {return opp_string((std::string(buf)+s).c_str());}

    /**
     * Concatenation
     */
    opp_string operator+(const opp_string& s) {return operator+(s.c_str());}

    /**
     * Concatenation
     */
    opp_string operator+(const std::string& s) {return operator+(s.c_str());}

};

inline std::ostream& operator<<(std::ostream& out, const opp_string& s)
{
    out << s.c_str(); return out;
}


/**
 * @brief Lightweight string vector, used internally in some parts of \opp.
 *
 * Inheritance is used to "de-templatize" the vector class, because the
 * Windows DLL interface is not really a friend of templated classes.
 *
 * @ingroup Utilities
 */
class SIM_API opp_string_vector : public std::vector<opp_string>
{
  public:
    opp_string_vector() {}
    opp_string_vector(const opp_string_vector& other) : std::vector<opp_string>(other) {}
};


/**
 * @brief Lightweight string-to-string map, used internally in some parts of \opp.
 *
 * Inheritance is used to "de-templatize" the map class, because the
 * Windows DLL interface is not really a friend of templated classes.
 *
 * @ingroup Utilities
 */
class SIM_API opp_string_map : public std::map<opp_string,opp_string>
{
  public:
    opp_string_map() {}
    opp_string_map(const opp_string_map& other) : std::map<opp_string,opp_string>(other) {}
};

}  // namespace omnetpp


#endif


