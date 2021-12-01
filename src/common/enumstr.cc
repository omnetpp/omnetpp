//==========================================================================
//  ENUMSTR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include "enumstr.h"

namespace omnetpp {
namespace common {

EnumStringIterator::EnumStringIterator(const char *s) : str(s)
{
    // loop through string to check its syntax
    while ((*this)++ != -1)
        ;

    // position on first value
    if (!err) {
        str = s;
        current = until = -1;
        (*this)++;
    }
}

static void skip_whitespace(const char *& str)
{
    while (*str == ' ' || *str == '\t')
        str++;
}

static int get_number(const char *& str, int& number)
{
    while (*str == ' ' || *str == '\t')
        str++;
    if (*str < '0' || *str > '9')
        return 0;
    else {
        number = atoi(str);
        while (*str >= '0' && *str <= '9')
            str++;
        return 1;
    }
}

int EnumStringIterator::operator++(int)
{
    if (err || !str)
        return -1;

    if (current < until)
        return ++current;

    skip_whitespace(str);
    if (*str == '\0')
        return current = -1;

    if (!get_number(str, current)) {
        err = true;
        return -1;
    }
    skip_whitespace(str);
    until = -1;
    if (*str == '\0')
        ;
    else if (*str == ',')
        str++;
    else if (*str == '-' || (*str == '.' && *(str+1) == '.')) {
        str++;
        if (*str == '.')
            str++;
        skip_whitespace(str);
        if (!get_number(str, until)) {
            err = true;
            return -1;
        }
        skip_whitespace(str);
        if (*str == '\0')
            ;
        else if (*str == ',')
            str++;
        else {
            err = true;
            return -1;
        }
        if (until < current)
            return operator++(0);
    }
    else {
        err = true;
        return -1;
    }
    return current;
}

}  // namespace common
}  // namespace omnetpp

