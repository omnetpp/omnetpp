//=========================================================================
//
// PVMDEFS.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Zoltan Vass, 1996
//
//   Contents:
//      Error and message type constant definitions.
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// PVM message tags:
enum {
     MSG_OK = 2001,
     MSG_STARTED,
     MSG_BROADCAST_TIDS,

     MSG_RUNNUMBER,
     MSG_SETUP_LINKS,

     MSG_SIMULATION_CMSG,
     MSG_SYNCPOINT,
     MSG_CANCELSYNCPOINT,

     MSG_PUTMSG_ONCONSOLE,
     MSG_PUTS_ONCONSOLE,
     MSG_GETS_ONCONSOLE,
     MSG_ASKYESNO_ONCONSOLE,

     MSG_REQUEST_STOPSIM,
     MSG_STOPSIM
};
