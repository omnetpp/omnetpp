//==========================================================================
// astnode.cc  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class ASTNode
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

#include "astnode.h"
#include "errorstore.h"
#include "exception.h"
#include "common/fileutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

using std::ostream;

long ASTNode::lastId = 0;
long ASTNode::numCreated = 0;
long ASTNode::numExisting = 0;

bool ASTNode::stringToBool(const char *s)
{
    if (!strcmp(s, "true"))
        return true;
    else if (!strcmp(s, "false"))
        return false;
    else
        throw NedException("Invalid attribute value '%s': Should be 'true' or 'false'", (s ? s : ""));
}

const char *ASTNode::boolToString(bool b)
{
    return b ? "true" : "false";
}

int ASTNode::stringToEnum(const char *s, const char *vals[], int nums[], int n)
{
    if (!s)
        throw NedException("Attribute cannot be empty: Should be one of the allowed words '%s', etc.", vals[0]);
    for (int i = 0; i < n; i++)
        if (!strcmp(s, vals[i]))
            return nums[i];

    if (n == 0)
        throw NedException("Call to stringToEnum() with n=0");
    throw NedException("Invalid attribute value '%s': Should be one of the allowed words '%s', etc.", s, vals[0]);
}

const char *ASTNode::enumToString(int b, const char *vals[], int nums[], int n)
{
    for (int i = 0; i < n; i++)
        if (nums[i] == b)
            return vals[i];

    if (n == 0)
        throw NedException("Call to enumToString() with n=0");
    throw NedException("Invalid integer value %d for enum attribute (not one of '%s'=%d etc)", b, vals[0], nums[0]);
}

void ASTNode::validateEnum(int b, const char *vals[], int nums[], int n)
{
    // code almost identical to enumToString()
    for (int i = 0; i < n; i++)
        if (nums[i] == b)
            return;

    if (n == 0)
        throw NedException("Call to validateEnum() with n=0");
    throw NedException("Invalid integer value %d for enum attribute (not one of '%s'=%d etc)", b, vals[0], nums[0]);
}

ASTNode::ASTNode()
{
    id = ++lastId;
    numCreated++;
    numExisting++;
}

ASTNode::ASTNode(ASTNode *parent)
{
    id = ++lastId;
    numCreated++;
    numExisting++;

    parent->appendChild(this);
}

ASTNode::~ASTNode()
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

ASTNode *ASTNode::dupTree() const
{
    ASTNode *newNode = dup();
    for (ASTNode *child = getFirstChild(); child; child = child->getNextSibling())
        newNode->appendChild(child->dupTree());
    return newNode;
}

void ASTNode::applyDefaults()
{
    int n = getNumAttributes();
    for (int i = 0; i < n; i++) {
        const char *defaultval = getAttributeDefault(i);
        if (defaultval)
            setAttribute(i, defaultval);
    }
}

long ASTNode::getId() const
{
    return id;
}

void ASTNode::setId(long _id)
{
    id = _id;
}

FileLine ASTNode::getSourceLocation() const
{
    return srcLoc;
}

void ASTNode::setSourceLocation(FileLine loc)
{
    srcLoc = loc;
    directory = directoryOf(loc.file.c_str());
}

const char *ASTNode::getSourceFileName() const
{
    return srcLoc.file.c_str();
}

int ASTNode::getSourceLineNumber() const
{
    return srcLoc.line;
}

const char *ASTNode::getSourceFileDirectory() const
{
    return directory.c_str();
}

const SourceRegion& ASTNode::getSourceRegion() const
{
    return srcRegion;
}

void ASTNode::setSourceRegion(const SourceRegion& region)
{
    srcRegion = region;
}

int ASTNode::lookupAttribute(const char *attr) const
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

const char *ASTNode::getAttribute(const char *attr) const
{
    int k = lookupAttribute(attr);
    return getAttribute(k);
}

void ASTNode::setAttribute(const char *attr, const char *value)
{
    int k = lookupAttribute(attr);
    setAttribute(k, value);
}

const char *ASTNode::getAttributeDefault(const char *attr) const
{
    int k = lookupAttribute(attr);
    return getAttributeDefault(k);
}

ASTNode *ASTNode::getParent() const
{
    return parent;
}

ASTNode *ASTNode::getFirstChild() const
{
    return firstChild;
}

ASTNode *ASTNode::getLastChild() const
{
    return lastChild;
}

ASTNode *ASTNode::getNextSibling() const
{
    return nextSibling;
}

ASTNode *ASTNode::getPrevSibling() const
{
    return prevSibling;
}

void ASTNode::appendChild(ASTNode *node)
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

void ASTNode::insertChildBefore(ASTNode *where, ASTNode *node)
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

ASTNode *ASTNode::removeChild(ASTNode *node)
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

ASTNode *ASTNode::getFirstChildWithTag(int tagcode) const
{
    ASTNode *node = firstChild;
    while (node) {
        if (node->getTagCode() == tagcode)
            return node;
        node = node->getNextSibling();
    }
    return nullptr;
}

ASTNode *ASTNode::getNextSiblingWithTag(int tagcode) const
{
    ASTNode *node = this->nextSibling;
    while (node && node->getTagCode() != tagcode)
        node = node->getNextSibling();
    return node;
}

ASTNode *ASTNode::getPreviousSiblingWithTag(int tagcode) const
{
    ASTNode *node = this->prevSibling;
    while (node && node->getTagCode() != tagcode)
        node = node->getPrevSibling();
    return node;
}

int ASTNode::getNumChildren() const
{
    int n = 0;
    for (ASTNode *node = firstChild; node; node = node->getNextSibling())
        n++;
    return n;
}

int ASTNode::getNumChildrenWithTag(int tagcode) const
{
    int n = 0;
    for (ASTNode *node = firstChild; node; node = node->getNextSibling())
        if (node->getTagCode() == tagcode)
            n++;

    return n;
}

ASTNode *ASTNode::getFirstChildWithAttribute(int tagcode, const char *attr, const char *attrvalue)
{
    for (ASTNode *child = getFirstChildWithTag(tagcode); child; child = child->getNextSiblingWithTag(tagcode)) {
        const char *val = child->getAttribute(attr);
        if (val && !strcmp(val, attrvalue))
            return child;
    }
    return nullptr;
}

ASTNode *ASTNode::getParentWithTag(int tagcode)
{
    ASTNode *node = this;
    while (node && node->getTagCode() != tagcode)
        node = node->getParent();
    return node;
}

void ASTNode::setUserData(UserData *data)
{
    delete userData;
    userData = data;
}

UserData *ASTNode::getUserData() const
{
    return userData;
}

}  // namespace nedxml
}  // namespace omnetpp

