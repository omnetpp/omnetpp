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

/**
 * Network packet class. It adds protocol and PDU type to cMessage.
 * The message kind must be either MK_PACKET or MK_INFO for cPackets.
 */
class SIM_API cPacket : public cMessage
{
  protected:
    short _protocol;
    short _pdu;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    explicit cPacket(const char *name=NULL, short protocol=0, short pdu=0) :
       cMessage(name,MK_PACKET) {_protocol=protocol;_pdu=pdu;}

    /**
     * Copy constructor.
     */
    cPacket (cPacket& m);

    /**
     * MISSINGDOC: cPacket:cPacket&operator=(cPacket&)
     */
    cPacket& operator=(cPacket& m);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() {return new cPacket(*this);}

    /**
     * Returns pointer to a string containing the class name, "cPacket".
     */
    virtual const char *className() const {return "cPacket";}

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cPacketIFC";}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name New attributes. */
    //@{

    /**
     * Returns the protocol ID.
     */
    short protocol()          {return _protocol;}

    /**
     * Returns the PDU type.
     */
    short pdu()               {return _pdu;}

    /**
     * Sets the protocol ID.
     */
    void setProtocol(short p) {_protocol=p;}

    /**
     * Sets the PDU type.
     */
    void setPdu(short p)      {_pdu=p;}
    //@}
};

#endif


