//=========================================================================
//  PARSIMUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PARSIMUTIL_H
#define __PARSIMUTIL_H


/**
 * Expects a -p<procId>,<numPartitions> command line argument, and parses it
 * into the variables passed. Throws exception on error.
 */
void getProcIdFromCommandLineArgs(int& myProcId, int& numPartitions,
                                  const char *caller);

#endif

