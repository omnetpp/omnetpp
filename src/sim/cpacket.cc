//========================================================================
//
//  CPACKET.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cPacket       : network packet
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include "macros.h"
#include "cpacket.h"

//=== registration
Register_Class(cPacket);

//=========================================================================

cPacket::cPacket(const cPacket& m) : cMessage()
{
    setName(m.name());
    operator=(m);
}

cPacket& cPacket::operator=(const cPacket& m)
{
    if (this==&m) return *this;

    cMessage::operator=(m);
    _protocol = m._protocol;
    _pdu = m._pdu;
    return *this;
}

void cPacket::info(char *buf)
{
    cMessage::info(buf);
}

