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

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <climits>
#include <cerrno>
#include <cmath>  // HUGE_VAL
#include <clocale>
#include <algorithm>
#include "omnetpp/platdep/platmisc.h"
#include "commonutil.h"
#include "opp_ctype.h"
#include "stringutil.h"
#include "stringtokenizer.h"

namespace omnetpp {
namespace common {

bool opp_isblank(const char *txt)
{
    if (txt != nullptr)
        for (const char *s = txt; *s; s++)
            if (!opp_isspace(*s))
                return false;

    return true;
}

std::string opp_trim(const std::string& text)
{
    int length = text.length();
    int pos = 0;
    while (pos < length && opp_isspace(text[pos]))
        pos++;
    int endpos = length;
    while (endpos > pos && opp_isspace(text[endpos-1]))
        endpos--;
    return text.substr(pos, endpos-pos);
}

bool opp_isvalididentifier(const char *s)
{
    if (!opp_isalphaext(s[0]))
        return false;
    while (*++s)
        if (!opp_isalnumext(*s))
            return false;
    return true;
}

const char *opp_findclosequote(const char *txt)
{
    char quot = txt[0];
    const char *s = txt+1;
    for (; *s && *s != quot; s++)
        if (*s == '\\' && *(s+1))
            s++;
    return (*s == quot) ? s : nullptr;
}

std::string opp_parsequotedstr(const char *txt, char quot)
{
    const char *endp;
    std::string ret = opp_parsequotedstr(txt, endp, quot);
    if (*endp)
        throw opp_runtime_error("Trailing garbage after string literal");
    return ret;
}

inline int h2d(char c)
{
    if (c >= '0' && c <= '9')
        return c-'0';
    if (c >= 'A' && c <= 'F')
        return c-'A'+10;
    if (c >= 'a' && c <= 'f')
        return c-'a'+10;
    return -1;
}

inline int h2d(const char *& s)
{
    int a = h2d(*s);
    if (a < 0)
        return 0;
    s++;
    int b = h2d(*s);
    if (b < 0)
        return a;
    s++;
    return a*16+b;
}

std::string opp_parsequotedstr(const char *txt, const char *& endp, char quot)
{
    const char *s = txt;
    while (opp_isspace(*s))
        s++;
    if (!quot && (*s=='"' || *s=='\''))
        quot = *s;
    if (*s++ != quot)
        throw opp_runtime_error("Missing opening quote");
    char *buf = new char[strlen(txt)+1];
    char *d = buf;
    for ( ; *s && *s != quot; s++, d++) {
        if (*s == '\\') {
            // allow backslash as quote character, also interpret backslash sequences
            s++;
            switch (*s) {
                case 'b': *d = '\b'; break;
                case 'f': *d = '\f'; break;
                case 'n': *d = '\n'; break;
                case 'r': *d = '\r'; break;
                case 't': *d = '\t'; break;
                case 'x': s++; *d = h2d(s); s--; break; // hex code
                case '\\': *d = '\\'; break;  // backslash needs to be backslashed
                case '\n': d--; break; // don't store line continuation (backslash followed by newline)
                case '\0': d--; s--; break; // string ends in stray backslash
                case '=':
                case ';':
                case ',': throw opp_runtime_error("Illegal escape sequence '\\%c' "
                          "(Hint: use double backslashes to quote display string "
                          "special chars: equal sign, comma, semicolon)", *s);
                default:
                    if (*s == quot)
                        *d = quot;
                    else
                        throw opp_runtime_error("Illegal escape sequence '\\%c'", *s);
            }
        }
        else {
            *d = *s;
        }
    }
    *d = '\0';
    if (*s++ != quot) {
        delete[] buf;
        throw opp_runtime_error("Missing closing quote");
    }
    while (opp_isspace(*s))
        s++;
    endp = s;  // if (*s!='\0'), something comes after the string

    std::string ret = buf;
    delete[] buf;
    return ret;
}

std::string opp_quotestr(const std::string& txt)
{
    size_t bufsize = 4 * txt.length() + 3; // a conservative guess
    char *buf = new char[bufsize];
    char *d = buf;
    *d++ = '"';
    const char *s = txt.c_str();
    while (*s) {
        switch (*s) {
            case '\b': *d++ = '\\'; *d++ = 'b'; s++; break;
            case '\f': *d++ = '\\'; *d++ = 'f'; s++; break;
            case '\n': *d++ = '\\'; *d++ = 'n'; s++; break;
            case '\r': *d++ = '\\'; *d++ = 'r'; s++; break;
            case '\t': *d++ = '\\'; *d++ = 't'; s++; break;
            case '"':  *d++ = '\\'; *d++ = '"'; s++; break;
            case '\\': *d++ = '\\'; *d++ = '\\'; s++; break;
            default: if (opp_iscntrl(*s)) {*d++='\\'; *d++='x'; snprintf(d, buf+bufsize-d, "%2.2X", *s++); d+=2;}
                     else {*d++ = *s++;}
        }
    }
    *d++ = '"';
    *d = '\0';

    std::string ret = buf;
    delete[] buf;
    return ret;
}

bool opp_needsquotes(const char *txt)
{
    if (!txt[0])
        return true;
    for (const char *s = txt; *s; s++)
        if (opp_isspace(*s) || *s == '\\' || *s == '"' || opp_iscntrl(*s))
            return true;

    return false;
}

std::string opp_stringf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    return opp_vstringf(fmt, args);
}

std::string opp_vstringf(const char *fmt, va_list& args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    const int initSize = 32;

    std::string buf(initSize, '\0');

    int len = vsnprintf(&buf.front(), initSize, fmt, args);
    va_end(args);

    // the content didn't fit in the buffer (len doesn't include the trailing zero)
    if (len >= initSize) {
        buf.resize(len + 1); // vsnprintf will write a trailing zero
        len = vsnprintf(&buf.front(), len + 1, fmt, args_copy);
    }
    va_end(args_copy);

    buf.resize(len);

    return buf;
}

int opp_vsscanf(const char *s, const char *fmt, va_list va)
{
    // A simplified vsscanf implementation, solely for cStatistic::freadvarsf.
    // Only recognizes %d, %u, %ld, %g and whitespace. '#' terminates scanning
    setlocale(LC_NUMERIC, "C");
    int k = 0;
    while (true) {
        if (*fmt == '%') {
            int n;
            if (fmt[1] == 'd') {
                k += sscanf(s, "%d%n", va_arg(va, int *), &n);
                s += n;
                fmt += 2;
            }
            else if (fmt[1] == 'u') {
                k += sscanf(s, "%u%n", va_arg(va, unsigned int *), &n);
                s += n;
                fmt += 2;
            }
            else if (fmt[1] == 'l' && fmt[2] == 'd') {
                k += sscanf(s, "%ld%n", va_arg(va, long *), &n);
                s += n;
                fmt += 3;
            }
            else if (fmt[1] == 'l' && fmt[2] == 'l' && fmt[3] == 'd') {
                k += sscanf(s, "%lld%n", va_arg(va, long long *), &n);
                s += n;
                fmt += 4;
            }
            else if (fmt[1] == 'l' && fmt[2] == 'u') {
                k += sscanf(s, "%lu%n", va_arg(va, unsigned long *), &n);
                s += n;
                fmt += 3;
            }
            else if (fmt[1] == 'l' && fmt[2] == 'l' && fmt[3] == 'u') {
                k += sscanf(s, "%llu%n", va_arg(va, unsigned long long *), &n);
                s += n;
                fmt += 4;
            }
            else if (fmt[1] == 'l' && fmt[2] == 'g') {
                k += sscanf(s, "%lg%n", va_arg(va, double *), &n);
                s += n;
                fmt += 3;
            }
            else if (fmt[1] == 'g') {
                k += sscanf(s, "%g%n", va_arg(va, float *), &n);
                s += n;
                fmt += 2;
            }
            else {
                throw opp_runtime_error("opp_vsscanf: Unsupported format \"%s\"", fmt);
            }
        }
        else if (opp_isspace(*fmt)) {
            while (opp_isspace(*s))
                s++;
            fmt++;
        }
        else if (*fmt == '\0' || *fmt == '#') {
            return k;
        }
        else {
            throw opp_runtime_error("opp_vsscanf: Unexpected char in format string \"%s\"", fmt);
        }
    }
}

std::string opp_replacesubstring(const std::string& text, const std::string& substring, const std::string& replacement, bool replaceAll)
{
    std::string tmp = text;
    std::string::size_type pos = 0;
    do {
        pos = tmp.find(substring, pos);
        if (pos == std::string::npos)
            break;
        tmp.replace(pos, substring.length(), replacement);
        pos += replacement.length();
    } while (replaceAll);
    return tmp;
}

namespace {
struct MapBasedResolver : public opp_substitutevariables_resolver
{
    const std::map<std::string, std::string>& map;
    MapBasedResolver(const std::map<std::string, std::string>& map) : map(map) {}
    virtual bool isVariableNameChar(char c) override
    {
        return opp_isalnum(c) || c == '_';
    }

