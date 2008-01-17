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

NAMESPACE_BEGIN


/**
 * Returns true if the string only contains whitespace.
 */
COMMON_API bool opp_isblank(const char *txt);

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
 * Duplicates a string, the result should be freed with delete[].
 * Named so in order to avoid clash with the simkernel's opp_strdup().
 */
COMMON_API char *opp_clonestr(const char *s);

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
 * Returns true if the first string ends in the second string.
 */
COMMON_API bool opp_stringendswith(const char *s, const char *ending);

/**
 * Dictionary-compare two strings, the main difference from strcasecmp()
 * being that integers embedded in the strings are compared in
 * numerical order.
 */
COMMON_API int strdictcmp(const char *s1, const char *s2);

NAMESPACE_END


#endif


