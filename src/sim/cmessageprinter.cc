//==========================================================================
//  CMESSAGEPRINTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

static const std::string SHOW_ID_COLUMN = "Show 'ID' column";
static const std::string SHOW_KIND_COLUMN = "Show 'Kind' column";
static const std::string SHOW_LENGTH_COLUMN = "Show 'Length' column";
static const std::string SHOW_DURATIONS_COLUMN = "Show 'Durations' column";
static const std::string SHOW_ISUPDATE_COLUMN = "Show 'TxUpdate?' column";
static const std::string SHOW_INFO_COLUMN = "Show 'Info' column";

int cDefaultMessagePrinter::getScoreFor(cMessage *msg) const
{
    return 10;
}

std::set<std::string> cDefaultMessagePrinter::getSupportedTags() const
{
    return {SHOW_ID_COLUMN, SHOW_KIND_COLUMN, SHOW_LENGTH_COLUMN, SHOW_DURATIONS_COLUMN, SHOW_ISUPDATE_COLUMN, SHOW_INFO_COLUMN};
}

std::set<std::string> cDefaultMessagePrinter::getDefaultEnabledTags() const
{
    return {SHOW_ISUPDATE_COLUMN, SHOW_INFO_COLUMN};
}

std::vector<std::string> cDefaultMessagePrinter::getColumnNames(const Options *options) const
{
    std::vector<std::string> columns;

    if (contains(options->enabledTags, SHOW_ID_COLUMN))
        columns.push_back("ID");

    if (contains(options->enabledTags, SHOW_KIND_COLUMN))
        columns.push_back("Kind");

    if (contains(options->enabledTags, SHOW_ISUPDATE_COLUMN))
        columns.push_back("TxUpdate?");

    if (contains(options->enabledTags, SHOW_LENGTH_COLUMN))
        columns.push_back("Length");

    if (contains(options->enabledTags, SHOW_DURATIONS_COLUMN))
        columns.push_back("Durations");

    if (contains(options->enabledTags, SHOW_INFO_COLUMN))
        columns.push_back("Info");

    return columns;
}

void cDefaultMessagePrinter::printMessage(std::ostream& os, cMessage *msg, const Options *options) const
{
    cPacket *pk = msg->isPacket() ? (cPacket *)msg : nullptr;

    if (contains(options->enabledTags, SHOW_ID_COLUMN))
        os << msg->getId() << "\t";

    if (contains(options->enabledTags, SHOW_KIND_COLUMN))
        os << msg->getKind() << "\t";

    if (contains(options->enabledTags, SHOW_ISUPDATE_COLUMN))
        os << (pk && pk->isUpdate() ? "*" : " ") << "\t";

    if (contains(options->enabledTags, SHOW_LENGTH_COLUMN)) {
        if (pk) {
            int64_t bitLength = pk->getBitLength();
            int fragBits = bitLength & 7;
            if (fragBits == 0)
                os << pk->getByteLength() << "B";
            else
                os << pk->getBitLength() << "b (" << pk->getByteLength() << "B " << fragBits << "b)";
        }
        os << "\t";
    }

    if (contains(options->enabledTags, SHOW_DURATIONS_COLUMN)) {
        if (pk) {
            os << pk->getDuration().ustr();
            if (pk->isUpdate())
                os << " (remaining " << pk->getRemainingDuration().ustr() << ")";
        }
        os << "\t";
    }

    if (contains(options->enabledTags, SHOW_INFO_COLUMN))
        os << msg->str() << "\t";

}

}  // namespace omnetpp

