//==========================================================================
//   INDEX.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Defines modules for documentation generation. Nothing useful for the
//  C++ compiler.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/**
 * @mainpage OMNeT++ API Reference
 *
 * This documentation is organized as follows:
 *
 *   - Modules: recommended starting point
 *   - Class Hierarchy
 *   - Compound List
 *   - File List
 *   - Compound Members
 *   - Compounds
 *
 * If you are just browsing the documentation to get familiar with
 * OMNeT++ in general, you should start from \b Modules.
 */


/**
 * @defgroup SimCore  Simulation core classes
 *
 * Simulation core classes:
 *    - cObject is the base class for most OMNeT++ classes
 *    - cModule, cCompoundModule and cSimpleModule represent modules in the simulation
 *    - cMessage represents events, and also messages sent among modules
 *    - cGate represents module gates
 *    - cPar represents module and message parameters
 *
 * Many other classes closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 */

/**
 * @defgroup SimSupport  Simulation supporting classes
 *
 * Classes that make it easier to write simulation models:
 *    - cPacket is a subclass of cMessage, it is used to represent packets, frames, etc. transmitted in a communication network
 *    - cOutVector is used to record vector simulation results (an output vector) to file
 *    - cTopology supports routing in telecommunication or multiprocessor networks.
 *    - cFSM is used to build Final State Machines
 *    - cWatch makes variables visible (inspectable) in Tkenv
 *    - opp_string: basic string class
 *
 * Many other classes closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 */

/**
 * @defgroup Containers  Container classes
 *
 * Container classes:
 *    - cQueue:
 *    - cLinkedList:
 *    - cArray:
 *    - cBag:
 *
 * Naturally, you can also use your own container classes (e.g. STL).
 * The disadvantage of doing so is that those container objects will
 * not appear and will not be inspectable under graphical user interfaces
 * like Tkenv (unless you wrap them into a class derived frm cObject).
 *
 * Some other classes, closely related to the above ones (for example their
 * iterators) are not listed here explicitly, but you can find them via
 * 'See also' links from their main classes.
 *
 * FIXME: must be completed.
 */

/**
 * @defgroup Statistics  Statistical data collection
 *
 * Statistical data collection classes:
 *     - cStatistic:
 *
 * Two main base classes: cStatistic and cDensityEstBase. Other classes are
 * mostly polimporphic on these two.
 *
 * Transient and result accuracy detection classes:
 *     - ...
 *
 * Some other classes, closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 *
 * FIXME: must be completed.
 */

/**
 * @defgroup Envir  User interface: cEnvir and ev
 */

/**
 * @defgroup Internals  Internal classes
 *
 * Internal classes, but have a published API!
 *
 * cSimulation.
 *
 * Registration classes.
 *
 * Some other classes, closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 *
 * FIXME: must be completed.
 */

/**
 * @defgroup ConstantsTypes  Constants, types, function typedefs
 */

/**
 * @defgroup Functions  Functions
 *
 * All sorts of functions.
 *
 * FIXME: must be completed.
 */

/**
 * @defgroup Macros  Macros
 *
 * FIXME: must be completed.
 */