    virtual std::string operator()(const std::string& name) override
    {
        const std::map<std::string, std::string>::const_iterator it = map.find(name);
        if (it == map.end())
            throw opp_runtime_error("Unknown variable $%s", name.c_str());
        return it->second;
    }
};
}  // namespace {

std::string opp_substitutevariables(const std::string& raw, const std::map<std::string, std::string>& vars)
{
    MapBasedResolver resolver(vars);
    return opp_substitutevariables(raw, resolver);
}

std::string opp_substitutevariables(const std::string& raw, opp_substitutevariables_resolver& resolver)
{
    std::string result;

    std::string::size_type dollarPos, varStart, varEnd, prevEnd = 0;
    while (prevEnd < raw.size() && (dollarPos = raw.find('$', prevEnd)) != std::string::npos) {
        // copy segment before '$'
        result.append(raw.substr(prevEnd, dollarPos-prevEnd));

        varStart = (dollarPos+1 < raw.size() && raw[dollarPos+1] == '{') ? dollarPos+2 : dollarPos+1;

        // find variable end
        for (varEnd = varStart; varEnd < raw.size() && resolver.isVariableNameChar(raw[varEnd]); ++varEnd)
            ;

        if (varEnd == varStart) {  // no varname after "$" or "${"
            result.append(raw.substr(dollarPos, varStart-dollarPos));  // copy "$" or "${"
        }
        else if (varStart == dollarPos+2 && (varEnd == raw.size() || raw[varEnd] != '}')) {  // started with "${", no closing "}"
            result.append(raw.substr(dollarPos, varEnd-dollarPos));
        }
        else {
            std::string name = raw.substr(varStart, varEnd-varStart);
            if (varStart == dollarPos+2)
                varEnd++;  // skip closing "}"
            std::string value = resolver(name);
            result.append(value);
        }

        // continue from end of the variable
        prevEnd = varEnd;
    }

    // append last segment
    if (prevEnd < raw.size())
        result.append(raw.substr(prevEnd));

    return result;
}

std::string opp_breaklines(const std::string& text, int lineLength)
{
    std::string buf = text;

    int leftMargin = 0;
    int length = buf.length();
    while (true) {
        int rightMargin = leftMargin + lineLength;
        if (rightMargin >= length)
            break;  // done
        bool here = false;
        int i;
        if (!here)
            for (i = leftMargin; i < rightMargin; i++)
                if (buf[i] == '\n') {
                    here = true;
                    break;
                }

        if (!here)
            for ( ; i >= leftMargin; i--)
                if (buf[i] == ' ' || buf[i] == '\n') {
                    here = true;
                    break;
                }

        if (!here)
            for (i = leftMargin; i < length; i++)
                if (buf[i] == ' ' || buf[i] == '\n') {
                    here = true;
                    break;
                }

        if (!here)
            break;  // done
        buf[i] = '\n';
        leftMargin = i+1;
    }

    return buf;
}

std::string opp_indentlines(const std::string& text, const std::string& indent)
{
    if (indent.empty() || text.empty())
        return text;
    std::string tmp = indent + opp_replacesubstring(text, "\n", std::string("\n")+indent, true);
    if (text[text.length()-1] == '\n')
        tmp = tmp.substr(0, tmp.length() - indent.length()); // remove indent after last line
    return tmp;
}

std::vector<std::string> opp_splitandtrim(const std::string& text)
{
    const char * const WHITESPACE = " \t\f\r\n";
    std::vector<std::string> items;
    size_t itemStart = text.find_first_not_of(WHITESPACE);
    while (itemStart != text.npos) {
        size_t endPos = text.find_first_of(WHITESPACE, itemStart);
        if (endPos == text.npos) {
            items.push_back(text.substr(itemStart));
            break;
        }
        else {
            items.push_back(text.substr(itemStart, endPos - itemStart));
            itemStart = text.find_first_not_of(WHITESPACE, endPos);
        }
    }
    return items;
}

std::vector<std::string> opp_split(const std::string& text, const std::string& separator)
{
    if (separator.empty())
        throw opp_runtime_error("opp_split(): separator cannot be empty");
    std::vector<std::string> items;
    size_t itemStart = 0;
    while (true) {
        size_t separatorPos = text.find(separator, itemStart);
        if (separatorPos == text.npos) {
            items.push_back(text.substr(itemStart));
            break;
        }
        else {
            items.push_back(text.substr(itemStart, separatorPos - itemStart));
            itemStart = separatorPos + separator.length();
        }
    }
    return items;
}

std::vector<std::string> opp_splitandtrim(const std::string& text, const std::string& separator)
{
    if (separator.empty())
        throw opp_runtime_error("opp_splitandtrim(): separator cannot be empty");
    std::vector<std::string> items = opp_split(text, separator);
    for (auto& item : items)
        item = opp_trim(item);
    if (items.size() == 1 && items[0].empty())
        items.clear();
    return items;
}

std::vector<std::string> opp_splitpath(const std::string& path)
{
    std::vector<std::string> items;
    size_t itemStart = 0;
    while (true) {
        size_t separatorPos = path.find_first_of(":;", itemStart);

        // do not split after drive letter in Windows (i.e. ignore ":" in "C:\Projects\...")
        if (path.length() >= separatorPos+2 && separatorPos == itemStart+1 && opp_isalpha(path[itemStart]) && path[itemStart+1] == ':' && (path[itemStart+2] == '/' || path[itemStart+2] == '\\'))
            separatorPos = path.find_first_of(":;", separatorPos+1);

        if (separatorPos == path.npos)
            break;

        if (separatorPos != itemStart)  // token not empty
            items.push_back(path.substr(itemStart, separatorPos - itemStart));

        itemStart = separatorPos + 1;
    }
    if (path.length() > itemStart) // token not empty
        items.push_back(path.substr(itemStart));
    return items;
}

std::vector<const char *> opp_cstrings(const std::vector<std::string>& v)
{
    std::vector<const char *> result;
    result.reserve(v.size());
    for (const std::string& s : v)
        result.push_back(s.c_str());
    return result;
}

std::string opp_abbreviate(const std::string& text, int maxlen)
{
    return text.size() <= maxlen ? text : text.substr(0, maxlen-3) + "...";
}

// helper for opp_formattable()
static std::vector<int> computeMaxColumnWidths(const std::string& text)
{
    std::vector<int> widths;
    std::vector<std::string> lines = opp_split(text, "\n");
    for (auto line : lines) {
        std::vector<std::string> items = opp_split(line, "\t");
        if (items.size() < 2)
            continue;  // not part of a table
        int col = 0;
        for (auto item : items) {
            int width = item.length();
            if ((int)widths.size() <= col)
                widths.push_back(width);
            else if (widths[col] < width)
                widths[col] = width;
            col++;
        }
    }
    return widths;
}

std::string opp_format_table(const std::string& text, int spacing, const std::vector<int>& userColumnWidths)
{
    std::vector<int> columnWidths = computeMaxColumnWidths(text);
    for (size_t i = 0; i < std::min(userColumnWidths.size(), columnWidths.size()); i++)
        if (userColumnWidths[i] > 0)
            columnWidths[i] = userColumnWidths[i]; // take over user's settings

    std::stringstream out;
    std::vector<std::string> lines = opp_split(text, "\n");
    int lineNum = 0;
    for (auto& line : lines) {
        std::vector<std::string> items = opp_split(line, "\t");
        int col = 0;
        for (auto item : items) {
            out << item;  //TODO if too long, cumulate overflows
            if (col != (int)items.size()-1) { // don't pad last item on line
                int padding = std::max(spacing, (int)(columnWidths[col] + spacing - item.length()));
                out << std::setw(padding) << "";
            }
            col++;
        }
        if (lineNum++ != (int)lines.size()-1) // except for last line
            out << std::endl;
    }
    return out.str();
}

bool opp_stringbeginswith(const char *s, const char *prefix)
{
    return strlen(s) >= strlen(prefix) && strncmp(s, prefix, strlen(prefix)) == 0;
}

bool opp_stringendswith(const char *s, const char *ending)
{
    int slen = strlen(s);
    int endinglen = strlen(ending);
    return slen >= endinglen && strcmp(s+slen-endinglen, ending) == 0;
}

std::string opp_substringbefore(const std::string& str, const std::string& substr)
{
    size_t pos = str.find(substr);
    return pos == std::string::npos ? "" : str.substr(0, pos);
}

std::string opp_substringafter(const std::string& str, const std::string& substr)
{
    size_t pos = str.find(substr);
    return pos == std::string::npos ? "" : str.substr(pos+substr.size());
}

std::string opp_substringbeforelast(const std::string& str, const std::string& substr)
{
    size_t pos = str.rfind(substr);
    return pos == std::string::npos ? "" : str.substr(0, pos);
}

std::string opp_substringafterlast(const std::string& str, const std::string& substr)
{
    size_t pos = str.rfind(substr);
    return pos == std::string::npos ? "" : str.substr(pos+substr.size());
}

std::string opp_removestart(const std::string& str, const std::string& prefix)
{
    return opp_stringbeginswith(str.c_str(), prefix.c_str()) ? str.substr(prefix.length()) : str;
}

std::string opp_removeend(const std::string& str, const std::string& end)
{
    return opp_stringendswith(str.c_str(), end.c_str()) ? str.substr(0, str.length()-end.length()) : str;
}

std::string opp_strlower(const char *s)
{
    std::string tmp = opp_nulltoempty(s);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), opp_tolower);
    return tmp;
}

