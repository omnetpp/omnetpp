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
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nedexception.h"
#include "nedutil.h"
#include "stringutil.h"
#include "displaystring.h"
#include "nedelements.h"

USING_NAMESPACE

static void renameTag(DisplayString& ds, const char *from, const char *to)
{
    int n = ds.getNumArgs(from);
    for (int i=0; i<n; i++)
        ds.setTagArg(to, i, ds.getTagArg(from,i));
    ds.removeTag(from);
}

std::string DisplayStringUtil::upgradeBackgroundDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    renameTag(ds, "p",  "bgp");
    renameTag(ds, "b",  "bgb");
    renameTag(ds, "tt", "bgtt");
    if (ds.containsTag("o"))
    {
        ds.setTagArg("bgb", 2, ds.getTagArg("o",0));
        ds.setTagArg("bgb", 3, ds.getTagArg("o",1));
        ds.setTagArg("bgb", 4, ds.getTagArg("o",2));
        ds.removeTag("o");
    }
    return ds.str();
}

std::string DisplayStringUtil::upgradeSubmoduleDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    if (ds.containsTag("o"))
    {
        ds.setTagArg("b", 3, ds.getTagArg("o",0));
        ds.setTagArg("b", 4, ds.getTagArg("o",1));
        ds.setTagArg("b", 5, ds.getTagArg("o",2));
        ds.removeTag("o");
    }
    return ds.str();
}

std::string DisplayStringUtil::upgradeConnectionDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    renameTag(ds, "o", "ls");
    // FIXME TT tag: the color parameter (old format 2nd) should go to the 3rd position in the new format
    // the 2nd par is position (was not supported previously)
    return ds.str();
}

void DisplayStringUtil::parseDisplayString(const char *s, DisplayString& ds)
{
    if (!ds.parse(s))
        throw opp_runtime_error("parse error in display string `%s'", s);
}

std::string DisplayStringUtil::toOldBackgroundDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    for (int i=0; i<ds.getNumTags(); i++)
    {
        const char *t = ds.getTagName(i);
        if (strcmp(t,"bgp")!=0 && strcmp(t,"bgb")!=0 && strcmp(t,"bgtt")!=0)
            ds.removeTag(i--);
    }
    renameTag(ds, "bgp",  "p");
    renameTag(ds, "bgb",  "b");
    renameTag(ds, "bgtt", "tt");
    if (ds.getNumArgs("b")>3)
    {
        ds.setTagArg("o", 0, ds.getTagArg("b",3));
        ds.setTagArg("o", 1, ds.getTagArg("b",4));
        ds.setTagArg("o", 2, ds.getTagArg("b",5));
        ds.setTagArg("b", 3, "");
        ds.setTagArg("b", 4, "");
        ds.setTagArg("b", 5, "");
    }
    return ds.str();
}

std::string DisplayStringUtil::toOldSubmoduleDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    if (ds.getNumArgs("b")>3)
    {
        ds.setTagArg("o", 0, ds.getTagArg("b",3));
        ds.setTagArg("o", 1, ds.getTagArg("b",4));
        ds.setTagArg("o", 2, ds.getTagArg("b",5));
        ds.setTagArg("b", 3, "");
        ds.setTagArg("b", 4, "");
        ds.setTagArg("b", 5, "");
    }
    return ds.str();
}

std::string DisplayStringUtil::toOldConnectionDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    renameTag(ds, "ls", "o");
    return ds.str();
}

//----

const char *NEDElementUtil::getLocalStringProperty(NEDElement *parent, const char *name)
{
    return propertyAsString(getLocalProperty(parent, name));
}

bool NEDElementUtil::getLocalBoolProperty(NEDElement *parent, const char *name)
{
    return propertyAsBool(getLocalProperty(parent, name));
}

PropertyElement *NEDElementUtil::getLocalProperty(NEDElement *parent, const char *name)
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

LiteralElement *NEDElementUtil::getTheOnlyValueFrom(PropertyElement *property)
{
    // first (only?) value of the default (only?) key
    if (!property)
        return NULL;
    NEDElement *propertyKey = property->getFirstChildWithAttribute(NED_PROPERTY_KEY, "name", "");
    int count = property->getNumChildrenWithTag(NED_PROPERTY_KEY);
    if (count != (propertyKey ? 1 : 0))
        throw NEDException(property, "should contain a single value");
    if (!propertyKey)
        return NULL;
    if (propertyKey->getNumChildrenWithTag(NED_LITERAL) >= 2)
        throw NEDException(property, "should contain a single value");
    return (LiteralElement *)propertyKey->getFirstChildWithTag(NED_LITERAL);
}

bool NEDElementUtil::propertyAsBool(PropertyElement *property)
{
    if (!property)
        return false;
    LiteralElement *literal = getTheOnlyValueFrom(property);
    if (!literal)
        return true;  // so that @isNetwork is equivalent to @isNetwork(true)
    const char *value = literal->getValue();
    if (strcmp(value, "true")!=0 && strcmp(value, "false")!=0)
        throw NEDException(property, "boolean value expected");
    return value[0]=='t';
}

const char *NEDElementUtil::propertyAsString(PropertyElement *property)
{
    if (!property)
        return NULL;
    LiteralElement *literal = getTheOnlyValueFrom(property);
    if (!literal)
        return NULL;
    return literal->getValue(); // return anything as string
}

int NEDElementUtil::compare(NEDElement *node1, NEDElement *node2)
{
    int diff;
    if ((diff=node1->getTagCode()-node2->getTagCode()) != 0)
        return diff;
    for (int i=0; i<node1->getNumAttributes(); i++)
        if ((diff = opp_strcmp(node1->getAttribute(i), node2->getAttribute(i)))!=0)
            return diff;
    return 0;
}

int NEDElementUtil::compareTree(NEDElement *node1, NEDElement *node2)
{
    int diff;
    if ((diff = compare(node1, node2)) != 0)
        return diff;

    // recurse
    for (NEDElement *child1 = node1->getFirstChild(), *child2 = node2->getFirstChild();
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

bool NEDElementUtil::isNEDType(NEDElement *node)
{
    int tag = node->getTagCode();
    return tag==NED_SIMPLE_MODULE || tag==NED_MODULE_INTERFACE || tag==NED_COMPOUND_MODULE ||
           tag==NED_CHANNEL_INTERFACE || tag==NED_CHANNEL;
}

