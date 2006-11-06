//==========================================================================
//  INDEX.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Defines modules for documentation generation. Nothing useful for the
//  C++ compiler.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


/**
 * @mainpage \opp API Reference
 *
 * If you are new to \opp, a good starting point for browsing the
 * documentation is cSimpleModule.
 *
 * Otherwise, pick one of the following categories, or choose from the
 * links at the top of this page:
 *
 * - @ref SimCore
 * - @ref Containers
 * - @ref RandomNumbers
 * - @ref Statistics
 * - @ref SimSupport
 * - @ref Envir
 * - @ref EnumsTypes
 * - @ref Functions
 * - @ref Macros
 * - @ref Internals
 * - @ref EnvirExtensions
 * - @ref ParsimBrief
 *
 * If you have used the \opp before:
 * - @ref APIChanges
 */

/**
 * @page APIChanges API Changes
 *
 * @verbinclude API-changes.txt
 */


/**
 * @defgroup SimCore  Simulation core classes
 *
 * Simulation core classes:       <!-- blank line needed for autobrief=yes -->
 *
 *    - cObject is the base class for most \opp classes
 *    - cModule, cCompoundModule and cSimpleModule represent modules
 *      in the simulation. The user implements new models by subclassing
 *      cSimpleModule and overriding at least its activity() or
 *      handleMessage() member function.
 *    - cMessage represents events, and also messages sent among modules
 *    - cGate represents module gates
 *    - cPar represents module and message parameters
 *    - cSimulation stores all modules of the network and the
 *      data structure for scheduled events (the <i>future event set</i>)
 *      Most methods are used internally, but some are useful for model
 *      developers as well.
 *
 * Many other classes closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 */

/**
 * @defgroup Containers  Container classes
 *
 * Container classes:      <!-- blank line needed for autobrief=yes -->
 *
 *    - cQueue: a (priority) queue for objects derived from cObject
 *    - cArray: a dynamic array for storing objects derived from cObject
 *
 * You can also use other container classes (such as STL's vector or map),
 * especially for structs or classes not derived from cObject.
 * For cMessage (and other cObject-rooted classes) the disadvantage of
 * STL is that those container objects will not appear and will not be
 * inspectable under graphical user interfaces like Tkenv. To make them
 * inspectable, you have to wrap them into a class derived from cObject.
 *
 * Some other classes, closely related to the above ones (for example their
 * iterators) are not listed here explicitly, but you can find them via
 * 'See also' links from their main classes.
 */

/**
 * @defgroup RandomNumbers  Random number generation
 *
 * <b>Distributions</b>
 *
 * There are several functions which generate random variates
 * from different distributions: uniform, exponential, normal,
 * truncated normal, gamma, beta, Erlang, Weibull, Bernoulli,
 * binomial, geometric, Poisson, and several more.
 *
 * The functions rely on the random number generator described below.
 *
 * The documentation of individual functions includes the generation method
 * it uses. The description may refer to one of the following publications:
 *
 * LawKelton: A.M. Law and W.D. Kelton, Simulation Modeling and Analysis,
 * 3rd ed., McGraw Hill, 2000.
 *
 * Banks: J. Banks: Handbook of Simulation, Wiley, 1998.
 *
 * I have also found the web site
 * <a href="http://www.xycoon.com">http://www.xycoon.com</a> extremely useful.
 *
 * <b>Random number generators</b>
 *
 * \opp provides several random number generators (streams) and several
 * random number generator algorithms (default is cMersenneTwister).
 * RNGs can be configured in omnetpp.ini.
 *
 * RNGs are made available via the cRNG interface, and the cModule::rng()
 * method. All functions returning random variates, etc. internally
 * call cModule::rng() and cRNG::intRand(), cRNG::doubleRand().
 */

