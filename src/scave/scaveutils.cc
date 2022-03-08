//=========================================================================
//  SCAVEUTILS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include <cstring>
#include <utility>
#include <clocale>
#include "omnetpp/platdep/platmisc.h"
#include "scaveutils.h"

namespace omnetpp {
namespace scave {


bool parseInt(const char *s, int& dest)
{
    char *e;
    dest = (int)strtol(s, &e, 10);
    return !*e;
}

bool parseLong(const char *s, long& dest)
{
    char *e;
    dest = strtol(s, &e, 10);
    return !*e;
}

bool parseInt64(const char *s, int64_t& dest)
{
    char *e;
    dest = strtoll(s, &e, 10);
    return !*e;
}

bool parseDouble(const char *s, double& dest)
{
    char *e;
    setlocale(LC_NUMERIC, "C");
    dest = strtod(s, &e);
    if (!*e) {
        return true;
    }

    // infinity: GCC: "inf" or "-inf"; MSVC: "1.#INF" or "-1.#INF"
    if (strstr(s, "inf") || strstr(s, "Inf") || strstr(s, "INF")) {
        dest = POSITIVE_INFINITY;  // +INF or -INF
        if (*s == '-')
            dest = -dest;
        return true;
    }

    // not-a-number: GCC: "nan"; MSVC: "1.#IND"
    if (strstr(s, "nan") || strstr(s, "NaN") || strstr(s, "NAN") || strstr(s, "IND")) {
        dest = NaN;
        return true;
    }

    return false;
}

bool parseSimtime(const char *s, simultime_t& dest)
{
    const char *e;
    simultime_t t;
    double d;

    try {
        t = BigDecimal::parse(s, e);
        switch (*e) {
            case '\0':
                dest = t;
                return true;

            // simtime might be given in scientific notation
            // in older vector files, try to parse as double
            case 'e':
            case 'E':
                if (parseDouble(s, d)) {
                    dest = BigDecimal(d);
                    return true;
                }
        }
    }
    catch (std::exception& e) {
        // overflow
    }
    return false;
}

std::string unquoteString(const char *str)
{
    std::string result;
    if (str[0] == '"') {
        const char *p = str+1;
        for ( ; *p; ++p) {
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

const std::string *ScaveStringPool::insert(const std::string& str)
{
    if (!lastInsertedPtr || *lastInsertedPtr != str) {
        std::pair<std::set<std::string>::iterator, bool> p = pool.insert(str);
        lastInsertedPtr = &(*p.first);
    }
    return lastInsertedPtr;
}

const std::string *ScaveStringPool::find(const std::string& str) const
{
    if (lastInsertedPtr && *lastInsertedPtr == str)
        return lastInsertedPtr;

    std::set<std::string>::const_iterator it = pool.find(str);
    return it != pool.end() ? &(*it) : nullptr;
}

}  // namespace scave
}  // namespace omnetpp

