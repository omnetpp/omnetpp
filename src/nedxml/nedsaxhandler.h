//==========================================================================
//   NEDSAXHANDLER.H -
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

#ifndef __NEDSAXHANDLER_H
#define __NEDSAXHANDLER_H


#include "nedelement.h"
#include "saxparser.h"


/**
 * SAX handler (to be used with SAXParser) that builds a NED object tree.
 * Elements of the tree are subclassed from NEDElement;
 * NEDElementFactory is used to actually create the objects.
 *
 * @ingroup XMLParser
 */
class NEDSAXHandler : public SAXHandler
{
    NEDElement *root;
    NEDElement *current;
    const char *sourcefilename;
  public:
    NEDSAXHandler(const char *filename);
    virtual ~NEDSAXHandler();
    virtual NEDElement *getTree();
    virtual void startElement(const char *name, const char **atts);
    virtual void endElement(const char *name);
    virtual void characterData(const char *s, int len);
    virtual void processingInstruction(const char *target, const char *data);
    virtual void comment(const char *data);
    virtual void startCdataSection();
    virtual void endCdataSection();
};

#endif

