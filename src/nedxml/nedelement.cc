//==========================================================================
// nedelement.cc  - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   class NEDElement
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream.h>

#include <string.h>
#include "nedelement.h"


long NEDElement::lastid = 0;

bool NEDElement::stringToBool(const char *s)
{
    return s && !strcmp(s,"true"); // FIXME: validation error if no match?
}

const char *NEDElement::boolToString(bool b)
{
    return b ? "true" : "false";
}

int NEDElement::stringToEnum(const char *s, const char *vals[], int nums[], int n)
{
    if (!s) return nums[0]; // FIXME: validation error if no match?
    for (int i=0; i<n; i++)
        if (!strcmp(s,vals[i]))
            return nums[i];
    return -1; // FIXME: validation error if no match?
}

const char *NEDElement::enumToString(int b, const char *vals[], int nums[], int n)
{
    for (int i=0; i<n; i++)
        if (nums[i]==b)
            return vals[i];
    return ""; // FIXME: validation error if no match?
}

NEDElement::NEDElement()
{
    parent = 0;
    firstchild = 0;
    lastchild = 0;
    prevsibling = 0;
    nextsibling = 0;

    id = ++lastid;
}

NEDElement::NEDElement(NEDElement *parent)
{
    this->parent = 0;
    firstchild = 0;
    lastchild = 0;
    prevsibling = 0;
    nextsibling = 0;

    id = ++lastid;

    parent->appendChild(this);
}

NEDElement::~NEDElement()
{
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
        setAttribute(i,getAttributeDefault(i));
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
    return srcloc;
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

void NEDElement::appendChild(NEDElement *node)
{
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



