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
//  g_logv("XML", G_LOG_LEVEL_WARNING, msg, args);
  va_end(args);
}

static void libxmlErrorHandler(void *userData, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
//  g_logv("XML", G_LOG_LEVEL_CRITICAL, msg, args);
  va_end(args);
}

static void libxmlFatalErrorHandler(void *userData, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
//  g_logv("XML", G_LOG_LEVEL_ERROR, msg, args);
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
    0, /* internalSubset */
    0, /* isStandalone */
    0, /* hasInternalSubset */
    0, /* hasExternalSubset */
    0, /* resolveEntity */
    0, /* getEntity */
    0, /* entityDecl */
    0, /* notationDecl */
    0, /* attributeDecl */
    0, /* elementDecl */
    0, /* unparsedEntityDecl */
    0, /* setDocumentLocator */
    0, /* startDocument */
    0, /* endDocument */
    (startElementSAXFunc)libxmlStartElementHandler, /* startElement */
    (endElementSAXFunc)libxmlEndElementHandler, /* endElement */
    0, /* reference */
    (charactersSAXFunc)libxmlCharacterDataHandler, /* characters */
    0, /* ignorableWhitespace */
    0, /* processingInstruction */
    (commentSAXFunc)libxmlCommentHandler, /* comment */
    (warningSAXFunc)libxmlWarningHandler, /* warning */
    (errorSAXFunc)libxmlErrorHandler, /* error */
    (fatalErrorSAXFunc)libxmlFatalErrorHandler, /* fatalError */
};

xmlParserCtxtPtr ctxt;  //FIXME very ugly -- should be class member or something...

bool SAXParser::parse(FILE *f)
{
    ctxt = xmlCreatePushParserCtxt(&libxmlSAXParser, saxhandler, NULL, 0, NULL);

    int n;
    char Buffer[512];
    while (n=fread(Buffer,  sizeof(char), 512,  f))
    {
        xmlParseChunk(ctxt, Buffer, n, 0);
    }

    /* to get an endDocument event we have to send EOF */
    Buffer[0]=EOF;
    xmlParseChunk(ctxt, Buffer, 10, 0);

    bool ok = true;
    if (!ctxt->wellFormed)
    {
        ok = false;
        sprintf(errortext, "parser error %d at line %d",
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