std::string opp_strupper(const char *s)
{
    std::string tmp = opp_nulltoempty(s);
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), opp_toupper);
    return tmp;
}

const char *opp_strnistr(const char *haystack, const char *needle, int n, bool caseSensitive)
{
    int needleLen = strlen(needle);
    if (n == 0)
        n = strlen(haystack);

    int slen = n - needleLen;

    for (const char *s = haystack; slen >= 0 && *s; s++, slen--)
        if (!(caseSensitive ? strncmp(s, needle, needleLen) : strncasecmp(s, needle, needleLen)))
            return s;

    return nullptr;
}

int opp_strdictcmp(const char *s1, const char *s2)
{
    int firstdiff = 0;
    char c1, c2;
    while ((c1 = *s1) != '\0' && (c2 = *s2) != '\0') {
        if (opp_isdigit(c1) && opp_isdigit(c2)) {
            char *s1end, *s2end;
            double d1 = strtod(s1, &s1end);
            double d2 = strtod(s2, &s2end);
            if (d1 != d2)
                return d1 < d2 ? -1 : 1;
            if (!firstdiff) {
                if (s1end-s1 < s2end-s2)
                    firstdiff = strncasecmp(s1, s2, s1end-s1);
                else
                    firstdiff = strncasecmp(s1, s2, s2end-s2);
                if (!firstdiff && (s1end-s1) != (s2end-s2))
                    firstdiff = (s1end-s1 < s2end-s2) ? -1 : 1;
            }
            s1 = s1end;
            s2 = s2end;
        }
        else if (c1 == c2) {  // very frequent in our case
            s1++;
            s2++;
        }
        else {
            char lc1 = opp_tolower(c1);
            char lc2 = opp_tolower(c2);
            if (lc1 != lc2)
                return lc1 < lc2 ? -1 : 1;
            if (c1 != c2 && !firstdiff && opp_isalpha(c1) && opp_isalpha(c2))
                firstdiff = opp_isupper(c2) ? -1 : 1;
            s1++;
            s2++;
        }
    }
    if (!*s1 && !*s2)
        return firstdiff;
    return *s2 ? -1 : 1;
}

