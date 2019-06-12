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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "common/stringpool.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"  // for ModNameParamResolver
#include "omnetpp/platdep/platmisc.h"
#include "minixpath.h"

using namespace omnetpp::common;

namespace omnetpp {

using std::ostream;

cXMLElement::cXMLElement(const char *tagName, cXMLElement *parent) : ename(getPooledName(tagName))
{
    if (parent)
        parent->appendChild(this);
}

cXMLElement::~cXMLElement()
{
    if (parent)
        parent->removeChild(this);

    while (firstChild)
        delete removeChild(firstChild);

    freeValue(value);
    deleteAttrs();
}

std::string cXMLElement::str() const
{
    std::stringstream os;
    os << "<" << getTagName();
    for (int i = 0; i < getNumAttrs(); i++)
        os << " " << getAttrName(i) << "=\"" << opp_xmlQuote(getAttrValue(i)) << "\"";
    if (!*getNodeValue() && !getFirstChild())
        os << "/>";
    else
        os << ">...</" << getTagName() << ">";

    std::string loc = getSourceLocation();
    if (!loc.empty())
        os << " at " << loc;
    return os.str();
}

void cXMLElement::forEachChild(cVisitor *v)
{
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling())
        v->visit(child);
}

const char *cXMLElement::getPooledName(const char *s)
{
    static StringPool namePool;
    return namePool.get(s);
}

static StringPool valuePool;

const char *cXMLElement::makeValue(const char *s)
{
    bool poolable = strlen(s) <= 5; // short strings are more likely occur multiple times in XML files, and there's little penalty for pooling them if they do not
    return poolable ? valuePool.get(s) : opp_strdup(s);
}

void cXMLElement::freeValue(const char *s)
{
    if (!valuePool.contains(s))
        delete [] s;
}

const char *cXMLElement::getTagName() const
{
    return ename;
}

void cXMLElement::setSourceLocation(const char *fname, int line)
{
    filename = getPooledName(fname);
    lineNumber = line;
}

const char *cXMLElement::getSourceLocation() const
{
    if (filename == nullptr)
        return "";
    std::stringstream os;
    os << filename << ":" << lineNumber;
    return getPooledName(os.str().c_str());
}

const char *cXMLElement::getNodeValue() const
{
    return opp_nulltoempty(value);
}

void cXMLElement::setNodeValue(const char *s)
{
    freeValue(value);
    value = makeValue(s);
}

void cXMLElement::setNodeValue(const char *s, int len)
{
    std::string tmp(s, len);
    setNodeValue(tmp.c_str());
}

void cXMLElement::appendNodeValue(const char *s, int len)
{
    std::string tmp(opp_nulltoempty(value));
    tmp.append(s, len);
    setNodeValue(tmp.c_str());
}

const char **cXMLElement::findAttr(const char *attr) const
{
    if (attrs)
        for (const char **p = attrs; *p; p += 2)
            if (strcmp(*p, attr) == 0)
                return p+1;
    return nullptr;
}

const char **cXMLElement::addAttr(const char *attr)
{
    // append new attr
    const char **newAttrs = new const char *[2*numAttrs+2+1];
    for (int i = 0; i < 2*numAttrs; i++)
        newAttrs[i] = attrs[i];
    const char **newAttr = newAttrs + 2*numAttrs;
    newAttr[0] = getPooledName(attr);
    newAttr[1] = nullptr; // value
    newAttr[2] = nullptr; // end marker
    delete [] attrs;
    attrs = newAttrs;
    numAttrs++;
    return &newAttr[1];
}

void cXMLElement::deleteAttrs()
{
    if (attrs) {
        for (const char **p = attrs; *p; p += 2)
            freeValue(*(p+1));
        delete [] attrs;
        attrs = nullptr;
        numAttrs = 0;
    }
}

const char *cXMLElement::getAttribute(const char *attr) const
{
    const char **pval = findAttr(attr);
    return pval == nullptr ? nullptr : opp_nulltoempty(*pval);
}

void cXMLElement::setAttribute(const char *attr, const char *value)
{
    const char **pval = findAttr(attr);
    if (!pval)
        pval = addAttr(attr);

    freeValue(*pval);
    *pval = makeValue(value);
}

void cXMLElement::setAttributes(const char **newAttrs)
{
    deleteAttrs();

    if (newAttrs && newAttrs[0]) {
        numAttrs = 0;
        for (const char **p = newAttrs; *p; p += 2)
            numAttrs++;
        attrs = new const char *[2*numAttrs+1];
        for (int i = 0; i < numAttrs; i++) {
            attrs[2*i] = getPooledName(newAttrs[2*i]);
            attrs[2*i+1] = makeValue(newAttrs[2*i+1]);
        }
        attrs[2*numAttrs] = nullptr;
    }
}

