//==========================================================================
//  XMLGENERATOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <iostream>
#include <iomanip>
#include "nedelements.h"
#include "xmlgenerator.h"

using std::ostream;
using std::endl;

void generateXML(ostream& out, NEDElement *tree, bool srcloc, int indentsize)
{
    NEDXMLGenerator xmlgen;
    xmlgen.setIndentSize(indentsize);
    xmlgen.setSourceLocationAttributes(srcloc);
    xmlgen.generate(out, tree);
}

//-----------------------------------------

NEDXMLGenerator::NEDXMLGenerator()
{
    indentsize = 2;
}

NEDXMLGenerator::~NEDXMLGenerator()
{
}

void NEDXMLGenerator::setSourceLocationAttributes(bool srcloc)
{
    printsrcloc = srcloc;
}

void NEDXMLGenerator::setIndentSize(int indentsiz)
{
    indentsize = indentsiz;
    if (indentsize>16) indentsize=16;
}

void NEDXMLGenerator::generate(ostream& out, NEDElement *tree)
{
    // xml header
    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl << endl;

    // recursively generate children
    doGenerate(out, tree, 0);
}

void NEDXMLGenerator::printAttrValue(ostream& out, const char *s)
{
    for (; *s; s++)
    {
        unsigned char c = *s;
        if (c=='<')
           out << "&lt;";
        else if (c=='>')
           out << "&gt;";
        else if (c=='"')
           out << "&quot;";
        else if (c=='&')
           out << "&amp;";
        else if (c<32)
           out << "&#" << int(c) << ";";
        else
           out << c;
    }
}

void NEDXMLGenerator::doGenerate(ostream& out, NEDElement *node, int level)
{
    const char *indent = "                ";
    indent = indent + strlen(indent) - indentsize;

    // indent + opening tag
    int i;
    for (i=0; i<level; i++)
        out << indent;
    out << "<" << node->getTagName();

    // location info
    if (printsrcloc && node->getSourceLocation())
    {
        out << " src-loc=\"";
        printAttrValue(out, node->getSourceLocation());
        out << "\"";
    }

    // output attributes, but only if they differ from their default values
    for (i=0; i<node->getNumAttributes(); i++)
    {
        const char *attrval = node->getAttribute(i);
        const char *defaultval = node->getAttributeDefault(i);
        if (!attrval) attrval = "";
        if (!defaultval) defaultval= "";
        if (strcmp(attrval,defaultval))
        {
            out << " " << node->getAttributeName(i);
            out << "=\"";
            printAttrValue(out, attrval);
            out << "\"";
        }
    }

    // if no children, merge closing tag using "<../>"
    if (!node->getFirstChild())
    {
       out << "/>" << endl;
       return;
    }
    out << ">" << endl;

    // children
    for (NEDElement *child=node->getFirstChild(); child; child=child->getNextSibling())
    {
        doGenerate(out, child, level+1);
    }

    // indent + closing tag
    for (i=0; i<level; i++)
        out << indent;
    out << "</" << node->getTagName() << ">" << endl;
}


