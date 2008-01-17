//==========================================================================
//  MINIXPATH.H - part of
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


#ifndef __MINIXPATH_H__
#define __MINIXPATH_H__

#include "cxmlelement.h"

NAMESPACE_BEGIN


/**
 * A minimalistic XPath interpreter.
 */
class MiniXPath
{
  private:
    cXMLElement *docNode; // document node (parent of root element) of the ongoing match
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

  public:
    /**
     * Ctor takes the resolver for parameters ($PARAM).
     */
    MiniXPath(cXMLElement::ParamResolver *resolver);

    /**
     * Returns the first match for pathexpr, from the given node as context.
     * Optional document node (i.e. parent of root element) will be used
     * if path expression starts with '/' or '//'.
     */
    cXMLElement *matchPathExpression(cXMLElement *contextNode, const char *pathexpr, cXMLElement *documentNode);
};

NAMESPACE_END


#endif