std::string opp_join(const char *separator, const char *s1, const char *s2)
{
    if (opp_isempty(s1))
        return opp_nulltoempty(s2);
    else if (opp_isempty(s2))
        return opp_nulltoempty(s1);
    else
        return std::string(s1) + separator + s2;
}

std::string opp_join(const char *separator, const std::string& s1, const std::string& s2)
{
    return s1.empty() ? s2 : s2.empty() ? s1 : s1 + separator + s2;
}

std::string opp_join(const char **strings, const char *separator, bool skipEmpty, char quoteChar)
{
    bool first = true;
    std::stringstream os;
    for (const char **itemptr = strings; *itemptr; itemptr++) {
        if (skipEmpty && opp_isempty(*itemptr))
            continue;
        if (!first)
            os << separator;
        if (quoteChar)
            os << quoteChar << *itemptr << quoteChar;
        else
            os << *itemptr;
        first = false;
    }
    return os.str();
}

std::string opp_join(const char **strings, int n, const char *separator, bool skipEmpty, char quoteChar)
{
    bool first = true;
    std::stringstream os;
    for (int i = 0; i < n; i++) {
        const char *item = strings[i];
        if (skipEmpty && opp_isempty(item))
            continue;
        if (!first)
            os << separator;
        if (quoteChar)
            os << quoteChar << item << quoteChar;
        else
            os << item;
        first = false;
    }
    return os.str();
}

