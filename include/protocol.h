//==========================================================================
//   PROTOCOL.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Protocol and PDU type enums for cPacket
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H


//
// The cPacket class is intended to be the base class for models of
// network packets/frames. cPacket has a protocol() and a pdu() field.
//
// This file is an initial try to collect protocols and frame/packet
// types and assign ids to them, for use with cPacket. The aim is
// to standardize on protocol models.
//
// Hopefully these enum lists will grow as people develop models and
// send in new protocol codes.
//
//
// THE cPacket CLASS AS WELL AS THIS FILE IS EXPERIMENTAL.
// PLEASE GIVE SOME FEEDBACK.
//


//
// protocols
//
// Protocol codes are now grouped by the OSI seven layers. I know OSI is
// kind of obsolete, but does anyone know a better scheme?
// Or better, a whole complete list of protocols with assigned numbers
// (like the list protocol codes that identify the embedded protocol
// in an ethernet frame). Write if you have an idea!
//
enum {

   // physical
   // starting at 1000

   // data-link
   PR_ETHERNET = 2000,
   PR_FDDI,
   PR_TOKENRING,
   PR_LAPB,
   PR_PPP,
   PR_WETHERNET,

   // network
   PR_IP = 3000,
   PR_ICMP,
   PR_IPV6,
   PR_CLNP,
   PR_X25,

   // transport
   PR_TCP = 4000,
   PR_UDP,
   PR_TP4,

   // session
   // starting at 5000

   // presentation
   PR_SSL = 6000,

   // application
   PR_TELNET = 7000,
   PR_HTTP,
   PR_SMTP,
   PR_POP3,
   PR_UDP_APP,
   PR_NFS,
   PR_XWINDOW

   // ATM
   // starting at 8000

   // ...
};


//
// PDU (frame,packet,etc) types
//
// The enum names should reflect the PURPOSE of the frame/packet type and
// not the specific name in one protocol. For example, I prefer
// PDU_CONNECT_REQUEST to PDU_LAPB_SABME or PDU_TP4_CR.
//
// Using generic PDU type names will make it easier to write more general
// and easier-to-reuse protocol models.
//
// Additions, corrections welcome!
//
enum {

   PDU_CONNECT_REQUEST,
   PDU_CONNECT_CONFIRM,
   PDU_DATA,
   PDU_EXPEDITED_DATA,
   PDU_ACK,
   PDU_DISCONNECT_REQUEST,
   PDU_DISCONNECT_CONFIRM
   //...
};

#endif
