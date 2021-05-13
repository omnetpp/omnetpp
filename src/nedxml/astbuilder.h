//==========================================================================
//  ASTBUILDER.H - part of
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

#ifndef __OMNETPP_NEDXML_ASTBUILDER_H
#define __OMNETPP_NEDXML_ASTBUILDER_H


#include "errorstore.h"
#include "astnode.h"
#include "common/saxparser.h"

namespace omnetpp {
namespace nedxml {


/**
 * @brief SAX handler (to be used with SAXParser) that builds an AST.
 *
 * Elements of the tree are subclassed from ASTNode;
 * ASTNodeFactory is used to actually create the objects.
 *
 * Usage:
 * <pre>
 *    NEDSAXHandler nedsaxhandler(filename);
 *    SAXParser parser;
 *
 *    parser.setHandler(&nedsaxhandler);
 *    parser.parse(filename);
 *
 *    ASTNode *result = nedsaxhandler.getTree();
 * </pre>
 *
 * @ingroup XMLParser
 */
class NEDXML_API ASTBuilder : public omnetpp::common::SaxHandler
{
    ASTNode *root = nullptr;
    ASTNode *current = nullptr;
    const char *sourceFilename;
    ErrorStore *errors;

  public:
    /**
     * Constructor. Filename is necessary to create correct src-loc info.
     */
    ASTBuilder(const char *filename, ErrorStore *e);

    /**
     * Destructor
     */
    virtual ~ASTBuilder();

    /**
     * Returns the object tree that was built up during XML parsing.
     */
    virtual ASTNode *getTree();

    /** @name SAX event handlers */
    //@{
    virtual void startElement(const char *name, const char **atts) override;
    virtual void endElement(const char *name) override;
    virtual void characterData(const char *s, int len) override;
    virtual void processingInstruction(const char *target, const char *data) override;
    //@}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

