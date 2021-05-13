//==========================================================================
//  SAXPARSER_LIBXML.H - part of
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

#ifndef __OMNETPP_COMMON_SAXPARSER_LIBXML_H
#define __OMNETPP_COMMON_SAXPARSER_LIBXML_H

#ifdef WITH_LIBXML

#include <cstdio>
#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/SAX.h>

#include "saxparser.h"

namespace omnetpp {
namespace common {

class COMMON_API LibxmlSaxParser : public SaxParser
{
  protected:
    bool discardIgnorableWhiteSpace = true;
    int nodeLine;  // line number of current node
    xmlParserCtxtPtr ctxt = nullptr;

  protected:
    // allows for a common implementation of parse() and parseContent()
    void doParse(const char *filename, const char *content);
    void generateSAXEvents(xmlNode *node, SaxHandler *sh);

  public:
    LibxmlSaxParser() {}
    virtual void parseFile(const char *filename) override;
    virtual void parseContent(const char *content) override;
    virtual int getCurrentLineNumber() override;

    void setDiscardIgnorableWhitespace(bool b) {discardIgnorableWhiteSpace = b;}
    bool getDiscardIgnorableWhitespace() const {return discardIgnorableWhiteSpace;}
};

}  // namespace common
}  // namespace omnetpp

#endif //WITH_LIBXML

#endif

