//==========================================================================
//  SAXPARSERLIBXML.CC - part of
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

#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/SAX.h>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <cstdio>
#include <cstdarg>
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "saxparser.h"

namespace omnetpp {
namespace nedxml {

using namespace omnetpp::common;

//
// Validating is supperted only if LibXML is at least 2.6.0
//
#if LIBXML_VERSION < 20600
#error At least libXML2 v2.6 is required for XML DTD validation support.
#endif

SAXParser::SAXParser()
{
    saxHandler = nullptr;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxHandler = sh;
    sh->setParser(this);
}

static int nodeLine;  // line number of current node
static xmlParserCtxtPtr ctxt;  // parser context

// static void dontPrintError(void *, xmlErrorPtr) {} // an xmlStructuredErrorFunc

static void generateSAXEvents(xmlNode *node, SAXHandler *sh)
{
    nodeLine = node->line;
    switch (node->type) {
        case XML_ELEMENT_NODE: {
            // collect attributes for startElement()
            int numAttrs = 0;
            xmlAttr *attr;
            for (attr = node->properties; attr; attr = (xmlAttr *)attr->next)
                numAttrs++;
            const char **attrs = new const char *[2*(numAttrs+1)];
            int k;
            for (attr = node->properties, k = 0; attr; attr = (xmlAttr *)attr->next, k += 2) {
                // ignore namespaces: pass "prefix:name" to SAX handler
                if (attr->ns) {
                    attrs[k] = new char[strlen((const char *)attr->name)+strlen((const char *)attr->ns->prefix)+2];
                    sprintf((char *)attrs[k], "%s:%s", attr->ns->prefix, attr->name);
                }
                else {
                    attrs[k] = (const char *)attr->name;
                }
                attrs[k+1] = (const char *)attr->children->content;  // first text node within attr
            }
            attrs[k] = nullptr;
            attrs[k+1] = nullptr;

            // element name. ignore namespaces: pass "prefix:name" to SAX handler
            char *nodename;
            if (node->ns) {
                nodename = new char[strlen((const char *)node->name)+strlen((const char *)node->ns->prefix)+2];
                sprintf(nodename, "%s:%s", node->ns->prefix, node->name);
            }
            else {
                nodename = (char *)node->name;
            }

            // invoke startElement()
            sh->startElement(nodename, attrs);

            // dealloc prefixed attr names and element name
            for (attr = node->properties, k = 0; attr; attr = (xmlAttr *)attr->next, k += 2)
                if (attr->ns)
                    delete[] (char *)attrs[k];

            delete[] attrs;
            if (node->ns)
                delete[] nodename;

            // recursive processing of children
            xmlNode *child;
            for (child = node->children; child; child = child->next)
                generateSAXEvents(child, sh);

            // invoke endElement()
            sh->endElement((const char *)node->name);
            break;
        }

        case XML_TEXT_NODE:
            sh->characterData((const char *)node->content, strlen((const char *)node->content));
            break;

        case XML_PI_NODE:
            // ignored: sh->processingInstruction((const char *)target,(const char *)data);
            break;

        case XML_COMMENT_NODE:
            sh->comment((const char *)node->content);
            break;

        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            break;  // ignore

        case XML_CDATA_SECTION_NODE:
        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_ATTRIBUTE_NODE:
            // should not occur (see XML_PARSE_xxx options)
            fprintf(stderr, "ERROR: libxml wrapper: generateSAXEvents(): node type %d unexpected\n", node->type);
            assert(0);
            break;

        default:
            // DTD stuff: ignore
            break;
    }
}

bool SAXParser::parse(const char *filename)
{
    return doParse(filename, nullptr);
}

bool SAXParser::parseContent(const char *content)
{
    return doParse(nullptr, content);
}

bool SAXParser::doParse(const char *filename, const char *content)
{
    assert((filename == nullptr) != (content == nullptr));  // exactly one of them is non-nullptr
    strcpy(errorText, "<error msg unfilled>");

    if (filename && !fileExists(filename)) {
        sprintf(errorText, "File not found");
        return false;
    }

    //
    // When there's a DTD given, we *must* use it, and complete default attrs from it.
    //
    // Strategy: build DOM tree with validation enabled, then generate SAX events
    // from it. LibXML's SAX2-based validation code isn't robust enough yet
    // (as of 09/2004)
    //
    ctxt = xmlNewParserCtxt();
    if (!ctxt) {
        strcpy(errorText, "Failed to allocate parser context");
        return false;
    }

    // parse the file
    unsigned options = XML_PARSE_DTDVALID  // validate with the DTD (but we'll have to revalidate after XInclude processing anyway)
        |XML_PARSE_DTDATTR  // complete default attributes from DTD
        |XML_PARSE_NOENT  // substitute entities
        |XML_PARSE_NONET  // forbid network access
        |XML_PARSE_NOBLANKS  // discard ignorable white space
        |XML_PARSE_NOCDATA  // merge CDATA as text nodes
        |XML_PARSE_NOERROR  // suppress error reports
        |  // XML_PARSE_XINCLUDE |  // would be nice, but does not work
        XML_PARSE_NOWARNING;  // suppress warning reports

    xmlDocPtr doc;
    if (filename)
        doc = xmlCtxtReadFile(ctxt, filename, nullptr, options);
    else
        doc = xmlCtxtReadMemory(ctxt, content, strlen(content), "string-literal", nullptr, options);

    // check if parsing succeeded
    if (!doc) {
        sprintf(errorText, "Parse error: %s at line %s:%d",
                opp_trim(ctxt->lastError.message).c_str(), ctxt->lastError.file, ctxt->lastError.line);
        xmlFreeParserCtxt(ctxt);
        return false;
    }

    // perform XInclude substitution. Note: errors will be dumped on stderr (these messages
    // cannot be captured via the public libXML2 API, as xmlXIncludeCtxt doesn't have
    // error/warning function ptrs as other ctxts)
    // xmlStructuredError = dontPrintError;
    int xincludeResult = xmlXIncludeProcess(doc);
    if (xincludeResult == -1) {  // error
        sprintf(errorText, "XInclude substitution error");  // further details unavailable from libXML without much pain
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

    if (hasDTD) {
        xmlValidCtxtPtr vctxt = xmlNewValidCtxt();
        vctxt->userData = errorText;
        vctxt->error = (xmlValidityErrorFunc)sprintf;
        vctxt->warning = (xmlValidityWarningFunc)sprintf;

        if (!xmlValidateDocument(vctxt, doc)) {
            strcpy(errorText, opp_trim(errorText).c_str());
            xmlFreeValidCtxt(vctxt);
            xmlFreeParserCtxt(ctxt);
            xmlFreeDoc(doc);
            return false;
        }
        xmlFreeValidCtxt(vctxt);
    }

    // traverse tree and generate SAX events
    xmlNode *root = xmlDocGetRootElement(doc);
    generateSAXEvents(root, saxHandler);

    // free parser context and document tree
    xmlFreeParserCtxt(ctxt);
    xmlFreeDoc(doc);
    return true;
}

int SAXParser::getCurrentLineNumber()
{
    return nodeLine;
}


}  // namespace nedxml
}  // namespace omnetpp

