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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_NEDXML_NEDUTIL_H
#define __OMNETPP_NEDXML_NEDUTIL_H

#include <string>
#include "nedelement.h"

namespace omnetpp {

namespace common { class DisplayString; };

namespace nedxml {


class PropertyElement;
class LiteralElement;

/**
 * @brief Display string conversions.
 *
 * @ingroup Misc
 */
class NEDXML_API DisplayStringUtil
{
  private:
    typedef omnetpp::common::DisplayString DisplayString;
    static void parseDisplayString(const char *s, DisplayString& ds);
  public:
    static std::string upgradeBackgroundDisplayString(const char *s);
    static std::string upgradeSubmoduleDisplayString(const char *s);
    static std::string upgradeConnectionDisplayString(const char *s);
    static std::string toOldBackgroundDisplayString(const char *s);
    static std::string toOldSubmoduleDisplayString(const char *s);
    static std::string toOldConnectionDisplayString(const char *s);
};

/**
 * @brief Various utilities for the NEDElement tree.
 *
 * @ingroup Misc
 */
class NEDXML_API NEDElementUtil
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

    /** @name Comparison */
    //@{
    /** Compares tagname and declared attrs (ignores id, srcloc, srcregion, etc) */
    static int compare(NEDElement *node1, NEDElement *node2);
    /** Recursive version of compare() */
    static int compareTree(NEDElement *node1, NEDElement *node2);
    //@}

    /** @name Misc */
    //@{
    static bool isNEDType(NEDElement *node);
    //@}
};

} // namespace nedxml
}  // namespace omnetpp


#endif

