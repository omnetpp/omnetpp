//=========================================================================
//  CRECEIVEDEXCEPTION.H - part of
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
  Copyright (C) 2003-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CRECEIVEDEXCEPTION_H
#define __OMNETPP_CRECEIVEDEXCEPTION_H

#include "omnetpp/cexception.h"

NAMESPACE_BEGIN

/**
 * Represents an exception that has been received from other partitions.
 *
 * @ingroup Parsim
 */
class cReceivedException : public cException
{
  public:
    /**
     * Constructor.
     */
    cReceivedException(int sourceProcId, const char *msg);
};

/**
 * Represents a termination exception that has been received from other
 * partitions.
 *
 * @ingroup Parsim
 */
class cReceivedTerminationException : public cTerminationException
{
  public:
    /**
     * Constructor.
     */
    cReceivedTerminationException(int sourceProcId, const char *msg);
};

NAMESPACE_END


#endif



