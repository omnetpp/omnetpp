//==========================================================================
//   XMLGEN.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __XMLGENERATOR_H
#define __XMLGENERATOR_H


#include <iostream.h>
#include "nedelements.h"

/**
 * Simple front-end to NEDXMLGenerator.
 *
 * @ingroup XMLGenerator
 */
void generateXML(ostream& out, NEDElement *tree, bool srcloc, int indentsize = 4);

/**
 * Serializes a NED object tree in XML format.
 *
 * @ingroup XMLGenerator
 */
class NEDXMLGenerator
{
  protected:
    bool printsrcloc;
    int indentsize;
    virtual void printAttrValue(ostream& out, const char *s);
    virtual void doGenerate(ostream& out, NEDElement *node, int level);
  public:
    NEDXMLGenerator();
    virtual ~NEDXMLGenerator();
    virtual void setSourceLocationAttributes(bool srcloc);
    virtual void setIndentSize(int indentsize);
    virtual void generate(ostream& out, NEDElement *tree);
};

#endif
