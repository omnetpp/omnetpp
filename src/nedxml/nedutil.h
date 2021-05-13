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
#include "astnode.h"

namespace omnetpp {
namespace nedxml {

class PropertyElement;
class LiteralElement;

/**
 * @brief Various utilities for the ASTNode tree.
 *
 * @ingroup Misc
 */
class NEDXML_API ASTNodeUtil
{
  public:
    /** @name Utilities for accessing properties */
    //@{
    static const char *getLocalStringProperty(ASTNode *parent, const char *name);
    static bool getLocalBoolProperty(ASTNode *parent, const char *name);
    static PropertyElement *getLocalProperty(ASTNode *parent, const char *name);
    static LiteralElement *getTheOnlyValueFrom(PropertyElement *property);
    static bool propertyAsBool(PropertyElement *property);
    static const char *propertyAsString(PropertyElement *property);
    //@}

    /** @name Comparison */
    //@{
    /** Compares tagname and declared attrs (ignores id, srcloc, srcregion, etc) */
    static int compare(ASTNode *node1, ASTNode *node2);
    /** Recursive version of compare() */
    static int compareTree(ASTNode *node1, ASTNode *node2);
    //@}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

