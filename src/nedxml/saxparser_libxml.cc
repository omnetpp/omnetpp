//==========================================================================
//  SAXPARSERLIBXML.CC - part of
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


#include <string>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/SAX.h>
#include "saxparser.h"

NAMESPACE_BEGIN


//
// Validating only if LibXML is at least 2.6.0
//
#if LIBXML_VERSION>20600

SAXParser::SAXParser()
{
    saxhandler = NULL;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxhandler = sh;
    sh->setParser(this);
}

static int nodeLine;  // line number of current node
static xmlParserCtxtPtr ctxt; // parser context

// static void dontPrintError(void *, xmlErrorPtr) {} // an xmlStructuredErrorFunc

static void generateSAXEvents(xmlNode *node, SAXHandler *sh)
{
    nodeLine = node->line;
    switch (node->type)
    {
        case XML_ELEMENT_NODE: {
            // collect attributes for startElement()
            int numAttrs = 0;
            xmlAttr *attr;
            for (attr = node->properties; attr; attr = (xmlAttr *)attr->next)
                numAttrs++;
            const char **attrs = new const char *[2*(numAttrs+1)];
            int k;
            for (attr = node->properties, k=0; attr; attr = (xmlAttr *)attr->next, k+=2)
            {
                // ignore namespaces: pass "prefix:name" to SAX handler
                if (attr->ns) {
                    attrs[k] = new char [strlen((const char *)attr->name)+strlen((const char *)attr->ns->prefix)+2];
                    sprintf((char *)attrs[k], "%s:%s", attr->ns->prefix, attr->name);
                } else {
                    attrs[k] = (const char *) attr->name;
                }
                attrs[k+1] = (const char *) attr->children->content; // first text node within attr
            }
            attrs[k] = NULL;
            attrs[k+1] = NULL;

            // element name. ignore namespaces: pass "prefix:name" to SAX handler
            char *nodename;
            if (node->ns) {
                nodename = new char [strlen((const char *)node->name)+strlen((const char *)node->ns->prefix)+2];
                sprintf(nodename, "%s:%s", node->ns->prefix, node->name);
            } else {
                nodename = (char *) node->name;
            }

            // invoke startElement()
            sh->startElement(nodename, attrs);

            // dealloc prefixed attr names and element name
            for (attr = node->properties, k=0; attr; attr = (xmlAttr *)attr->next, k+=2)
                if (attr->ns)
                    delete [] (char *)attrs[k];
            delete [] attrs;
            if (node->ns)
                delete [] nodename;

            // recursive processing of children
            xmlNode *child;
            for (child = node->children; child; child = child->next)
                generateSAXEvents(child, sh);

            // invoke endElement()
            sh->endElement((const char *)node->name);
            break;
        }
        case XML_TEXT_NODE:
            sh->characterData((const char *)node->content,strlen((const char *)node->content));
            break;
        case XML_PI_NODE:
            // FIXME sh->processingInstruction((const char *)target,(const char *)data);
            break;
        case XML_COMMENT_NODE:
            sh->comment((const char *)node->content);
            break;
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            break; // ignore
        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_ATTRIBUTE_NODE:
            // should not occur (see XML_PARSE_xxx options)
            fprintf(stderr,"ERROR: libxml wrapper: generateSAXEvents(): node type %d unexpected\n",node->type);
            assert(0);
            break;
        default:
            // DTD stuff: ignore
            break;

    }
}

bool SAXParser::parse(const char *filename)
{
    return doParse(filename, NULL);
}

bool SAXParser::parseContent(const char *content)
{
    return doParse(NULL, content);
}

