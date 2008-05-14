//==========================================================================
//  NEDSAXHANDLER.H - part of
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

#ifndef __NEDSAXHANDLER_H
#define __NEDSAXHANDLER_H


#include "nedelement.h"
#include "nederror.h"
#include "saxparser.h"

NAMESPACE_BEGIN


/**
 * SAX handler (to be used with SAXParser) that builds a NED object tree.
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
    const char *sourcefilename;
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
    virtual void startElement(const char *name, const char **atts);
    virtual void endElement(const char *name);
    virtual void characterData(const char *s, int len);
    virtual void processingInstruction(const char *target, const char *data);
    virtual void comment(const char *data);
    virtual void startCdataSection();
    virtual void endCdataSection();
    //@}
};

NAMESPACE_END


#endif

