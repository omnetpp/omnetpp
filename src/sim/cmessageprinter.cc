//==========================================================================
//  CMESSAGEPRINTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "commonutil.h"
#include "cmessage.h"
#include "cmessageprinter.h"
#include "regmacros.h"

NAMESPACE_BEGIN

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


NAMESPACE_END

