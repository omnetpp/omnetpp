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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <iostream>

#include "nedelement.h"
#include "nederror.h"
#include "nedexception.h"

namespace omnetpp {
namespace nedxml {

using std::ostream;

long NEDElement::lastId = 0;
long NEDElement::numCreated = 0;
long NEDElement::numExisting = 0;

bool NEDElement::stringToBool(const char *s)
{
    if (!strcmp(s, "true"))
        return true;
    else if (!strcmp(s, "false"))
        return false;
    else
        throw NEDException("Invalid attribute value '%s': Should be 'true' or 'false'", (s ? s : ""));
}

const char *NEDElement::boolToString(bool b)
{
    return b ? "true" : "false";
}

int NEDElement::stringToEnum(const char *s, const char *vals[], int nums[], int n)
{
    if (!s)
        throw NEDException("Attribute cannot be empty: Should be one of the allowed words '%s', etc.", vals[0]);
    for (int i = 0; i < n; i++)
        if (!strcmp(s, vals[i]))
            return nums[i];

    if (n == 0)
        throw NEDException("Call to stringToEnum() with n=0");
    throw NEDException("Invalid attribute value '%s': Should be one of the allowed words '%s', etc.", s, vals[0]);
}

const char *NEDElement::enumToString(int b, const char *vals[], int nums[], int n)
{
    for (int i = 0; i < n; i++)
        if (nums[i] == b)
            return vals[i];

    if (n == 0)
        throw NEDException("Call to enumToString() with n=0");
    throw NEDException("Invalid integer value %d for enum attribute (not one of '%s'=%d etc)", b, vals[0], nums[0]);
}

void NEDElement::validateEnum(int b, const char *vals[], int nums[], int n)
{
    // code almost identical to enumToString()
    for (int i = 0; i < n; i++)
        if (nums[i] == b)
            return;

    if (n == 0)
        throw NEDException("Call to validateEnum() with n=0");
    throw NEDException("Invalid integer value %d for enum attribute (not one of '%s'=%d etc)", b, vals[0], nums[0]);
}

NEDElement::NEDElement()
{
    parent = nullptr;
    firstChild = nullptr;
    lastChild = nullptr;
    prevSibling = nullptr;
    nextSibling = nullptr;
    userData = nullptr;

    id = ++lastId;
    numCreated++;
    numExisting++;
}

NEDElement::NEDElement(NEDElement *parent)
{
    this->parent = nullptr;
    firstChild = nullptr;
    lastChild = nullptr;
    prevSibling = nullptr;
    nextSibling = nullptr;
    userData = nullptr;

    id = ++lastId;
    numCreated++;
    numExisting++;

    parent->appendChild(this);
}

NEDElement::~NEDElement()
{
    if (parent) {
        parent->removeChild(this);
    }
    delete userData;
    while (firstChild) {
        delete removeChild(firstChild);
    }
    numExisting--;
}

NEDElement *NEDElement::dupTree() const
{
    NEDElement *newNode = dup();
    for (NEDElement *child = getFirstChild(); child; child = child->getNextSibling())
        newNode->appendChild(child->dupTree());
    return newNode;
}

void NEDElement::applyDefaults()
{
    int n = getNumAttributes();
    for (int i = 0; i < n; i++) {
        const char *defaultval = getAttributeDefault(i);
        if (defaultval)
            setAttribute(i, defaultval);
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
    return srcLoc.c_str();
}

void NEDElement::setSourceLocation(const char *loc)
{
    srcLoc = loc ? loc : "";
}

const NEDSourceRegion& NEDElement::getSourceRegion() const
{
    return srcRegion;
}

void NEDElement::setSourceRegion(const NEDSourceRegion& region)
{
    srcRegion = region;
}

int NEDElement::lookupAttribute(const char *attr) const
{
    int n = getNumAttributes();
    for (int i = 0; i < n; i++) {
        const char *attnamei = getAttributeName(i);
        if (attr[0] == attnamei[0] && !strcmp(attr, attnamei)) {
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
    setAttribute(k, value);
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
    return firstChild;
}

NEDElement *NEDElement::getLastChild() const
{
    return lastChild;
}

NEDElement *NEDElement::getNextSibling() const
{
    return nextSibling;
}

NEDElement *NEDElement::getPrevSibling() const
{
    return prevSibling;
}

void NEDElement::appendChild(NEDElement *node)
{
    if (node->parent)
        node->parent->removeChild(node);
    node->parent = this;
    node->prevSibling = lastChild;
    node->nextSibling = nullptr;
    if (node->prevSibling)
        node->prevSibling->nextSibling = node;
    else
        firstChild = node;
    lastChild = node;
}

void NEDElement::insertChildBefore(NEDElement *where, NEDElement *node)
{
    if (node->parent)
        node->parent->removeChild(node);
    if (!where) {
        appendChild(node);
        return;
    }
    node->parent = this;
    node->prevSibling = where->prevSibling;
    node->nextSibling = where;
    where->prevSibling = node;
    if (node->prevSibling)
        node->prevSibling->nextSibling = node;
    else
        firstChild = node;
}

NEDElement *NEDElement::removeChild(NEDElement *node)
{
    if (node->prevSibling)
        node->prevSibling->nextSibling = node->nextSibling;
    else
        firstChild = node->nextSibling;
    if (node->nextSibling)
        node->nextSibling->prevSibling = node->prevSibling;
    else
        lastChild = node->prevSibling;
    node->parent = node->prevSibling = node->nextSibling = nullptr;
    return node;
}

NEDElement *NEDElement::getFirstChildWithTag(int tagcode) const
{
    NEDElement *node = firstChild;
    while (node) {
        if (node->getTagCode() == tagcode)
            return node;
        node = node->getNextSibling();
    }
    return nullptr;
}

NEDElement *NEDElement::getNextSiblingWithTag(int tagcode) const
{
    NEDElement *node = this->nextSibling;
    while (node) {
        if (node->getTagCode() == tagcode)
            return node;
        node = node->getNextSibling();
    }
    return nullptr;
}

int NEDElement::getNumChildren() const
{
    int n = 0;
    for (NEDElement *node = firstChild; node; node = node->getNextSibling())
        n++;
    return n;
}

int NEDElement::getNumChildrenWithTag(int tagcode) const
{
    int n = 0;
    for (NEDElement *node = firstChild; node; node = node->getNextSibling())
        if (node->getTagCode() == tagcode)
            n++;

    return n;
}

NEDElement *NEDElement::getFirstChildWithAttribute(int tagcode, const char *attr, const char *attrvalue)
{
    for (NEDElement *child = getFirstChildWithTag(tagcode); child; child = child->getNextSiblingWithTag(tagcode)) {
        const char *val = child->getAttribute(attr);
        if (val && !strcmp(val, attrvalue))
            return child;
    }
    return nullptr;
}

NEDElement *NEDElement::getParentWithTag(int tagcode)
{
    NEDElement *node = this;
    while (node && node->getTagCode() != tagcode)
        node = node->getParent();
    return node;
}

void NEDElement::setUserData(NEDElementUserData *data)
{
    delete userData;
    userData = data;
}

NEDElementUserData *NEDElement::getUserData() const
{
    return userData;
}

}  // namespace nedxml
}  // namespace omnetpp

