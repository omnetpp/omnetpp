//==========================================================================
//   SAXPARSERLIBXML.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <libxml/parser.h>
#include <libxml/SAX.h>
#include "saxparser.h"


SAXParser::SAXParser()
{
    saxhandler = NULL;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxhandler = sh;
    sh->setParser(this);
}

extern "C" void xmlDebugDumpDocument(FILE * output, xmlDocPtr doc);

static void generateSAXEvents(xmlNode *node, SAXHandler *sh)
{
    switch (node->type)
    {
        case XML_ELEMENT_NODE: {
            // collect attributes for startElement()
            int numAttrs = 0;
            xmlAttr *attr;
            for (attr = node->properties; attr; attr = (xmlAttr *)attr->next)
                numAttrs++;
            const char **attrs = new const char *[2*(numAttrs+1)];
            int k = 0;
            for (attr = node->properties; attr; attr = (xmlAttr *)attr->next)
            {
                attrs[k] = (const char *) attr->name;
                attrs[k+1] = (const char *) attr->children->content; // first text node within attr
                k += 2;
            }
            attrs[k] = NULL;
            attrs[k+1] = NULL;

            printf("<%s, %d attrs>\n", node->name, numAttrs);

            // invoke startElement()
            sh->startElement((const char *)node->name, attrs);
            delete [] attrs;

            // recursive processing of children
            xmlNode *child;
            for (child = node->children; child; child = child->next)
                if (child->type == XML_ELEMENT_NODE)
                    generateSAXEvents(child, sh);

            // invoke endElement()
            printf("</%s>\n", node->name);
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
        case XML_CDATA_SECTION_NODE:
            sh->startCdataSection();
            sh->characterData((const char *)node->content,strlen((const char *)node->content));
            sh->endCdataSection();
            break;

        case XML_ENTITY_REF_NODE:
        case XML_ENTITY_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            // should not occur (see XML_PARSE_xxx options)
            break;
        default:
            break;

    }
}

static xmlParserCtxtPtr ctxt;  //FIXME very ugly -- should be class member or something...

bool SAXParser::parse(const char *filename)
{
    ctxt = xmlNewParserCtxt();
    if (!ctxt)
    {
        strcpy(errortext, "Failed to allocate parser context");
	return false;
    }

    // parse the file
    unsigned options = XML_PARSE_DTDVALID | // validate with the DTD
                       XML_PARSE_DTDATTR |  // complete default attributes from DTD
                       XML_PARSE_NOENT |    // substitute entities
                       XML_PARSE_NONET |    // forbid network access
                       XML_PARSE_NOCDATA |  // merge CDATA as text nodes
                       XML_PARSE_NOERROR |  // suppress error reports
                       XML_PARSE_NOWARNING; // suppress warning reports
    xmlDocPtr doc = xmlCtxtReadFile(ctxt, filename, NULL, options);
    // check if parsing suceeded
    if (!doc)
    {
        strcpy(errortext, "Parse error (document not well-formed)");
        xmlFreeParserCtxt(ctxt);
	return false;
    }
    if (!ctxt->valid)
    {
        strcpy(errortext, "Validation error (document does not conform to DTD)");
        xmlFreeParserCtxt(ctxt);
	xmlFreeDoc(doc);
	return false;
    }

xmlSaveFile("-", doc);
//__asm int 3;

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
    return ctxt->input->line;
}



/*-----------------------------------------------------------------------------

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

static void libxmlProcessingInstructionHandler(void *userData, const xmlChar *target, const xmlChar *data)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->processingInstruction((const char *)target,(const char *)data);
}

static void libxmlCommentHandler(void *userData, const xmlChar *data)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->comment((const char *)data);
}


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

static xmlParserCtxtPtr ctxt;  //FIXME very ugly -- should be class member or something...

bool SAXParser::parse(const char *filename)
{
    FILE *f = fopen(filename,"r");
    if (!f)
    {
        sprintf(errortext, "Cannot open file");
        return false;
    }

    ctxt = xmlCreatePushParserCtxt(&libxmlSAXParser, saxhandler, NULL, 0, NULL);

    int n;
    char Buffer[512];
    while (n=fread(Buffer,  sizeof(char), 512,  f))
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
    return ok;
}

int SAXParser::getCurrentLineNumber()
{
    return ctxt->input->line;
}

*/
