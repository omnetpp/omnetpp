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
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"  // for ModNameParamResolver
#include "omnetpp/platdep/platmisc.h"
#include "minixpath.h"

using namespace omnetpp::common;

namespace omnetpp {

using std::ostream;

cXMLElement::cXMLElement(const char *tagName, const char *srclocation, cXMLElement *parent)
{
    ename = tagName;
    srcloc = srclocation;

    this->parent = nullptr;
    firstChild = nullptr;
    lastChild = nullptr;
    prevSibling = nullptr;
    nextSibling = nullptr;

    if (parent)
        parent->appendChild(this);
}

cXMLElement::~cXMLElement()
{
    if (parent)
        parent->removeChild(this);

    while (firstChild)
        delete removeChild(firstChild);
}

std::string cXMLElement::str() const
{
    std::stringstream os;
    os << "<" << getTagName();
    for (auto & attr : getAttributes())
        os << " " << attr.first << "=\"" << opp_xmlQuote(attr.second.c_str()) << "\"";
    if (!*getNodeValue() && !getFirstChild())
        os << "/>";
    else
        os << ">...</" << getTagName() << ">";

    const char *loc = getSourceLocation();
    if (!opp_isempty(loc))
        os << " at " << loc;
    return os.str();
}

void cXMLElement::forEachChild(cVisitor *v)
{
    for (cXMLElement *child = getFirstChild(); child; child = child->getNextSibling())
        v->visit(child);
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
    if (it == attrs.end())
        return nullptr;
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
    return !attrs.empty();
}

const cXMLAttributeMap& cXMLElement::getAttributes() const
{
    return attrs;
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
    return getAttributes().size();
}

std::string cXMLElement::getAttr(int index) const
{
    cXMLAttributeMap map = getAttributes();
    cXMLAttributeMap::iterator it = map.begin();
    for (int i = 0; i < index && it != map.end(); i++)
        ++it;
    ASSERT(it != map.end());
    return it->first + "=\"" + opp_xmlQuote(it->second.c_str()) + "\"";
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

