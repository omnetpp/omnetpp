//==========================================================================
//   ONSTARTUP.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  EXECUTE_ON_STARTUP macro and supporting class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ONSTARTUP_H
#define __ONSTARTUP_H

#include "defs.h"


/**
 * Allows code fragments to be collected in global scope which will
 * then be executed from main() right after program startup. This is
 * used by in OMNeT++ for building global registration lists of
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
// EXECUTE_ON_STARTUP(EthernetModule, (new EthernetModuleType()->setOwner(&modules)))
//

/**
 * Supporting class for EXECUTE_ON_STARTUP macro.
 *
 * @group Internals
 */
class ExecuteOnStartup
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

#endif

