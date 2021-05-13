//==========================================================================
//  XMLASTPARSER.H - part of
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


#ifndef __OMNETPP_NEDXML_XMLASTPARSER_H
#define __OMNETPP_NEDXML_XMLASTPARSER_H

#include "errorstore.h"
#include "astnode.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Simple front-end to load an AST from an XML file. Invokes SAXParser
 * with ASTBuilder.
 *
 * @ingroup XMLParser
 */
NEDXML_API ASTNode *parseXML(const char *filename, ErrorStore *errors);

}  // namespace nedxml
}  // namespace omnetpp


#endif

