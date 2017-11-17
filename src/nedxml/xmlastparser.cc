//==========================================================================
//  XMLASTPARSER.CC - part of
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

#include "saxparser.h"
#include "errorstore.h"
#include "xmlastparser.h"
#include "astbuilder.h"

namespace omnetpp {
namespace nedxml {

ASTNode *parseXML(const char *filename, ErrorStore *errors)
{
    ASTBuilder sh(filename, errors);
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parse(filename);
    if (!ok) {
        errors->addError("", "error reading '%s': %s", filename, parser.getErrorMessage());
        return nullptr;
    }
    return sh.getTree();
}

}  // namespace nedxml
}  // namespace omnetpp

