//==========================================================================
//  ENUMSTR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Implementation of the following classes:
//    EnumStringIterator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include "enumstr.h"

EnumStringIterator::EnumStringIterator(const char *s)
{
     // loop through string to check its syntax
     str = s;
     current = until = -1;
     err = 0;
     while ((*this)++ != -1);

     // position on first value
     if (!err) {
         str = s;
         current = until = -1;
         (*this)++;
     }
}

static void skip_whitespace(const char *&str)
{
     while (*str==' ' || *str=='\t') str++;
}

static int get_number(const char *&str, int& number)
{

     while (*str==' ' || *str=='\t') str++;
     if (*str<'0' || *str>'9')
         return 0;
     else
     {
         number = atoi(str);
         while (*str>='0' && *str<='9') str++;
         return 1;
     }
}

int EnumStringIterator::operator++(int)
{
     if (err || !str)
        return -1;

     if (current<until)
        return ++current;

     skip_whitespace(str);
     if (*str=='\0')
        return current=-1;

     if (!get_number(str,current)) {err=1;return -1;}
     until = -1;
     if (*str=='\0')
         ;
     else if (*str==',')
         str++;
     else if (*str=='-')
     {
         str++;
         skip_whitespace(str);
         if (!get_number(str,until)) {err=1;return -1;}
         skip_whitespace(str);
         if (*str=='\0')
            ;
         else if (*str==',')
            str++;
         else
            {err=1;return -1;}
     }
     else
         {err=1;return -1;}
     return current;
}
