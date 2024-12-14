//=========================================================================
//  CRECEIVEDEXCEPTION.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "creceivedexception.h"

namespace omnetpp {

cReceivedException::cReceivedException(int sourcePartitionId, const char *msg)
    : cException("Error occurred in partitionId=%d: %s", sourcePartitionId, msg)
{
}

cReceivedTerminationException::cReceivedTerminationException(int sourcePartitionId, const char *msg)
    : cTerminationException("Terminating simulation on request from partitionId=%d: %s", sourcePartitionId, msg)
{
}

}  // namespace omnetpp

