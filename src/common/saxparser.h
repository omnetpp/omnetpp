//==========================================================================
//  SAXPARSER.H - part of
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

#ifndef __OMNETPP_COMMON_SAXPARSER_H
#define __OMNETPP_COMMON_SAXPARSER_H

#include <cstdio>
#include "commondefs.h"

namespace omnetpp {
namespace common {

class SaxParser;

/**
 * @brief Base class for SAX event handlers needed by SaxParser.
 */
class COMMON_API SaxHandler
{
  protected:
    SaxParser *parser = nullptr;

  public:
    /**
     * Virtual destructor
     */
    virtual ~SaxHandler() {}

    /**
     * Called by the parser.
     */
    virtual void setParser(SaxParser *parser) { this->parser = parser; }

    /**
     * Called by the parser on SAX StartElement events.
     */
    virtual void startElement(const char *name, const char **atts) = 0;

    /**
     * Called by the parser on SAX EndElement events.
     */
    virtual void endElement(const char *name) = 0;

    /**
     * Called by the parser on SAX CharacterData events.
     */
    virtual void characterData(const char *s, int len) = 0;

    /**
     * Called by the parser on SAX ProcessingInstruction events.
     */
    virtual void processingInstruction(const char *target, const char *data) = 0;
};


/**
 * @brief Base class XML SAX parsers.
 *
 * Implementations are based on XML parsing libraries such as LibXML or
 * YXML, a tiny XML parser library.
 *
 * @ingroup XMLParser
 */
class COMMON_API SaxParser
{
  protected:
    SaxHandler *saxHandler = nullptr;

  public:
    /**
     * Constructor
     */
    SaxParser() {}

    /**
     * Destructor
     */
    virtual ~SaxParser() {}

    /**
     * Install a SAX handler into the parser.
     */
    virtual void setHandler(SaxHandler *sh) { saxHandler = sh; saxHandler->setParser(this); }

    /**
     * Parse XML input read from the given file. Methods of the SAX handler
     * will be called as the parser processes the file. Errors are signaled
     * via exceptions.
     */
    virtual void parseFile(const char *filename) = 0;

    /**
     * Parse the given string as XML content. Methods of the SAX handler
     * will be called as the parser processes the file. Errors are signaled
     * via exceptions.
     */
    virtual void parseContent(const char *content) = 0;

    /**
     * Returns the current line number in the input. Can be called from SAX handler code.
     */
    virtual int getCurrentLineNumber() = 0;
};

}  // namespace common
}  // namespace omnetpp

#endif

