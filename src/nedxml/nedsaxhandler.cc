//==========================================================================
//   NEDSAXHANDLER.CC -
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


#include "nedsaxhandler.h"
#include "nedelements.h"
#include "nederror.h"


NEDSAXHandler::NEDSAXHandler(const char *fname)
{
    root = current = 0;
    sourcefilename = fname;
}

NEDSAXHandler::~NEDSAXHandler()
{
    delete root;
}

NEDElement *NEDSAXHandler::getTree()
{
    NEDElement *tree = root;
    root = current = 0;
    return tree;
}

void NEDSAXHandler::startElement(const char *name, const char **atts)
{
    // initialize node
    NEDElement *node = NEDElementFactory::getInstance()->createNodeWithTag(name);

    // "debug info"
    char buf[200];
    sprintf(buf,"%s:%d",sourcefilename, parser->getCurrentLineNumber());
    node->setSourceLocation(buf);

    // set attributes
    for (int i=0; atts[i]; i+=2) {
        try {
            node->setAttribute(atts[i], atts[i+1]);
        } catch (NEDException *e) {
            NEDError(node, "error in attribute '%s': %s", atts[i],e->errorMessage());
            delete e;
        }
    }

    // add to tree
    if (!root) {
        root = current = node;
    } else {
        current->appendChild(node);
        current = node;
    }
}

void NEDSAXHandler::endElement(const char *name)
{
    current = current->getParent();
}

void NEDSAXHandler::characterData(const char *s, int len)
{
    // ignore
}

void NEDSAXHandler::processingInstruction(const char *target, const char *data)
{
    // ignore
}

void NEDSAXHandler::comment(const char *data)
{
    // ignore
}

void NEDSAXHandler::startCdataSection()
{
    // ignore
}

void NEDSAXHandler::endCdataSection()
{
    // ignore
}

