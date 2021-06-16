//==========================================================================
//  XMLGENERATOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "xmlgenerator.h"

namespace omnetpp {
namespace nedxml {

using std::ostream;
using std::endl;

void generateXML(ostream& out, ASTNode *tree, bool srcloc, int indentsize)
{
    XMLGenerator xmlgen;
    xmlgen.setIndentSize(indentsize);
    xmlgen.setSourceLocationAttributes(srcloc);
    xmlgen.generate(out, tree);
}

std::string generateXML(ASTNode *tree, bool srcloc, int indentsize)
{
    std::stringstream out;
    generateXML(out, tree, srcloc, indentsize);
    return out.str();
}

//-----------------------------------------

void XMLGenerator::setSourceLocationAttributes(bool srcloc)
{
    printSrcLoc = srcloc;
}

void XMLGenerator::setIndentSize(int indentsiz)
{
    indentSize = indentsiz;
    if (indentSize > 16)
        indentSize = 16;
}

void XMLGenerator::generate(ostream& out, ASTNode *tree)
{
    // xml header
    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl << endl;

    // recursively generate children
    doGenerate(out, tree, 0);
}

std::string XMLGenerator::generate(ASTNode *tree)
{
    std::stringstream out;
    generate(out, tree);
    return out.str();
}

void XMLGenerator::printAttrValue(ostream& out, const char *s)
{
    for ( ; *s; s++) {
        unsigned char c = *s;
        if (c == '<')
            out << "&lt;";
        else if (c == '>')
            out << "&gt;";
        else if (c == '"')
            out << "&quot;";
        else if (c == '&')
            out << "&amp;";
        else if (c < 32)
            out << "&#" << int(c) << ";";
        else
            out << c;
    }
}

void XMLGenerator::doGenerate(ostream& out, ASTNode *node, int level)
{
    const char *indent = "                ";
    indent = indent + strlen(indent) - indentSize;

    // indent + opening tag
    int i;
    for (i = 0; i < level; i++)
        out << indent;
    out << "<" << node->getTagName();

    // location info
    if (printSrcLoc && !node->getSourceLocation().empty()) {
        out << " src-loc=\"";
        printAttrValue(out, node->getSourceLocation().str().c_str());
        out << "\"";

        const SourceRegion& r = node->getSourceRegion();
        if (r.startLine > 0) {  // filled in
            out << " src-region=\"" << r.startLine << ":" << r.startColumn << "-"
                << r.endLine << ":" << r.endColumn << "\"";
        }
    }

    // output attributes, but only if they differ from their default values
    for (i = 0; i < node->getNumAttributes(); i++) {
        const char *attrval = node->getAttribute(i);
        const char *defaultval = node->getAttributeDefault(i);
        if (!attrval)
            attrval = "";
        if (!defaultval)
            defaultval = "";
        if (strcmp(attrval, defaultval)) {
            out << " " << node->getAttributeName(i);
            out << "=\"";
            printAttrValue(out, attrval);
            out << "\"";
        }
    }

    // if no children, merge closing tag using "<../>"
    if (!node->getFirstChild()) {
        out << "/>" << endl;
        return;
    }
    out << ">" << endl;

    // children
    for (ASTNode *child = node->getFirstChild(); child; child = child->getNextSibling()) {
        doGenerate(out, child, level+1);
    }

    // indent + closing tag
    for (i = 0; i < level; i++)
        out << indent;
    out << "</" << node->getTagName() << ">" << endl;
}

}  // namespace nedxml
}  // namespace omnetpp

