//==========================================================================
//   SAXPARSER.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/



#ifndef __SAXPARSER_H
#define __SAXPARSER_H

#include <stdio.h>

class SAXParser;

/**
 * Base class for SAX event handlers needed by XMLParser.
 * All event handlers provided by this class are empty, one must
 * subclass SAXHandler and redefine the event handler to make them
 * do something useful.
 *
 * @ingroup XMLParser
 */
class SAXHandler
{
    friend class SAXParser;
  protected:
    SAXParser *parser;
    virtual void setParser(SAXParser *p) {parser=p;}
  public:
    SAXHandler() {parser=0;}
    virtual ~SAXHandler() {}
    virtual void startElement(const char *name, const char **atts)  {}
    virtual void endElement(const char *name)  {}
    virtual void characterData(const char *s, int len)  {}
    virtual void processingInstruction(const char *target, const char *data)  {}
    virtual void comment(const char *data)  {}
    virtual void startCdataSection()  {}
    virtual void endCdataSection()  {}
};

/**
 * Front-end to XML parsers (non-validating, SAX parsers).
 * The current implementation uses Expat.
 * One must provide a SAXHandler for this class to be useful.
 *
 * @ingroup XMLParser
 */
class SAXParser
{
  protected:
    char errortext[512];
    SAXHandler *saxhandler;
    void *currentparser;
  public:
    SAXParser();
    void setHandler(SAXHandler *sh);
    int parse(FILE *f);
    int getCurrentLineNumber();
};

#endif

