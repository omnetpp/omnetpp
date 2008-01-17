//=========================================================================
//  CWATCH.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//
//   Member functions of
//    cWatchBase etc: make primitive types, structs etc inspectable
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cwatch.h"
#include "stringutil.h"

USING_NAMESPACE

std::string cWatch_stdstring::info() const
{
    return opp_quotestr(r.c_str());
}

void cWatch_stdstring::assign(const char *s)
{
    if (s[0]=='"' && s[strlen(s)-1]=='"')
    {
        r = opp_parsequotedstr(s);
    }
    else
    {
        r = s;
    }
}

