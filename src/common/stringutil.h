//=========================================================================
//  STRINGUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _STRINGUTIL_H_
#define _STRINGUTIL_H_

#include <stdarg.h>
#include <string>
#include "commondefs.h"
#include "commonutil.h"

//NAMESPACE_BEGIN

/**
 * Returns true if the string is NULL or has zero length.
 */
inline bool opp_isempty(const char *s)  {return !s || !s[0];}

/**
 * Returns the pointer passed as argument unchanged, except that if it was NULL,
 * it returns a pointer to a null string ("").
 */
inline const char *opp_nulltoempty(const char *s)  {return s ? s : "";}

/**
 * Returns true if the string only contains whitespace.
 */
COMMON_API bool opp_isblank(const char *txt);

//
// The following few inline functions are present in the simkernel's utils.h too;
// define them conditionally to prevent clashes.
//
#ifndef __SIM_UTIL_H

/**
 * Same as the standard strlen() function, except that does not crash
 * on NULL pointers but returns 0.
 */
inline int opp_strlen(const char *s)
{
    return s ? strlen(s) : 0;
}

/**
 * Duplicates the string, using <tt>new char[]</tt>. For NULLs and empty
 * strings it returns NULL.
 */
inline char *opp_strdup(const char *s)
{
    if (!s || !s[0]) return NULL;
    char *p = new char[strlen(s)+1];
    strcpy(p,s);
    return p;
}

/**
 * Same as the standard strcpy() function, except that NULL pointers
 * in the second argument are treated like pointers to a null string ("").
 */
inline char *opp_strcpy(char *s1, const char *s2)
{
    return strcpy(s1, s2 ? s2 : "");
}

/**
 * Same as the standard strcmp() function, except that NULL pointers
 * are treated exactly as empty strings ("").
 */
inline int opp_strcmp(const char *s1, const char *s2)
{
    if (s1)
        return s2 ? strcmp(s1,s2) : (*s1 ? 1 : 0);
    else
        return (s2 && *s2) ? -1 : 0;
}
#endif //__SIM_UTIL_H

/**
 * Reverse of opp_quotestr(): remove quotes and resolve backslashed escapes.
 *
 * Throws an exception if there's a parse error. If there's anything
 * (non-whitespace) in the input after the string literal, endp is set to
 * that character; otherwise endp is set to point to the terminating zero
 * of the string.
 */
COMMON_API std::string opp_parsequotedstr(const char *txt, const char *&endp);

/**
 * Reverse of opp_quotestr(): remove quotes and resolve backslashed escapes.
 *
 * Throws an exception if there's a parse error.
 */
COMMON_API std::string opp_parsequotedstr(const char *txt);

/**
 * Surround the given string with "quotes", also escape with backslash
 * where needed.
 */
COMMON_API std::string opp_quotestr(const char *txt);

/**
 * Returns true if the string contains space, backslash, quote, or anything
 * else that would make quoting (opp_quotestr()) necessary before writing
 * it into a data file.
 */
COMMON_API bool opp_needsquotes(const char *txt);

/**
 * Combines opp_needsquotes() and opp_quotestr().
 */
inline std::string opp_quotestr_ifneeded(const char *txt)
{
    return opp_needsquotes(txt) ? opp_quotestr(txt) : std::string(txt);
}

/**
 * A macro version of opp_quotestr_ifneeded(). This is more efficient,
 * because it avoids conversion to std::string when no quoting is needed.
 */
#define QUOTE(txt)   (opp_needsquotes(txt) ? opp_quotestr(txt).c_str() : (txt))

/**
 * Create a string using printf-like formatting. Limit: 1023 chars.
 */
COMMON_API std::string opp_stringf(const char *fmt, ...);

/**
 * Create a string using printf-like formatting. Limit: 1023 chars.
 */
COMMON_API std::string opp_vstringf(const char *fmt, va_list& args);

/**
 * A limited vsscanf implementation, used by cStatistic::freadvarsf()
 */
COMMON_API int opp_vsscanf(const char *s, const char *fmt, va_list va);

/**
 * Performs find/replace within a string.
 */
COMMON_API std::string opp_replacesubstring(const char *s, const char *substring, const char *replacement, bool replaceAll);

/**
 * Inserts newlines into the string, performing rudimentary line breaking.
 */
COMMON_API std::string opp_breaklines(const char *text, int maxLineLength);

/**
 * Indent each line of the input text.
 */
COMMON_API std::string opp_indentlines(const char *text, const char *indent);

/**
 * Returns true if the first string begins with the second string.
 */
COMMON_API bool opp_stringbeginswith(const char *s, const char *prefix);

/**
 * Returns true if the first string ends in the second string.
 */
COMMON_API bool opp_stringendswith(const char *s, const char *ending);

/**
 * Concatenates up to four strings. Returns a pointer to a static buffer
 * of length 256. If the result length would exceed 256, it is truncated.
 */
COMMON_API char *opp_concat(const char *s1, const char *s2, const char *s3=NULL, const char *s4=NULL);

/**
 * If either s1 or s2 is empty, returns the other one, otherwise returns
 * s1 + separator + s2.
 */
COMMON_API std::string opp_join(const char *separator, const char *s1, const char *s2);

/**
 * Dictionary-compare two strings, the main difference from strcasecmp()
 * being that integers embedded in the strings are compared in
 * numerical order.
 */
COMMON_API int strdictcmp(const char *s1, const char *s2);

/**
 * Like the standard strtol() with base==0, but throws opp_runtime_error if an
 * overflow occurs during conversion.
 */
COMMON_API long opp_strtol(const char *s, char **endptr);

/**
 * Like the standard atol(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number.
 */
COMMON_API long opp_atol(const char *s);

/**
 * Like the standard strtoul() with base==0, but throws opp_runtime_error if an
 * overflow occurs during conversion.
 */
COMMON_API unsigned long opp_strtoul(const char *s, char **endptr);

/**
 * Like the standard atol(), but for unsigned long, and throws opp_runtime_error
 * if an overflow occurs during conversion, or if there is (non-whitespace)
 * trailing garbage after the number.
 */
COMMON_API unsigned long opp_atoul(const char *s);

/**
 * Like the standard strtod(), but throws opp_runtime_error if an overflow
 * occurs during conversion.
 */
COMMON_API double opp_strtod(const char *s, char **endptr);

/**
 * Like the standard atof(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number.
 */
COMMON_API double opp_atof(const char *s);

//NAMESPACE_END


#endif


