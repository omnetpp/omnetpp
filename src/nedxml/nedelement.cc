//==========================================================================
// nedelement.cc  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDElement
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <iostream>

#include "nedelement.h"
#include "nederror.h"

using std::ostream;


long NEDElement::lastid = 0;

bool NEDElement::stringToBool(const char *s)
{
    if (!strcmp(s,"true"))
        return true;
    else if (!strcmp(s,"false"))
        return false;
    else
        throw new NEDException("invalid attribute value '%s': should be 'true' or 'false'",(s?s:""));
}

const char *NEDElement::boolToString(bool b)
{
    return b ? "true" : "false";
}

int NEDElement::stringToEnum(const char *s, const char *vals[], int nums[], int n)
{
    if (!s)
        throw new NEDException("attribute cannot be empty: should be one of the allowed words '%s', etc.",vals[0]);
    for (int i=0; i<n; i++)
        if (!strcmp(s,vals[i]))
            return nums[i];
    if (n==0) throw new NEDException("call to stringToEnum() with n=0");
    throw new NEDException("invalid attribute value '%s': should be one of the allowed words '%s', etc.",s,vals[0]);
}

const char *NEDElement::enumToString(int b, const char *vals[], int nums[], int n)
{
    for (int i=0; i<n; i++)
        if (nums[i]==b)
            return vals[i];
    if (n==0) throw new NEDException("call to enumToString() with n=0");
    throw new NEDException("invalid integer value %d for enum attribute (not one of '%s'=%d etc)",b,vals[0],nums[0]);
}

NEDElement::NEDElement()
{
    parent = 0;
    firstchild = 0;
    lastchild = 0;
    prevsibling = 0;
    nextsibling = 0;
    userdata = 0;

    id = ++lastid;
}

NEDElement::NEDElement(NEDElement *parent)
{
    this->parent = 0;
    firstchild = 0;
    lastchild = 0;
    prevsibling = 0;
    nextsibling = 0;
    userdata = 0;

    id = ++lastid;

    parent->appendChild(this);
}

NEDElement::~NEDElement()
{
    if (parent)
    {
        parent->removeChild(this);
    }
    delete userdata;
    while (firstchild)
    {
        delete removeChild(firstchild);
    }
}

void NEDElement::applyDefaults()
{
    int n = getNumAttributes();
    for (int i=0; i<n; i++)
    {
        const char *defaultval = getAttributeDefault(i);
        if (defaultval && defaultval[0])
            setAttribute(i,defaultval);
    }
}

long NEDElement::getId() const
{
    return id;
}

void NEDElement::setId(long _id)
{
    id = _id;
}

const char *NEDElement::getSourceLocation() const
{
    return srcloc.c_str();
}

void NEDElement::setSourceLocation(const char *loc)
{
    srcloc = loc;
}

int NEDElement::lookupAttribute(const char *attr) const
{
    int n = getNumAttributes();
    for (int i=0; i<n; i++)
    {
        const char *attnamei = getAttributeName(i);
        if (attr[0]==attnamei[0] && !strcmp(attr,attnamei))
        {
            return i;
        }
    }
    return -1;
}

const char *NEDElement::getAttribute(const char *attr) const
{
    int k = lookupAttribute(attr);
    return getAttribute(k);
}

void NEDElement::setAttribute(const char *attr, const char *value)
{
    int k = lookupAttribute(attr);
    setAttribute(k,value);
}

const char *NEDElement::getAttributeDefault(const char *attr) const
{
    int k = lookupAttribute(attr);
    return getAttributeDefault(k);
}

NEDElement *NEDElement::getParent() const
{
    return parent;
}

NEDElement *NEDElement::getFirstChild() const
{
   return firstchild;
}

NEDElement *NEDElement::getLastChild() const
{
    return lastchild;
}

NEDElement *NEDElement::getNextSibling() const
{
    return nextsibling;
}

NEDElement *NEDElement::getPrevSibling() const
{
    return prevsibling;
}

void NEDElement::appendChild(NEDElement *node)
{
    if (node->parent)
        node->parent->removeChild(node);
    node->parent = this;
    node->prevsibling = lastchild;
    node->nextsibling = 0;
    if (node->prevsibling)
        node->prevsibling->nextsibling = node;
    else
        firstchild = node;
    lastchild = node;
}

void NEDElement::insertChildBefore(NEDElement *where, NEDElement *node)
{
    if (node->parent)
        node->parent->removeChild(node);
    node->parent = this;
    node->prevsibling = where->prevsibling;
    node->nextsibling = where;
    where->prevsibling = node;
    if (node->prevsibling)
        node->prevsibling->nextsibling = node;
    else
        firstchild = node;
}

NEDElement *NEDElement::removeChild(NEDElement *node)
{
    if (node->prevsibling)
        node->prevsibling->nextsibling = node->nextsibling;
    else
        firstchild = node->nextsibling;
    if (node->nextsibling)
        node->nextsibling->prevsibling = node->prevsibling;
    else
        lastchild = node->prevsibling;
    node->parent = node->prevsibling = node->nextsibling = 0;
    return node;
}

NEDElement *NEDElement::getFirstChildWithTag(int tagcode) const
{
    NEDElement *node = firstchild;
    while (node)
    {
        if (node->getTagCode()==tagcode)
            return node;
        node = node->getNextSibling();
    }
    return 0;
}

NEDElement *NEDElement::getNextSiblingWithTag(int tagcode) const
{
    NEDElement *node = this->nextsibling;
    while (node)
    {
        if (node->getTagCode()==tagcode)
            return node;
        node = node->getNextSibling();
    }
    return 0;
}


int NEDElement::getNumChildren() const
{
    int n=0;
    for (NEDElement *node = firstchild; node; node = node->getNextSibling())
        n++;
    return n;
}

int NEDElement::getNumChildrenWithTag(int tagcode) const
{
    int n=0;
    for (NEDElement *node = firstchild; node; node = node->getNextSibling())
        if (node->getTagCode()==tagcode)
            n++;
    return n;
}

NEDElement *NEDElement::getFirstChildWithAttribute(int tagcode, const char *attr, const char *attrvalue)
{
    for (NEDElement *child=getFirstChildWithTag(tagcode); child; child = child->getNextSiblingWithTag(tagcode))
    {
        const char *val = child->getAttribute(attr);
        if (val && !strcmp(val,attrvalue))
            return child;
    }
    return NULL;
}

NEDElement *NEDElement::getParentWithTag(int tagcode)
{
    NEDElement *parent = this->getParent();
    while (parent && parent->getTagCode()!=tagcode)
        parent = parent->getParent();
    return parent;
}

void NEDElement::setUserData(NEDElementUserData *data)
{
    delete userdata;
    userdata = data;
}

NEDElementUserData *NEDElement::getUserData() const
{
    return userdata;
}

