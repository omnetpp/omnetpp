//=========================================================================
//  CRECEIVEDEXCEPTION.H - part of
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

#ifndef __CRECEIVEDEXCEPTION_H__
#define __CRECEIVEDEXCEPTION_H__

#include "cexception.h"

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

#endif



