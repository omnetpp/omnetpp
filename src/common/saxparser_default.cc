//==========================================================================
//  SAXPARSER_DEFAULT.CC - part of
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
#include "saxparser_default.h"
#include "saxparser_libxml.h"
#include "saxparser_yxml.h"
#include "exception.h"

namespace omnetpp {
namespace common {

static const char *ERR_MISSING_LIBXML =
        "Refusing to parse XML documents with DTDs with the embedded non-DTD-aware parser "
        "(Reason: DTD may alter document content via default-attribute completion. "
        "Recompile OMNeT++ with LibXML support or remove DOCTYPE declaration. Be aware "
        "that LibXML consumes multiple times as much memory as the embedded parser, "
        "so it may be unsuitable with large files)";

void DefaultSaxParser::parseFile(const char *filename)
{
    // The "yxml" parser does not support DTD validation, so use libXML instead
    // for documents that have one. To this end, we read the beginning of the file
    // to determine whether it contains a DOCTYPE declaration or not.

    char head[4096] = "";
    FILE *f = fopen(filename, "r");
    if (f) {
        size_t len = fread(head, 1, sizeof(head)-1, f);
        head[len] = 0;
        fclose(f);
    }

    if (containsDoctype(head)) {
#ifdef WITH_LIBXML
        LibxmlSaxParser parser;
        parser.setHandler(saxHandler);
        parser.parseFile(filename);
#else
        throw opp_runtime_error("Cannot load '%s': %s", filename, ERR_MISSING_LIBXML);
#endif
    }
    else {
        YxmlSaxParser parser;
        parser.setHandler(saxHandler);
        parser.parseFile(filename);
    }
}

void DefaultSaxParser::parseContent(const char *content)
{
    // The "yxml" parser does not support DTD validation, so use libXML instead
    // for documents that have one.

    if (containsDoctype(content)) {
#ifdef WITH_LIBXML
        LibxmlSaxParser parser;
        parser.setHandler(saxHandler);
        parser.parseContent(content);
#else
        throw opp_runtime_error("Cannot parse string '%15s...': %s", content, ERR_MISSING_LIBXML);
#endif
    }
    else {
        YxmlSaxParser parser;
        parser.setHandler(saxHandler);
        parser.parseContent(content);
    }
}

bool DefaultSaxParser::containsDoctype(const char *s)
{
    // a doctype declaration starts with "<!DOCTYPE", but that string may also occur inside of a CDATA section
    const char *cdataPos = strstr(s, "<![CDATA[");
    const char *doctypePos = strstr(s, "<!DOCTYPE ");
    return doctypePos != nullptr && (!cdataPos || doctypePos < cdataPos);

    (void)ERR_MISSING_LIBXML; // generate usage to suppress "unused variable" warning
}

}  // namespace common
}  // namespace omnetpp