bool SAXParser::doParse(const char *filename, const char *content)
{
    assert((filename==NULL) != (content==NULL));  // exactly one of them is non-NULL
    strcpy(errortext, "<error msg unfilled>");

    //
    // When there's a DTD given, we *must* use it, and complete default attrs from it.
    //
    // Strategy: build DOM tree with validation enabled, then generate SAX events
    // from it. LibXML's SAX2-based validation code isn't robust enough yet
    // (as of 09/2004)
    //
    ctxt = xmlNewParserCtxt();
    if (!ctxt)
    {
        strcpy(errortext, "Failed to allocate parser context");
        return false;
    }

    // parse the file
    unsigned options = XML_PARSE_DTDVALID | // validate with the DTD (but we'll have to revalidate after XInclude processing anyway)
                       XML_PARSE_DTDATTR |  // complete default attributes from DTD
                       XML_PARSE_NOENT |    // substitute entities
                       XML_PARSE_NONET |    // forbid network access
                       XML_PARSE_NOBLANKS | // discard ignorable white space
                       XML_PARSE_NOCDATA |  // merge CDATA as text nodes
                       XML_PARSE_NOERROR |  // suppress error reports
                       //XML_PARSE_XINCLUDE |  // would be nice, but does not work
                       XML_PARSE_NOWARNING; // suppress warning reports

    xmlDocPtr doc;
    if (filename)
        doc = xmlCtxtReadFile(ctxt, filename, NULL, options);
    else
        doc = xmlCtxtReadMemory(ctxt, content, strlen(content), "string-literal", NULL, options);

    // check if parsing succeeded
    if (!doc)
    {
        sprintf(errortext, "Parse error: %s at line %s:%d",
                ctxt->lastError.message, ctxt->lastError.file, ctxt->lastError.line);
        xmlFreeParserCtxt(ctxt);
        return false;
    }

    // perform XInclude substitution. Note: errors will be dumped on stderr (these messages
    // cannot be captured via the public libXML2 API, as xmlXIncludeCtxt doesn't have
    // error/warning function ptrs as other ctxts)
    //xmlStructuredError = dontPrintError;
    int xincludeResult = xmlXIncludeProcess(doc);
    if (xincludeResult == -1) // error
    {
        sprintf(errortext, "XInclude substitution error"); // further details unavailable from libXML without much pain
        xmlFreeParserCtxt(ctxt);
        xmlFreeDoc(doc);
        return false;
    }

    // validate with the DTD again, because the document only becomes complete
    // after XInclude processing. Must check whether we have a DTD first, otherwise
    // there'll be a XML_ERR_NO_DTD error (ctxt->errNo).
    bool hasDTD = false;
    for (xmlNode *child = doc->children; child; child = child->next)
        if (child->type == XML_DTD_NODE)
            hasDTD = true;
    if (hasDTD)
    {
        xmlValidCtxtPtr vctxt = xmlNewValidCtxt();
        vctxt->userData = errortext;
        vctxt->error = (xmlValidityErrorFunc) sprintf;
        vctxt->warning = (xmlValidityWarningFunc) sprintf;

        if (!xmlValidateDocument(vctxt, doc))
        {
            sprintf(errortext, "Validation error: %s", std::string(errortext).c_str());
            xmlFreeValidCtxt(vctxt);
            xmlFreeParserCtxt(ctxt);
            xmlFreeDoc(doc);
            return false;
        }
        xmlFreeValidCtxt(vctxt);
    }

    // traverse tree and generate SAX events
    xmlNode *root = xmlDocGetRootElement(doc);
    generateSAXEvents(root, saxhandler);

    // free parser context and document tree
    xmlFreeParserCtxt(ctxt);
    xmlFreeDoc(doc);
    return true;
}

int SAXParser::getCurrentLineNumber()
{
    return nodeLine;
}


#else

//
// Old, SAX1-based code, left here as fallback. Doesn't perform DTD validation
// and attr completion.
//

static void libxmlStartElementHandler(void *userData, const xmlChar *name, const xmlChar **atts)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->startElement((const char *)name, (const char **)atts);
}

static void libxmlEndElementHandler(void *userData, const xmlChar *name)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->endElement((const char *)name);
}

static void libxmlCharacterDataHandler(void *userData, const xmlChar *s, int len)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->characterData((const char *)s,len);
}

