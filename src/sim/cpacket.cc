//========================================================================
//  CPACKET.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cPacket       : network packet
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include "macros.h"
#include "cpacket.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

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

std::string cPacket::info() const
{
    return cMessage::info();
}

void cPacket::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cMessage::netPack(buffer);

    buffer->pack(_protocol);
    buffer->pack(_pdu);
#endif
}

void cPacket::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cMessage::netUnpack(buffer);

    buffer->unpack(_protocol);
    buffer->unpack(_pdu);
#endif
}