std::string opp_join(const std::vector<std::string>& strings, const char *separator, bool skipEmpty, char quoteChar)
{
    bool first = true;
    std::stringstream os;
    for (auto item : strings) {
        if (skipEmpty && item.empty())
            continue;
        if (!first)
            os << separator;
        if (quoteChar)
            os << quoteChar << item << quoteChar;
        else
            os << item;
        first = false;
    }
    return os.str();
}

std::string opp_indexedname(const char *name, int index)
{
    if (index == -1)
        return name;
    else {
        std::stringstream os;
        os << name << "[" << index << "]";
        return os.str();
    }
}

char *opp_indexedname(char *buf, size_t bufsize, const char *name, int index)
{
    size_t len = strlen(name);
    if (bufsize < len + 16)
        throw opp_runtime_error("Name '%s' is too long", name);

    strcpy(buf, name);
    char *s = buf + len;
    *s++ = '[';
    if ((unsigned)index < 10) {
        *s++ = '0' + index;
        *s++ = ']';
        *s++ = 0;
    }
    else if ((unsigned)index < 100) {
        *s++ = '0' + index/10;
        *s++ = '0' + index%10;
        *s++ = ']';
        *s++ = 0;
    }
    else {
        snprintf(s, buf+bufsize-s, "%d]", index);
    }
    return buf;
}

char *opp_itoa(char *buf, int d)
{
    snprintf(buf, 12, "%d", d);
    return buf;
}

