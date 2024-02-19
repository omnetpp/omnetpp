//==========================================================================
//  SAXPARSER_YXML.CC - part of
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

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "saxparser_yxml.h"
#include "stringutil.h"
#include "exception.h"

namespace omnetpp {
namespace common {

void YxmlSaxParser::parseFile(const char *filename)
{
    // setup
    lastFilename = filename;
    resetState();

    FILE *f = fopen(filename, "r");
    if (!f)
        throw opp_runtime_error("Cannot open file: '%s'", filename);

    // read file chunk-by-chunk and parse it
    char buf[4096];
    size_t len;
    while ((len = fread(buf, 1, sizeof(buf), f)) > 0)
        parseChunk(buf, len);

    // finalize
    yxml_ret_t r = yxml_eof(&parserState);
    if (r != YXML_OK)
        error(r);
}

void YxmlSaxParser::parseContent(const char *content)
{
    lastFilename = "string-content";
    resetState();
    parseChunk(content, strlen(content));
    yxml_ret_t r = yxml_eof(&parserState);
    if (r != YXML_OK)
        error(r);
}

int YxmlSaxParser::getCurrentLineNumber()
{
    return fakeLineNumber >= 0 ? fakeLineNumber : parserState.line;
}

void YxmlSaxParser::resetState()
{
    yxml_init(&parserState, parserMemory, MEMSIZE);
    insideElementOpenTag = insideContent = insidePI = false;
    fakeLineNumber = -1;
    name.clear();
    elemStartLine = -1;
    attrs.clear();
    attrptrs.clear();
    stringbuf.clear();
    elemStack.clear();
}

void YxmlSaxParser::parseChunk(const char *chunk, int chunklen)
{
    yxml_t *x = &parserState;
    const char *p = chunk;
    const char *endp = chunk+chunklen;

    while (p < endp) {
        char ch = *p++;
        yxml_ret_t code = yxml_parse(x, ch);
        if (code == YXML_OK)
            continue;
        if (code < 0)
            error(code);

        if (insideElementOpenTag && code != YXML_ATTRSTART && code != YXML_ATTRVAL && code != YXML_ATTREND) {
            attrptrs.resize(attrs.size()+1);
            for (size_t i = 0; i < attrs.size(); i++)
                attrptrs[i] = attrs[i].c_str();
            attrptrs[attrs.size()] = nullptr;
            fakeLineNumber = elemStartLine;
            saxHandler->startElement(name.c_str(), attrptrs.data());
            fakeLineNumber = -1;
            stringbuf.clear();
            attrs.clear();
            attrptrs.clear();
            insideElementOpenTag = false;
        }
        else if (insideContent && code != YXML_CONTENT) {
            if (!discardIgnorableWhitespace || !opp_isblank(stringbuf.c_str()))
                saxHandler->characterData(stringbuf.c_str(), stringbuf.size());
            stringbuf.clear();
            insideContent = false;
        }
        else if (insidePI && code != YXML_PICONTENT && code != YXML_PIEND) {
            saxHandler->processingInstruction(name.c_str(), stringbuf.c_str());
            name.clear();
            stringbuf.clear();
            insidePI = false;
        }

        switch (code) {
            case YXML_OK:
                break;
            case YXML_ELEMSTART:
                insideElementOpenTag = true;
                name = x->elem;
                elemStartLine = x->line;
                elemStack.push_back(x->elem);
                assert(attrs.empty());
                break;
            case YXML_ATTRSTART:
                assert(stringbuf.empty());
                break;
            case YXML_ATTRVAL:
                stringbuf.append(x->data);
                break;
            case YXML_ATTREND:
                attrs.push_back(x->attr);
                attrs.push_back(std::string());
                attrs.back() = std::move(stringbuf);
                assert(stringbuf.empty());
                break;
            case YXML_ELEMEND:
                saxHandler->endElement(elemStack.back());
                elemStack.pop_back();
                break;
            case YXML_CONTENT:
                stringbuf.append(x->data);
                insideContent = true;
                break;
            case YXML_PISTART:
                name = x->pi;
                insidePI = true;
                break;
            case YXML_PICONTENT:
                stringbuf.append(x->data);
                break;
            case YXML_PIEND:
                break;
            default:
                break;
        }
    }
}

void YxmlSaxParser::error(yxml_ret_t r)
{
    const char *msg;
    switch(r) {
        case YXML_EEOF: msg = "Unexpected end of document"; break;
        case YXML_EREF: msg = "Invalid character reference or entity reference"; break;
        case YXML_ECLOSE: msg = "Close tag does not match open tag"; break;
        case YXML_ESTACK: msg = "Parse buffer too small (document is too deeply nested or an element name, attribute name or PI target is too long)"; break;
        case YXML_ESYN: msg = "Syntax error"; break;
        default: msg = "Unknown error code"; break;
    }
    throw opp_runtime_error("Parse error: %s at %s:%u", msg, lastFilename.c_str(), (unsigned)parserState.line);
}

}  // namespace common
}  // namespace omnetpp
