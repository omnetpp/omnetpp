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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cmath>
#include "common/stringutil.h"
#include "common/displaystring.h"
#include "exception.h"
#include "nedutil.h"
#include "nedelements.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

const char *ASTNodeUtil::getLocalStringProperty(ASTNode *parent, const char *name)
{
    return propertyAsString(getLocalProperty(parent, name));
}

bool ASTNodeUtil::getLocalBoolProperty(ASTNode *parent, const char *name)
{
    return propertyAsBool(getLocalProperty(parent, name));
}

PropertyElement *ASTNodeUtil::getLocalProperty(ASTNode *parent, const char *name)
{
    // find first as direct child, then in potential "parameters" section
    PropertyElement *result = (PropertyElement *)parent->getFirstChildWithAttribute(NED_PROPERTY, "name", name);
    if (result)
        return result;
    parent = parent->getFirstChildWithTag(NED_PARAMETERS);
    if (parent)
        result = (PropertyElement *)parent->getFirstChildWithAttribute(NED_PROPERTY, "name", name);
    return result;
}

LiteralElement *ASTNodeUtil::getTheOnlyValueFrom(PropertyElement *property)
{
    // first (only?) value of the default (only?) key
    if (!property)
        return nullptr;
    ASTNode *propertyKey = property->getFirstChildWithAttribute(NED_PROPERTY_KEY, "name", "");
    int count = property->getNumChildrenWithTag(NED_PROPERTY_KEY);
    if (count != (propertyKey ? 1 : 0))
        throw NedException(property, "Should contain a single value");
    if (!propertyKey)
        return nullptr;
    if (propertyKey->getNumChildrenWithTag(NED_LITERAL) >= 2)
        throw NedException(property, "Should contain a single value");
    return (LiteralElement *)propertyKey->getFirstChildWithTag(NED_LITERAL);
}

bool ASTNodeUtil::propertyAsBool(PropertyElement *property)
{
    if (!property)
        return false;
    LiteralElement *literal = getTheOnlyValueFrom(property);
    if (!literal)
        return true;  // so that @isNetwork is equivalent to @isNetwork(true)
    const char *value = literal->getValue();
    if (strcmp(value, "true") != 0 && strcmp(value, "false") != 0)
        throw NedException(property, "Boolean value expected");
    return value[0] == 't';
}

const char *ASTNodeUtil::propertyAsString(PropertyElement *property)
{
    if (!property)
        return nullptr;
    LiteralElement *literal = getTheOnlyValueFrom(property);
    if (!literal)
        return nullptr;
    return literal->getValue();  // return anything as string
}

int ASTNodeUtil::compare(ASTNode *node1, ASTNode *node2)
{
    int diff;
    if ((diff = node1->getTagCode()-node2->getTagCode()) != 0)
        return diff;
    for (int i = 0; i < node1->getNumAttributes(); i++)
        if ((diff = opp_strcmp(node1->getAttribute(i), node2->getAttribute(i))) != 0)
            return diff;

    return 0;
}

int ASTNodeUtil::compareTree(ASTNode *node1, ASTNode *node2)
{
    int diff;
    if ((diff = compare(node1, node2)) != 0)
        return diff;

    // recurse
    for (ASTNode *child1 = node1->getFirstChild(), *child2 = node2->getFirstChild();
         child1 || child2;
         child1 = child1->getNextSibling(), child2 = child2->getNextSibling())
    {
        if (!child1 || !child2)
            return child1 ? -1 : 1;
        if ((diff = compareTree(child1, child2)) != 0)
            return diff;
    }
    return 0;
}

}  // namespace nedxml
}  // namespace omnetpp

