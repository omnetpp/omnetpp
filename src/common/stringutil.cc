//=========================================================================
//  STRINGUTIL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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

std::string opp_trim(const char *txt)
{
    if (!txt)
        return "";
    while (opp_isspace(*txt))
        txt++;
    const char *end = txt + strlen(txt);
    while (end > txt && opp_isspace(*(end-1)))
        end--;
    return std::string(txt, end-txt);
}

std::string opp_parsequotedstr(const char *txt)
{
    const char *endp;
    std::string ret = opp_parsequotedstr(txt, endp);
    if (*endp)
        throw opp_runtime_error("trailing garbage after string literal");
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

std::string opp_parsequotedstr(const char *txt, const char *& endp)
{
    const char *s = txt;
    while (opp_isspace(*s))
        s++;
    if (*s++ != '"')
        throw opp_runtime_error("missing opening quote");
    char *buf = new char[strlen(txt)+1];
    char *d = buf;
    for ( ; *s && *s != '"'; s++, d++) {
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
                case '"': *d = '"'; break;  // quote needs to be backslashed
                case '\\': *d = '\\'; break;  // backslash needs to be backslashed
                case '\n': d--; break; // don't store line continuation (backslash followed by newline)
                case '\0': d--; s--; break; // string ends in stray backslash
                case '=':
                case ';':
                case ',': throw opp_runtime_error("illegal escape sequence `\\%c' "
                          "(Hint: use double backslashes to quote display string "
                          "special chars: equal sign, comma, semicolon)", *s);
                default:  throw opp_runtime_error("illegal escape sequence `\\%c'", *s);
            }
        }
        else {
            *d = *s;
        }
    }
    *d = '\0';
    if (*s++ != '"') {
        delete[] buf;
        throw opp_runtime_error("missing closing quote");
    }
    while (opp_isspace(*s))
        s++;
    endp = s;  // if (*s!='\0'), something comes after the string

    std::string ret = buf;
    delete[] buf;
    return ret;
}

std::string opp_quotestr(const char *txt)
{
    char *buf = new char[4*strlen(txt)+3];  // a conservative guess
    char *d = buf;
    *d++ = '"';
    const char *s = txt;
    while (*s) {
        switch (*s) {
            case '\b': *d++ = '\\'; *d++ = 'b'; s++; break;
            case '\f': *d++ = '\\'; *d++ = 'f'; s++; break;
            case '\n': *d++ = '\\'; *d++ = 'n'; s++; break;
            case '\r': *d++ = '\\'; *d++ = 'r'; s++; break;
            case '\t': *d++ = '\\'; *d++ = 't'; s++; break;
            case '"':  *d++ = '\\'; *d++ = '"'; s++; break;
            case '\\': *d++ = '\\'; *d++ = '\\'; s++; break;
            default: if (opp_iscntrl(*s)) {*d++='\\'; *d++='x'; sprintf(d,"%2.2X",*s++); d+=2;}
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

#define BUFLEN    1024

std::string opp_stringf(const char *fmt, ...)
{
    char buf[BUFLEN];
    VSNPRINTF(buf, BUFLEN, fmt);
    return buf;
}

std::string opp_vstringf(const char *fmt, va_list& args)
{
    char buf[BUFLEN];
    vsnprintf(buf, BUFLEN, fmt, args);
    buf[BUFLEN-1] = '\0';
    return buf;
}

#undef BUFLEN

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
            else if (fmt[1] == 'l' && fmt[2] == 'u') {
                k += sscanf(s, "%lu%n", va_arg(va, unsigned long *), &n);
                s += n;
                fmt += 3;
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
                throw opp_runtime_error("opp_vsscanf: unsupported format '%s'", fmt);
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
            throw opp_runtime_error("opp_vsscanf: unexpected char in format: '%s'", fmt);
        }
    }
}

std::string opp_replacesubstring(const char *s, const char *substring, const char *replacement, bool replaceAll)
{
    std::string text = s;
    std::string::size_type pos = 0;
    do {
        pos = text.find(substring, pos);
        if (pos == std::string::npos)
            break;
        text.replace(pos, strlen(substring), replacement);
        pos += strlen(replacement);
    } while (replaceAll);
    return text;
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
            throw opp_runtime_error("unknown variable: $%s", name.c_str());
        return it->second;
    }
};
} // namespace {

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

