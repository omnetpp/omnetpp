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
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CRECEIVEDEXCEPTION_H
#define __OMNETPP_CRECEIVEDEXCEPTION_H

#include "omnetpp/cexception.h"

namespace omnetpp {

/**
 * @brief Represents an exception that has been received from other partitions.
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
 * @brief Represents a termination exception that has been received from other
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

}  // namespace omnetpp


#endif



