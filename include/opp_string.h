//==========================================================================
//   OPP_STRING.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  opp_string class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OPP_STRING_H
#define __OPP_STRING_H

#include <vector>
#include <map>
#include <ostream>
#include "simkerneldefs.h"
#include "util.h"

NAMESPACE_BEGIN

/**
 * Lightweight string class, used internally in some parts of \opp.
 * In simulation models it is better to use std::string or const char *
 * instead.
 *
 * opp_string has only one data member, a char* pointer. Allocation and
 * deallocation of the contents takes place via opp_strdup() and operator
 * delete.
 *
 * @ingroup Internals
 */
class SIM_API opp_string
{
  private:
    char *str;

  public:
    /**
     * Constructor.
     */
    opp_string()  {str = 0;}

    /**
     * Constructor.
     */
    opp_string(const char *s)  {str = opp_strdup(s);}

    /**
     * Constructor.
     */
    opp_string(const std::string& s)  {str = opp_strdup(s.c_str());}

    /**
     * Copy constructor.
     */
    opp_string(const opp_string& s)  {str = opp_strdup(s.str);}

    /**
     * Destructor.
     */
    ~opp_string()  {delete [] str;}

    /**
     * Return pointer to the string.
     */
    const char *c_str() const  {return str ? str : "";}

    /**
     * Null (empty) string or not.
     */
    bool empty() const  {return !str || !str[0];}

    /**
     * Returns pointer to the internal buffer where the string is stored.
     * It is allowed to write into the string via this pointer, but the
     * length of the string should not be exceeded.
     */
    char *buffer()  {return str;}

    /**
     * Allocates a buffer of the given size.
     */
    char *reserve(unsigned size)  {delete[] str;str=new char[size];return str;}

    /**
     * Deletes the old value and opp_strdup()'s the new value
     * to create the object's own copy.
     */
    const char *operator=(const char *s)  {delete[] str;str=opp_strdup(s);return str;}

    /**
     * Assignment.
     */
    opp_string& operator=(const opp_string& s)  {operator=(s.str); return *this;}

    /**
     * Assignment.
     */
    opp_string& operator=(const std::string& s)  {operator=(s.c_str()); return *this;}

    /**
     * Comparison.
     */
    bool operator<(const opp_string& s) const  {return opp_strcmp(str,s.str) < 0;}
};

inline std::ostream& operator<<(std::ostream& out, const opp_string& s)
{
    out << s.c_str(); return out;
}


/**
 * Lightweight string vector, used internally in some parts of \opp.
 * Inheritance is use to "de-templatize" the vector class, because the
 * Windows DLL interface isn't really a friend of templated classes.
 *
 * @ingroup Internals
 */
class SIM_API opp_string_vector : public std::vector<opp_string>
{
  public:
    opp_string_vector() {}
    opp_string_vector(const opp_string_vector& other) {*this = other;}
};


/**
 * Lightweight string vector, used internally in some parts of \opp.
 * Inheritance is use to "de-templatize" the map class, because the
 * Windows DLL interface isn't really a friend of templated classes.
 *
 * @ingroup Internals
 */
class SIM_API opp_string_map : public std::map<opp_string,opp_string>
{
  public:
    opp_string_map() {}
    opp_string_map(const opp_string_map& other) {*this = other;}
};

NAMESPACE_END


#endif


