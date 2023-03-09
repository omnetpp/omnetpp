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

#ifndef __OMNETPP_COMMON_STRINGUTIL_H
#define __OMNETPP_COMMON_STRINGUTIL_H

#include <cstdarg>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include "commondefs.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

//
// GUIDELINES for choosing between std::string and const char * for new functions:
// 1. No std::string if const char * is sufficient.
// 2. No mixing within one function signature: if return type or one arg is std::string, use std::string throughout.
//

/**
 * Returns true if the string is nullptr or has zero length.
 */
inline bool opp_isempty(const char *s)  {return !s || !s[0];}

/**
 * Returns the pointer passed as argument unchanged, except that if it was nullptr,
 * it returns a pointer to a null string ("").
 */
inline const char *opp_nulltoempty(const char *s)  {return s ? s : "";}

/**
 * Returns the pointer passed as argument unchanged, except that if it was empty,
 * it returns the second argument.
 */
inline const char *opp_emptytodefault(const char *s, const char *defaultString)  {return opp_isempty(s) ? defaultString : s;}

/**
 * Returns true if the string only contains whitespace.
 */
COMMON_API bool opp_isblank(const char *txt);

/**
 * Same as the standard strlen() function, except that does not crash
 * on nullptr but returns 0.
 */
inline int opp_strlen(const char *s)
{
    return s ? strlen(s) : 0;
}

/**
 * Duplicates the string, using <tt>new char[]</tt>. For nullptr and empty
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
 * Duplicates the string, using <tt>new char[]</tt>. For nullptr and empty
 * strings it returns nullptr. len is maximum length, longer strings will
 * be truncated.
 */
inline char *opp_strndup(const char *s, int len)
{
    if (!s || !s[0]) return nullptr;
    char *p = new char[len+1];
    strncpy(p,s,len);
    p[len] = 0;
    return p;
}

/**
 * Same as the standard strcpy() function, except that nullptr
 * in the second argument is treated as a pointer to an empty string ("").
 */
inline char *opp_strcpy(char *s1, const char *s2)
{
    return strcpy(s1, s2 ? s2 : "");
}

/**
 * Same as the standard strcmp() function, except that nullptr
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
 * Aren't you sick and tired of having to write strcmp()==0 or !strcmp() for the millionth time?
 */
inline bool opp_streq(const char *s1, const char *s2)
{
    return opp_strcmp(s1, s2) == 0;
}

/**
 * Removes any leading and trailing whitespace.
 */
COMMON_API std::string opp_trim(const std::string& text);

/**
 * Checks whether a string looks like a valid identifier (starts with underscore
 * or alpha, continues with underscore, alpha or digit characters).
 */
COMMON_API bool opp_isvalididentifier(const char *s);

/**
 * Find the end of a quoted string constant, obeying backslashed escapes.
 * The first character of the string specifies which character is used as
 * quotation mark. Returns a pointer to the matching quote character, or
 * nullptr if the string ended without finding one.
 */
COMMON_API const char *opp_findclosequote(const char *txt);

/**
 * Reverse of opp_quotestr(): remove quotes and resolve backslashed escapes.
 * The quot argument specifies which character is used as quotation mark;
 * acceptable values are double quote ("hello") and apostrophe ('hello'),
 * and the NUL value which means "whichever the input string starts with".
 *
 * Throws an exception if there is a parse error. If there is anything
 * (non-whitespace) in the input after the string literal, endp is set to
 * that character; otherwise endp is set to point to the terminating zero
 * of the string.
 */
COMMON_API std::string opp_parsequotedstr(const char *txt, const char *&endp, char quot='"');

/**
 * Reverse of opp_quotestr(): remove quotes and resolve backslashed escapes.
 * The quot argument specifies which character is used as quotation mark;
 * acceptable values are double quote ("hello") and apostrophe ('hello'),
 * and the NUL value which means "whichever the input string starts with".
 *
 * Throws an exception if there is a parse error.
 */
COMMON_API std::string opp_parsequotedstr(const char *txt, char quot='"');

/**
 * Surround the given string with "quotes", also escape with backslash
 * where needed.
 */
COMMON_API std::string opp_quotestr(const std::string& txt);

/**
 * Returns true if the string contains space, backslash, quote, or anything
 * else that would make quoting (opp_quotestr()) necessary before writing
 * it into a data file.
 */
COMMON_API bool opp_needsquotes(const char *txt);

/**
 * Combines opp_needsquotes() and opp_quotestr().
 */
inline std::string opp_quotestr_ifneeded(const std::string& txt)
{
    return opp_needsquotes(txt.c_str()) ? opp_quotestr(txt) : txt;
}

