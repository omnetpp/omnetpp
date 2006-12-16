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

class DisplayString;

/**
 * Display string conversions.
 */
class DisplayStringUtil
{
  private:
    static void parseQuotedDisplayString(const char *s, DisplayString& ds);
  public:
    static std::string upgradeBackgroundDisplayString(const char *s);
    static std::string upgradeSubmoduleDisplayString(const char *s);
    static std::string upgradeConnectionDisplayString(const char *s);
    static std::string toOldBackgroundDisplayStringQuoted(const char *s);
    static std::string toOldSubmoduleDisplayStringQuoted(const char *s);
    static std::string toOldConnectionDisplayStringQuoted(const char *s);
};

#endif

