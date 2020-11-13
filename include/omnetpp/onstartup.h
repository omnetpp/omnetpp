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

// magic number for identifying a release build.
#define RELEASE_OPPSIM_MAGIC_NUMBER 0x12345678

/**
 * @brief Allows code fragments to be collected in global scope which will
 * then be executed from main() right after program startup. This is
 * used by in \opp for building global registration lists of
 * module types, network types, etc. Registration lists in fact
 * are a simple substitute for Java's Class.forName() method...
 *
 * @hideinitializer
 */
#define EXECUTE_ON_STARTUP(CODE)  \
  namespace { \
    void __ONSTARTUP_FUNC() {CODE;} \
    static omnetpp::CodeFragments __ONSTARTUP_OBJ(__ONSTARTUP_FUNC, omnetpp::CodeFragments::STARTUP); \
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
    static omnetpp::CodeFragments __ONSTARTUP_OBJ(__ONSTARTUP_FUNC, omnetpp::CodeFragments::SHUTDOWN); \
  }

/**
 * @brief Supporting class for the EXECUTE_ON_STARTUP and EXECUTE_ON_SHUTDOWN macros.
 *
 * @ingroup Internals
 */
class SIM_API CodeFragments
{
  public:
    enum Type {STARTUP, SHUTDOWN};
  private:
    Type type;
    void (*code)();
    CodeFragments *next;
    static CodeFragments *head;
  public:
    CodeFragments(void (*code)(), Type type);
    ~CodeFragments() {}
    static void executeAll(Type type);
};

}  // namespace omnetpp


#endif

