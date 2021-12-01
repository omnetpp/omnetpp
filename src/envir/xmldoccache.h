//==========================================================================
//  XMLDOCCACHE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    cXMLDocCache : reads and caches XML config files
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_XMLDOCCACHE_H
#define __OMNETPP_ENVIR_XMLDOCCACHE_H

#include <map>
#include <string>
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/cxmlelement.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * Reads and caches XML config files.
 */
class ENVIR_API XMLDocCache : public cObject
{
  protected:
    typedef std::map<std::string,cXMLElement*> XMLDocMap;
    XMLDocMap documentCache; // key is the filename
    XMLDocMap contentCache; // key is the content (XML text)

  protected:
    cXMLElement *parseDocument(const char *filename);
    cXMLElement *parseContent(const char *content);

  public:
    /**
     * Constructor
     */
    XMLDocCache() {}

    /**
     * Destructor
     */
    virtual ~XMLDocCache();

    /**
     * Returns the DOM tree of the given document.
     *
     * NOTE: The returned node is the equivalent of the DOM "Document node",
     * which is the <i>parent</i> of the root element, not the root element
     * itself.
     */
    virtual cXMLElement *getDocument(const char *filename);

    /**
     * Returns the DOM tree of the given content (XML string).
     *
     * NOTE: The returned node is the equivalent of the DOM "Document node",
     * which is the <i>parent</i> of the root element, not the root element
     * itself.
     */
    virtual cXMLElement *getParsed(const char *content);

    /**
     * Removes the given document from the cache, and deletes its cXMLElement
     * tree. Nothing happens if the file has not been loaded.
     */
    virtual void forgetDocument(const char *filename);

    /**
     * Removes the given parsed content from the cache, and deletes its
     * cXMLElement tree. Nothing happens if the content string has not been
     * parsed yet.
     */
    virtual void forgetParsed(const char *content);

    /**
     * Empties the document cache.
     */
    virtual void flushDocumentCache();

    /**
     * Empties the parsed content cache.
     */
    virtual void flushParsedContentCache();
};

}  // namespace envir
}  // namespace omnetpp


#endif

