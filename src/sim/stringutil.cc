//=========================================================================
//  STRINGUTIL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "omnetpp/stringutil.h"
#include "omnetpp/cexception.h"

namespace omnetpp {


#define TRY(code)   try { code; } catch (std::exception& e) { throw cRuntimeError("%s", e.what()); }

bool opp_isblank(const char *txt)
{
    return common::opp_isblank(txt);
}

std::string opp_trim(const std::string& text)
{
    return common::opp_trim(text);
}

char *opp_strprettytrunc(char *dest, const char *src, unsigned maxlen)
{
    if (!src) {
        *dest = '\0';
        return dest;
    }
    strncpy(dest, src, maxlen);
    if (strlen(src) > maxlen) {
        dest[maxlen] = '\0';
        if (maxlen >= 3)
            dest[maxlen-1] = dest[maxlen-2] = dest[maxlen-3] = '.';
    }
    return dest;
}

std::string opp_stringf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    return common::opp_vstringf(fmt, args);
}

std::string opp_vstringf(const char *fmt, va_list& args)
{
    return common::opp_vstringf(fmt, args);
}

std::string opp_replacesubstring(const std::string& text, const std::string& substring, const std::string& replacement, bool replaceAll)
{
    return common::opp_replacesubstring(text, substring, replacement, replaceAll);
}

std::vector<std::string> opp_splitandtrim(const std::string& text)
{
    return common::opp_splitandtrim(text);
}

std::vector<std::string> opp_split(const std::string& text, const std::string& separator)
{
    return common::opp_split(text, separator);
}

std::vector<std::string> opp_splitandtrim(const std::string& text, const std::string& separator)
{
    return common::opp_splitandtrim(text, separator);
}

bool opp_stringbeginswith(const char *s, const char *prefix)
{
    return common::opp_stringbeginswith(s, prefix);
}

bool opp_stringendswith(const char *s, const char *ending)
{
    return common::opp_stringendswith(s, ending);
}

std::string opp_substringbefore(const std::string& str, const std::string& substr)
{
    return common::opp_substringbefore(str, substr);
}

std::string opp_substringafter(const std::string& str, const std::string& substr)
{
    return common::opp_substringafter(str, substr);
}

std::string opp_substringbeforelast(const std::string& str, const std::string& substr)
{
    return common::opp_substringbeforelast(str, substr);
}

std::string opp_substringafterlast(const std::string& str, const std::string& substr)
{
    return common::opp_substringafterlast(str, substr);
}

std::string opp_removestart(const std::string& str, const std::string& prefix)
{
    return common::opp_removestart(str, prefix);
}

std::string opp_removeend(const std::string& str, const std::string& end)
{
    return common::opp_removeend(str, end);
}

std::string opp_strlower(const char *s)
{
    return common::opp_strlower(s);
}

std::string opp_strupper(const char *s)
{
    return common::opp_strupper(s);
}

const char *opp_strnistr(const char *haystack, const char *needle, int n, bool caseSensitive)
{
    return common::opp_strnistr(haystack, needle, n, caseSensitive);
}

int opp_strdictcmp(const char *s1, const char *s2)
{
    return common::opp_strdictcmp(s1, s2);
}

std::string opp_join(const char *separator, const char *s1, const char *s2)
{
    return common::opp_join(separator, s1, s2);
}

std::string opp_join(const char *separator, const std::string& s1, const std::string& s2)
{
    return common::opp_join(separator, s1, s2);
}

std::string opp_join(const char **strings, const char *separator, bool skipEmpty, char quoteChar)
{
    return common::opp_join(strings, separator, skipEmpty, quoteChar);
}

std::string opp_join(const char **strings, int n, const char *separator, bool skipEmpty, char quoteChar)
{
    return common::opp_join(strings, n, separator, skipEmpty, quoteChar);
}

std::string opp_join(const std::vector<std::string>& strings, const char *separator, bool skipEmpty, char quoteChar)
{
    return common::opp_join(strings, separator, skipEmpty, quoteChar);
}

char *opp_itoa(char *buf, int d)
{
    return common::opp_itoa(buf, d);
}

char *opp_ltoa(char *buf, long d)
{
    return common::opp_ltoa(buf, d);
}

char *opp_i64toa(char *buf, int64_t d)
{
    return common::opp_i64toa(buf, d);
}

char *opp_dtoa(char *buf, const char *format, double d)
{
    return common::opp_dtoa(buf, format, d);
}

long opp_strtol(const char *s, char **endptr)
{
    TRY(return common::opp_strtol(s, endptr));
}

long opp_atol(const char *s)
{
    TRY(return common::opp_atol(s));
}

unsigned long opp_strtoul(const char *s, char **endptr)
{
    TRY(return common::opp_strtoul(s, endptr));
}

unsigned long opp_atoul(const char *s)
{
    TRY(return common::opp_atoul(s));
}

long long opp_strtoll(const char *s, char **endptr)
{
    TRY(return common::opp_strtoll(s, endptr));
}

long long opp_atoll(const char *s)
{
    TRY(return common::opp_atoll(s));
}

unsigned long long opp_strtoull(const char *s, char **endptr)
{
    TRY(return common::opp_strtoull(s, endptr));
}

unsigned long long opp_atoull(const char *s)
{
    TRY(return common::opp_atoull(s));
}

double opp_strtod(const char *s, char **endptr)
{
    TRY(return common::opp_strtod(s, endptr));
}

double opp_atof(const char *s)
{
    TRY(return common::opp_atof(s));
}

}  // namespace omnetpp

