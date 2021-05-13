//==========================================================================
//  SAXPARSER_YXML.H - part of
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

#ifndef __OMNETPP_COMMON_SAXPARSER_YXML_H
#define __OMNETPP_COMMON_SAXPARSER_YXML_H

#include <vector>
#include <string>
#include "yxml.h"
#include "saxparser.h"

namespace omnetpp {
namespace common {

class COMMON_API YxmlSaxParser : public SaxParser
{
  protected:
    bool discardIgnorableWhitespace = true;

    static const int MEMSIZE = 4096;
    char parserMemory[MEMSIZE];
    yxml_t parserState;

    std::string lastFilename; // for error messages
    bool insideElementOpenTag = false;
    bool insideContent = false;
    bool insidePI = false;
    int fakeLineNumber = -1;

    std::string name; // element name / PI target
    int elemStartLine = -1;
    std::vector<std::string> attrs;
    std::vector<const char *> attrptrs;
    std::string stringbuf;
    std::vector<const char *> elemStack;

  protected:
    void resetState();
    void parseChunk(const char *chunk, int chunklen);
    void error(yxml_ret_t r);

  public:
    virtual void parseFile(const char *filename) override;
    virtual void parseContent(const char *content) override;
    virtual int getCurrentLineNumber() override;
    void setDiscardIgnorableWhitespace(bool b) {discardIgnorableWhitespace = b;}
    bool getDiscardIgnorableWhitespace() const {return discardIgnorableWhitespace;}
};

}  // namespace common
}  // namespace omnetpp

#endif

