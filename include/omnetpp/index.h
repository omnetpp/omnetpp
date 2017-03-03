//==========================================================================
//  INDEX.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// This file defines modules for documentation generation.
// Nothing useful for the C++ compiler.

namespace omnetpp {

/**
 * @mainpage  \opp API Reference
 * If you are new to \opp, a good starting point for browsing
 * the documentation is cSimpleModule.
 *
 * Otherwise, pick one of the following categories, or choose from the
 * links at the top of this page:
 *
 * - @ref SimCore
 * - @ref SimSupport
 * - @ref SimTime
 * - @ref Containers
 * - @ref RandomNumbers
 * - @ref Statistics
 * - @ref Canvas
 * - @ref OSG
 * - @ref FSM
 * - @ref Utilities
 * - @ref Logging
 * - @ref ExtensionPoints
 * - @ref Internals
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
 * @defgroup SimCore  Simulation Core
 *
 * @brief Classes in this group, such as cSimpleModule or cMessage, are
 * essential to writing \opp simulation models.
 *
 *    - cModule and cSimpleModule represent modules in the simulation.
 *      The user implements new modules by subclassing cSimpleModule and
 *      overriding its handleMessage() or activity() member function.
 *    - cChannel and subclasses encapulate properties of connections between modules
 *    - cMessage represents events, and also messages sent among modules
 *    - cPar represents parameters of modules and channels
 *    - cGate represents module gates (attachment points of connections)
 */

/**
 * @defgroup SimSupport  Simulation Support
 *
 * @brief Classes and other items in this group are important part of the
 * simulation kernel, but they are not of primary interest to simulation model
 * authors.
 *
 * Central classes are:
 *    - cObject and cOwnedObject are the base classes for most \opp classes
 *    - cSimulation stores the network with its mudules and channels,
 *      the future events set, and the event scheduler.
 *    - cEnvir represents the runtume environment or user interface of the
 *      simulation kernel and simulations.
 */

/**
 * @defgroup Containers  Container Classes
 *
 * @brief \opp provides container classes that are aware of the simulation
 * kernel's ownership mechanism, and also make contents inspectable in
 * runtime GUIs like Qtenv or Tkenv.
 *
 * In addition, one can also use standard C++ container classes like
 * std::vector or std::map. Note that these containers will not show up
 * in Tkenv or Qtenv inspectors unless you use the WATCH_VECTOR(), WATCH_MAP()
 * macros.
 */

/**
 * @defgroup SimTime  Simulation Time

 * @brief The simulation kernel and models use simtime_t to represent simulation
 * time. simtime_t is an alias to the SimTime class that is 64-bit fixed-point
 * representation with a globally shared exponent. This group describes simtime_t,
 * SimTime, and related types and macros.
 */

/**
 * @defgroup RandomNumbers  Random Number Generation
 *
 * <b>Distributions</b>
 *
 * The simulation library provides support for generating random variates
 * from various continuous and discrete distributions: uniform, exponential,
 * normal, truncated  normal, gamma, beta, Erlang, Weibull, Bernoulli,
 * binomial, geometric, Poisson, and more.
 *
 * Generators come in two flavours: as plain functions (taking an RNG
 * and the parameters of the distribution as arguments), and as classes
 * that subclass cRandom and encapsulate both the RNG and the parameters
 * of the distribution.
 *
 * <b>Random number generators</b>
 *
 * \opp provides several random number generators (streams) and several
 * random number generator algorithms (default is cMersenneTwister).
 * RNGs can be configured in omnetpp.ini. RNGs are made available via
 * the cRNG interface, and the cModule::getRNG() method. All functions
 * returning random variates, etc. internally call cModule::getRNG()
 * and cRNG::intRand(), cRNG::doubleRand().
 *
 * Note: The documentation of individual functions may refer to the
 * following publications as [LawKelton] or [Banks]:
 *   - [LawKelton] Simulation Modeling and Analysis, A.M. Law and W.D.
 *     Kelton, 3rd ed., McGraw Hill, 2000
 *   - [Banks] Handbook of Simulation, J. Banks, Wiley, 1998
 */

/**
 * @defgroup Statistics  Statistical Result Collection
 *
 * @brief \opp provides a variety of statistical classes. There are basic
 * classes which compute basic statistics like mean and standard deviation,
 * some classes deal with density estimation, and other classes support
 * automatic detection of the end of a transient, and automatic detection
 * of accuracy of collected statistics.
 *
 * The two main abstract base classes are cStatistic and cDensityEstBase.
 * Most other classes are mostly polymorphic on these two, which means
 * most functionality in subclasses is available via virtual functions
 * defined in cStatistic and cDensityEstBase.
 *
 * The transient detection and result accuracy classes are derived from
 * the cTransientDetection and cAccuracyDetection abstract base classes.
 *
 * The classes are:
 *    - cOutVector is used to record vector simulation results (an output
 *      vector, containing (time, value) pairs) to file
 *    - cStdDev keeps mean, standard deviation, minimum and maximum value etc.
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
 * @defgroup Canvas  Canvas (2D Graphics)
 *
 * @brief \opp provides a 2D figure-based drawing API. The central classes
 * are cFigure and cCanvas.
 *
 * One can create figure objects (derived from cFigure), and place them on
 * a canvas (cCanvas). Every module has its own canvas (cModule::getCanvas()),
 * and one may create additional ones directly.
 *
 * cFigure contains the following utility classes:
 *    - cFigure::Point, cFigure::Rectangle, cFigure::Color, cFigure::Font,
 *      cFigure::Transform, cFigure::RGBA, cFigure::Pixmap.
 *
 * Figures for grouping:
 *    - cGroupFigure - for grouping other figures, no visual appearance
 *
 * Line figures:
 *    - cLineFigure - a single line segment
 *    - cArcFigure - arc
 *    - cPolylineFigure - (smoothed) polyline
 *
 * Shape figures:
 *    - cRectangleFigure - (rounded) rectangle
 *    - cOvalFigure - circle or ellipse
 *    - cRingFigure - ring
 *    - cPieSliceFigure - pie slice
 *    - cPolygonFigure - (smoothed) polygon
 *    - cPathFigure - optionally filled path, modeled after SVG
 *
 * Text figures:
 *    - cTextFigure - text
 *    - cLabelFigure - non-zooming text
 *
 * Image figures:
 *    - cImageFigure - image loaded from the image path
 *    - cIconFigure - non-zooming image loaded from the image path
 *    - cPixmapFigure - programmatically created image
 */

/**
 * @defgroup OSG  OSG (3D Graphics)
 *
 * @brief \opp provides support for 3D visualization of simulation scenes,
 * with the help of the OpenSceneGraph and osgEarth libraries. Assemble
 * an OSG scene and give it to a cOsgCanvas for visualization.
 */

/**
 * @defgroup FSM  FSM Support
 *
 * @brief Macros and classes for writing Finite State Machines. An FSM is
 * defined with the FSM_Switch() macro.
 */

/**
 * @defgroup Signals  Signals
 *
 * @brief Simulation signals (or just signals) provide a way of publish-subscribe
 * communication for models. Signals are represented by the type simsignal_t,
 * are emitted on a module or channel using cComponent::emit(), and propagate
 * up in the module tree. At any level, one may add listeners (cIListener)
 * with cComponent::subscribe().
 *
 * The simulation kernel also utilizes signals for proving notifications about
 * model changes such as creation and deletion of modules. The corresponding
 * signals are PRE_MODEL_CHANGE and POST_MODEL_CHANGE, and details are provided
 * via subclasses of cModelChangeNotification (various cPre/cPost..Notification
 * classes).
 */

/**
 * @defgroup Utilities  Utility Classes
 *
 * @brief This group is a collection of classes and functions that make it easier
 * to write simulation models.
 */

/**
 * @defgroup Logging  Logging
 *
 * @brief \opp provides a logging mechanism for models, with support for log levels,
 * filtering, a configurable log prefix, and more.
 *
 * See EV_INFO for a starting point.
 */

/**
 * @defgroup WatchMacros  WATCH Macros
 *
 * @brief WATCH macros make normal variables show up in Tkenv/Qtenv inspectors.
 */

/**
 * @defgroup RegMacros Registration Macros
 *
 * @brief Macros for registering various classes, functions and other extension
 * items with the simulation kernel. Registration is necessary for allowing
 * instantiation by class name and attaching meta-information.
 */

/**
 * @defgroup ExtensionPoints  Extension Points
 *
 * @brief Classes in this group provide a plugin mechanism that can be used to
 * customize the functionality of the simulation kernel or the Envir user
 * interface library.
 */

/**
 * @defgroup Internals  Internal Classes
 *
 * @brief The classes described here are used internally by the simulation kernel.
 * They are normally of very little interest to the simulation programmer.
 * Note that although these internal classes do have a documented API,
 * they may change more often than other classes, simply because
 * they are not used in simulation models and thus backwards compatibility
 * is less important.
 */

/**
 * @defgroup ParsimBrief  Parallel Simulation Extension
 *
 * @brief Classes in this group belong to the parallel simulation feature.
 * For more information, please see the separate Parallel Simulation API
 * which is generated from the source files in <tt>src/sim/parsim</tt>
 * directory.
 */

} // namespace omnetpp


