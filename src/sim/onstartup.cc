//==========================================================================
//  ONSTARTUP.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  supporting class for EXECUTE_ON_STARTUP macro
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "onstartup.h"

USING_NAMESPACE

#if NDEBUG
extern "C" SIM_API bool __is_release_oppsim__;
bool __is_release_oppsim__ = true;
#endif

// Checks whether an other instance of the shared library has already been loaded.
// This can happen when some parts of the simulation are linked with debug
// while others are linked with the release version of the oppsim shared library.
// This is an error: we must tell the user about it, and abort.
class StartupChecker
{
    public:
        StartupChecker()
        {
            char *loaded = getenv("__OPPSIM_LOADED__");
            if (loaded != NULL && strcmp(loaded, "yes") == 0 ) {
                fprintf(stderr,
                       "\n<!> Error: Attempt to load the oppsim shared library more than once. "
                       "This usually happens when part of your simulation is using release libraries "
                       "while other parts are using the debug version. Make sure to rebuild all parts "
                       "of your model in either release or debug mode!\n");
                exit(1);
            }

            // we set the environment variable so a new instance will be able to
            // detect it (note: there is no setenv() on windows)
            putenv((char *)"__OPPSIM_LOADED__=yes");
        }
};

StartupChecker startupChecker;


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

    // null out list to prevent double execution on subsequent calls (e.g. after dll loading)
    ExecuteOnStartup::head = NULL;
}


