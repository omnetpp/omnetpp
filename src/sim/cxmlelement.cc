//==========================================================================
//  CXMLELEMENT.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Contents:
//   class cXMLElement
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include <string>
#include "cxmlelement.h"
#include "cexception.h"

#ifdef _MSC_VER
#define strcasecmp  stricmp
#endif

using std::ostream;


cXMLElement::cXMLElement(const char *tagname, const char *srclocation, cXMLElement *parent)
{
    ename = tagname;
    srcloc = srclocation;

    this->parent = 0;
    firstchild = 0;
    lastchild = 0;
    prevsibling = 0;
    nextsibling = 0;

    if (parent)
        parent->appendChild(this);
}

cXMLElement::~cXMLElement()
{
    if (parent)
    {
        parent->removeChild(this);
    }
    while (firstchild)
    {
        delete removeChild(firstchild);
    }
}

const char *cXMLElement::getTagName() const
{
    return ename.c_str();
}

const char *cXMLElement::getSourceLocation() const
{
    return srcloc.c_str();
}

const char *cXMLElement::getNodeValue() const
{
    return value.c_str();
}

void cXMLElement::setNodeValue(const char *s, int len)
{
    value.assign(s, len);
}

void cXMLElement::appendNodeValue(const char *s, int len)
{
    value.append(s, len);
}

const char *cXMLElement::getAttribute(const char *attr) const
{
    cXMLAttributeMap::const_iterator it = attrs.find(std::string(attr));
    if (it==attrs.end())
        return NULL;
    return it->second.c_str();
}

void cXMLElement::setAttribute(const char *attr, const char *value)
{
    attrs[std::string(attr)] = std::string(value);
}

cXMLElement *cXMLElement::getParentNode() const
{
    return parent;
}

cXMLElement *cXMLElement::getFirstChild() const
{
   return firstchild;
}

cXMLElement *cXMLElement::getLastChild() const
{
    return lastchild;
}

cXMLElement *cXMLElement::getNextSibling() const
{
    return nextsibling;
}

cXMLElement *cXMLElement::getPreviousSibling() const
{
    return prevsibling;
}

void cXMLElement::appendChild(cXMLElement *node)
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

void cXMLElement::insertChildBefore(cXMLElement *where, cXMLElement *node)
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

cXMLElement *cXMLElement::removeChild(cXMLElement *node)
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

bool cXMLElement::hasChildren() const
{
    return firstchild!=NULL;
}

bool cXMLElement::hasAttributes() const
{
    return !attrs.empty();
}

const cXMLAttributeMap& cXMLElement::getAttributes() const
{
    return attrs;
}


cXMLElementList cXMLElement::getChildren() const
{
    cXMLElementList list;
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
        list.push_back(child);
    return list;
}

cXMLElementList cXMLElement::getChildrenByTagName(const char *tagname) const
{
    cXMLElementList list;
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
        if (!strcasecmp(child->getTagName(),tagname))
            list.push_back(child);
    return list;
}

cXMLElementList cXMLElement::getElementsByTagName(const char *tagname) 
{
    cXMLElementList list;
    if (!strcasecmp(getTagName(),tagname))
        list.push_back(this);
    doGetElementsByTagName(tagname,list);
    return list;
}

void cXMLElement::doGetElementsByTagName(const char *tagname, cXMLElementList& list) const
{
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
    {
        if (!strcasecmp(child->getTagName(),tagname))
            list.push_back(child);
        child->doGetElementsByTagName(tagname,list);
    }
}

cXMLElement *cXMLElement::getFirstChildWithAttribute(const char *tagname, const char *attr, const char *attrvalue) const
{
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
    {
        if (!tagname || !strcasecmp(child->getTagName(),tagname))
        {
            const char *val = child->getAttribute(attr);
            if (val && (!attrvalue || !strcmp(val,attrvalue)))
                return child;
        }
    }
    return NULL;
}

