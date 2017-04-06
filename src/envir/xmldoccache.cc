//==========================================================================
//  XMLDOCCACHE.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "xmldoccache.h"

#include "common/fileutil.h"
#include "nedxml/saxparser.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;
using namespace omnetpp::nedxml;

namespace omnetpp {
namespace envir {

// We depend on WITH_NETBUILDER because it brings the nedxml library which
// contains the XML parser. TBD XML parser should be an independent library
#ifdef WITH_NETBUILDER

/**
 * SAX handler (to be used with SAXParser) that builds a cXMLElement tree.
 */
class cXMLSAXHandler : public SAXHandler
{
    cXMLElement *root;
    cXMLElement *current;
    const char *sourcefilename;

  public:
    /**
     * Constructor. Filename is necessary to create correct src-loc info.
     */
    cXMLSAXHandler(const char *filename);

    /**
     * Destructor
     */
    virtual ~cXMLSAXHandler();

    /**
     * Returns the object tree that was built up during XML parsing.
     */
    virtual cXMLElement *getTree();

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

//---

cXMLSAXHandler::cXMLSAXHandler(const char *fname)
{
    root = current = new cXMLElement("/", "", nullptr);  // "Document node" (used as sort of a sentry)
    sourcefilename = fname;
}

cXMLSAXHandler::~cXMLSAXHandler()
{
    delete root;
}

cXMLElement *cXMLSAXHandler::getTree()
{
    cXMLElement *tree = root;
    root = current = new cXMLElement("/", "", nullptr);
    return tree;
}

void cXMLSAXHandler::startElement(const char *name, const char **atts)
{
    // source location
    char srcloc[500];
    sprintf(srcloc, "%s:%d", sourcefilename, parser->getCurrentLineNumber());

    // create element
    cXMLElement *node = new cXMLElement(name, srcloc, current);
    current = node;

    // set attributes
    for (int i = 0; atts && atts[i]; i += 2)
        node->setAttribute(atts[i], atts[i+1]);
}

void cXMLSAXHandler::endElement(const char *name)
{
    current = current->getParentNode();
}

void cXMLSAXHandler::characterData(const char *s, int len)
{
    current->appendNodeValue(s, len);
}

void cXMLSAXHandler::processingInstruction(const char *target, const char *data)
{
    // ignore
}

void cXMLSAXHandler::comment(const char *data)
{
    // ignore
}

void cXMLSAXHandler::startCdataSection()
{
    // ignore
}

void cXMLSAXHandler::endCdataSection()
{
    // ignore
}

#endif  // WITH_NETBUILDER

//=========================================================

XMLDocCache::XMLDocCache()
{
}

XMLDocCache::~XMLDocCache()
{
    for (auto & i : documentCache)
        delete i.second;
    for (auto & i : contentCache)
        delete i.second;
}

cXMLElement *XMLDocCache::parseDocument(const char *filename)
{
#ifndef WITH_NETBUILDER
    throw cRuntimeError("Cannot load '%s': XML config file support currently requires "
                        "WITH_NETBUILDER option (check configure.user or configuser.vc, then "
                        "rebuild OMNeT++)", filename);
#else
    cXMLSAXHandler sh(filename);
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parse(filename);
    if (!ok)
        throw cRuntimeError("Cannot load '%s': %s", filename, parser.getErrorMessage());

    return sh.getTree();
#endif
}

cXMLElement *XMLDocCache::parseContent(const char *content)
{
#ifndef WITH_NETBUILDER
    throw cRuntimeError("Cannot parse XML string: XML support currently requires "
                        "WITH_NETBUILDER option (check configure.user or configuser.vc, then "
                        "rebuild OMNeT++)");
#else
    cXMLSAXHandler sh("content");
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parseContent(content);
    if (!ok)
        throw cRuntimeError("Cannot parse XML string: %s", parser.getErrorMessage());

    return sh.getTree();
#endif
}

cXMLElement *XMLDocCache::getDocument(const char *filename)
{
    // if found, return it from cache
    std::string key = tidyFilename(toAbsolutePath(filename).c_str());
    XMLDocMap::iterator it = documentCache.find(key);
    if (it != documentCache.end())
        return it->second;

    // load and store in cache
    cXMLElement *documentnode = parseDocument(filename);
    documentCache[key] = documentnode;
    return documentnode;
}

cXMLElement *XMLDocCache::getParsed(const char *content)
{
    // if found, return it from cache
    XMLDocMap::iterator it = contentCache.find(content);
    if (it != contentCache.end())
        return it->second;

    // parse and store in cache
    cXMLElement *documentnode = parseContent(content);
    contentCache[content] = documentnode;
    return documentnode;
}

void XMLDocCache::forgetDocument(const char *filename)
{
    std::string key = tidyFilename(toAbsolutePath(filename).c_str());
    XMLDocMap::iterator it = documentCache.find(key);
    if (it != documentCache.end()) {
        cXMLElement *node = it->second;
        documentCache.erase(it);
        delete node;
    }
}

void XMLDocCache::forgetParsed(const char *content)
{
    XMLDocMap::iterator it = contentCache.find(content);
    if (it != contentCache.end()) {
        cXMLElement *node = it->second;
        contentCache.erase(it);
        delete node;
    }
}

void XMLDocCache::flushDocumentCache()
{
    for (auto & i : documentCache)
        delete i.second;
    documentCache.clear();
}

void XMLDocCache::flushParsedContentCache()
{
    for (auto & i : contentCache)
        delete i.second;
    contentCache.clear();
}

}  // namespace envir
}  // namespace omnetpp

