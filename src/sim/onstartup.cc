//==========================================================================
//   STARTUP.CC - part of
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  supporting class for EXECUTE_ON_STARTUP macro
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "onstartup.h"
#include "cexception.h"


ExecuteOnStartup *ExecuteOnStartup::head;


ExecuteOnStartup::ExecuteOnStartup(void (*_code_to_exec)())
{
    code_to_exec = _code_to_exec;

    // add to list
    next = head;
    head = this;
}


ExecuteOnStartup::~ExecuteOnStartup()
{
}


void ExecuteOnStartup::execute()
{
    code_to_exec();
}

void ExecuteOnStartup::executeAll()
{
    ExecuteOnStartup *p = ExecuteOnStartup::head;
    while (p)
    {
        p->execute();
        p = p->next;
    }
}


