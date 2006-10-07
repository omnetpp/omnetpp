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

#include "defs.h"

class cArray;


/**
 * Allows code fragments to be collected in global scope which will
 * then be executed from main() right after program startup. This is
 * used by in \opp for building global registration lists of
 * module types, network types, etc. Registration lists in fact
 * are a simple substitute for Java's Class.forName() method...
 *
 * @hideinitializer
 */
#define EXECUTE_ON_STARTUP(NAME, CODE)  \
 static void __##NAME##_code() {CODE;} \
 static ExecuteOnStartup __##NAME##_reg(__##NAME##_code);


//
// Example:
// EXECUTE_ON_STARTUP(EthernetModule, moduletypes->instance().add(new EthernetModuleType());)
//

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


/**
 * Singleton class, used for global registration lists.
 *
 * @ingroup Internals
 */
class cRegistrationList
{
  private:
    cArray *inst;
    const char *tmpname;
  public:
    cRegistrationList(const char *name);
    ~cRegistrationList();
    cArray *instance();
    void clear();
};

#endif