cXMLElement *cXMLElement::getParentNode() const
{
    return parent;
}

cXMLElement *cXMLElement::getFirstChild() const
{
    return firstChild;
}

cXMLElement *cXMLElement::getLastChild() const
{
    return lastChild;
}

cXMLElement *cXMLElement::getNextSibling() const
{
    return nextSibling;
}

cXMLElement *cXMLElement::getPreviousSibling() const
{
    return prevSibling;
}

void cXMLElement::appendChild(cXMLElement *node)
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

void cXMLElement::insertChildBefore(cXMLElement *where, cXMLElement *node)
{
    if (node->parent)
        node->parent->removeChild(node);
    node->parent = this;
    node->prevSibling = where->prevSibling;
    node->nextSibling = where;
    where->prevSibling = node;
    if (node->prevSibling)
        node->prevSibling->nextSibling = node;
    else
        firstChild = node;
}

cXMLElement *cXMLElement::removeChild(cXMLElement *node)
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

bool cXMLElement::hasChildren() const
{
    return firstChild != nullptr;
}

bool cXMLElement::hasAttributes() const
{
    return attrs != nullptr;
}

cXMLAttributeMap cXMLElement::getAttributes() const
{
    cXMLAttributeMap map;
    if (attrs)
        for (const char **p = attrs; *p; p += 2)
            map[*p] = opp_nulltoempty(*(p+1));
    return map;
}

cXMLElement *cXMLElement::getFirstChildWithTag(const char *tagName) const
{
    cXMLElement *node = this->getFirstChild();
    while (node) {
        if (!strcasecmp(node->getTagName(), tagName))
            return node;
        node = node->getNextSibling();
    }
    return nullptr;
}

cXMLElement *cXMLElement::getNextSiblingWithTag(const char *tagName) const
{
    cXMLElement *node = this->getNextSibling();
    while (node) {
        if (!strcasecmp(node->getTagName(), tagName))
            return node;
        node = node->getNextSibling();
    }
    return nullptr;
}

cXMLElementList cXMLElement::getChildren() const
{
    cXMLElementList list;
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling())
        list.push_back(child);
    return list;
}

cXMLElementList cXMLElement::getChildrenByTagName(const char *tagName) const
{
    cXMLElementList list;
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling())
        if (!strcasecmp(child->getTagName(), tagName))
            list.push_back(child);

    return list;
}

cXMLElementList cXMLElement::getElementsByTagName(const char *tagName) const
{
    cXMLElementList list;
    if (!strcasecmp(getTagName(), tagName))
        list.push_back(const_cast<cXMLElement *>(this));
    doGetElementsByTagName(tagName, list);
    return list;
}

void cXMLElement::doGetElementsByTagName(const char *tagName, cXMLElementList& list) const
{
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling()) {
        if (!strcasecmp(child->getTagName(), tagName))
            list.push_back(child);
        child->doGetElementsByTagName(tagName, list);
    }
}

cXMLElement *cXMLElement::getFirstChildWithAttribute(const char *tagName, const char *attr, const char *attrValue) const
{
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling()) {
        if (!tagName || !strcasecmp(child->getTagName(), tagName)) {
            const char *val = child->getAttribute(attr);
            if (val && (!attrValue || !strcmp(val, attrValue)))
                return child;
        }
    }
    return nullptr;
}

cXMLElement *cXMLElement::getElementById(const char *idAttrValue) const
{
    const char *id = getAttribute("id");
    if (id && !strcmp(id, idAttrValue))
        return const_cast<cXMLElement *>(this);
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling()) {
        cXMLElement *res = child->getElementById(idAttrValue);
        if (res)
            return res;
    }
    return nullptr;
}

cXMLElement *cXMLElement::getDocumentElementByPath(cXMLElement *documentNode, const char *pathExpr, cXMLElement::ParamResolver *resolver)
{
    return MiniXPath(resolver).matchPathExpression(documentNode, pathExpr, documentNode);
}

cXMLElement *cXMLElement::getElementByPath(const char *pathExpr, cXMLElement *root, cXMLElement::ParamResolver *resolver) const
{
    if (pathExpr[0] == '/' && !root)
        throw cRuntimeError("cXMLElement::getElementByPath(): Absolute path expression "
                            "(that begins with  '/') can only be used if root node is "
                            "also specified (path expression: '%s')", pathExpr);
    if (root && !root->getParentNode())
        throw cRuntimeError("cXMLElement::getElementByPath(): Root element must have a "
                            "parent node, the \"document node\" (path expression: '%s')", pathExpr);

    return MiniXPath(resolver).matchPathExpression(const_cast<cXMLElement *>(this),
                                                   pathExpr,
                                                   root ? root->getParentNode() : nullptr);
}