char *opp_ltoa(char *buf, long d)
{
    snprintf(buf, 20, "%ld", d);
    return buf;
}

char *opp_i64toa(char *buf, int64_t d)
{
    snprintf(buf, 20, "%" PRId64, d);
    return buf;
}

char *opp_dtoa(char *buf, const char *format, double d)
{
    if (std::isfinite(d))
        snprintf(buf, 32, format, d);
    else if (std::isinf(d))
        strcpy(buf, d<0 ? "-inf" : "inf");
    else // must be nan
        strcpy(buf, "nan");
    return buf;
}

char *opp_ttoa(char *buf, int64_t t, int scaleexp, char *& endp)
{
    if (scaleexp < -18 || scaleexp > 18)
        throw opp_runtime_error("opp_ttoa(): scaleexp=%d out of accepted range [-18,18]", scaleexp);

    // prepare for conversion
    endp = buf + 63;  //19+19+5 should be enough, but play it safe
    *endp = '\0';
    char *s = endp;
    if (t == 0) {
        *--s = '0';
        return s;
    }

    // convert digits
    bool negative = t < 0;
    if (!negative)
        t = -t;  // make t negative! otherwise we can't handle INT64_MIN (as it has no positive equivalent)

    while (scaleexp > 0) {
        *--s = '0';
        scaleexp--;
    }
    char *mark = s;

    bool skipzeros = true;
    int decimalplace = scaleexp;
    do {
        int64_t res = t / 10;
        int digit = 10*res - t;  // note: t is negative!

        if (skipzeros && (digit != 0 || decimalplace >= 0))
            skipzeros = false;
        if (decimalplace++ == 0 && s != mark)
            *--s = '.';
        if (!skipzeros)
            *--s = '0' + digit;
        t = res;
    } while (t);

    // add leading zeros, decimal point, etc if needed
    if (decimalplace <= 0) {
        while (decimalplace++ < 0)
            *--s = '0';
        *--s = '.';
        *--s = '0';
    }

    if (negative)
        *--s = '-';
    return s;
}

inline void check_garbage_after_int(const char *endptr, const char *s)
{
    while (opp_isspace(*endptr))
        endptr++;
    if (*endptr)
        throw opp_runtime_error("Cannot parse \"%s\" as an unsigned integer", s);
}

inline bool ishex(const char *s)
{
    // accept decimal and hex numbers (0x), but ignore leading zero as octal prefix;
    // C's octal notation is not explicit enough, and causes confusion e.g. with
    // the runnumber-width configuration option (bug #232)
    return (s[0] == '0' && opp_toupper(s[1]) == 'X') ||
        ((s[0] == '+' || s[0] == '-') && s[1] == '0' && opp_toupper(s[2]) == 'X');
}

inline void skip_whitespace(const char *&s)
{
    while (opp_isspace(*s))
        s++;
}

long opp_strtol(const char *s, char **endptr)
{
    skip_whitespace(s);
    errno = 0;
    long d = strtol(s, endptr, ishex(s) ? 16 : 10);
    if ((d == LONG_MAX || d == LONG_MIN) && errno == ERANGE)
        throw opp_runtime_error("Cannot represent \"%s\" in the target integer type", s);
    return d;
}

long opp_atol(const char *s)
{
    char *endptr;
    long d = opp_strtol(s, &endptr);
    check_garbage_after_int(endptr, s);
    return d;
}

unsigned long opp_strtoul(const char *s, char **endptr)
{
    skip_whitespace(s);
    errno = 0;
    unsigned long d = strtoul(s, endptr, ishex(s) ? 16 : 10);
    if (d == ULONG_MAX && errno == ERANGE)
        throw opp_runtime_error("Cannot represent \"%s\" in the target integer type", s);
    return d;
}

unsigned long opp_atoul(const char *s)
{
    char *endptr;
    unsigned long d = opp_strtoul(s, &endptr);
    check_garbage_after_int(endptr, s);
    return d;
}

long long opp_strtoll(const char *s, char **endptr)
{
    skip_whitespace(s);
    errno = 0;
    long long d = strtoll(s, endptr, ishex(s) ? 16 : 10);
    if ((d == LLONG_MAX || d == LLONG_MIN) && errno == ERANGE)
        throw opp_runtime_error("Cannot represent \"%s\" in the target integer type", s);
    return d;
}

long long opp_atoll(const char *s)
{
    char *endptr;
    long long d = opp_strtoll(s, &endptr);
    check_garbage_after_int(endptr, s);
    return d;
}

unsigned long long opp_strtoull(const char *s, char **endptr)
{
    skip_whitespace(s);
    errno = 0;
    unsigned long long d = strtoull(s, endptr, ishex(s) ? 16 : 10);
    if (d == ULLONG_MAX && errno == ERANGE)
        throw opp_runtime_error("Cannot represent \"%s\" in the target integer type", s);
    return d;
}

