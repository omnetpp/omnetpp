//=========================================================================
//  COMMONUTIL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>
#include <clocale>
#include <string>
#include <typeindex>
#include <unordered_map>
#include "omnetpp/platdep/platmisc.h"
#include "commonutil.h"
#include "opp_ctype.h"
#ifndef _WIN32
#include <unistd.h>
#endif

namespace omnetpp {
namespace common {


static const double zero = 0.0;
double const NaN = zero / zero;
double const POSITIVE_INFINITY = 1.0 / zero;
double const NEGATIVE_INFINITY = -1.0 / zero;

bool __insidemain = false;
bool __exiting = false;

//----

void setPosixLocale()
{
    setlocale(LC_ALL, "C");
}

//----

const char *opp_gethostname()
{
#ifdef _WIN32
    // Windows also has gethostname(), but we don't want to pull in winsock2 just for that
    return getenv("COMPUTERNAME");
#else
    static char buf[128];
    if (gethostname(buf, sizeof(buf)) == 0)
        return buf;
    else if (getenv("HOST") != nullptr)
        return getenv("HOST");
    else
        return getenv("HOSTNAME");
#endif
}

//----

void panic(const char *message)
{
    fprintf(stderr, "<!> %s\n", message);
    fprintf(stderr, "Note: This is an unrecoverable error due to technical reasons, aborting.\n");
    abort();
}

//----

int CallTracer::depth;

CallTracer::CallTracer(const char *fmt, ...)
{
    char buf[1024];
    VSNPRINTF(buf, 1024, fmt);
    funcname = buf;
    std::cout << std::setw(depth++*4) << "" << funcname << " {" << std::endl;
}

CallTracer::~CallTracer()
{
    std::cout << std::setw(--depth*4) << "" << "}";
    if (!result.empty())
        std::cout << ", result: " << result;
    std::cout << std::endl;
}

void CallTracer::printf(const char *fmt, ...)
{
    char buf[1024];
    VSNPRINTF(buf, 1024, fmt);
    std::cout << std::setw(depth*4) << "" << "  " << buf << std::endl;
}

//----
static std::string demangle(const char *mangledName)
{
    const char *s = mangledName;
#ifdef __GNUC__
    // gcc's typeinfo returns mangled name:
    //   - Foo -> "3Foo"
    //   - omnetpp::Foo -> "N7omnetpp3FooE"
    //   - omnetpp::inner::Foo -> "N7omnetpp5inner3FooE"
    //   - std::runtime_error -> "St13runtime_error"
    //   - Foo* -> "P3Foo" (P prefix means pointer)
    //   - Dn -> decltype(nullptr)
    // see libiberty/cp_demangle.c, or run c++filt -t <name> on any mangled type name
    //
    if (opp_isdigit(*s)) {
        // no namespace: just skip the leading number
        while (opp_isdigit(*s))
            s++;
        return s;
    }

    std::string result;
    std::string prefix, suffix;
    if (*s == 'P' && *(s+1) == 'K') {
        // PKx -> "const x *"
        prefix = "const ";
        suffix = " *";
        s += 2;
    }
    while (true) {
        if (*s == 'P')
            suffix += " *";
        else if (*s == 'K')
            suffix += " const";
        else
            break;
        s++;
    }

    switch (*s) {
        case 'v': result = "void"; break;
        case 'b': result = "bool"; break;
        case 's': result = "short"; break;
        case 't': result = "unsigned short"; break;
        case 'i': result = "int"; break;
        case 'j': result = "unsigned int"; break;
        case 'l': result = "long"; break;
        case 'm': result = "unsigned long"; break;
        case 'f': result = "float"; break;
        case 'd': result = "double"; break;
        case 'c': result = "char"; break;
        case 'a': result = "signed char"; break;
        case 'h': result = "unsigned char"; break;
        case 'N': {
            // mangled name contains namespace: decode it
            result.reserve(strlen(s)+8);
            s++; // skip leading 'N'
            while (opp_isdigit(*s)) {
                int len = (int)strtol(s, (char **)&s, 10);
                if (!result.empty()) result += "::";
                result.append(s, len);
                s += len;
            }
            break;
        }
        case 'S': {
            // probably std::something, e.g. "St13runtime_error"
            switch (s[1]) {
            // some "Sx" prefixes are special abbreviations
            case 'a': result = "std::allocator"; break;
            case 'b': result = "std::basic_string"; break;
            case 's': result = "std::string"; break;
            case 'i': result = "std::istream"; break;
            case 'o': result = "std::ostream"; break;
            case 'd': result = "std::iostream"; break;
            case 't':
                // "St" -> std::
                s+=2;
                result.reserve(strlen(s)+8);
                result.append("std");
                while (opp_isalpha(*s)) s++; // skip possible other modifiers
                while (opp_isdigit(*s)) {
                    int len = (int)strtol(s, (char **)&s, 10);
                    if (!result.empty()) result += "::";
                    result.append(s, len);
                    s += len;
                }
                break;
            }
            break;
        }
        default: {
            if (opp_isdigit(*s)) {
                // no namespace: just skip the leading number
                while (opp_isdigit(*s))
                    s++;
                result = s;
            }
            else if (strcmp(s, "Dn") == 0)
                result = "nullptr_t";
            else {
                // dunno how to interpret it, just return it unchanged
                result = s;
            }
        }
    }
    return prefix + result + suffix;

#else

    // MSVC prepends the string with "class " (possibly other compilers too)
    if (s[0]=='c' && s[1]=='l' && s[2]=='a' && s[3]=='s' && s[4]=='s' && s[5]==' ')
        s+=6;
    else if (s[0]=='s' && s[1]=='t' && s[2]=='r' && s[3]=='u' && s[4]=='c' && s[5]=='t' && s[6]==' ')
        s+=7;
    else if (s[0]=='e' && s[1]=='n' && s[2]=='u' && s[3]=='m' && s[4]==' ')
        s+=5;
    return s;
#endif
}

std::unordered_map<std::type_index, std::string> demangledNames;

const char *opp_typename(const std::type_info& t)
{
    if (t == typeid(std::string))
        return "std::string";  // otherwise we'd get "std::basic_string<........>"

    auto it = demangledNames.find(t);
    if (it == demangledNames.end()) {
        demangledNames[t] = demangle(t.name());
        it = demangledNames.find(t);
    }
    return it->second.c_str();
}


}  // namespace common
}  // namespace omnetpp

