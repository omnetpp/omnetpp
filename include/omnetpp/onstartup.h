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
 * @brief Supporting class for the EXECUTE_ON_STARTUP and EXECUTE_ON_SHUTDOWN macros.
 *
 * @ingroup Internals
 */
class SIM_API CodeFragments
{
  public:
    enum Type {EARLY_STARTUP, STARTUP, SHUTDOWN};
    Type type;
    void (*code)();
    const char *sourceCode;
    CodeFragments *next;
    static CodeFragments *head;
  public:
    CodeFragments(void (*code)(), const char *sourceCode, Type type);
    ~CodeFragments() {}
    static void executeAll(Type type, bool removeDoneItems=true);
    static bool hasItemsOfType(Type type);
};

}  // namespace omnetpp


#endif

