//==========================================================================
//   PARSIMINDEX.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Defines modules for documentation generation. Nothing useful for the
//  C++ compiler.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/**
 * @defgroup Parsim  Parallel simulation support
 *
 * These classes are internal to the implementation of the parallel
 * simulation support in OMNeT++ -- nothing interesting here for
 * the simulation programmer.
 *
 * Parallel simulation support has a layered architecture, with the
 * following layers:
 *
 * 1. Communications layer, represented by cParsimCommunications.
 *    This encapsulates all details of message passing between parts
 *    of a program that executes in parallel, and hides details of
 *    the communications library (MPI, PVM, ...).
 *
 * 2. Partition layer, represented by cParsimPartition. This encapsulates
 *    the task of distributing the simulation model over several
 *    partitions, and handles messaging between these partitions.
 *    It relies on layer 1 for this.
 *
 * 3. Synchronization layer, represented by cParsimSynchronizer.
 *    It encapsulates the different parallel simulation algorithms
 *    like the conservative null message algorithm. This layer
 *    heavily cooperates with the message scheduler of the simulation.
 *
 * See corresponding classes for more information.
 */


