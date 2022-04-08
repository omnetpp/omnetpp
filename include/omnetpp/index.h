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
 * the documentation is cSimpleModule. Alternatively, start
 * browsing one of the following categories:
 *
 * - @ref Fundamentals
 * - @ref ModelComponents
 * - @ref SimProgr
 * - @ref SimCore
 * - @ref SimTime
 * - @ref RandomNumbers
 * - @ref Statistics
 * - @ref ResultFiltersRecorders
 * - @ref Expressions
 * - @ref Canvas
 * - @ref OSG
 * - @ref FSM
 * - @ref Signals
 * - @ref Logging
 * - @ref Misc
 * - @ref WatchMacros
 * - @ref RegMacros
 * - @ref StringFunctions
 * - @ref UtilityFunctions
 * - @ref EnvirAndExtensions
 * - @ref Internals
 * - @ref ParsimBrief
 *
 * The full list of categories is also available in the sidebar and from the menu,
 * under Modules.
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
 * @defgroup Fundamentals Fundamentals
 *
 * @brief Fundamental library classes. The most important ones are:
 *
 *    - cObject is the base class for the vast majority of classes in \opp
 *    - cOwnedObject is the base class for objects whose ownership is tracked at runtime
 *    - cCoroutine is a generic coroutine class that cSimpleModule uses for running its activity() method
 *    - cRuntimeError is thrown when the simulation encounters an error
 *    - cClassDescriptor subclasses provide reflection information about objects
 */

/**
 * @defgroup ModelComponents Model Components
 *
 * @brief Classes in this group make up static components of the simulation model.
 *
 *    - cModule and subclasses represent modules in the simulation
 *    - cChannel and subclasses represent channels between modules
 *    - cPar represents parameters of modules and channels
 *    - cGate represents module gates (attachment points of connections in modules)
 */

/**
 * @defgroup SimProgr Simulation Programming
 *
 * @brief Classes in this group are essential to writing \opp simulation models.
 *
 *    - cSimpleModule represents active modules in the simulation.
 *      The user implements new modules by subclassing cSimpleModule and
 *      overriding its handleMessage() or activity() member function.
 *    - cMessage represents events, and also messages sent among modules
 *    - cPacket is a subclass of cMessage that represents network packets
 *    - cQueue is a generic FIFO data structure for storing objects
 *    - cPacketQueue is a cQueue subclass specialized for cPacket objects
 *    - cTopology is a utility class for discovering the topology of the model
 *      (which is often a communication network), and finding shortest paths in it
 */

/**
 * @defgroup SimCore  Simulation Core
 *
 * @brief Simulation infrastructure that makes DES work.
 *
 *    - cSimulation stores the network with its modules and channels,
 *      the future events set, and the event scheduler.
 *    - cEvent represents a simulation event, but it is mostly intended for
 *      internal use (models should use cMessage)
 *    - cFutureEventSet represents the future events set (FES) of the simulation,
 *      and cEventHeap is its default, heap-based implementation
 *    - cScheduler is the interface for simulation event schedulers, and
 *      cSequentialScheduler and cRealTimeScheduler are its two built-in
 *      implementations
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
 * Generators come in two flavors: as plain functions (taking an RNG
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
 * @brief The primary way of recording statistics from simulations
 * is by means of signals and declared statistics, i.e. using @statistic
 * properties in NED files. However, the simulation library also provides
 * some classes in case programmatic result collection is needed.
 *
 * Scalar values can be recorded in the output scalar file with the
 * recordScalar() method of the module or channel. To collect statistic
 * summaries (mean, stddev, etc.) or histograms, use the cStdDev and
 * cHistogram classes. Their contents can be recorded into the output
 * scalar file with the recordStatistic() method of the module or channel,
 * or with the record() method of the statistic object itself.
 * To record output vectors (time series data), use the cOutVector class.
 *
 * Declarative (@statistic-based) result recording is also extensible, via the
 * cResultFilter and cResultRecorder classes; see @ref ResultFiltersRecorders.
 *
 * All result collection methods eventually delegate to "record" methods
 * in cEnvir, i.e. the actual recording is decoupled from the result
 * collection part, and can be changed without affecting the rest of the code.
 *
 * The central classes are:
 *    - cStdDev computes statistics like (unweighted and weighted) mean,
 *      standard deviation, minimum and maximum value.
 *    - cHistogram is a generic histogram class. Bin creation can be
 *      customized via various histogram strategy classes like e.g.
 *      cAutoRangeHistogramStrategy.
 *    - cPSquare is a class that uses the P<sup>2</sup> algorithm by Jain
 *      and Chlamtac. The algorithm calculates quantiles without storing
 *      the observations.
 *    - cOutVector provides a way to record output vectors from the simulation.
 *
 * Some other classes closely related to the above ones are not listed
 * here explicitly, but you can find them via 'See also' links from their
 * main classes.
 */

