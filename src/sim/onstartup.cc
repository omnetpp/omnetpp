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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <cstring>
#include "omnetpp/onstartup.h"

namespace omnetpp {


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
            if (loaded != nullptr && strcmp(loaded, "yes") == 0 ) {
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

CodeFragments *CodeFragments::head;

CodeFragments::CodeFragments(void (*code)(), const char *sourceCode, Type type, SimulationLifecycleEventType lifecycleEvent) :
type(type), lifecycleEvent(lifecycleEvent), code(code), sourceCode(sourceCode)
{
    next = head;
    head = this;
    (void)this->sourceCode; // eliminate warning
}

CodeFragments::CodeFragments(void(*code)(), const char *sourceCode, Type type) :
        CodeFragments(code, sourceCode, type, LF_NONE) {}

CodeFragments::CodeFragments(void(*code)(), const char *sourceCode, SimulationLifecycleEventType lifecycleEvent) :
        CodeFragments(code, sourceCode, SIMULATION_LIFECYCLE_EVENT, lifecycleEvent) {}

void CodeFragments::executeAll(Type type, SimulationLifecycleEventType lifecycleEvent, bool removeDoneItems)
{
    CodeFragments *p = CodeFragments::head;
    while (p) {
        if (p->type == type && p->lifecycleEvent == lifecycleEvent && p->code != nullptr) {
            p->code();
            if (removeDoneItems)
                p->code = nullptr;
        }
        p = p->next;
    }
}

bool CodeFragments::hasItemsOfType(Type type, SimulationLifecycleEventType lifecycleEvent)
{
    CodeFragments *p = CodeFragments::head;
    while (p) {
        if (p->type == type && p->lifecycleEvent == lifecycleEvent && p->code != nullptr)
            return true;
        p = p->next;
    }
    return false;
}

}  // namespace omnetpp

