//==========================================================================
//  ASTBUILDER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "astbuilder.h"

#include "errorstore.h"
#include "nedelements.h"
#include "msgelements.h"
#include "exception.h"

namespace omnetpp {
namespace nedxml {

ASTBuilder::ASTBuilder(const char *fname, ErrorStore *e): sourceFilename(fname), errors(e)
{
}

ASTBuilder::~ASTBuilder()
{
    delete root;
}

ASTNode *ASTBuilder::getTree()
{
    ASTNode *tree = root;
    root = current = nullptr;
    return tree;
}

void ASTBuilder::startElement(const char *name, const char **atts)
{
    // initialize node
    ASTNode *node;
    bool unknown = false;
    try {
        try {
            node = NedAstNodeFactory().createElementWithTag(name);
        } catch (NedException& e) {
            node = MsgAstNodeFactory().createElementWithTag(name);
        }
    }
    catch (NedException& e) {
        errors->addError(current, "error: %s", e.what());
        node = new UnknownElement();
        node->setAttribute("element", name);
    }

    node->setSourceLocation(FileLine(sourceFilename, parser->getCurrentLineNumber()));

    // set attributes
    if (!unknown) {
        for (int i = 0; atts && atts[i]; i += 2) {
            try {
                node->setAttribute(atts[i], atts[i+1]);
            }
            catch (NedException& e) {
                errors->addError(node, "error in attribute '%s': %s", atts[i], e.what());
            }
        }
    }

    // add to tree
    if (!root) {
        root = current = node;
    }
    else {
        current->appendChild(node);
        current = node;
    }
}

void ASTBuilder::endElement(const char *name)
{
    current = current->getParent();
}

void ASTBuilder::characterData(const char *s, int len)
{
    // ignore
}

void ASTBuilder::processingInstruction(const char *target, const char *data)
{
    // ignore
}

}  // namespace nedxml
}  // namespace omnetpp

