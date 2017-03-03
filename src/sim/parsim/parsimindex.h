//==========================================================================
//  PARSIMINDEX.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Defines modules for documentation generation. Nothing useful for the
//  C++ compiler.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/**
 * @mainpage \opp Parallel simulation support
 *
 * This documentation describes the architecture of the distributed
 * parallel simulation implementation in \opp.
 *
 * All classes documented here are used internally -- if you are into
 * writing simulation models, there is nothing interesting for you here.
 *
 * Click the following link for details: @ref Parsim
 */

/**
 * @defgroup Parsim  Parallel simulation support
 *
 * Parallel simulation support has a layered architecture, with the
 * following layers:
 *
 *    -# Communications layer, represented by cParsimCommunications.
 *    This encapsulates all details of message passing between parts
 *    of a program that executes in parallel, and hides details of
 *    the communications library (MPI, PVM, ...). Subclasses implemented
 *    here are cMPICommunications, cNamedPipeCommunications,
 *    cFileCommunications.
 *    -# Partition layer, represented by cParsimPartition. This encapsulates
 *    the task of distributing the simulation model over several
 *    partitions, and handles messaging between these partitions.
 *    It relies on layer 1 for this.
 *    -# Synchronization layer, represented by cParsimSynchronizer.
 *    It encapsulates the different parallel simulation algorithms
 *    like the conservative null message algorithm. This layer
 *    heavily cooperates with the message scheduler of the simulation.
 *
 * See corresponding classes for more information.
 */