/**
 * @defgroup ResultFiltersRecorders Result Filters and Recorders
 *
 * Result filters and recorders are objects that participate in declarative
 * statistics recording via <tt>@statistic</tt> properties in NED files.
 *
 * Central classes:
 *    - cResultFilter is the base class for result filters
 *    - cResultRecorder is the base class for result recorders
 *
 * New result filter/recorder classes need to be registered with the
 * Register_ResultFilter() / Register_ResultFilter2() and
 * Register_ResultRecorder() / Register_ResultRecorder2() macros
 * to be accessible from <tt>@statistic</tt>.
 */

/**
 * @defgroup Expressions  Expression Evaluation
 *
 * @brief Classes related to evaluating generic/arithmetic expressions and match expressions.
 *
 *    - cExpression encapsulates an expression that can be evaluated. This is
 *      an abstract base class.
 *    - cDynamicExpression subclasses cExpression, and implements an expression
 *      parsed at runtime
 *    - cMatchExpression allows matching objects against a filter expression
 *    - cValue is a variant-style value class for representing intermediate results
 *      during expression evaluation
 *    - cValueMap, cValueArray, cValueHolder are container classes that represent
 *      elements in a JSON-style data structure that certain expressions may
 *      produce as result
 *    - cXmlElement represents an XML element
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
 * @defgroup OSG  OSG/osgEarth Support (3D Graphics)
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
 * @defgroup Signals  Simulation Signals
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
 * @defgroup Logging  Logging
 *
 * @brief \opp provides a logging mechanism for models, with support for log levels,
 * filtering, a configurable log prefix, and more.
 *
 * See EV_LOG() for an overview.
 *
 * If you are encountering compile errors, see also EV_STATICCONTEXT.
 */

/**
 * @defgroup Misc Miscellaneous
 *
 * @brief Miscellaneous types.
 */

/**
 * @defgroup WatchMacros  WATCH Macros
 *
 * @brief WATCH macros make normal variables show up in Qtenv inspectors.
 */

/**
 * @defgroup RegMacros Registration Macros
 *
 * @brief Macros for registering various classes, functions and other extension
 * items with the simulation kernel. Registration is necessary for allowing
 * instantiation by class name and attaching meta-information.
 */

/**
 * @defgroup StringFunctions String Functions
 *
 * @brief Miscellaneous string-related utility functions.
 */

/**
 * @defgroup UtilityFunctions  Utility Functions
 *
 * @brief This group is a collection of miscellaneous utility functions.
 */

/**
 * @defgroup EnvirAndExtensions  Envir and Extensions
 *
 * @brief Simulations take input in the form of parameters and configuration
 * settings, and produce various outputs such as statistical results, log, etc.
 * Implementations of such input and output facilities  are not part of the
 * simulation core (cSimulation,etc.), but instead, they are delegated to the
 * "environment" of the simulation, represented by the cEnvir interface.
 * The Envir library (src/envir) contains the default implementation of cEnvir.
 *
 * cEnvir is a facade which delegates most of its subtasks to further components:
 * cConfiguration, cIOutputScalarManager etc, which themselves may have multiple
 * alternative implementations.
 *
 *    - cEnvir represents the "environment" of the simulation
 *    - cConfiguration provides parameter values and configuration settings
 *    - cIOutputScalarManager is an interface for recording scalar results (.sca files)
 *    - cIOutputVectorManager is an interface for recording vector results (.vec files)
 *    - See list for more interfaces
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
 * which is generated from the source files in the <tt>src/sim/parsim</tt>
 * directory.
 */

}  // namespace omnetpp


