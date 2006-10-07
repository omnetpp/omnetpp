//=========================================================================
//  CRECEIVEDEXCEPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "creceivedexception.h"

cReceivedException::cReceivedException(int sourceProcId, const char *msg)
 : cException("Error occurred in procId=%d: %s", sourceProcId, msg)
{
}

cReceivedTerminationException::cReceivedTerminationException(int sourceProcId, const char *msg)
 : cTerminationException("Terminating simulation on request from procId=%d: %s", sourceProcId, msg)
{
}



