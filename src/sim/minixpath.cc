//==========================================================================
//  MINIXPATH.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
// Contents:
//   class MiniXPath
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
#include "minixpath.h"
#include "cexception.h"

#ifdef _MSC_VER
#define strcasecmp  stricmp
#endif



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
        res = recursiveMatch(child, pathexpr);
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
        if (node->getParentNode() && node->getParentNode()!=docNode)
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

/* this function is currently unused
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

cXMLElement *MiniXPath::matchPathExpression(cXMLElement *contextNode, const char *pathexpr, cXMLElement *documentNode)
{
    this->docNode = documentNode;
    if (pathexpr[0]=='/')
    {
        // we need the document node if path starts with "/"
        if (documentNode==NULL)
            throw new cRuntimeError("Mini XPath engine: cannot evaluate a path starting with '/' "
                                    "if the documentNode optional parameter is not supplied");

        // plain "/" or "/." or "/./." doesn't match anything (try with any XPath interpreter)
        while (pathexpr[0]=='/' && pathexpr[1]=='.' && pathexpr[2]=='/')
            pathexpr += 2;
        if (!pathexpr[0] || (pathexpr[0]=='/' && !pathexpr[1]) || (pathexpr[0]=='/' && pathexpr[1]=='.' && !pathexpr[2]))
            return NULL;

        // match
        return matchSeparator(docNode, pathexpr);
    }
    else
    {
        // plain ".", "./." or "././." matches the context node itself
        while (pathexpr[0]=='.' && pathexpr[1]=='/' && pathexpr[2]!='/')
            pathexpr += 2;
        if (!pathexpr[0])
            return NULL;  // plain "./" is nothing
        if (pathexpr[0]=='.' && !pathexpr[1])
            return contextNode;

        // match
        return matchStep(contextNode, pathexpr);
    }
}

