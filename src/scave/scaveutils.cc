//=========================================================================
//  INDEXFILE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include "platmisc.h"
#include "scaveutils.h"


NAMESPACE_BEGIN

static double dblZero = 0.0;
double dblNaN = dblZero / dblZero;
double dblPositiveInfinity = 1 / dblZero;
double dblNegativeInfinity = -1 / dblZero;

bool parseInt(const char *s, int &dest)
{
    char *e;
    dest = (int)strtol(s,&e,10);
    return !*e;
}

bool parseLong(const char *s, long &dest)
{
    char *e;
    dest = strtol(s,&e,10);
    return !*e;
}

bool parseInt64(const char *s, int64 &dest)
{
    char *e;
    dest = strtoll(s,&e,10);
    return !*e;
}

bool parseDouble(const char *s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (!*e)
    {
        return true;
    }
    if (strstr(s,"INF") || strstr(s, "inf"))
    {
        dest = dblPositiveInfinity;  // +INF or -INF
        if (*s=='-') dest = -dest;
        return true;
    }
    if (strstr(s, "IND") || strcmp(s,"nan")==0)
    {
        dest = dblNaN;
        return true;
    }

    return false;
}

bool parseSimtime(const char *s, simultime_t &dest)
{
    const char *e;
    simultime_t t;
    double d;

    try {
        t = BigDecimal::parse(s, e);
        switch (*e)
        {
        case '\0': dest = t; return true;
        // simtime might be given in scientific notation
        // in older vector files, try to parse as double
        case 'e': case 'E': if (parseDouble(s, d)) { dest = BigDecimal(d); return true; }
        }
    } catch (std::exception &e) {
        // overflow
    }

    return false;
}

std::string unquoteString(const char *str)
{
    std::string result;
    if (str[0] == '"')
    {
        const char *p = str+1;
        for (; *p; ++p)
        {
            if (*p == '\\' && (*(p+1) == '\\' || *(p+1) == '"'))
                result.push_back(*(++p));
            else if (*p == '"' && *(p+1) == '\0')
                break;
            else
                result.push_back(*p);
        }
        if (*p != '"')
            throw opp_runtime_error("Missing closing quote\n");
    }
    else
        result = str;
    return result;
}

NAMESPACE_END
