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
#include <string>
#include "cxmlelement.h"
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

//---------------

/*
 * A minimalistic XPath interpreter.
 */
class MiniXPath
{
  private:
    cXMLElement::ParamResolver *resolver;
  private:
    bool parseTagNameFromStepExpr(std::string& tagname, const char *stepexpr, int len);
    bool parseBracketedNum(int& n, const char *s, int len);
    bool parseConstant(std::string& value, const char *s, int len);
    bool parseBracketedAttrEquals(std::string& attr, std::string& value, const char *s, int len);
    cXMLElement *getFirstSiblingWithAttribute(cXMLElement *firstsibling, const char *tagname, const char *attr, const char *attrvalue);
    cXMLElement *getNthSibling(cXMLElement *firstsibling, const char *tagname, int n);
    cXMLElement *recursiveMatch(cXMLElement *node, const char *pathexpr);
    cXMLElement *matchSeparator(cXMLElement *node, const char *seppathexpr);
    cXMLElement *matchStep(cXMLElement *node, const char *pathexpr);
    bool nodeMatchesStepExpr(cXMLElement *node, const char *stepexpr, int steplen);
  public:
    /**
     * Ctor takes the resolver for parameters ($PARAM).
     */
    MiniXPath(cXMLElement::ParamResolver *resolver);
    /**
     * Returns the first match for pathexpr, from the given node as context.
     * Optional root element will be used if path expression starts with '/' or '//'.
     */
    cXMLElement *matchPathExpression(cXMLElement *node, const char *pathexpr, cXMLElement *root);
};

MiniXPath::MiniXPath(cXMLElement::ParamResolver *resolver)
{
    this->resolver = resolver;
}

bool MiniXPath::parseTagNameFromStepExpr(std::string& tagname, const char *stepexpr, int len)
{
    const char *lbracket = strchr(stepexpr, '[');
    if (!lbracket || lbracket-stepexpr>=len)
        tagname.assign(stepexpr,len);
    else
        tagname.assign(stepexpr,lbracket-stepexpr);
    return true;
}

bool MiniXPath::parseBracketedNum(int& n, const char *s, int len)
{
    if (*s!='[' || *(s+len-1)!=']')
        return false;
    for (const char *s1=s+1; s1<s+len-1; s1++)
        if (!isdigit(*s1))
            return false;
    n = atoi(s+1);
    return true;
}

bool MiniXPath::parseConstant(std::string& value, const char *s, int len)
{
    // we get the part after the equal sign in "[@attrname=...]", try to
    // match 'value', "value" or $PARAM
    if (*s=='\'' && *(s+len-1)=='\'')
    {
        value.assign(s+1, len-2);
        return true;
    }
    else if (*s=='"' && *(s+len-1)=='"')
    {
        value.assign(s+1, len-2);
        return true;
    }
    else if (*s=='$' && resolver!=NULL)
    {
       return resolver->resolve(std::string(s+1,len-1).c_str(), value);
    }
    return false;
}

bool MiniXPath::parseBracketedAttrEquals(std::string& attr, std::string& value, const char *s, int len)
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
    return parseConstant(value, equalsign+1, s+len-1-equalsign-1);
}

cXMLElement *MiniXPath::getFirstSiblingWithAttribute(cXMLElement *firstsibling, const char *tagname, const char *attr, const char *attrvalue)
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

cXMLElement *MiniXPath::getNthSibling(cXMLElement *firstsibling, const char *tagname, int n)
{
    for (cXMLElement *child=firstsibling; child; child=child->getNextSibling())
        if (!tagname || !strcasecmp(child->getTagName(),tagname))
            if (n--==0)
                return child;
    return NULL;
}

cXMLElement *MiniXPath::recursiveMatch(cXMLElement *node, const char *pathexpr)
{
    cXMLElement *res = matchStep(node, pathexpr);
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
cXMLElement *MiniXPath::matchSeparator(cXMLElement *node, const char *seppathexpr)
{
    assert(!*seppathexpr || *seppathexpr=='/');
    if (!*seppathexpr)
        return node; // end of pattern
    else if (*(seppathexpr+1)=='/')
        return recursiveMatch(node, seppathexpr+2); // separator is "//"  -- match in any depth
    else
        return matchStep(node, seppathexpr+1); // separator is "/"  -- match a child
}

// "node": the current node (".") whose children we'll try to match
cXMLElement *MiniXPath::matchStep(cXMLElement *node, const char *pathexpr)
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
        cXMLElement *nthnode = getNthSibling(node->getFirstChild(), NULL, n);
        if (!nthnode)
            return NULL;
        return matchSeparator(nthnode, sep);
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
    else if (parseTagNameFromStepExpr(tagname, stepexpr, steplen) && steplen==(int)tagname.length())
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
    else if (parseTagNameFromStepExpr(tagname, stepexpr, steplen) && parseBracketedNum(n, stepexpr+tagname.length(), steplen-tagname.length()))
    {
        cXMLElement *nthnode = getNthSibling(node->getFirstChild(), tagname.c_str(), n);
        if (!nthnode)
            return NULL;
        return matchSeparator(nthnode, sep);
    }
    else if (parseTagNameFromStepExpr(tagname, stepexpr, steplen) && parseBracketedAttrEquals(attr, value, stepexpr+tagname.length(), steplen-tagname.length()))
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
        throw new cRuntimeError("cXMLElement::getElementByPath(): invalid path expression `%s'", pathexpr);
    }
}