/**
 * A macro version of opp_quotestr_ifneeded(). This is more efficient,
 * because it avoids conversion to std::string when no quoting is needed.
 * USE WITH CARE.
 */
#define QUOTE(txt)   (opp_needsquotes(txt) ? opp_quotestr(txt).c_str() : (txt))

/**
 * Create a string using printf-like formatting. Allocates storage dynamically.
 */
_OPP_GNU_ATTRIBUTE(format(printf, 1, 2))
COMMON_API std::string opp_stringf(const char *fmt, ...);

/**
 * Create a string using printf-like formatting. Allocates storage dynamically.
 */
COMMON_API std::string opp_vstringf(const char *fmt, va_list& args);

/**
 * A limited vsscanf implementation, used by cStatistic::freadvarsf()
 */
COMMON_API int opp_vsscanf(const char *s, const char *fmt, va_list va);

/**
 * Performs find/replace within a string.
 */
COMMON_API std::string opp_replacesubstring(const std::string& text, const std::string& substring, const std::string& replacement, bool replaceAll);

/**
 * For opp_substitutevariables().
 */
struct opp_substitutevariables_resolver {
    virtual ~opp_substitutevariables_resolver() {}
    virtual bool isVariableNameChar(char c) = 0;
    virtual std::string operator()(const std::string&) = 0;
};

/**
 * Substitutes dollar variables ($name or ${name}) into the given string.
 * The second arg is a function that should return the value of a variable
 * from its name. (It may also throw an exception if there is no such variable.)
 */
COMMON_API std::string opp_substitutevariables(const std::string& raw, opp_substitutevariables_resolver& resolver);

/**
 * Substitutes dollar variables ($name or ${name}) into the given string, using the
 * dictionary passed as second arg. Throws an exception if a variable is not found.
 */
COMMON_API std::string opp_substitutevariables(const std::string& raw, const std::map<std::string,std::string>& vars);

/**
 * Inserts newlines into the string, performing rudimentary line breaking.
 */
COMMON_API std::string opp_breaklines(const std::string& text, int maxLineLength);

/**
 * Indent each line of the input text.
 */
COMMON_API std::string opp_indentlines(const std::string& text, const std::string& indent);

/**
 * Split a string into parts by whitespace sequences. Returned items will have no
 * leading or trailing whitespace. Returns an empty array if the input string is
 * empty or only contains whitespace.
 */
COMMON_API std::vector<std::string> opp_splitandtrim(const std::string& text);

/**
 * Split a string into parts separated by the given separator. The separator
 * cannot be empty. If the input string is empty, it returns an empty array.
 * Otherwise, it returns exactly #separators+1 items.
 */
COMMON_API std::vector<std::string> opp_split(const std::string& text, const std::string& separator);

/**
 * Split a string into parts separated by the given separator, trimming each item of whitespace.
 * The separator cannot be empty. Returns an empty array if the input string doesn't contain
 * anything but whitespace.
 */
COMMON_API std::vector<std::string> opp_splitandtrim(const std::string& text, const std::string& separator);

/**
 * Split a string that contains a series of file system paths (directory names),
 * like the PATH variable on Windows and *nix systems. Both colons and semicolons
 * are regarded as separators, regardless of the operating systems. Care is taken
 * not to split the string after possible Windows path letters, e.g. "foo:C:\tmp"
 * is parsed correcly as ["foo", "C:\tmp"], and not as ["foo", "C", "\tmp"].
 * Empty items are skipped.
 */
COMMON_API std::vector<std::string> opp_splitpath(const std::string& path);

/**
 * Returns a vector of C strings that point to c_str() of the strings in the input vector.
 */
COMMON_API std::vector<const char *> opp_cstrings(const std::vector<std::string>& v);

/**
 * Ensure that the string is not longer than maxlen. If it is, it is truncated
 * and "..." is added.
 */
COMMON_API std::string opp_abbreviate(const std::string& text, int maxlen);

/**
 * Aligns columns of a table. Table columns should be separated by tab.
 * Columns widths are automatically determined, but can be overridden
 * by specifying positive numbers at the respective indices in userColumnWidths[]
 * (i.e. specify 0 or -1 in the array to use automatic width.)
 */
COMMON_API std::string opp_format_table(const std::string& text, int spacing=2, const std::vector<int>& userColumnWidths=std::vector<int>());

/**
 * Returns true if the first string begins with the second string.
 */
COMMON_API bool opp_stringbeginswith(const char *s, const char *prefix);

/**
 * Returns true if the first string ends in the second string.
 */
COMMON_API bool opp_stringendswith(const char *s, const char *ending);

/**
 * Returns the substring up to the first occurrence of the given substring, or "".
 */
