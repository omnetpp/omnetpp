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
#include "omnetpp/cmessage.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cmessageprinter.h"
#include "omnetpp/regmacros.h"

using namespace omnetpp::common;

namespace omnetpp {

Register_MessagePrinter(cDefaultMessagePrinter);

int cDefaultMessagePrinter::getScoreFor(cMessage *msg) const
{
    return 10;
}

void cDefaultMessagePrinter::printMessage(std::ostream& os, cMessage *msg) const
{
    os << "id=" << msg->getId() << "  kind=" << msg->getKind();
    if (msg->isPacket()) {
        cPacket *pk = (cPacket *)msg;
        os << " length=" << pk->getByteLength() << " bytes";
    }
}

}  // namespace omnetpp

