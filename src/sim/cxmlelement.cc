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
#include "common/pooledstring.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"  // for ModNameParamResolver
#include "omnetpp/platdep/platmisc.h"
#include "minixpath.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

using std::ostream;

cXMLElement::cXMLElement(const char *tagName, cXMLElement *parent) : tagName(tagName)
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
}

std::string cXMLElement::str() const
{
    std::stringstream os;
    os << "<" << getTagName();
    for (int i = 0; i < getNumAttrs(); i++)
        os << " " << getAttrName(i) << "=\"" << opp_xmlquote(getAttrValue(i)) << "\"";
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
        if (!v->visit(child))
            return;
}

const char *cXMLElement::getTagName() const
{
    return tagName.c_str();
}

void cXMLElement::setSourceLocation(const char *fname, int line)
{
    loc = FileLine(fname, line);
}

const  char *cXMLElement::getSourceLocation() const
{
    return opp_staticpooledstring::get(loc.str().c_str());
}

const char *cXMLElement::getNodeValue() const
{
    return value.c_str();
}

void cXMLElement::setNodeValue(const char *s)
{
    value = opp_nulltoempty(s);
}

void cXMLElement::setNodeValue(const char *s, int len)
{
    setNodeValue(std::string(s, len).c_str());
}

void cXMLElement::appendNodeValue(const char *s, int len)
{
    setNodeValue(value.str().append(s, len).c_str());
}

cXMLElement::Attr *cXMLElement::findAttr(const char *name) const
{
    opp_pooledstring attrName = name;
    for (const Attr& attr : attrs)
        if (attr.first == attrName)
            return const_cast<Attr*>(&attr); // khmm
    return nullptr;
}

void cXMLElement::addAttr(const char *name, const char *value)
{
    attrs.push_back(Attr(name,value));
}

const char *cXMLElement::getAttribute(const char *name) const
{
    Attr *attr = findAttr(name);
    return attr == nullptr ? nullptr : attr->second.c_str();
}

void cXMLElement::setAttribute(const char *name, const char *newValue)
{
    name = opp_nulltoempty(name);
    newValue = opp_nulltoempty(newValue);
    Attr *attr = findAttr(name);
    if (attr == nullptr)
        addAttr(name, newValue);
    else
        attr->second = newValue;
}

void cXMLElement::setAttributes(const char **newAttrs)
{
    attrs.clear();
    if (newAttrs)
        for (const char **p = newAttrs; *p; p += 2)
            addAttr(*p, *(p+1));
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

cXMLAttributeMap cXMLElement::getAttributes() const
{
    cXMLAttributeMap map;
    for (const auto& attr : attrs)
        map[attr.first.str()] = attr.second.str();
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
        os << " " << attr.first << "=\"" << opp_xmlquote(attr.second.c_str()) << "\"";
    if (!*getNodeValue() && !getFirstChild()) {
        os << "/>\n";
        return;
    }
    os << ">";
    os << opp_xmlquote(getNodeValue());
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
    return attrs.size();
}

const char *cXMLElement::getAttrName(int index) const
{
    return attrs.at(index).first.c_str();
}

const char *cXMLElement::getAttrValue(int index) const
{
    return attrs.at(index).second.c_str();
}

std::string cXMLElement::getAttrDesc(int index) const
{
    std::stringstream out;
    out << getAttrName(index) << "=\"" + opp_xmlquote(getAttrValue(index)) << "\"";
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
        value = mod->isVector() ? std::to_string(mod->getIndex()) : "";
    else if (!strcmp(paramName, "MODULE_ID"))
        value = std::to_string(mod->getId());

    else if (!strcmp(paramName, "PARENTMODULE_FULLPATH"))
        value = parentModule ? parentModule->getFullPath() : "";
    else if (!strcmp(paramName, "PARENTMODULE_FULLNAME"))
        value = parentModule ? parentModule->getFullName() : "";
    else if (!strcmp(paramName, "PARENTMODULE_NAME"))
        value = parentModule ? parentModule->getName() : "";
    else if (!strcmp(paramName, "PARENTMODULE_INDEX"))
        value = (parentModule  && parentModule->isVector()) ? std::to_string(parentModule->getIndex()) : "";
    else if (!strcmp(paramName, "PARENTMODULE_ID"))
        value = parentModule ? std::to_string(parentModule->getId()) : "";

    else if (!strcmp(paramName, "GRANDPARENTMODULE_FULLPATH"))
        value = grandparentModule ? grandparentModule->getFullPath() : "";
    else if (!strcmp(paramName, "GRANDPARENTMODULE_FULLNAME"))
        value = grandparentModule ? grandparentModule->getFullName() : "";
    else if (!strcmp(paramName, "GRANDPARENTMODULE_NAME"))
        value = grandparentModule ? grandparentModule->getName() : "";
    else if (!strcmp(paramName, "GRANDPARENTMODULE_INDEX"))
        value = (grandparentModule && grandparentModule->isVector()) ? std::to_string(grandparentModule->getIndex()) : "";
    else if (!strcmp(paramName, "GRANDPARENTMODULE_ID"))
        value = grandparentModule ? std::to_string(grandparentModule->getId()) : "";
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

