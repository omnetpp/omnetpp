//==========================================================================
// nedutil.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   misc util functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nederror.h"
#include "nedutil.h"


std::string DisplayStringUtil::upgradeBackgroundDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::upgradeSubmoduleDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::upgradeConnectionDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::toOldBackgroundDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::toOldSubmoduleDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::toOldConnectionDisplayString(const char *s)
{
    return s;
}

