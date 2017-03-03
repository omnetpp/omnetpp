//==========================================================================
//  SIMTIME_T.H - part of
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

#ifndef __OMNETPP_SIMTIME_T_H
#define __OMNETPP_SIMTIME_T_H

#include <cstdlib>
#include <string>
#include <cmath>
#include <cfloat>
#include "simkerneldefs.h"
#include "simtime.h"

#ifdef USE_DOUBLE_SIMTIME
#error USE_DOUBLE_SIMTIME is no longer supported -- use OMNeT++ 4.x if you need it
#endif

namespace omnetpp {

/**
 * @brief Represents simulation time.
 * @ingroup SimTime
 */
typedef SimTime simtime_t;

/**
 * @brief Constant reference to a simtime_t.
 *
 * In many configurations (e.g. debug builds), it is more efficient to return
 * a SimTime from a function as a const ref than by value.
 *
 * @ingroup SimTime
 */
typedef const simtime_t& simtime_t_cref;

/**
 * @brief This type must be used for global variables representing simulation time.
 *
 * The normal simtime_t type (i.e. SimTime) cannot be used for static variables,
 * because the scale exponent is only available after the configuration has been
 * read by the simulation program. This type is simply an alias of `double`.
 *
 * @ingroup SimTime
 */
typedef const double   const_simtime_t;

/**
 * @brief The maximum representable simulation time with the current resolution.
 * @ingroup SimTime
 */
#define SIMTIME_MAX    omnetpp::SimTime::getMaxTime()

/**
 * @brief Zero simulation time.
 *
 * Using SIMTIME_ZERO can be more efficient than using the 0 constant, especially
 * in non-optimized (debug) builds, because it spares the SimTime constructor call.
 *
 * @ingroup SimTime
 */
#define SIMTIME_ZERO   omnetpp::SimTime::ZERO

/**
 * @brief Convert simtime_t to a C string.
 * @ingroup SimTime
 */
#define SIMTIME_STR(t) ((t).str().c_str())

/**
 * @brief Convert simtime_t to a double. This conversion incurs precision loss.
 * @ingroup SimTime
 */
#define SIMTIME_DBL(t) ((t).dbl())

} // namespace omnetpp

#endif