cXMLElement *cXMLElement::getElementById(const char *idattrvalue)
{
    const char *id = getAttribute("id");
    if (id && !strcmp(id,idattrvalue))
        return this;
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
    {
        cXMLElement *res = child->getElementById(idattrvalue);
        if (res)
            return res;
    }
    return NULL;
}

//---
// Minimalistic XPath interpreter follows...

static bool parseTagName(std::string& tagname, const char *stepexpr, int len)
{
    const char *lbracket = strchr(stepexpr, '[');
    if (!lbracket || lbracket-stepexpr>=len)
        tagname.assign(stepexpr,len);
    else
        tagname.assign(stepexpr,lbracket-stepexpr);
    return true;
}

static bool parseBracketedNum(int& n, const char *s, int len)
{
    if (*s!='[' || *(s+len-1)!=']')
        return false;
    for (const char *s1=s+1; s1<s+len-1; s1++)
        if (!isdigit(*s1))
            return false;
    n = atoi(s+1);
    return true;
}

static bool parseBracketedAttrEquals(std::string& attr, std::string& value, const char *s, int len)
{
    // try to match "[@attrname='value']"
    if (len<7)
        return false;
    if (*s!='[' || *(s+len-1)!=']')
        return false;
    if (*(s+1)!='@')
        return false;
    const char *equalsign = strchr(s+2, '=');
    if (!equalsign || equalsign-s>=len)
        return false;
    attr.assign(s+2, equalsign-s-2);
    if (*(equalsign+1)!='\'' && *(equalsign+1)!='"')
        return false;
    if (*(s+len-2)!='\'' && *(s+len-2)!='"')
        return false;
    value.assign(equalsign+2, s+len-2-equalsign-2);
    return true;
}

static cXMLElement *getFirstSiblingWithAttribute(cXMLElement *firstsibling, const char *tagname, const char *attr, const char *attrvalue)
{
    for (cXMLElement *child=firstsibling; child; child=child->getNextSibling())
    {
        if (!tagname || !strcasecmp(child->getTagName(),tagname))
        {
            const char *val = child->getAttribute(attr);
            if (val && (!attrvalue || !strcmp(val,attrvalue)))
                return child;
        }
    }
    return NULL;
}

static cXMLElement *getNthSibling(cXMLElement *firstsibling, const char *tagname, int n)
{
    for (cXMLElement *child=firstsibling; child; child=child->getNextSibling())
        if (!tagname || !strcasecmp(child->getTagName(),tagname))
            if (n--==0)
                return child;
    return NULL;
}

static cXMLElement *matchPathExpression(cXMLElement *node, const char *pathexpr);

static cXMLElement *recursiveMatch(cXMLElement *node, const char *pathexpr)
{
    cXMLElement *res = matchPathExpression(node, pathexpr);
    if (res)
        return res;
    for (cXMLElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        cXMLElement *res = recursiveMatch(child, pathexpr);
        if (res)
            return res;
    }
    return NULL;
}

// handle "/" or "//" at front of pattern
static cXMLElement *matchSeparator(cXMLElement *node, const char *seppathexpr)
{
    assert(!*seppathexpr || *seppathexpr=='/');
    if (!*seppathexpr)
        return node; // end of pattern
    else if (*(seppathexpr+1)=='/')
        return recursiveMatch(node, seppathexpr+2); // separator is "//"  -- match in any depth
    else
        return matchPathExpression(node, seppathexpr+1); // separator is "/"  -- match a child
}

