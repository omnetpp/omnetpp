//==========================================================================
// nedutil.h - part of
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


#ifndef __NEDUTIL_H
#define __NEDUTIL_H

#include <string>
#include "nedelement.h"

/**
 * Display string conversions.
 */
class DisplayStringUtil
{
  public:
    static std::string upgradeBackgroundDisplayString(const char *s);
    static std::string upgradeSubmoduleDisplayString(const char *s);
    static std::string upgradeConnectionDisplayString(const char *s);
    static std::string toOldBackgroundDisplayString(const char *s);
    static std::string toOldSubmoduleDisplayString(const char *s);
    static std::string toOldConnectionDisplayString(const char *s);
};

#endif

