//==========================================================================
//  XMLDOCCACHE.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
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
#include "common/saxparser_default.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

/**
 * SAX handler that builds a cXMLElement tree.
 */
class cXmlSaxHandler : public SaxHandler
{
    cXMLElement *root;
    cXMLElement *current;
    const char *sourcefilename;

  public:
    /**
     * Constructor. Filename is necessary to create correct src-loc info.
     */
    cXmlSaxHandler(const char *filename);

    /**
     * Destructor
     */
    virtual ~cXmlSaxHandler();

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
    //@}
};

//---

cXmlSaxHandler::cXmlSaxHandler(const char *fname): sourcefilename(fname)
{
    root = current = new cXMLElement("/", nullptr);  // "Document node" (used as sort of a sentry)
}

cXmlSaxHandler::~cXmlSaxHandler()
{
    delete root;
}

cXMLElement *cXmlSaxHandler::getTree()
{
    cXMLElement *tree = root;
    root = current = new cXMLElement("/", nullptr);
    return tree;
}

void cXmlSaxHandler::startElement(const char *name, const char **atts)
{
    cXMLElement *node = new cXMLElement(name, current);
    node->setSourceLocation(sourcefilename, parser->getCurrentLineNumber());
    if (atts)
        node->setAttributes(atts);

    current = node;
}

void cXmlSaxHandler::endElement(const char *name)
{
    current = current->getParentNode();
}

void cXmlSaxHandler::characterData(const char *s, int len)
{
    current->appendNodeValue(s, len);
}

void cXmlSaxHandler::processingInstruction(const char *target, const char *data)
{
    // ignore
}

//=========================================================

XMLDocCache::~XMLDocCache()
{
    for (auto & i : documentCache)
        dropAndDelete(i.second);
    for (auto & i : contentCache)
        dropAndDelete(i.second);
}

cXMLElement *XMLDocCache::parseDocument(const char *filename)
{
    try {
        cXmlSaxHandler saxHandler(filename);
        DefaultSaxParser parser;

        parser.setHandler(&saxHandler);
        parser.parseFile(filename);
        return saxHandler.getTree();
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s", e.what()); // just convert
    }
}

cXMLElement *XMLDocCache::parseContent(const char *content)
{
    try {
        cXmlSaxHandler saxHandler("content");
        DefaultSaxParser parser;

        parser.setHandler(&saxHandler);
        parser.parseContent(content);
        return saxHandler.getTree();
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s", e.what()); // just convert
    }
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
    take(documentnode);
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
    take(documentnode);
    return documentnode;
}

void XMLDocCache::forgetDocument(const char *filename)
{
    std::string key = tidyFilename(toAbsolutePath(filename).c_str());
    XMLDocMap::iterator it = documentCache.find(key);
    if (it != documentCache.end()) {
        cXMLElement *node = it->second;
        documentCache.erase(it);
        dropAndDelete(node);
    }
}

void XMLDocCache::forgetParsed(const char *content)
{
    XMLDocMap::iterator it = contentCache.find(content);
    if (it != contentCache.end()) {
        cXMLElement *node = it->second;
        contentCache.erase(it);
        dropAndDelete(node);
    }
}

void XMLDocCache::flushDocumentCache()
{
    for (auto & i : documentCache)
        dropAndDelete(i.second);
    documentCache.clear();
}

void XMLDocCache::flushParsedContentCache()
{
    for (auto & i : contentCache)
        dropAndDelete(i.second);
    contentCache.clear();
}

}  // namespace envir
}  // namespace omnetpp

