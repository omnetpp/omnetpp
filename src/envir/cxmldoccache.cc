//==========================================================================
//  XMLDOCCACHE.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Implementation of
//    cXMLDocCache : reads and caches XML config files
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifdef _MSC_VER
// disable "identifier was truncated to '255' characters in the debug information" warnings
#pragma warning(disable:4786)
#endif


#include "cpolymorphic.h"
#include "cxmldoccache.h"
#include "cxmlelement.h"
#include "cexception.h"
#include "../nedxml/saxparser.h"


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
    virtual void startElement(const char *name, const char **atts);
    virtual void endElement(const char *name);
    virtual void characterData(const char *s, int len);
    virtual void processingInstruction(const char *target, const char *data);
    virtual void comment(const char *data);
    virtual void startCdataSection();
    virtual void endCdataSection();
    //@}
};

//---

cXMLSAXHandler::cXMLSAXHandler(const char *fname)
{
    root = current = new cXMLElement("/", "", NULL); // "Document node" (used as sort of a sentry)
    sourcefilename = fname;
}

cXMLSAXHandler::~cXMLSAXHandler()
{
    delete root;
}

cXMLElement *cXMLSAXHandler::getTree()
{
    cXMLElement *tree = root;
    root = current = new cXMLElement("/", "", NULL);
    return tree;
}

void cXMLSAXHandler::startElement(const char *name, const char **atts)
{
    // source location
    char srcloc[500];
    sprintf(srcloc,"%s:%d", sourcefilename, parser->getCurrentLineNumber());

    // create element
    cXMLElement *node = new cXMLElement(name, srcloc, current);
    current = node;

    // set attributes
    for (int i=0; atts && atts[i]; i+=2)
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

#endif  //WITH_NETBUILDER

//=========================================================

cXMLDocCache::cXMLDocCache()
{
}

cXMLDocCache::~cXMLDocCache()
{
    for (XMLDocMap::iterator i=cache.begin(); i!=cache.end(); ++i)
        delete i->second;
}

cXMLElement *cXMLDocCache::parseDocument(const char *filename)
{
#ifndef WITH_NETBUILDER
    throw new cRuntimeError("Cannot load `%s': XML config file support currently requires "
                            "WITH_NETBUILDER option (check configure.user or configuser.vc, then "
                            "rebuild OMNeT++)", filename);
#else
    cXMLSAXHandler sh(filename);
    SAXParser parser;

    parser.setHandler(&sh);
    bool ok = parser.parse(filename);
    if (!ok)
        throw new cRuntimeError("Error reading `%s': %s", filename, parser.getErrorMessage());

    return sh.getTree();
#endif
}

cXMLElement *cXMLDocCache::getDocument(const char *filename)
{
    // if found, return it from cache
    XMLDocMap::iterator i = cache.find(std::string(filename));
    if (i!=cache.end())
        return i->second;

    // load and store in cache
    // TODO resolve <xi:include>
    cXMLElement *documentnode = parseDocument(filename);
    cache[filename] = documentnode;

    return documentnode;
}


