//==========================================================================
// nedstring.cc  - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   class NEDString
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include "nedstring.h"

char *NEDString::_strdup(const char *s)
{
    if (!s || !s[0]) return NULL;
    char *p = new char[strlen(s)+1];
    if (p) strcpy(p,s);
    return p;
}

bool NEDString::operator==(const char *s)
{
    if (!str)
        return !s || !s[0];
    if (!s)
        return false;
    return !strcmp(str,s);
}

NEDString& NEDString::operator+=(const char *s)
{
    if (!s || !s[0])
        return *this;
    if (!str)
        return operator=(s);

    int lstr = strlen(str);
    char *newstr = new char[lstr + strlen(s)+1];
    strcpy(newstr,str);
    strcpy(newstr+lstr,s);
    delete [] str;
    str = newstr;
    return *this;
}

