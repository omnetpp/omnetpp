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

NAMESPACE_BEGIN

//FIXME split to several files

class DisplayString;
class PropertyElement;
class LiteralElement;

/**
 * Display string conversions.
 */
class DisplayStringUtil
{
  private:
    static void parseDisplayString(const char *s, DisplayString& ds);
  public:
    static std::string upgradeBackgroundDisplayString(const char *s);
    static std::string upgradeSubmoduleDisplayString(const char *s);
    static std::string upgradeConnectionDisplayString(const char *s);
    static std::string toOldBackgroundDisplayString(const char *s);
    static std::string toOldSubmoduleDisplayString(const char *s);
    static std::string toOldConnectionDisplayString(const char *s);
};

class NEDElementUtil
{
  public:
    /** @name Utilities for accessing properties */
    //@{
    static const char *getLocalStringProperty(NEDElement *parent, const char *name);
    static bool getLocalBoolProperty(NEDElement *parent, const char *name);
    static PropertyElement *getLocalProperty(NEDElement *parent, const char *name);
    static LiteralElement *getTheOnlyValueFrom(PropertyElement *property);
    static bool propertyAsBool(PropertyElement *property);
    static const char *propertyAsString(PropertyElement *property);
    //@}
};

NAMESPACE_END


#endif

