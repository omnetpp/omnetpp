//=========================================================================
//  STRINGUTIL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"
#include "stringutil.h"


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
    if (c>='0' && c<='9') return c-'0';
    if (c>='A' && c<='F') return c-'A'+10;
    if (c>='a' && c<='f') return c-'a'+10;
    return -1;
}

inline int h2d(const char *&s)
{
    int a = h2d(*s);
    if (a<0) return 0;
    s++;
    int b = h2d(*s);
    if (b<0) return a;
    s++;
    return a*16+b;
}

std::string opp_parsequotedstr(const char *txt, const char *&endp)
{
    const char *s = txt;
    while (isspace(*s))
        s++;
    if (*s++!='"')
        throw opp_runtime_error("missing opening quote");
    char *buf = new char [strlen(txt)+1];
    char *d = buf;
    for (; *s && *s!='"'; s++, d++)
    {
        if (*s=='\\')
        {
            // allow backslash as quote character, also interpret backslash sequences
            s++;
            switch(*s)
            {
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
        else
        {
            *d = *s;
        }
    }
    *d = '\0';
    if (*s++!='"')
        {delete [] buf; throw opp_runtime_error("missing closing quote"); }
    while (isspace(*s))
        s++;
    endp = s;  // if (*s!='\0'), something comes after the string

    std::string ret = buf;
    delete [] buf;
    return ret;
}

std::string opp_quotestr(const char *txt)
{
    char *buf = new char[2*strlen(txt)+3];  // a conservative guess
    char *d = buf;
    *d++ = '"';
    const char *s = txt;
    while (*s)
    {
        switch (*s)
        {
            case '\b': *d++ = '\\'; *d++ = 'b'; s++; break;
            case '\f': *d++ = '\\'; *d++ = 'f'; s++; break;
            case '\n': *d++ = '\\'; *d++ = 'n'; s++; break;
            case '\r': *d++ = '\\'; *d++ = 'r'; s++; break;
            case '\t': *d++ = '\\'; *d++ = 't'; s++; break;
            case '"':  *d++ = '\\'; *d++ = '"'; s++; break;
            case '\\': *d++ = '\\'; *d++ = '\\'; s++; break;
            default: if (*s<32) {*d++='\\'; *d++='x'; sprintf(d,"%2.2X",*s++); d+=2;}
                     else {*d++ = *s++;}
        }
    }
    *d++ = '"';
    *d = '\0';

    std::string ret = buf;
    delete [] buf;
    return ret;
}

bool opp_needsquotes(const char *txt)
{
    for (const char *s = txt; *s; s++)
        if (isspace(*s) || *s=='\\' || *s=='"')
            return true;
    return false;
}

std::string opp_stringf(const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 1024, fmt, args);
    va_end(args);
    return buf;
}

std::string opp_vstringf(const char *fmt, va_list& args)
{
    char buf[1024];
    vsnprintf(buf, 1024, fmt, args);
    va_end(args);
    return buf;
}

int strdictcmp(const char *s1, const char *s2)
{
    int casediff = 0;
    char c1, c2;
    while ((c1=*s1)!='\0' && (c2=*s2)!='\0')
    {
        if (isdigit(c1) && isdigit(c2))
        {
            unsigned long l1 = strtoul(s1, const_cast<char **>(&s1), 10);
            unsigned long l2 = strtoul(s2, const_cast<char **>(&s2), 10);
            if (l1!=l2)
                return l1<l2 ? -1 : 1;
        }
        else if (c1==c2) // very frequent in our case
        {
            s1++;
            s2++;
        }
        else
        {
            char lc1 = tolower(c1);
            char lc2 = tolower(c2);
            if (lc1!=lc2)
                return lc1<lc2 ? -1 : 1;
            if (c1!=c2 && !casediff && isalpha(c1) && isalpha(c2))
                casediff = isupper(c2) ? -1 : 1;
            s1++;
            s2++;
        }
    }
    if (!*s1 && !*s2)
        return casediff;
    return *s2 ? -1 : 1;
}

/* for testing:
#include <stdio.h>
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
