//==========================================================================
//  NEDSAXHANDLER.H - part of
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

#ifndef __OMNETPP_NEDXML_NEDSAXHANDLER_H
#define __OMNETPP_NEDXML_NEDSAXHANDLER_H


#include "nedelement.h"
#include "nederror.h"
#include "saxparser.h"

namespace omnetpp {
namespace nedxml {


/**
 * @brief SAX handler (to be used with SAXParser) that builds a NED object tree.
 *
 * Elements of the tree are subclassed from NEDElement;
 * NEDElementFactory is used to actually create the objects.
 *
 * Usage:
 * <pre>
 *    NEDSAXHandler nedsaxhandler(filename);
 *    SAXParser parser;
 *
 *    parser.setHandler(&nedsaxhandler);
 *    parser.parse(filename);
 *
 *    NEDElement *result = nedsaxhandler.getTree();
 * </pre>
 *
 * @ingroup XMLParser
 */
class NEDXML_API NEDSAXHandler : public SAXHandler
{
    NEDElement *root;
    NEDElement *current;
    const char *sourceFilename;
    NEDErrorStore *errors;

  public:
    /**
     * Constructor. Filename is necessary to create correct src-loc info.
     */
    NEDSAXHandler(const char *filename, NEDErrorStore *e);

    /**
     * Destructor
     */
    virtual ~NEDSAXHandler();

    /**
     * Returns the object tree that was built up during XML parsing.
     */
    virtual NEDElement *getTree();

    /** @name SAX event handlers */
    //@{
    virtual void startElement(const char *name, const char **atts) override;
    virtual void endElement(const char *name) override;
    virtual void characterData(const char *s, int len) override;
    virtual void processingInstruction(const char *target, const char *data) override;
    virtual void comment(const char *data) override;
    virtual void startCdataSection() override;
    virtual void endCdataSection() override;
    //@}
};

} // namespace nedxml
}  // namespace omnetpp


#endif

