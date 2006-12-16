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
#include "stringutil.h"


char *opp_quotestr(const char *txt)
{
    char *buf = new char[2*strlen(txt)+3];  // a conservative guess
    char *d = buf;
    *d++ = '"';
    const char *s = txt;
    while (*s)
    {
        switch (*s)
        {
            case '\n': *d++ = '\\'; *d++ = 'n'; s++; break;
            case '\r': *d++ = '\\'; *d++ = 'r'; s++; break;
            case '\t': *d++ = '\\'; *d++ = 't'; s++; break;
            case '"': *d++ = '\\'; *d++ = '"'; s++; break;
            case '\\': *d++ = '\\'; *d++ = '\\'; s++; break;
            default: *d++ = *s++;
        }
    }
    *d++ = '"';
    *d = '\0';
    return buf;
}

char *opp_parsequotedstr(const char *txt, const char *&endp)
{
    const char *s = txt;
    while (*s==' ' || *s=='\t')
        s++;
    if (*s++!='"')
        return NULL;  // no opening quote
    char *buf = new char [strlen(txt)+1];
    char *d = buf;
    while (*s && *s!='"')
    {
        if (*s++!='\\')
            *d++ = *--s; // typical: no backslash
        else if (*s=='n')
            *d++ = '\n';
        else if (*s=='r')
            *d++ = '\r';
        else if (*s=='t')
            *d++ = '\t';
        else if (*s=='\n')
            ; // ignore line continuation (backslash followed by newline)
        else
            *d++ = *s; // unrecognized backslashed char -- just ignore the backslash
        s++;
    }
    *d = '\0';
    if (*s++!='"')
        {delete [] buf;return NULL;}  // no closing quote
    while (*s==' ' || *s=='\t')
        s++;
    endp = s;  // if (*s!='\0'), something comes after the string
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
