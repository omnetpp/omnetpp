//==========================================================================
//  ONSTARTUP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  EXECUTE_ON_STARTUP macro and supporting class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ONSTARTUP_H
#define __ONSTARTUP_H

#include <vector>
#include <map>
#include "simkerneldefs.h"
#include "cownedobject.h"

NAMESPACE_BEGIN


// Generating identifiers unique for this file. See MSVC Help for __COUNTER__
// for more info.
#define __CONCAT1(x,y) x##y
#define __CONCAT2(prefix,line) __CONCAT1(prefix,line)
#define MAKE_UNIQUE_WITHIN_FILE(prefix) __CONCAT2(prefix,__LINE__)

// helpers for EXECUTE_ON_STARTUP
// IMPORTANT: if you change "__onstartup_func_" below, linkall.pl must also be updated!
#define __ONSTARTUP_FUNC  MAKE_UNIQUE_WITHIN_FILE(__onstartup_func_)
#define __ONSTARTUP_OBJ   MAKE_UNIQUE_WITHIN_FILE(__onstartup_obj_)

// helper
#define __FILEUNIQUENAME__  MAKE_UNIQUE_WITHIN_FILE(__uniquename_)

/**
 * Allows code fragments to be collected in global scope which will
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
    static ExecuteOnStartup __ONSTARTUP_OBJ(__ONSTARTUP_FUNC); \
  };


/**
 * Supporting class for EXECUTE_ON_STARTUP macro.
 *
 * @ingroup Internals
 */
class SIM_API ExecuteOnStartup
{
  private:
    void (*code_to_exec)();
    ExecuteOnStartup *next;
    static ExecuteOnStartup *head;
  public:
    ExecuteOnStartup(void (*code_to_exec)());
    ~ExecuteOnStartup();
    void execute();
    static void executeAll();
};

NAMESPACE_END


#endif

