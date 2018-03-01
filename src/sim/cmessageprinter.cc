//==========================================================================
//  CMESSAGEPRINTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/commonutil.h"
#include "common/stlutil.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cmessageprinter.h"
#include "omnetpp/regmacros.h"

using namespace omnetpp::common;

namespace omnetpp {

Register_MessagePrinter(cDefaultMessagePrinter);

static const std::string showIdColumnTag = "Show 'ID' column";
static const std::string showKindColumnTag = "Show 'Kind' column";
static const std::string showLengthColumnTag = "Show 'Length' column";

int cDefaultMessagePrinter::getScoreFor(cMessage *msg) const
{
    return 10;
}

std::set<std::string> cDefaultMessagePrinter::getSupportedTags() const
{
    return {showIdColumnTag, showKindColumnTag, showLengthColumnTag};
}

std::set<std::string> cDefaultMessagePrinter::getDefaultEnabledTags() const
{
    return {showIdColumnTag, showKindColumnTag, showLengthColumnTag};
}

std::vector<std::string> cDefaultMessagePrinter::getColumnNames(const Options *options) const
{
    std::vector<std::string> columns;

    if (contains(options->enabledTags, showIdColumnTag))
        columns.push_back("ID");

    if (contains(options->enabledTags, showKindColumnTag))
        columns.push_back("Kind");

    if (contains(options->enabledTags, showLengthColumnTag))
        columns.push_back("Length");

    return columns;
}

void cDefaultMessagePrinter::printMessage(std::ostream& os, cMessage *msg, const Options *options) const
{
    if (contains(options->enabledTags, showIdColumnTag))
        os << msg->getId() << "\t";

    if (contains(options->enabledTags, showKindColumnTag))
        os << msg->getKind() << "\t";

    if (msg->isPacket() && contains(options->enabledTags, showLengthColumnTag))
        os << ((cPacket *)msg)->getByteLength() << " bytes";
}

}  // namespace omnetpp