COMMON_API std::string opp_substringbefore(const std::string& str, const std::string& substr);

/**
 * Returns the substring after the first occurrence of the given substring, or "".
 */
COMMON_API std::string opp_substringafter(const std::string& str, const std::string& substr);

/**
 * Returns the substring up to the last occurrence of the given substring, or "".
 */
COMMON_API std::string opp_substringbeforelast(const std::string& str, const std::string& substr);

/**
 * Returns the substring after the last occurrence of the given substring, or "".
 */
COMMON_API std::string opp_substringafterlast(const std::string& str, const std::string& substr);

/**
 * Remove the prefix if the string starts with it, otherwise return the string unchanged.
 */
COMMON_API std::string opp_removestart(const std::string& str, const std::string& prefix);

/**
 * Remove the given end string if the s string ends with it, otherwise return the string unchanged.
 */
COMMON_API std::string opp_removeend(const std::string& str, const std::string& end);

/**
 * Locates the first occurrence of the nul-terminated string needle in the
 * string haystack, where not more than n characters are searched. Characters
 * that appear after a '\0' character are not searched.
 */
COMMON_API const char *opp_strnistr(const char *haystack, const char *needle, int n, bool caseSensitive);

/**
 * Converts the string to lower case, and returns the result.
 */
COMMON_API std::string opp_strlower(const char *s);

/**
 * Converts the string to upper case, and returns the result.
 */
COMMON_API std::string opp_strupper(const char *s);

/**
 * Dictionary-compare two strings, the main difference from strcasecmp()
 * being that integers embedded in the strings are compared in
 * numerical order.
 */
COMMON_API int opp_strdictcmp(const char *s1, const char *s2);

/**
 * If either s1 or s2 is empty, returns the other one, otherwise returns
 * s1 + separator + s2.
 */
COMMON_API std::string opp_join(const char *separator, const char *s1, const char *s2);

/**
 * If either s1 or s2 is empty, returns the other one, otherwise returns
 * s1 + separator + s2.
 */
COMMON_API std::string opp_join(const char *separator, const std::string& s1, const std::string& s2);

/**
 * Concatenate the strings passed in the nullptr-terminated const char * array, using
 * the given separator and putting each item between quoteChars unless it is '\0'.
 * Empty elements are optionally skipped.
 */
COMMON_API std::string opp_join(const char **strings, const char *separator, bool skipEmpty=false, char quoteChar=0);

/**
 * Concatenate the strings passed in the const char * array of size n, using
 * the given separator and putting each item between quoteChars unless it is '\0'.
 * Empty elements are optionally skipped.
 */
COMMON_API std::string opp_join(const char **strings, int n, const char *separator, bool skipEmpty=false, char quoteChar=0);

/**
 * Concatenate the strings passed in the vector, using the given separator,
 * and putting each item between quoteChars unless it is '\0'.
 * Empty elements are optionally skipped.
 */
COMMON_API std::string opp_join(const std::vector<std::string>& strings, const char *separator, bool skipEmpty=false, char quoteChar=0);

/**
 * Returns "name[index]", or "name" if index==-1.
 */
COMMON_API std::string opp_indexedname(const char *name, int index=-1);

/**
 * Prints "name[index]" into the buffer in an efficient way, and returns its pointer.
 */
COMMON_API char *opp_indexedname(char *buf, size_t bufsize, const char *name, int index);

/**
 * Prints the d integer into the given buffer, then returns the buffer pointer.
 */
COMMON_API char *opp_itoa(char *buf, int d);

/**
 * Prints the d integer into the given buffer, then returns the buffer pointer.
 */
COMMON_API char *opp_ltoa(char *buf, long d);

/**
 * Prints the d integer into the given buffer, then returns the buffer pointer.
 */
COMMON_API char *opp_i64toa(char *buf, int64_t d);

/**
 * Prints the d double into the given buffer, then returns the buffer pointer.
 * If d is finite, the given printf format is used (e.g. "%g"), otherwise
 * it prints "inf", "-inf" or "nan". (Note that printf's handling of NaN and
 * infinity is platform-dependent, e.g. MSVC produces "1.#QNAN" and "1.#INF".)
 */
COMMON_API char *opp_dtoa(char *buf, const char *format, double d);

/**
 * Utility function to convert a 64-bit fixed point number into a string
 * buffer. scaleexp must be in the -18..18 range, and the buffer must be
 * at least 64 bytes long. A pointer to the result string will be
 * returned. A pointer to the terminating '\\0' will be returned in endp.
 *
 * ATTENTION: For performance reasons, the returned pointer will point
 * *somewhere* into the buffer, but NOT necessarily at the beginning.
 */
COMMON_API char *opp_ttoa(char *buf, int64_t t, int scaleexp, char *& endp);