unsigned long long opp_atoull(const char *s)
{
    char *endptr;
    unsigned long long d = opp_strtoull(s, &endptr);
    check_garbage_after_int(endptr, s);
    return d;
}

double opp_strtod(const char *s, char **endptr)
{
    setlocale(LC_NUMERIC, "C");
    double d = strtod(s, endptr);
    if ((d == -HUGE_VAL || d == HUGE_VAL) && errno==ERANGE)
        throw opp_runtime_error("Cannot represent \"%s\" in double", s);
    return d;
}

double opp_atof(const char *s)
{
    char *endptr;
    setlocale(LC_NUMERIC, "C");
    double d = opp_strtod(s, &endptr);
    while (opp_isspace(*endptr))
        endptr++;
    if (*endptr)
        throw opp_runtime_error("Cannot parse \"%s\" as a real number", s);
    return d;
}

std::string opp_formatdouble(double value, int numSignificantDigits)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "%.*g", numSignificantDigits, value);
    return buf;
}

std::string opp_makedatetimestring()
{
    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);
    char timestr[32];
    snprintf(timestr, sizeof(timestr), "%04d%02d%02d-%02d:%02d:%02d",
            1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    return timestr;
}

const char *opp_findmatchingparen(const char *s)
{
    char openParen = *s;
    char closeParen;
    switch (*s) {
    case '(': closeParen = ')'; break;
    case '{': closeParen = '}'; break;
    case '[': closeParen = ']'; break;
    default: throw opp_runtime_error("String does not start with a left parenthesis, curly brace or square bracket");
    }

    s++;

    int parens = 1;
    while (*s && parens > 0) {
        if (*s == openParen)
            parens++;
        else if (*s == closeParen)
            parens--;
        else if (*s == '"' || *s == '\'') {
            s = opp_findclosequote(s);
            if (!s)
                return nullptr;
        }
        s++;
    }
    return parens > 0 ? nullptr : s-1;
}

static bool isInvalidChar(char c)  // illegal or trouble-causing
{
    // see http://www.mtu.edu/umc/services/digital/writing/characters-avoid/
    return c < ' ' || c >= 127 || strchr("/\\:?*<>|'`\"$%{}^&", c) != nullptr;
}

std::string opp_sanitizefilename(const std::string& fileName)
{
    std::string tmp = fileName;
    tmp.erase(std::remove_if(tmp.begin(), tmp.end(), &isInvalidChar), tmp.end());
    return tmp;
}

std::string opp_filenameencode(const std::string& src)
{
    std::stringstream os;
    for (char c : src) {
        if (c == ' ')
            os << "_";
        else if (c == '_')
            os << "#_";
        else if (c == '#')
            os << "##";
        else if (c < ' ' || c >= 127 || strchr("/\\:?*<>|'`\"$%{}^&", c))
            os << "#" << std::hex << std::setw(2) << (int)c;
        else
            os << c;
    }
    return os.str();
}

inline char hexToChar(char first, char second)
{
    int digit;
    digit = (first >= 'A' ? ((first & 0xDF) - 'A') + 10 : (first - '0'));
    digit *= 16;
    digit += (second >= 'A' ? ((second & 0xDF) - 'A') + 10 : (second - '0'));
    return static_cast<char>(digit);
}

std::string opp_urldecode(const std::string& src)
{
    std::string result;
    std::string::const_iterator iter;
    char c;

    for (iter = src.begin(); iter != src.end(); ++iter) {
        switch (*iter) {
            case '+':
                result.append(1, ' ');
                break;

            case '%':
                if (std::distance(iter, src.end()) >= 2 && opp_isdigit(*(iter + 1)) && opp_isxdigit(*(iter + 2))) {
                    c = *++iter;
                    result.append(1, hexToChar(c, *++iter));
                }
                else {
                    result.append(1, '%');
                }
                break;

            default:
                result.append(1, *iter);
                break;
        }
    }

    return result;
}

std::string opp_latexquote(const std::string& str)
{
    std::string tmp = str;
    tmp = opp_replacesubstring(tmp, "\\", "\b", true);  // temporarily
    tmp = opp_replacesubstring(tmp, "{", "\\{", true);
    tmp = opp_replacesubstring(tmp, "}", "\\}", true);
    tmp = opp_replacesubstring(tmp, "_", "\\_", true);
    tmp = opp_replacesubstring(tmp, "$", "\\$", true);
    tmp = opp_replacesubstring(tmp, "%", "\\%", true);
    tmp = opp_replacesubstring(tmp, "&", "\\&", true);
    tmp = opp_replacesubstring(tmp, "#", "\\#", true);
    tmp = opp_replacesubstring(tmp, "\b", "{\\textbackslash}", true);
    tmp = opp_replacesubstring(tmp, "~", "{\\textasciitilde}", true);
    tmp = opp_replacesubstring(tmp, "\n", "\\\\\n", true);

    return tmp.c_str();
}

