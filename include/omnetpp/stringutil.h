//=========================================================================
//  STRINGUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_STRINGUTIL_H
#define __OMNETPP_STRINGUTIL_H

#include <cstdarg>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include "simkerneldefs.h"

namespace omnetpp {

/**
 * @addtogroup StringFunctions
 * @{
 */

/**
 * @brief Returns true if the string is nullptr or has zero length.
 */
inline bool opp_isempty(const char *s)  {return !s || !s[0];}

/**
 * @brief Returns the pointer passed as argument unchanged, except that if it was nullptr,
 * it returns a pointer to a null string ("").
 */
inline const char *opp_nulltoempty(const char *s)  {return s ? s : "";}

/**
 * @brief Returns the pointer passed as argument unchanged, except that if it was empty,
 * it returns the second argument.
 */
inline const char *opp_emptytodefault(const char *s, const char *defaultString)  {return opp_isempty(s) ? defaultString : s;}

/**
 * @brief Returns true if the string only contains whitespace.
 */
SIM_API bool opp_isblank(const char *txt);

/**
 * @brief Same as the standard strlen() function, except that does not crash
 * on nullptr but returns 0.
 */
inline int opp_strlen(const char *s)
{
    return s ? strlen(s) : 0;
}

/**
 * @brief Duplicates the string, using `new char[]`. For nullptr and empty
 * strings it returns nullptr.
 */
inline char *opp_strdup(const char *s)
{
    if (!s || !s[0]) return nullptr;
    char *p = new char[strlen(s)+1];
    strcpy(p,s);
    return p;
}

/**
 * @brief Same as the standard strcpy() function, except that nullptr
 * in the second argument is treated as a pointer to an empty string ("").
 */
inline char *opp_strcpy(char *s1, const char *s2)
{
    return strcpy(s1, s2 ? s2 : "");
}

/**
 * @brief Same as the standard strcmp() function, except that nullptr
 * is treated exactly as an empty string ("").
 */
inline int opp_strcmp(const char *s1, const char *s2)
{
    if (s1)
        return s2 ? strcmp(s1,s2) : (*s1 ? 1 : 0);
    else
        return (s2 && *s2) ? -1 : 0;
}

/**
 * @brief Removes any leading and trailing whitespace.
 */
SIM_API std::string opp_trim(const std::string& text);

/**
 * @brief Copies src string into dest, and if its length would exceed maxlen,
 * it is truncated with an ellipsis. For example, `opp_strprettytrunc(buf,"long-long",6)`
 * yields `"lon..."`.
 */
SIM_API char *opp_strprettytrunc(char *dest, const char *src, unsigned maxlen);

/**
 * @brief Create a string using printf-like formatting. Allocates storage dynamically.
 */
_OPP_GNU_ATTRIBUTE(format(printf, 1, 2))
SIM_API std::string opp_stringf(const char *fmt, ...);

/**
 * @brief Create a string using printf-like formatting. Allocates storage dynamically.
 */
SIM_API std::string opp_vstringf(const char *fmt, va_list& args);

/**
 * @brief Performs find/replace within a string.
 */
SIM_API std::string opp_replacesubstring(const std::string& text, const std::string& substring, const std::string& replacement, bool replaceAll);

/**
 * @brief Split a string into parts by whitespace sequences. Returned items will have no
 * leading or trailing whitespace. Returns an empty array if the input string is
 * empty or only contains whitespace.
 */
SIM_API std::vector<std::string> opp_splitandtrim(const std::string& text);

/**
 * @brief Split a string into parts separated by the given separator. If the input string
 * is empty, it returns an empty array. Otherwise, it returns exactly \#separators+1 items.
 * The separator cannot be empty.
 */
SIM_API std::vector<std::string> opp_split(const std::string& text, const std::string& separator);

/**
 * @brief Split a string into parts separated by the given separator, trimming each item of whitespace.
 * Returns an empty array if the input string doesn't contain anything but whitespace.
 * The separator cannot be empty.
 */
SIM_API std::vector<std::string> opp_splitandtrim(const std::string& text, const std::string& separator);

/**
 * @brief Returns true if the first string begins with the second string.
 */
SIM_API bool opp_stringbeginswith(const char *s, const char *prefix);

/**
 * @brief Returns true if the first string ends in the second string.
 */
SIM_API bool opp_stringendswith(const char *s, const char *ending);

/**
 * @brief Returns the substring up to the first occurrence of the given substring, or "".
 */
SIM_API std::string opp_substringbefore(const std::string& str, const std::string& substr);

/**
 * @brief Returns the substring after the first occurrence of the given substring, or "".
 */
SIM_API std::string opp_substringafter(const std::string& str, const std::string& substr);

/**
 * @brief Returns the substring up to the last occurrence of the given substring, or "".
 */
SIM_API std::string opp_substringbeforelast(const std::string& str, const std::string& substr);

/**
 * @brief Returns the substring after the last occurrence of the given substring, or "".
 */
SIM_API std::string opp_substringafterlast(const std::string& str, const std::string& substr);

/**
 * @brief Remove the prefix if the s string starts with it, otherwise return the string unchanged.
 */
SIM_API std::string opp_removestart(const std::string& str, const std::string& prefix);

/**
 * @brief Remove the given end string if the s string ends with it, otherwise return the string unchanged.
 */
SIM_API std::string opp_removeend(const std::string& str, const std::string& end);

/**
 * @brief Converts the string to lower case, and returns the result.
 */
SIM_API std::string opp_strlower(const char *s);

/**
 * @brief Converts the string to upper case, and returns the result.
 */
SIM_API std::string opp_strupper(const char *s);

/**
 * @brief Locates the first occurrence of the nul-terminated string needle in the
 * string haystack, where not more than n characters are searched. Characters
 * that appear after a '\0' character are not searched.
 */
SIM_API const char *opp_strnistr(const char *haystack, const char *needle, int n, bool caseSensitive);

/**
 * @brief Dictionary-compare two strings, the main difference from strcasecmp()
 * being that integers embedded in the strings are compared in numerical order.
 */
SIM_API int opp_strdictcmp(const char *s1, const char *s2);

/**
 * @brief If either s1 or s2 is empty, returns the other one, otherwise returns
 * s1 + separator + s2.
 */
SIM_API std::string opp_join(const char *separator, const char *s1, const char *s2);

/**
 * @brief If either s1 or s2 is empty, returns the other one, otherwise returns
 * s1 + separator + s2.
 */
SIM_API std::string opp_join(const char *separator, const std::string& s1, const std::string& s2);

/**
 * @brief Concatenate the strings passed in the nullptr-terminated const char * array, using
 * the given separator and putting each item between quoteChars unless it is '\0'.
 * Empty elements are optionally skipped.
 */
SIM_API std::string opp_join(const char **strings, const char *separator, bool skipEmpty=false, char quoteChar=0);

/**
 * @brief Concatenate the strings passed in the const char * array of size n, using
 * the given separator and putting each item between quoteChars unless it is '\0'.
 * Empty elements are optionally skipped.
 */
SIM_API std::string opp_join(const char **strings, int n, const char *separator, bool skipEmpty=false, char quoteChar=0);

/**
 * @brief Concatenate the strings passed in the vector, using the given separator,
 * and putting each item between quoteChars unless it is '\0'.
 * Empty elements are optionally skipped.
 */
SIM_API std::string opp_join(const std::vector<std::string>& strings, const char *separator, bool skipEmpty=false, char quoteChar=0);

/**
 * @brief Prints the d integer into the given buffer, then returns the buffer pointer.
 */
SIM_API char *opp_itoa(char *buf, int d);

/**
 * @brief Prints the d integer into the given buffer, then returns the buffer pointer.
 */
SIM_API char *opp_ltoa(char *buf, long d);

/**
 * @brief Prints the d integer into the given buffer, then returns the buffer pointer.
 */
SIM_API char *opp_i64toa(char *buf, int64_t d);

/**
 * @brief Prints the d double into the given buffer, then returns the buffer pointer.
 * If d is finite, the given printf format is used (e.g. "%g"), otherwise
 * it prints "inf", "-inf" or "nan". (Note that printf's handling of NaN and
 * infinity is platform-dependent, e.g. MSVC produces "1.#QNAN" and "1.#INF".)
 */
SIM_API char *opp_dtoa(char *buf, const char *format, double d);

/**
 * @brief Like the standard strtol(), but throws opp_runtime_error if an overflow
 * occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
SIM_API long opp_strtol(const char *s, char **endptr);

/**
 * @brief Like the standard atol(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number. Accepts decimal and C-style hexadecimal notation, but not
 * octal (leading zeroes are simply discarded and the number is interpreted as
 * decimal).
 */
SIM_API long opp_atol(const char *s);

/**
 * @brief Like the standard strtoul(), but throws opp_runtime_error if an
 * overflow occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
SIM_API unsigned long opp_strtoul(const char *s, char **endptr);

/**
 * @brief Like the standard atol(), but for unsigned long, and throws opp_runtime_error
 * if an overflow occurs during conversion, or if there is (non-whitespace)
 * trailing garbage after the number. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
SIM_API unsigned long opp_atoul(const char *s);

/**
 * @brief Like the standard strtoll(), but throws opp_runtime_error if an overflow
 * occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
SIM_API long long opp_strtoll(const char *s, char **endptr);

/**
 * @brief Like the standard atoll(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number. Accepts decimal and C-style hexadecimal notation, but not
 * octal (leading zeroes are simply discarded and the number is interpreted as
 * decimal).
 */
SIM_API long long opp_atoll(const char *s);

/**
 * @brief Like the standard strtoull(), but throws opp_runtime_error if an overflow
 * occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
SIM_API unsigned long long opp_strtoull(const char *s, char **endptr);

/**
 * @brief Like the standard atoull(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number. Accepts decimal and C-style hexadecimal notation, but not
 * octal (leading zeroes are simply discarded and the number is interpreted as
 * decimal).
 */
SIM_API unsigned long long opp_atoull(const char *s);

/**
 * @brief Like the standard strtod(), but throws opp_runtime_error if an overflow
 * occurs during conversion.
 */
SIM_API double opp_strtod(const char *s, char **endptr);

/**
 * @brief Like the standard atof(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number.
 */
SIM_API double opp_atof(const char *s);

/** @} */

}  // namespace omnetpp

#endif


