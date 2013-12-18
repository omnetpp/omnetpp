//=========================================================================
//  CRECEIVEDEXCEPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "creceivedexception.h"

NAMESPACE_BEGIN

cReceivedException::cReceivedException(int sourceProcId, const char *msg)
 : cException("Error occurred in procId=%d: %s", sourceProcId, msg)
{
}

cReceivedTerminationException::cReceivedTerminationException(int sourceProcId, const char *msg)
 : cTerminationException("Terminating simulation on request from procId=%d: %s", sourceProcId, msg)
{
}

NAMESPACE_END