std::string opp_latex_insert_breaks(const std::string& str)
{
    std::string tmp = str;
    tmp = opp_replacesubstring(tmp, "-", "-{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, "=", "={\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, ",", ",{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, "/", "/{\\allowbreak}", true);

    // properties and variables
    tmp = opp_replacesubstring(tmp, "@", "@{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, "$\\{", "$\\{{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, "\\}", "\\}{\\allowbreak}", true);

    // colons
    tmp = opp_replacesubstring(tmp, "::", ">>>doublecolon<<<", true);
    tmp = opp_replacesubstring(tmp, ":", ":{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, ">>>doublecolon<<<", "::{\\allowbreak}", true);

    // dots
    tmp = opp_replacesubstring(tmp, "...", ">>>tripledot<<<", true);
    tmp = opp_replacesubstring(tmp, "..", ">>>doubledot<<<", true);
    tmp = opp_replacesubstring(tmp, ".", ".{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, ">>>tripledot<<<", "...{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp, ">>>doubledot<<<", "..{\\allowbreak}", true);

    tmp = opp_replacesubstring(tmp, " ", " {\\allowbreak}", true);

    char ch;
    char previousCh;
    for (int i = 0; i < (int)tmp.length(); i++) {
        ch = tmp[i];
        // avoid putting a break opportunity into cFoo
        if (i != 0 && previousCh != 'c' && opp_isalpha(previousCh) && opp_isalpha(ch) && opp_islower(previousCh) && opp_isupper(ch)) {
            char replacement[] = "\\-";
            tmp.replace(i, 0, replacement);
            i += strlen(replacement);
        }
        previousCh = ch;
    }

    return tmp;
}

std::string opp_markup2latex(const std::string& str)
{
    // replace `monospace text` to \ttt{monospace text}
    std::string tmp = str;
    std::string::size_type begin;
    std::string::size_type end;
    std::string::size_type pos = 0;
    static const char substring[] = "`";
    static const char *replacement[2] = {"\\ttt{", "}"};
    int idx = 0;
    do {
        pos = tmp.find(substring, pos);
        if (pos == std::string::npos)
            break;
        if (idx == 0) {
            begin = pos + strlen(replacement[idx]);
            tmp.replace(pos, strlen(substring), replacement[idx]);
            pos += strlen(replacement[idx]);
        }
        else {
            end = pos;
            tmp.replace(pos, strlen(substring), replacement[idx]);
            pos += strlen(replacement[idx]);
            // replace content
            std::string processed = opp_latex_insert_breaks(tmp.substr(begin, end - begin));
            pos += strlen(processed.c_str()) - (end - begin);
            tmp.replace(begin, end - begin, processed);
        }
        idx = 1 - idx;
    } while (true);
    if (idx)
        throw opp_runtime_error("Missing right backtick from text >>> %s <<<", str.c_str());

    return tmp;
}

std::string opp_xmlquote(const std::string& str)
{
    if (!strchr(str.c_str(), '<') && !strchr(str.c_str(), '>') && !strchr(str.c_str(), '&') && !strchr(str.c_str(), '"'))
        return str;

    std::stringstream out;
    for (const char *s = str.c_str(); *s; s++) {
        char c = *s;
        switch(c) {
            case '<': out << "&lt;"; break;
            case '>': out << "&gt;"; break;
            case '&': out << "&amp;"; break;
            case '"': out << "&quot;"; break;
            default: out << c;
        }
    }
    return out.str();
}

std::string opp_formati64(int64_t n, const char *digitSep)
{
    std::stringstream os;
    os << n;
    std::string str = os.str();
    os.str("");

    for (size_t i = 0; i < str.length(); ++i) {
        if (i == 0 || (int)(i - str.length()) % 3 != 0)
            os << str[i];
        else
            os << digitSep << str[i];
    }
    return os.str();
}

std::string opp_garble(const std::string& str, const std::string& garblehrase)
{
    size_t passlen = garblehrase.length();
    if (passlen == 0)
        throw opp_runtime_error("opp_garble(): empty garblephrase");
    std::string tmp = str;
    for (size_t i = 0; i < tmp.length(); i++) {
        char c = garblehrase[i % passlen];
        tmp[i] = tmp[i] ^ c ^ ((c&3)<<6); // top 1-2 bits are usually zero, reuse lower 2 bits for them
    }
    return tmp;
}

std::string opp_ungarble(const std::string& str, const std::string& garblephrase)
{
    return opp_garble(str, garblephrase);
}

}  // namespace common
}  // namespace omnetpp

