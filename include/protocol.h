//==========================================================================
//   PROTOCOL.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Protocol constants
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H


//
// THIS FILE IS EXPERIMENTAL. PLEASE GIVE FEEDBACK.
//


//
// protocols
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
// DEPRECATED -- do not use.
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
