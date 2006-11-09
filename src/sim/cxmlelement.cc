//==========================================================================
//  CXMLELEMENT.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
// Contents:
//   class cXMLElement
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

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
#include <sstream>
#include <string>
#include "cxmlelement.h"
#include "minixpath.h"
#include "cexception.h"
#include "cenvir.h"
#include "cmodule.h" // for ModNameParamResolver

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

cXMLElement *cXMLElement::getFirstChildWithTag(const char *tagname) const
{
    cXMLElement *node = this->getFirstChild();
    while (node)
    {
        if (!strcasecmp(node->getTagName(),tagname))
            return node;
        node = node->getNextSibling();
    }
    return NULL;
}

cXMLElement *cXMLElement::getNextSiblingWithTag(const char *tagname) const
{
    cXMLElement *node = this->getNextSibling();
    while (node)
    {
        if (!strcasecmp(node->getTagName(),tagname))
            return node;
        node = node->getNextSibling();
    }
    return NULL;
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

cXMLElementList cXMLElement::getElementsByTagName(const char *tagname) const
{
    cXMLElementList list;
    if (!strcasecmp(getTagName(),tagname))
        list.push_back(const_cast<cXMLElement *>(this));
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

cXMLElement *cXMLElement::getElementById(const char *idattrvalue) const
{
    const char *id = getAttribute("id");
    if (id && !strcmp(id,idattrvalue))
        return const_cast<cXMLElement *>(this);
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
    {
        cXMLElement *res = child->getElementById(idattrvalue);
        if (res)
            return res;
    }
    return NULL;
}

cXMLElement *cXMLElement::getDocumentElementByPath(cXMLElement *documentnode, const char *pathexpr,
                                                   cXMLElement::ParamResolver *resolver)
{
    return MiniXPath(resolver).matchPathExpression(documentnode, pathexpr, documentnode);
}

cXMLElement *cXMLElement::getElementByPath(const char *pathexpr, cXMLElement *root, cXMLElement::ParamResolver *resolver) const
{
    if (pathexpr[0]=='/' && !root)
        throw new cRuntimeError("cXMLElement::getElementByPath(): absolute path expression "
                                "(that begins with  '/') can only be used if root node is "
                                "also specified (path expression: `%s')", pathexpr);
    if (root && !root->getParentNode())
        throw new cRuntimeError("cXMLElement::getElementByPath(): root element must have a "
                                "parent node, the \"document node\" (path expression: `%s')", pathexpr);

    return MiniXPath(resolver).matchPathExpression(const_cast<cXMLElement *>(this),
                                                   pathexpr,
                                                   root ? root->getParentNode() : NULL);
}

std::string cXMLElement::tostr(int depth) const
{
    std::stringstream os;
    int i;
    for (i=0; i<depth; i++) os << "  ";
    os << "<" << getTagName();
    cXMLAttributeMap map = getAttributes();
    for (cXMLAttributeMap::iterator it=map.begin(); it!=map.end(); it++)
        os << " " << it->first << "=\"" << it->second << "\"";
    if (!*getNodeValue() && !getFirstChild())
        {os << "/>\n"; return os.str();}
    os << ">";
    os << getNodeValue();
    if (!getFirstChild())
        {os << "</" << getTagName() << ">\n"; return os.str();}
    os << "\n";
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
        os << child->tostr(depth+1);
    for (i=0; i<depth; i++) os << "  ";
    os << "</" << getTagName() << ">\n";
    return os.str();
}

std::string cXMLElement::detailedInfo() const
{
    return tostr(0);
}

//---------------

static std::string my_itostr(int d)
{
    char buf[32];
    sprintf(buf, "%d", d);
    return std::string(buf);
}

bool ModNameParamResolver::resolve(const char *paramname, std::string& value)
{
    //printf("resolving $%s in context=%s\n", paramname, mod ? mod->fullPath().c_str() : "NULL");
    if (!mod)
        return false;
    cModule *parentMod = mod->parentModule();
    cModule *grandparentMod = parentMod ? parentMod->parentModule() : NULL;

    if (!strcmp(paramname, "MODULE_FULLPATH"))
        value = mod->fullPath();
    else if (!strcmp(paramname, "MODULE_FULLNAME"))
        value = mod->fullName();
    else if (!strcmp(paramname, "MODULE_NAME"))
        value = mod->name();
    else if (!strcmp(paramname, "MODULE_INDEX"))
        value = my_itostr(mod->index());
    else if (!strcmp(paramname, "MODULE_ID"))
        value = my_itostr(mod->id());

    else if (!strcmp(paramname, "PARENTMODULE_FULLPATH") && parentMod)
        value = parentMod->fullPath();
    else if (!strcmp(paramname, "PARENTMODULE_FULLNAME") && parentMod)
        value = parentMod->fullName();
    else if (!strcmp(paramname, "PARENTMODULE_NAME") && parentMod)
        value = parentMod->name();
    else if (!strcmp(paramname, "PARENTMODULE_INDEX") && parentMod)
        value = my_itostr(parentMod->index());
    else if (!strcmp(paramname, "PARENTMODULE_ID") && parentMod)
        value = my_itostr(parentMod->id());

    else if (!strcmp(paramname, "GRANDPARENTMODULE_FULLPATH") && grandparentMod)
        value = grandparentMod->fullPath();
    else if (!strcmp(paramname, "GRANDPARENTMODULE_FULLNAME") && grandparentMod)
        value = grandparentMod->fullName();
    else if (!strcmp(paramname, "GRANDPARENTMODULE_NAME") && grandparentMod)
        value = grandparentMod->name();
    else if (!strcmp(paramname, "GRANDPARENTMODULE_INDEX") && grandparentMod)
        value = my_itostr(grandparentMod->index());
    else if (!strcmp(paramname, "GRANDPARENTMODULE_ID") && grandparentMod)
        value = my_itostr(grandparentMod->id());
    else
        return false;

    //printf("  --> '%s'\n", value.c_str());
    return true;
}

bool StringMapParamResolver::resolve(const char *paramname, std::string& value)
{
    StringMap::iterator it = params.find(paramname);
    if (it==params.end())
        return false;
    value = it->second;
    return true;
}

