//=========================================================================
//  PARSIMUTIL.H - part of
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

#ifndef __PARSIMUTIL_H
#define __PARSIMUTIL_H

NAMESPACE_BEGIN

/**
 * Expects a -p<procId>,<numPartitions> command line argument, and parses it
 * into the variables passed. Throws exception on error.
 */
void getProcIdFromCommandLineArgs(int& myProcId, int& numPartitions,
                                  const char *caller);

NAMESPACE_END

#endif

