//==========================================================================
// exception.cc -
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

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include "common/commonutil.h"
#include "exception.h"
#include "astnode.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

#define BUFLEN    1024

NedException::NedException(const char *messagefmt...) : std::runtime_error("")
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    msg = message;
}

NedException::NedException(ASTNode *context, const char *messagefmt...) : std::runtime_error("")
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);

    std::string loc = context ? context->getSourceLocation().str() : "";
    if (!loc.empty())
        msg = std::string(message) + ", at " + loc;
    else if (context)
        msg = std::string(context->getTagName()) + ": " + message;
    else
        msg = message;
}

}  // namespace nedxml
}  // namespace omnetpp