// "node": the current node (".") whose children we'll try to match
static cXMLElement *matchPathExpression(cXMLElement *node, const char *pathexpr)
{
    // find end of first pattern step
    const char *sep = strchr(pathexpr, '/');
    if (!sep) sep = pathexpr+strlen(pathexpr);

    const char *stepexpr = pathexpr;
    int steplen = sep-pathexpr;

    // match first pattern step.
    // might be one of: ".", "..", "*", "tagname", "tagname[n]", "tagname[@attr='value']"
    int n;
    std::string tagname, attr, value;
    if (!strncmp(stepexpr,".",steplen))
    {
        return matchSeparator(node, sep);
    }
    else if (!strncmp(stepexpr,"..",steplen))
    {
        if (node->getParentNode())
            return matchSeparator(node->getParentNode(), sep);
        return NULL;
    }
    else if (!strncmp(stepexpr,"*",steplen))
    {
        for (cXMLElement *child=node->getFirstChild(); child; child=child->getNextSibling())
        {
            cXMLElement *res = matchSeparator(child, sep);
            if (res)
                return res;
        }
        return NULL;
    }
    else if (stepexpr[0]=='*' && parseBracketedNum(n, stepexpr+1, steplen-1))
    {
        cXMLElement *newnode = getNthSibling(node->getFirstChild(), NULL, n);
        return matchSeparator(newnode, sep);
    }
    else if (stepexpr[0]=='*' && parseBracketedAttrEquals(attr, value, stepexpr+1, steplen-1))
    {
        for (cXMLElement *child=getFirstSiblingWithAttribute(node->getFirstChild(), NULL, attr.c_str(), value.c_str());
             child;
             child=getFirstSiblingWithAttribute(child->getNextSibling(), NULL, attr.c_str(), value.c_str()))
        {
            cXMLElement *res = matchSeparator(child, sep);
            if (res)
                return res;
        }
        return NULL;
    }
    else if (parseTagName(tagname, stepexpr, steplen) && steplen==(int)tagname.length())
    {
        for (cXMLElement *child=getNthSibling(node->getFirstChild(), tagname.c_str(), 0);
             child;
             child=getNthSibling(child->getNextSibling(), tagname.c_str(), 0))
        {
            cXMLElement *res = matchSeparator(child, sep);
            if (res)
                return res;
        }
        return NULL;
    }
    else if (parseTagName(tagname, stepexpr, steplen) && parseBracketedNum(n, stepexpr+tagname.length(), steplen-tagname.length()))
    {
        cXMLElement *newnode = getNthSibling(node->getFirstChild(), tagname.c_str(), n);
        return matchSeparator(newnode, sep);
    }
    else if (parseTagName(tagname, stepexpr, steplen) && parseBracketedAttrEquals(attr, value, stepexpr+tagname.length(), steplen-tagname.length()))
    {
        for (cXMLElement *child=getFirstSiblingWithAttribute(node->getFirstChild(), tagname.c_str(), attr.c_str(), value.c_str());
             child;
             child=getFirstSiblingWithAttribute(child->getNextSibling(), tagname.c_str(), attr.c_str(), value.c_str()))
        {
            cXMLElement *res = matchSeparator(child, sep);
            if (res)
                return res;
        }
        return NULL;
    }
    else
    {
        throw new cException("cXMLElement::getElementByPath(): invalid path expression `%s'", pathexpr);
    }
}

cXMLElement *cXMLElement::getDocumentElementByPath(cXMLElement *documentnode, const char *pathexpr)
{
    const char *pathexpr1 = pathexpr;
    while (pathexpr1[0]=='/')
        pathexpr1++;
    if (pathexpr1[0]=='.')
        throw new cException("cXMLElement::getDocumentElementByPath(): paths with `.' at the front "
                             "are only supported by getElementByPath() (path expression: `%s')", pathexpr);
    return matchPathExpression(documentnode, pathexpr1);
}

cXMLElement *cXMLElement::getElementByPath(const char *pathexpr)
{
    if (pathexpr[0]=='/')
        throw new cException("cXMLElement::getElementByPath(): paths beginning with `/' "
                             "not supported by getElementByPath() (path expression: `%s')", pathexpr);
    return matchPathExpression(this, pathexpr);
}