/*
bool MiniXPath::nodeMatchesStepExpr(cXMLElement *node, const char *stepexpr, int steplen)
{
    // match first pattern step.
    // might be one of: ".", "..", "*", "tagname", "tagname[n]", "tagname[@attr='value']"
    int n;
    std::string tagname, attr, value;
    if (!strncmp(stepexpr,".",steplen))
    {
        return true;
    }
    else if (!strncmp(stepexpr,"..",steplen))
    {
        return false;
    }
    else if (!strncmp(stepexpr,"*",steplen))
    {
        return true;
    }
    else if (stepexpr[0]=='*' && parseBracketedNum(n, stepexpr+1, steplen-1))
    {
        return n==0;
    }
    else if (stepexpr[0]=='*' && parseBracketedAttrEquals(attr, value, stepexpr+1, steplen-1))
    {
        const char *attrvalue = node->getAttribute(attr.c_str());
        return attrvalue && value==attrvalue;
    }
    else if (parseTagNameFromStepExpr(tagname, stepexpr, steplen) && steplen==(int)tagname.length())
    {
        return tagname==node->getTagName();
    }
    else if (parseTagNameFromStepExpr(tagname, stepexpr, steplen) && parseBracketedNum(n, stepexpr+tagname.length(), steplen-tagname.length()))
    {
        return tagname==node->getTagName() && n==0;
    }
    else if (parseTagNameFromStepExpr(tagname, stepexpr, steplen) && parseBracketedAttrEquals(attr, value, stepexpr+tagname.length(), steplen-tagname.length()))
    {
        const char *attrvalue = node->getAttribute(attr.c_str());
        return tagname==node->getTagName() && attrvalue && value==attrvalue;
    }
    else
    {
        throw new cRuntimeError("cXMLElement::getElementByPath(): invalid step in expression `%s'", stepexpr);
    }
}
*/

cXMLElement *MiniXPath::matchPathExpression(cXMLElement *node, const char *pathexpr, cXMLElement *root)
{
    ASSERT(root!=NULL || pathexpr[0]!='/');

    if (pathexpr[0]=='/')
        return matchSeparator(root->getParentNode(), pathexpr);
    else if (pathexpr[0]=='.')
        return matchStep(node, pathexpr);
    else
        return matchStep(node->getParentNode(), pathexpr);
}

/*
cXMLElement *MiniXPath::matchPathExpression(cXMLElement *node, const char *pathexpr, cXMLElement *root)
{
    // we don't have a document element (root node doesn't have a parent),
    // that's why matching the beginning of the path has to be handled specially
    // (matchSeparator(), matchStep(), recursiveMatch() all work on the
    // children of the node passed)

    ASSERT(root!=NULL || pathexpr[0]!='/');

    // explore then skip starting '/' or '//'
    bool slash = (pathexpr[0]=='/' && pathexpr[1]!='/');
    bool slashslash = (pathexpr[0]=='/' && pathexpr[1]=='/');
    while (pathexpr[0]=='/') pathexpr++;

    // find end of first pattern step
    const char *sep = strchr(pathexpr, '/');
    if (!sep) sep = pathexpr+strlen(pathexpr);

    const char *stepexpr = pathexpr;
    int steplen = sep-pathexpr;

    // if pathexpr starts with '/' or '//', then match from root
    if (slashslash)
    {
        node = root; // match from root

        // check if root matches
        if (nodeMatchesStepExpr(node, stepexpr, steplen) && matchSeparator(node, sep))
            return node;

         // no, try to match deeper
         return recursiveMatch(node, sep);
    }
    else
    {
        // if starts with '/', match root node
        if (slash)
            node = root;

        // check if 1st step matches our context node
        if (!nodeMatchesStepExpr(node, stepexpr, steplen))
            return NULL;

         // ok, match the rest
         return matchSeparator(node, sep);
     }
}
*/
//-------------------------------------

cXMLElement *cXMLElement::getDocumentElementByPath(cXMLElement *documentnode, const char *pathexpr,
                                                   cXMLElement::ParamResolver *resolver)
{
    cXMLElement *root = documentnode->getFirstChild();
    return MiniXPath(resolver).matchPathExpression(root, pathexpr, root);
}

cXMLElement *cXMLElement::getElementByPath(const char *pathexpr, cXMLElement::ParamResolver *resolver, cXMLElement *root)
{
    if (pathexpr[0]=='/' && !root)
        throw new cRuntimeError("cXMLElement::getElementByPath(): absolute path expression "
                                "(one starting with  '/') can only be used if root node is "
                                "also specified (path expression: `%s')", pathexpr);
    return MiniXPath(resolver).matchPathExpression(this, pathexpr, root);
}

void cXMLElement::debugDump(int depth) const
{
    int i;
    for (i=0; i<depth; i++) ev << "  ";
    ev << "<" << getTagName();
    cXMLAttributeMap map = getAttributes();
    for (cXMLAttributeMap::iterator it=map.begin(); it!=map.end(); it++)
        ev << " " << it->first << "=\"" << it->second << "\"";
    if (!*getNodeValue() && !getFirstChild())
        {ev << "/>\n"; return;}
    ev << ">";
    ev << getNodeValue();
    if (!getFirstChild())
        {ev << "</" << getTagName() << ">\n"; return;}
    ev << "\n";
    for (cXMLElement *child=getFirstChild(); child; child=child->getNextSibling())
        child->debugDump(depth+1);
    for (i=0; i<depth; i++) ev << "  ";
    ev << "</" << getTagName() << ">\n";
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

