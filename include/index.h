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
 *    - cQueue: a (priority) queue for objects derived from cObject
 *    - cLinkedList: queue for structs and objects not derived from cObject
 *    - cArray: a dynamic array for storing objects
 *    - cBag: dynamic array for structs and objects not derived from cObject
 *
 * Naturally, you can also use your own container classes (e.g. STL).
 * The disadvantage of doing so is that those container objects will
 * not appear and will not be inspectable under graphical user interfaces
 * like Tkenv. To make them inspectable, you have to wrap them into a class
 * derived from cObject.
 *
 * Some other classes, closely related to the above ones (for example their
 * iterators) are not listed here explicitly, but you can find them via
 * 'See also' links from their main classes.
 */

/**
 * @defgroup Statistics  Statistical data collection
 *
 * OMNeT++ provides a variety of statistical classes. There are basic classes
 * which compute basic statistics like mean and standard deviation,
 * some classes deal with density estimation, and other classes support
 * automatic detection of the end of a transient, and automatic detection
 * of accuracy of collected statistics.
 *
 * The two main abstact base classes are cStatistic and cDensityEstBase.
 * Most other classes are mostly polimporphic on these two, which means
 * most functionality in subclasses is available via virtual functions
 * defined in cStatistic and cDensityEstBase.
 * The transient detection and result accuracy classes are derived from
 * the cTransientDetection and cAccuracyDetection abstract base classes.
 *
 * The classes are:
 *   - cStdDev keeps number of samples, mean, standard deviation, minimum
 *     and maximum value etc.
 *   - cWeightedStdDev is similar to cStdDev, but accepts weighted observations.
 *     cWeightedStdDev can be used for example to calculate time average.
 *     It is the only weighted statistics class.
 *   - cLongHistogram and cDoubleHistogram are descendants of cStdDev and
 *     also keep an approximation of the distribution of the observations
 *     using equidistant (equal-sized) cell histograms.
 *   - cVarHistogram implements a histogram where cells do not need to be
 *     the same size. You can manually add the cell (bin) boundaries,
 *     or alternatively, automatically have a partitioning created where
 *     each bin has the same number of observations (or as close to that
 *     as possible).
 *   - cPSquare is a class that uses the P<sup>2</sup> algorithm by Jain
 *     and Chlamtac. The algorithm calculates quantiles without storing
 *     the observations.
 *   - cKSplit uses a novel, experimental method, based on an adaptive
 *     histogram-like algorithm.
 *
 * Transient and result accuracy detection classes:
 *   - cTDExpandingWindows is a transient detection algorithm which uses
 *     the sliding window approach.
 *   - cADByStddev is a result accuracy detection algorithm which
 *     works by checking the standard deviation of the observations
 *
 * Some other classes closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
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

