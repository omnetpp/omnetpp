//==========================================================================
//  XMLGENERATOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __XMLGENERATOR_H
#define __XMLGENERATOR_H


#include <iostream>
#include "nedelements.h"

using std::ostream;

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
    /**
     * Constructor
     */
    NEDXMLGenerator();

    /**
     * Destructor
     */
    virtual ~NEDXMLGenerator();

    /**
     * Enable or disable generation of src-loc attributes in the output XML.
     * src-loc attributes contain filename-line-column information that refers
     * to the original document. For example, if the NEDElement tree was
     * produced by parsing a NED file, src-loc attributes refer to locations
     * in the NED file.
     */
    virtual void setSourceLocationAttributes(bool srcloc);

    /**
     * Set indent size in the output XML.
     */
    virtual void setIndentSize(int indentsize);

    /**
     * Serialize the object tree as XML. The XML declaration will be:
     * <pre>
     * <?xml version="1.0"?>
     * </pre>
     * I.e. unspecified encoding, and no document type will be included.
     */
    virtual void generate(ostream& out, NEDElement *tree);
};

#endif