/**
 * Like the standard strtol(), but throws opp_runtime_error if an overflow
 * occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
COMMON_API long opp_strtol(const char *s, char **endptr);

/**
 * Like the standard atol(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number. Accepts decimal and C-style hexadecimal notation, but not
 * octal (leading zeroes are simply discarded and the number is interpreted as
 * decimal).
 */
COMMON_API long opp_atol(const char *s);

/**
 * Like the standard strtoul(), but throws opp_runtime_error if an
 * overflow occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
COMMON_API unsigned long opp_strtoul(const char *s, char **endptr);

/**
 * Like the standard atol(), but for unsigned long, and throws opp_runtime_error
 * if an overflow occurs during conversion, or if there is (non-whitespace)
 * trailing garbage after the number. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
COMMON_API unsigned long opp_atoul(const char *s);

/**
 * Like the standard strtoll(), but throws opp_runtime_error if an overflow
 * occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
COMMON_API long long opp_strtoll(const char *s, char **endptr);

/**
 * Like the standard atoll(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number. Accepts decimal and C-style hexadecimal notation, but not
 * octal (leading zeroes are simply discarded and the number is interpreted as
 * decimal).
 */
COMMON_API long long opp_atoll(const char *s);

/**
 * Like the standard strtoull(), but throws opp_runtime_error if an overflow
 * occurs during conversion. Accepts decimal and C-style hexadecimal
 * notation, but not octal (leading zeroes are simply discarded and the number
 * is interpreted as decimal).
 */
COMMON_API unsigned long long opp_strtoull(const char *s, char **endptr);

/**
 * Like the standard atoull(), but throws opp_runtime_error if an overflow
 * occurs during conversion, or if there is (non-whitespace) trailing garbage
 * after the number. Accepts decimal and C-style hexadecimal notation, but not
 * octal (leading zeroes are simply discarded and the number is interpreted as
 * decimal).
 */
COMMON_API unsigned long long opp_atoull(const char *s);

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

/**
 * Formats the given double using printf's "%g" formatting. NOTE: This function
 * is needed by the IDE (nativelibs).
 */
COMMON_API std::string opp_formatdouble(double value, int numSignificantDigits);

/**
 * Returns the current date/time as a string in the "yyyymmdd-hh:mm:ss" format.
 */
COMMON_API std::string opp_makedatetimestring();

/**
 * s should point to an open parenthesis, curly brace or square bracket.
 * The function returns the pointer to the matching close paren/brace/bracket,
 * or nullptr if not found. It does not search inside string constants
 * delimited by double quotes (") or single quotes ('); it uses opp_findmatchingquote()
 * to parse them. Note: a nullptr return value (unmatched left paren) may also
 * be caused by an unterminated string constant.
 */
COMMON_API const char *opp_findmatchingparen(const char *s);

/**
 * Remove illegal characters from a string, so that it is safe to use as a
 * file name. Characters that have special meaning in various shells or are
 * also removed. The file name should not contain a directory part, because
 * slashes (and backslashes) are also removed from the string.
 */
COMMON_API std::string opp_sanitizefilename(const std::string& fileName);

/**
 * Encode a string for use as (part of) a file name, in a URLEncode-like
 * manner (but e.g. using '#' instead '%', because '%' is interpreted by the
 * Windows shell as variable or arg reference).
 */
COMMON_API std::string opp_filenameencode(const std::string& src);

/**
 * Decode an URL-encoded string.
 */
COMMON_API std::string opp_urldecode(const std::string& src);

/**
 * Rudimentary quoting for LaTeX. Escape underscores, backslashes, dollar signs, etc.
 */
COMMON_API std::string opp_latexquote(const std::string& str);

/**
 * Add break opportunities to LaTeX source.
 */
COMMON_API std::string opp_latex_insert_breaks(const std::string& str);

/**
 * Convert opp markup to LaTeX.
 */
COMMON_API std::string opp_markup2latex(const std::string& str);

/**
 * XML-quote the string.
 */
COMMON_API std::string opp_xmlquote(const std::string& str);

/**
 * Convert an integer to string, using the specified string to separate digit groups.
 */
COMMON_API std::string opp_formati64(int64_t n, const char *digitSep);

/**
 * Garble a string so that it is not directly readable as ASCII.
 * Use opp_ungarble() to restore the original text.
 */
COMMON_API std::string opp_garble(const std::string& str, const std::string& garblephrase);

/**
 * Restore the original string after it has been garbled with opp_garble().
 */
COMMON_API std::string opp_ungarble(const std::string& str, const std::string& garblephrase);

}  // namespace common
}  // namespace omnetpp

#endif


