//==========================================================================
//   CPACKET.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cPacket : network packet class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPACKET_H
#define __CPACKET_H

#include "cmessage.h"

//==========================================================================
// cPacket: network packet class
//  - adds protocol and PDU type to cMessage
//  - message kind must be either MK_PACKET or MK_INFO for cPackets
//

/**
 * FIXME: 
 * cPacket: network packet class
 *  - adds protocol and PDU type to cMessage
 *  - message kind must be either MK_PACKET or MK_INFO for cPackets
 * 
 */
class SIM_API cPacket : public cMessage
{
  protected:
    short _protocol;
    short _pdu;
  public:

    /**
     * MISSINGDOC: cPacket:cPacket(char*,short,short)
     */
    explicit cPacket(const char *name=NULL, short protocol=0, short pdu=0) :

    /**
     * MISSINGDOC: cPacket:cMessage(,)
     */
       cMessage(name,MK_PACKET) {_protocol=protocol;_pdu=pdu;}

    /**
     * MISSINGDOC: cPacket:cPacket(cPacket&)
     */
    cPacket (cPacket& m);

    /**
     * MISSINGDOC: cPacket:cObject*dup()
     */
    virtual cObject *dup() {return new cPacket(*this);}

    /**
     * MISSINGDOC: cPacket:char*className()
     */
    const char *className() const {return "cPacket";}

    /**
     * MISSINGDOC: cPacket:cPacket&operator=(cPacket&)
     */
    cPacket& operator=(cPacket& m);

    /**
     * MISSINGDOC: cPacket:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cPacketIFC";}

    /**
     * MISSINGDOC: cPacket:void info(char*)
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cPacket:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cPacket:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    /**
     * FIXME: new functions
     */
    short protocol()          {return _protocol;}

    /**
     * MISSINGDOC: cPacket:short pdu()
     */
    short pdu()               {return _pdu;}

    /**
     * MISSINGDOC: cPacket:void setProtocol(short)
     */
    void setProtocol(short p) {_protocol=p;}

    /**
     * MISSINGDOC: cPacket:void setPdu(short)
     */
    void setPdu(short p)      {_pdu=p;}
};

#endif