std::string opp_breaklines(const char *text, int lineLength)
{
    char *buf = new char[strlen(text)+1];
    strcpy(buf, text);

    int leftMargin = 0;
    int length = strlen(buf);
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

    std::string tmp = buf;
    delete[] buf;
    return tmp;
}

std::string opp_indentlines(const char *text, const char *indent)
{
    std::string tmp = std::string(indent) + opp_replacesubstring(text, "\n", (std::string("\n")+indent).c_str(), true);
    if (*(text + strlen(text) - 1) == '\n')
        tmp = tmp.substr(0, tmp.size() - strlen(indent)); // remove indent after last line
    return tmp;
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

char *opp_concat(const char *s1,
                 const char *s2,
                 const char *s3,
                 const char *s4)
{
    // concatenate strings into a static buffer
    // FIXME throw error if string overflows!!!
    static char buf[256];
    char *bufEnd = buf+255;
    char *dest=buf;
    if (s1) while (*s1 && dest!=bufEnd) *dest++ = *s1++;
    if (s2) while (*s2 && dest!=bufEnd) *dest++ = *s2++;
    if (s3) while (*s3 && dest!=bufEnd) *dest++ = *s3++;
    if (s4) while (*s4 && dest!=bufEnd) *dest++ = *s4++;
    *dest = 0;
    return buf;
}

char *opp_strupr(char *s)
{
    char *txt = s;
    while (*s) {
        *s = opp_toupper(*s);
        s++;
    }
    return txt;
}

char *opp_strlwr(char *s)
{
    char *txt = s;
    while (*s) {
        *s = opp_tolower(*s);
        s++;
    }
    return txt;
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

std::string opp_join(const char *separator, const char *s1, const char *s2)
{
    if (opp_isempty(s1))
        return opp_nulltoempty(s2);
    else if (opp_isempty(s2))
        return opp_nulltoempty(s1);
    else
        return std::string(s1) + separator + s2;
}

std::string opp_join(const char **strings, const char *separator)
{
    std::stringstream os;
    for (const char **stringPtr = strings; *stringPtr; stringPtr++) {
        if (stringPtr != strings)
            os << separator;
        os << *stringPtr;
    }
    return os.str();
}

// returns 0 iff the two strings are equal by strcmp().
int strdictcmp(const char *s1, const char *s2)
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

/* for testing:
   #include <cstdio>
   int qsortfunc(const void *a, const void *b)
   {
    return strdictcmp(*(char**)a, *(char**)b);
   }

   int main(int argc, char **argv)
   {
    qsort(argv+1, argc-1, sizeof(char*), qsortfunc);
    for (int i=1; i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n");
    return 0;
   }

   Expected results:
   dictcmp a b c d c1 c2 ca cd --> a b c c1 c2 ca cd d
   dictcmp a aaa aa aaaaa aaaa --> a aa aaa aaaa aaaaa
   dictcmp a aaa Aa AaAaa aaaa --> a Aa aaa aaaa AaAaa
   dictcmp a1b a2b a11b a13b a20b --> a1b a2b a11b a13b a20b
 */

char *opp_itoa(char *buf, int d)
{
    sprintf(buf, "%d", d);
    return buf;
}

char *opp_ltoa(char *buf, long d)
{
    sprintf(buf, "%ld", d);
    return buf;
}

char *opp_i64toa(char *buf, int64_t d)
{
    sprintf(buf, "%" INT64_PRINTF_FORMAT "d", d);
    return buf;
}

char *opp_dtoa(char *buf, const char *format, double d)
{
    sprintf(buf, format, d);
    return buf;
}

long opp_strtol(const char *s, char **endptr)
{
    // accept decimal and hex numbers (0x), but ignore leading zero as octal prefix;
    // C's octal notation is not explicit enough, and causes confusion e.g. with
    // the runnumber-width configuration option (bug #232)
    while (opp_isspace(*s))
        s++;
    bool ishex = (s[0] == '0' && opp_toupper(s[1]) == 'X') ||
        ((s[0] == '+' || s[0] == '-') && s[1] == '0' && opp_toupper(s[2]) == 'X');
    errno = 0;
    long d = strtol(s, endptr, ishex ? 16 : 10);
    if ((d == LONG_MAX || d == LONG_MIN) && errno == ERANGE)
        throw opp_runtime_error("overflow converting `%s' to long", s);
    return d;
}

long opp_atol(const char *s)
{
    char *endptr;
    long d = opp_strtol(s, &endptr);
    while (opp_isspace(*endptr))
        endptr++;
    if (*endptr)
        throw opp_runtime_error("`%s' is not a valid integer", s);
    return d;
}

unsigned long opp_strtoul(const char *s, char **endptr)
{
    // accept decimal and hex numbers (0x), but ignore leading zero as octal prefix;
    // C's octal notation is not explicit enough, and causes confusion e.g. with
    // the runnumber-width configuration option (bug #232)
    while (opp_isspace(*s))
        s++;
    bool ishex = (s[0] == '0' && opp_toupper(s[1]) == 'X') ||
        (s[0] == '+' && s[1] == '0' && opp_toupper(s[2]) == 'X');
    errno = 0;
    unsigned long d = strtoul(s, endptr, ishex ? 16 : 10);
    if (d == ULONG_MAX && errno == ERANGE)
        throw opp_runtime_error("overflow converting `%s' to unsigned long", s);
    return d;
}

unsigned long opp_atoul(const char *s)
{
    char *endptr;
    unsigned long d = opp_strtoul(s, &endptr);
    while (opp_isspace(*endptr))
        endptr++;
    if (*endptr)
        throw opp_runtime_error("`%s' is not a valid unsigned integer", s);
    return d;
}

double opp_strtod(const char *s, char **endptr)
{
    setlocale(LC_NUMERIC, "C");
    double d = strtod(s, endptr);
    if (d == -HUGE_VAL || d == HUGE_VAL)
        throw opp_runtime_error("overflow converting `%s' to double", s);
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
        throw opp_runtime_error("`%s' is not a valid double value", s);
    return d;
}

std::string opp_makedatetimestring()
{
    time_t t = time(nullptr);
    struct tm tm = *localtime(&t);
    char timestr[32];
    sprintf(timestr, "%04d%02d%02d-%02d:%02d:%02d",
            1900+tm.tm_year, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
    return timestr;
}

const char *opp_findmatchingquote(const char *s)
{
    while (opp_isspace(*s))
        s++;
    if (*s++ != '"')
        throw opp_runtime_error("missing opening quote");
    for ( ; *s && *s != '"'; s++)
        if (*s == '\\')
            s++;

    return *s ? s : nullptr;
}

const char *opp_findmatchingparen(const char *s)
{
    while (opp_isspace(*s))
        s++;
    if (*s++ != '(')
        throw opp_runtime_error("missing left parenthesis");

    int parens = 1;
    while (*s && parens > 0) {
        if (*s == '(')
            parens++;
        else if (*s == ')')
            parens--;
        else if (*s == '"') {
            s = opp_findmatchingquote(s);
            if (!s)
                return nullptr;
        }
        s++;
    }
    return parens > 0 ? nullptr : s;
}

static bool isInvalidChar(char c)  // illegal or trouble-causing
{
    // see http://www.mtu.edu/umc/services/digital/writing/characters-avoid/
    return c < ' ' || c >= 127 || strchr("/\\:?*<>|'`\"$%{}^&", c) != nullptr;
}

std::string opp_sanitizeFileName(const std::string& fileName)
{
    std::string tmp = fileName;
    tmp.erase(std::remove_if(tmp.begin(), tmp.end(), &isInvalidChar), tmp.end());
    return tmp;
}

std::string opp_filenameencode(const std::string& src)
{
    std::stringstream os;
    for (std::string::const_iterator iter = src.begin(); iter != src.end(); ++iter) {
        char c = *iter;
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

std::string opp_latexQuote(const char *s)
{
    std::string tmp = s;
    tmp = opp_replacesubstring(tmp.c_str(), "\\", "\b", true);  // temporarily
    tmp = opp_replacesubstring(tmp.c_str(), "{", "\\{", true);
    tmp = opp_replacesubstring(tmp.c_str(), "}", "\\}", true);
    tmp = opp_replacesubstring(tmp.c_str(), "_", "\\_", true);
    tmp = opp_replacesubstring(tmp.c_str(), "$", "\\$", true);
    tmp = opp_replacesubstring(tmp.c_str(), "%", "\\%", true);
    tmp = opp_replacesubstring(tmp.c_str(), "&", "\\&", true);
    tmp = opp_replacesubstring(tmp.c_str(), "#", "\\#", true);
    tmp = opp_replacesubstring(tmp.c_str(), "\b", "{\\textbackslash}", true);
    tmp = opp_replacesubstring(tmp.c_str(), "\n", "\\\\", true);
    tmp = opp_replacesubstring(tmp.c_str(), "~", "{\\textasciitilde}", true);

    return tmp.c_str();
}

std::string opp_latexInsertBreaks(const char *s)
{
    std::string tmp = s;
    tmp = opp_replacesubstring(tmp.c_str(), "-", "-{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), "=", "={\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), ",", ",{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), "/", "/{\\allowbreak}", true);

    // properties and variables
    tmp = opp_replacesubstring(tmp.c_str(), "@", "@{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), "$\\{", "$\\{{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), "\\}", "\\}{\\allowbreak}", true);

    // colons
    tmp = opp_replacesubstring(tmp.c_str(), "::", ">>>doublecolon<<<", true);
    tmp = opp_replacesubstring(tmp.c_str(), ":", ":{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), ">>>doublecolon<<<", "::{\\allowbreak}", true);

    // dots
    tmp = opp_replacesubstring(tmp.c_str(), "...", ">>>tripledot<<<", true);
    tmp = opp_replacesubstring(tmp.c_str(), "..", ">>>doubledot<<<", true);
    tmp = opp_replacesubstring(tmp.c_str(), ".", ".{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), ">>>tripledot<<<", "...{\\allowbreak}", true);
    tmp = opp_replacesubstring(tmp.c_str(), ">>>doubledot<<<", "..{\\allowbreak}", true);

    tmp = opp_replacesubstring(tmp.c_str(), " ", " {\\allowbreak}", true);

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

std::string opp_markup2Latex(const char *s)
{
    // replace `monospace text` to \ttt{monospace text}
    std::string tmp = s;
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
            std::string processed = opp_latexInsertBreaks(tmp.substr(begin, end - begin).c_str());
            pos += strlen(processed.c_str()) - (end - begin);
            tmp.replace(begin, end - begin, processed);
        }
        idx = 1 - idx;
    } while (true);
    if (idx)
        throw opp_runtime_error("missing right backtick from text >>> %s <<<", s);

    return tmp;
}

std::string opp_xmlQuote(const std::string& str)
{
    if (!strchr(str.c_str(), '<') && !strchr(str.c_str(), '>') && !strchr(str.c_str(), '"'))
        return str;

    std::stringstream out;
    for (const char *s = str.c_str(); *s; s++) {
        char c = *s;
        if (c == '<')
            out << "&lt;";
        else if (c == '>')
            out << "&gt;";
        else if (c == '"')
            out << "&quot;";
        else
            out << c;
    }
    return out.str();
}

std::string opp_format(int64_t n, const char *digitSep)
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

}  // namespace common
}  // namespace omnetpp

