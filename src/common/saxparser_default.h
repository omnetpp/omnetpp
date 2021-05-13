//==========================================================================
//  SAXPARSER_DEFAULT.H - part of
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

#ifndef __OMNETPP_COMMON_SAXPARSER_DEFAULT_H
#define __OMNETPP_COMMON_SAXPARSER_DEFAULT_H

#include <vector>
#include <string>
#include "saxparser.h"

namespace omnetpp {
namespace common {

class COMMON_API DefaultSaxParser : public SaxParser
{
  protected:
    bool containsDoctype(const char *s);
  public:
    virtual void parseFile(const char *filename) override;
    virtual void parseContent(const char *content) override;
    virtual int getCurrentLineNumber() override {return -1;} // unused
};

}  // namespace common
}  // namespace omnetpp

#endif

