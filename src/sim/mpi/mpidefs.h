//=========================================================================
//
// MPIDEFS.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Eric Wu, 2001
//
//   Contents:
//      Error and message type constant definitions.
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2001 Eric Wu
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MPIDEFS_H
#define __MPIDEFS_H

#include <mpi.h>

// MPI rank:
#define MPIMASTER 0 // rank of master

// temperaroy defined value (will be removed!)
#define MAXSIZE 1000
#define WAIT_TIME 100000

// MPI message tags:
enum {
     MPIMSG_DATA = 1001, 
     MPIMSG_SETUP_LINK,

     MPIMSG_BROADCAST,
     MPIMSG_ACK,     
     MPIMSG_OK,

     MPIMSG_PUTMSG_ONCONSOLE,
     MPIMSG_PUTS_ONCONSOLE,
     MPIMSG_GETS_ONCONSOLE,
     MPIMSG_ASKYESNO_ONCONSOLE,

     MPIMSG_RUNNUMBER,

     MPIMSG_SIMULATION_CMSG,
     MPIMSG_SYNCPOINT,
     MPIMSG_CANCELSYNCPOINT,

     MPIMSG_REQUEST_STOPSIM,
     MPIMSG_STOPSIM

     // ---------------------- END OF MPI MESSAGES ------------//

     //     MSG_OK = 2001,
     //     MSG_STARTED,
};

#endif