/*
static void libxmlProcessingInstructionHandler(void *userData, const xmlChar *target, const xmlChar *data)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->processingInstruction((const char *)target,(const char *)data);
}
*/

static void libxmlCommentHandler(void *userData, const xmlChar *data)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->comment((const char *)data);
}


/*
static void libxmlStartCdataSectionHandler(void *userData)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->startCdataSection();
}

static void libxmlEndCdataSectionHandler(void *userData)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->endCdataSection();
}
*/

static void libxmlWarningHandler(void *userData, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  // g_logv("XML", G_LOG_LEVEL_WARNING, msg, args);
  // printf(msg, args);
  va_end(args);
}

static void libxmlErrorHandler(void *userData, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  // g_logv("XML", G_LOG_LEVEL_CRITICAL, msg, args);
  // printf(msg, args);
  va_end(args);
}

static void libxmlFatalErrorHandler(void *userData, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  // g_logv("XML", G_LOG_LEVEL_ERROR, msg, args);
  // printf(msg, args);
  va_end(args);
}

SAXParser::SAXParser()
{
    saxhandler = NULL;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxhandler = sh;
    sh->setParser(this);
}

static xmlSAXHandler libxmlSAXParser = {
    0, // internalSubset
    0, // isStandalone
    0, // hasInternalSubset
    0, // hasExternalSubset
    0, // resolveEntity
    0, // getEntity
    0, // entityDecl
    0, // notationDecl
    0, // attributeDecl
    0, // elementDecl
    0, // unparsedEntityDecl
    0, // setDocumentLocator
    0, // startDocument
    0, // endDocument
    (startElementSAXFunc)libxmlStartElementHandler, // startElement
    (endElementSAXFunc)libxmlEndElementHandler, // endElement
    0, // reference
    (charactersSAXFunc)libxmlCharacterDataHandler, // characters
    0, // ignorableWhitespace
    0, // processingInstruction
    (commentSAXFunc)libxmlCommentHandler, // comment
    (warningSAXFunc)libxmlWarningHandler, // warning
    (errorSAXFunc)libxmlErrorHandler, // error
    (fatalErrorSAXFunc)libxmlFatalErrorHandler, // fatalError
};

static xmlParserCtxtPtr ctxt;

bool SAXParser::parse(const char *filename)
{
    LIBXML_TEST_VERSION

    printf("\n*** WARNING: Your LibXML version is too old: " LIBXML_DOTTED_VERSION
           ". DTD validation and attribute completion is currently OFF. "
           "Please upgrade to at least version 2.6.0!\n\n");

    FILE *f = fopen(filename,"r");
    if (!f)
    {
        sprintf(errortext, "Cannot open file");
        return false;
    }

    ctxt = xmlCreatePushParserCtxt(&libxmlSAXParser, saxhandler, NULL, 0, NULL);

    int n;
    char Buffer[512];
    while (0 != (n=fread(Buffer,  sizeof(char), 512, f)))
    {
        xmlParseChunk(ctxt, Buffer, n, 0);
    }

    xmlParseChunk(ctxt, Buffer, 0, 1);

    bool ok = true;
    if (!ctxt->wellFormed)
    {
        ok = false;
        sprintf(errortext, "parser error %d at line %d",
                ctxt->errNo, // TODO something better
                ctxt->input->line);
    }

    if (!ctxt->valid)
    {
        ok = false;
        sprintf(errortext, "validation error %d at line %d",
                ctxt->errNo, // TODO something better
                ctxt->input->line);
    }

    ctxt->sax = NULL;

    xmlFreeParserCtxt(ctxt);
    fclose(f);

    return ok;
}

bool SAXParser::parseContent(const char *xml)
{
    sprintf(errortext, "XML parsing of string literals is not supported with "
                       "this libXML2 version, at least version 2.6.0 is required");
    return false;
}

int SAXParser::getCurrentLineNumber()
{
    return ctxt->input->line;
}

#endif

NAMESPACE_END

