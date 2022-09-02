//=========================================================================
//  CCONTEXTSWITCHER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Utility functions
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccontextswitcher.h"

namespace omnetpp {

cContextSwitcher::cContextSwitcher(const cComponent *newContext)
{
    // save current context and switch to new
    simulation = newContext->getSimulation();
    callerContext = simulation->getContext();
    simulation->setContext(const_cast<cComponent *>(newContext));
}

cContextSwitcher::~cContextSwitcher()
{
    // restore old context
    if (!callerContext)
        simulation->setGlobalContext();
    else
        simulation->setContext(callerContext);
}

//----

OPP_THREAD_LOCAL va_list dummy_va;

OPP_THREAD_LOCAL int cMethodCallContextSwitcher::depth = 0;

cMethodCallContextSwitcher::cMethodCallContextSwitcher(const cComponent *newContext) :
    cContextSwitcher(newContext)
{
    depth++;
}

void cMethodCallContextSwitcher::methodCall(const char *methodFmt, ...)
{
    cComponent *newContext = simulation->getContext();
    if (newContext != callerContext) {
        va_list va;
        va_start(va, methodFmt);
        EVCB.componentMethodBegin(callerContext, newContext, methodFmt, va, false);
        va_end(va);
    }
}

void cMethodCallContextSwitcher::methodCallSilent(const char *methodFmt, ...)
{
    cComponent *newContext = simulation->getContext();
    if (newContext != callerContext) {
        va_list va;
        va_start(va, methodFmt);
        EVCB.componentMethodBegin(callerContext, newContext, methodFmt, va, true);
        va_end(va);
    }
}

void cMethodCallContextSwitcher::methodCallSilent()
{
    cComponent *newContext = simulation->getContext();
    if (newContext != callerContext)
        EVCB.componentMethodBegin(callerContext, newContext, nullptr, dummy_va, true);
}

cMethodCallContextSwitcher::~cMethodCallContextSwitcher()
{
    depth--;
    cComponent *methodContext = simulation->getContext();
    if (methodContext != callerContext)
        EVCB.componentMethodEnd();
}

}  // namespace omnetpp

