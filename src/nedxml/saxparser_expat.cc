//==========================================================================
//  SAXPARSER.CC - part of
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

#include <cstring>
#include <cstdio>
#include <cassert>
#include <cstdio>
#include "expat.h"
#include "saxparser.h"

namespace omnetpp {
namespace nedxml {


static bool hasDTD;

static void expatStartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->startElement(name, atts);
}

static void expatEndElementHandler(void *userData, const XML_Char *name)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->endElement(name);
}

static void expatCharacterDataHandler(void *userData, const XML_Char *s, int len)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->characterData(s, len);
}

static void expatProcessingInstructionHandler(void *userData, const XML_Char *target, const XML_Char *data)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->processingInstruction(target, data);
}

static void expatCommentHandler(void *userData, const XML_Char *data)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->comment(data);
}

static void expatStartCdataSectionHandler(void *userData)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->startCdataSection();
}

static void expatEndCdataSectionHandler(void *userData)
{
    SAXHandler *sh = (SAXHandler *)userData;
    sh->endCdataSection();
}

static void expatStartDoctypeDeclHandler(void *userData,
                                         const XML_Char *, // doctypeName
                                         const XML_Char *, // sysid
                                         const XML_Char *, // pubid
                                         int)              // has_internal_subset
{
    hasDTD = true;
}

static XML_Parser expatCreateParser(void *userData)
{
    // note: XML_Parser is actually a pointer to a dynamically allocated struct
    XML_Parser parser = XML_ParserCreate(nullptr);
    XML_SetUserData(parser, userData);
    XML_SetStartElementHandler(parser, expatStartElementHandler);
    XML_SetEndElementHandler(parser, expatEndElementHandler);
    XML_SetCharacterDataHandler(parser, expatCharacterDataHandler);
    XML_SetProcessingInstructionHandler(parser, expatProcessingInstructionHandler);
    XML_SetCommentHandler(parser, expatCommentHandler);
    XML_SetStartCdataSectionHandler(parser, expatStartCdataSectionHandler);
    XML_SetEndCdataSectionHandler(parser, expatEndCdataSectionHandler);
    XML_SetStartDoctypeDeclHandler(parser, expatStartDoctypeDeclHandler);
    return parser;
}

SAXParser::SAXParser()
{
    saxHandler = nullptr;
}

void SAXParser::setHandler(SAXHandler *sh)
{
    saxHandler = sh;
    sh->setParser(this);
}

bool SAXParser::parse(const char *filename)
{
    // open file
    FILE *f = fopen(filename, "r");
    if (!f) {
        sprintf(errorText, "Cannot open file");
        return false;
    }

    // prepare parser
    XML_Parser parser = expatCreateParser(saxHandler);
    currentParser = &parser;

    // read file chunk-by-chunk and parse it
    char buf[BUFSIZ];
    bool done;
    bool err = false;
    hasDTD = false;
    do {
        size_t len = fread(buf, 1, sizeof(buf), f);
        done = len < sizeof(buf);
        if (!XML_Parse(parser, buf, len, done)) {
            sprintf(errorText, "Parse error: %s at line %ld",
                    XML_ErrorString(XML_GetErrorCode(parser)),
                    (long)XML_GetCurrentLineNumber(parser));
            err = true;
            done = true;
        }
        if (hasDTD) {
            sprintf(errorText, "Cannot validate document and complete default attributes "
                               "from DTD, because underlying parser (Expat) does not support it. "
                               "Recompile OMNeT++ with another parser (LibXML), or remove DOCTYPE from "
                               " %s",
                    filename);
            err = true;
            done = true;
        }
    } while (!done);

    // cleanup and return
    XML_ParserFree(parser);
    fclose(f);
    return !err;
}

bool SAXParser::parseContent(const char *content)
{
    // prepare parser
    XML_Parser parser = expatCreateParser(saxHandler);
    currentParser = &parser;

    bool err = false;
    hasDTD = false;

    if (!XML_Parse(parser, content, strlen(content), true)) {
        sprintf(errorText, "Parse error: %s at line %llu",
                XML_ErrorString(XML_GetErrorCode(parser)),
                XML_GetCurrentLineNumber(parser));
        err = true;
    }
    if (hasDTD) {
        sprintf(errorText, "Cannot validate document and complete default attributes "
                           "from DTD, because underlying parser (Expat) does not support it. "
                           "Recompile OMNeT++ with another parser (LibXML), or remove DOCTYPE from "
                           "all XML content");
        err = true;
    }

    // cleanup and return
    XML_ParserFree(parser);
    return !err;
}

bool SAXParser::doParse(const char *filename, const char *content)
{
    // function not used with Expat
    return false;
}

int SAXParser::getCurrentLineNumber()
{
    return XML_GetCurrentLineNumber(*(XML_Parser *)currentParser);
}

}  // namespace nedxml
}  // namespace omnetpp

