//==========================================================================
//  ONSTARTUP.H - part of
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

#ifndef __OMNETPP_ONSTARTUP_H
#define __OMNETPP_ONSTARTUP_H

#include <vector>
#include <map>
#include "simkerneldefs.h"
#include "cownedobject.h"

namespace omnetpp {

// Generating identifiers unique for this file. See MSVC Help for __COUNTER__ for more info.
#define __OPPCONCAT1(x,y) x##y
#define __OPPCONCAT2(prefix,line) __OPPCONCAT1(prefix,line)
#define MAKE_UNIQUE_WITHIN_FILE(prefix) __OPPCONCAT2(prefix,__LINE__)

// helpers for EXECUTE_ON_STARTUP
// IMPORTANT: if you change "__onstartup_func_" below, linkall.pl must also be updated!
#define __ONSTARTUP_FUNC  MAKE_UNIQUE_WITHIN_FILE(__onstartup_func_)
#define __ONSTARTUP_OBJ   MAKE_UNIQUE_WITHIN_FILE(__onstartup_obj_)

// helper
#define __FILEUNIQUENAME__  MAKE_UNIQUE_WITHIN_FILE(__uniquename_)

/**
 * @ingroup ExecMacros
 * @{
 */

/**
 * @brief Allows code fragments to be collected in global scope which will
 * then be executed from main() right after program startup. The main use is
 * to contribute items to registration lists of module types, network types, etc.
 *
 * @hideinitializer
 */
#define EXECUTE_ON_EARLY_STARTUP(CODE)  \
  namespace { \
    void __ONSTARTUP_FUNC() {CODE;} \
    static omnetpp::CodeFragments __ONSTARTUP_OBJ(__ONSTARTUP_FUNC, #CODE, omnetpp::CodeFragments::EARLY_STARTUP); \
  }

/**
 * @brief Allows code fragments to be collected in global scope which will
 * then be executed once the main infrastructure (cSimulation instance,
 * cEnvir instance, simulation runner, etc.) have been set up. The main
 * use is to hook lifecycle listeners on the active cSimulation instance.
 *
 * @hideinitializer
 */
#define EXECUTE_ON_STARTUP(CODE)  \
  namespace { \
    void __ONSTARTUP_FUNC() {CODE;} \
    static omnetpp::CodeFragments __ONSTARTUP_OBJ(__ONSTARTUP_FUNC, #CODE, omnetpp::CodeFragments::STARTUP); \
  }

/**
 * @brief Allows code fragments to be collected in global scope which will then be
 * executed on normal shutdown, just before control returns from main().
 * This is used by \opp for deallocating static global data structures such as
 * registration lists.
 *
 * @hideinitializer
 */
//NOTE: implementation reuses some of the *startup* macros
#define EXECUTE_ON_SHUTDOWN(CODE)  \
  namespace { \
    void __ONSTARTUP_FUNC() {CODE;} \
    static omnetpp::CodeFragments __ONSTARTUP_OBJ(__ONSTARTUP_FUNC, #CODE, omnetpp::CodeFragments::SHUTDOWN); \
  }

/**
 * @brief Registers a code fragment to be executed when the given lifecycle
 * event occurs on any cSimulation instance. EVENT should be a lifecycle event
 * type, a value from the SimulationLifecycleEventType enum.
 *
 * @hideinitializer
 */
#define EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(EVENT, CODE)  \
  namespace { \
    void __ONSTARTUP_FUNC() {CODE;} \
    static omnetpp::CodeFragments __ONSTARTUP_OBJ(__ONSTARTUP_FUNC, #CODE, omnetpp::EVENT); \
  }


/** @brief Registers a code fragment to be executed before a network is set up. */
#define EXECUTE_PRE_NETWORK_SETUP(CODE)       EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_PRE_NETWORK_SETUP, CODE)

/** @brief Registers a code fragment to be executed after a network is set up. */
#define EXECUTE_POST_NETWORK_SETUP(CODE)      EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_POST_NETWORK_SETUP, CODE)

/** @brief Registers a code fragment to be executed before a network is initialized. */
#define EXECUTE_PRE_NETWORK_INITIALIZE(CODE)  EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_PRE_NETWORK_INITIALIZE, CODE)

/** @brief Registers a code fragment to be executed after a network is initialized. */
#define EXECUTE_POST_NETWORK_INITIALIZE(CODE) EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_POST_NETWORK_INITIALIZE, CODE)

/** @brief Registers a code fragment to be executed before processing the first simulation event of a simulation. */
#define EXECUTE_ON_SIMULATION_START(CODE)     EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_ON_SIMULATION_START, CODE)

/** @brief Registers a code fragment to be executed when a simulation is paused. */
#define EXECUTE_ON_SIMULATION_PAUSE(CODE)     EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_ON_SIMULATION_PAUSE, CODE)

/** @brief Registers a code fragment to be executed when a paused simulation is resumed. */
#define EXECUTE_ON_SIMULATION_RESUME(CODE)    EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_ON_SIMULATION_RESUME, CODE)

/** @brief Registers a code fragment to be executed after processing the last event of a simulation.  */
#define EXECUTE_ON_SIMULATION_SUCCESS(CODE)   EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_ON_SIMULATION_SUCCESS, CODE)

/** @brief Registers a code fragment to be executed when an error (exception) occurs in a simulation. */
#define EXECUTE_ON_SIMULATION_ERROR(CODE)     EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_ON_SIMULATION_ERROR, CODE)

/** @brief Registers a code fragment to be executed before finalizing a simulation. */
#define EXECUTE_PRE_NETWORK_FINISH(CODE)      EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_PRE_NETWORK_FINISH, CODE)

/** @brief Registers a code fragment to be executed after finalizing a simulation. */
#define EXECUTE_POST_NETWORK_FINISH(CODE)     EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_POST_NETWORK_FINISH, CODE)

/** @brief Registers a code fragment to be executed at the end of processing events in a simulation, whether simulation terminated successfully or with an error. */
#define EXECUTE_ON_RUN_END(CODE)              EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_ON_RUN_END, CODE)

/** @brief Registers a code fragment to be executed before a network is deleted. */
#define EXECUTE_PRE_NETWORK_DELETE(CODE)      EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_PRE_NETWORK_DELETE, CODE)

/** @brief Registers a code fragment to be executed after a network is deleted. */
#define EXECUTE_POST_NETWORK_DELETE(CODE)     EXECUTE_ON_SIMULATION_LIFECYCLE_EVENT(LF_POST_NETWORK_DELETE, CODE)

/** @} */

/**
 * @brief Supporting class for the EXECUTE_ON_STARTUP and EXECUTE_ON_SHUTDOWN macros.
 *
 * @ingroup Internals
 */
class SIM_API CodeFragments
{
  public:
    enum Type {EARLY_STARTUP, STARTUP, SHUTDOWN, SIMULATION_LIFECYCLE_EVENT};
  private:
    Type type;
    SimulationLifecycleEventType lifecycleEvent;
    void (*code)();
    const char *sourceCode;
    CodeFragments *next;
    static CodeFragments *head;
  private:
    CodeFragments(void (*code)(), const char *sourceCode, Type type, SimulationLifecycleEventType lifecycleEvent);
    static void executeAll(Type type, SimulationLifecycleEventType lifecycleEvent, bool removeDoneItems);
    static bool hasItemsOfType(Type type, SimulationLifecycleEventType lifecycleEvent);
  public:
    CodeFragments(void (*code)(), const char *sourceCode, Type type);
    CodeFragments(void (*code)(), const char *sourceCode, SimulationLifecycleEventType lifecycleEvent);
    ~CodeFragments() {}
    static void executeAll(Type type, bool removeDoneItems=true) {executeAll(type, LF_NONE, removeDoneItems);}
    static void executeAll(SimulationLifecycleEventType lifecycleEvent, bool removeDoneItems=true) {executeAll(SIMULATION_LIFECYCLE_EVENT, lifecycleEvent, removeDoneItems);}
    static bool hasItemsOfType(Type type) {return hasItemsOfType(type, LF_NONE);}
    static bool hasItemsOfType(SimulationLifecycleEventType lifecycleEvent) {return hasItemsOfType(SIMULATION_LIFECYCLE_EVENT, lifecycleEvent);}
};

}  // namespace omnetpp

#endif