void cXMLElement::print(std::ostream& os, int indentLevel) const
{
    for (int i = 0; i < indentLevel; i++)
        os << "  ";
    os << "<" << getTagName();
    for (auto & attr : getAttributes())
        os << " " << attr.first << "=\"" << opp_xmlQuote(attr.second.c_str()) << "\"";
    if (!*getNodeValue() && !getFirstChild()) {
        os << "/>\n";
        return;
    }
    os << ">";
    os << opp_xmlQuote(getNodeValue());
    if (!getFirstChild()) {
        os << "</" << getTagName() << ">\n";
        return;
    }
    os << "\n";
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling())
        child->print(os, indentLevel + 1);
    for (int i = 0; i < indentLevel; i++)
        os << "  ";
    os << "</" << getTagName() << ">\n";
}

int cXMLElement::getNumAttrs() const
{
    return numAttrs;
}

const char *cXMLElement::getAttrName(int index) const
{
    ASSERT(index < numAttrs);
    return attrs[2*index];
}

const char *cXMLElement::getAttrValue(int index) const
{
    ASSERT(index < numAttrs);
    return opp_nulltoempty(attrs[2*index+1]);
}

std::string cXMLElement::getAttrDesc(int index) const
{
    std::stringstream out;
    out << getAttrName(index) << "=\"" + opp_xmlQuote(getAttrValue(index)) << "\"";
    return out.str();
}

int cXMLElement::getNumChildren() const
{
    int n = 0;
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling())
        n++;
    return n;
}

cXMLElement *cXMLElement::getChild(int index) const
{
    cXMLElement *child = getFirstChild();
    for (int i = 0; i < index && child; i++)
        child = child->getNextSibling();
    return child;
}

std::string cXMLElement::getXML() const
{
    std::stringstream os;
    print(os, 0);
    return os.str();
}

//---------------

static std::string my_itostr(int d)
{
    char buf[32];
    sprintf(buf, "%d", d);
    return std::string(buf);
}

bool ModNameParamResolver::resolve(const char *paramName, std::string& value)
{
    if (!mod)
        return false;

    cModule *parentModule = mod->getParentModule();
    cModule *grandparentModule = parentModule ? parentModule->getParentModule() : nullptr;

    if (!strcmp(paramName, "MODULE_FULLPATH"))
        value = mod->getFullPath();
    else if (!strcmp(paramName, "MODULE_FULLNAME"))
        value = mod->getFullName();
    else if (!strcmp(paramName, "MODULE_NAME"))
        value = mod->getName();
    else if (!strcmp(paramName, "MODULE_INDEX"))
        value = my_itostr(mod->getIndex());
    else if (!strcmp(paramName, "MODULE_ID"))
        value = my_itostr(mod->getId());

    else if (!strcmp(paramName, "PARENTMODULE_FULLPATH") && parentModule)
        value = parentModule->getFullPath();
    else if (!strcmp(paramName, "PARENTMODULE_FULLNAME") && parentModule)
        value = parentModule->getFullName();
    else if (!strcmp(paramName, "PARENTMODULE_NAME") && parentModule)
        value = parentModule->getName();
    else if (!strcmp(paramName, "PARENTMODULE_INDEX") && parentModule)
        value = my_itostr(parentModule->getIndex());
    else if (!strcmp(paramName, "PARENTMODULE_ID") && parentModule)
        value = my_itostr(parentModule->getId());

    else if (!strcmp(paramName, "GRANDPARENTMODULE_FULLPATH") && grandparentModule)
        value = grandparentModule->getFullPath();
    else if (!strcmp(paramName, "GRANDPARENTMODULE_FULLNAME") && grandparentModule)
        value = grandparentModule->getFullName();
    else if (!strcmp(paramName, "GRANDPARENTMODULE_NAME") && grandparentModule)
        value = grandparentModule->getName();
    else if (!strcmp(paramName, "GRANDPARENTMODULE_INDEX") && grandparentModule)
        value = my_itostr(grandparentModule->getIndex());
    else if (!strcmp(paramName, "GRANDPARENTMODULE_ID") && grandparentModule)
        value = my_itostr(grandparentModule->getId());
    else
        return false;

    return true;
}

bool StringMapParamResolver::resolve(const char *paramName, std::string& value)
{
    StringMap::iterator it = params.find(paramName);
    if (it == params.end())
        return false;
    value = it->second;
    return true;
}

}  // namespace omnetpp