/**
 * @defgroup Statistics  Statistical data collection
 *
 * \opp provides a variety of statistical classes. There are basic classes
 * which compute basic statistics like mean and standard deviation,
 * some classes deal with density estimation, and other classes support
 * automatic detection of the end of a transient, and automatic detection
 * of accuracy of collected statistics.
 *
 * The two main abstract base classes are cStatistic and cDensityEstBase.
 * Most other classes are mostly polymorphic on these two, which means
 * most functionality in subclasses is available via virtual functions
 * defined in cStatistic and cDensityEstBase.
 * The transient detection and result accuracy classes are derived from
 * the cTransientDetection and cAccuracyDetection abstract base classes.
 *
 * The classes are:
 *    - cOutVector is used to record vector simulation results (an output
 *      vector, containing <i>(time, value)</i> pairs) to file
 *    - cStdDev keeps number of samples, mean, standard deviation, minimum
 *      and maximum value etc.
 *    - cWeightedStdDev is similar to cStdDev, but accepts weighted observations.
 *      cWeightedStdDev can be used for example to calculate time average.
 *      It is the only weighted statistics class.
 *    - cLongHistogram and cDoubleHistogram are descendants of cStdDev and
 *      also keep an approximation of the distribution of the observations
 *      using equidistant (equal-sized) cell histograms.
 *    - cVarHistogram implements a histogram where cells do not need to be
 *      the same size. You can manually add the cell (bin) boundaries,
 *      or alternatively, automatically have a partitioning created where
 *      each bin has the same number of observations (or as close to that
 *      as possible).
 *    - cPSquare is a class that uses the P<sup>2</sup> algorithm by Jain
 *      and Chlamtac. The algorithm calculates quantiles without storing
 *      the observations.
 *    - cKSplit uses a novel, experimental method, based on an adaptive
 *      histogram-like algorithm.
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
 * @defgroup SimSupport  Utility classes
 *
 * Classes that make it easier to write simulation models: <!-- blank line needed for autobrief=yes -->
 *
 *    - cXMLElement makes XML configuration available for simple modules
 *    - cTopology supports routing in telecommunication or multiprocessor networks
 *    - cStringTokenizer splits up a string to words
 *    - cFSM is used to build Final State Machines
 *    - cWatch makes variables visible (inspectable) in Tkenv
 *    - cVisitor: subclass from this to traverse the tree of simulation objects
 *
 * Many other classes closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 */

/**
 * @defgroup Envir  User interface: cEnvir and ev
 */

/**
 * @defgroup EnumsTypes  Enums, types, function typedefs
 */

/**
 * @defgroup Functions  Functions
 */

/**
 * @defgroup Macros  Macros
 */

/**
 * @defgroup Internals  Internal classes
 *
 * The classes described here are used internally by the simulation kernel.
 * They are normally of very little interest to the simulation programmer.
 * Note that although these internal classes do have a documented API,
 * they may change more often than other classes, simply because
 * they aren't used in simulation models and thus backwards compatibility
 * is less important.
 *
 * Classes associated with simulation execution:
 *   - cSimulation has a single instance. It stores the network and
 *     manages simulation runs.
 *   - cScheduler is a base class for schedulers (see in @ref EnvirExtensions
 *     as well)
 *   - cMessageHeap is used internally by cSimulation as FES (Future Event Set)
 *   - cCoroutine is the engine behind activity()-based simple modules
 *   - cWatch is the class behind the WATCH() and LWATCH() macros
 *
 * Registration classes are listed below. They play the role of a central
 * registry in \opp -- each instance holds some specific piece of (static)
 * information or serves as a factory object for other objects.
 *
 * Registration objects play an important role at network build time (they
 * store information about available module, channel, etc. types and can
 * instantiate them), and for inspectors in graphical user interfaces like
 * Tkenv.
 *
 *   - cModuleInterface stores the list of gates and parameters declared for a
 *     module type
 *   - cModuleType can instantiate a module type
 *   - cChannelType can instantiate a channel type
 *   - cNetworkType can instantiate a network type (build up a network)
 *   - cFunctionType stores a pointer to a math function accessible from NED
 *   - cClassRegister can instantiate an object type (it is the class behind
 *     the createOne() function)
 *   - cClassDescriptor provides a generic way to access data in a struct
 *     or class (somewhat analogous to Java reflection)
 *   - cEnum maps enum values to their string representations and vica versa
 *
 * Some other classes, closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 */

/**
 * @defgroup EnvirExtensions  Extension interface to Envir
 *
 * Classes in this group provide a plugin mechanism that can be used to
 * customize the functionality of the Envir user interface library
 * (and also Cmdenv and Tkenv, because they build on Envir).
 *
 * To customize, subclass from the classes below, and select the new class
 * in <tt>omnetpp.ini</tt>, using the <tt>rng-class=</tt>,
 * <tt>outputvectormanager-class=</tt>, <tt>outputscalarmanager-class=</tt>,
 * etc. config entries.
 */

/**
 * @defgroup ParsimBrief  Parallel simulation extension
 *
 * These classes are used with the parallel simulation feature.
 * For more information, please see the separate Parallel Simulation API
 * which is generated from the source files in <tt>src/sim/parsim</tt>
 * directory.
 */
