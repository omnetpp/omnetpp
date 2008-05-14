//==========================================================================
//  NEDSAXHANDLER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "nedsaxhandler.h"
#include "nedelements.h"
#include "nederror.h"
#include "nedexception.h"

USING_NAMESPACE


NEDSAXHandler::NEDSAXHandler(const char *fname, NEDErrorStore *e)
{
    root = current = 0;
    sourcefilename = fname;
    errors = e;
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
    NEDElement *node;
    bool unknown = false;
    try {
        node = NEDElementFactory::getInstance()->createElementWithTag(name);
    }
    catch (NEDException& e) {
        errors->addError(current, "error: %s", e.what());
        node = new UnknownElement();
        node->setAttribute("element", name);
    }

    // "debug info"
    char buf[200];
    sprintf(buf,"%s:%d",sourcefilename, parser->getCurrentLineNumber());
    node->setSourceLocation(buf);

    // set attributes
    if (!unknown)
    {
        for (int i=0; atts && atts[i]; i+=2) {
            try {
                node->setAttribute(atts[i], atts[i+1]);
            }
            catch (NEDException& e) {
                errors->addError(node, "error in attribute '%s': %s", atts[i], e.what());
            }
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

