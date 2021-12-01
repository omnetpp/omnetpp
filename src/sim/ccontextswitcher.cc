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

#include "common/commonutil.h"
#include "common/unitconversion.h"
#include "common/opp_ctype.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;

namespace omnetpp {

cContextSwitcher::cContextSwitcher(const cComponent *newContext)
{
    // save current context and switch to new
    cSimulation *simulation = getSimulation();
    callerContext = simulation->getContext();
    simulation->setContext(const_cast<cComponent *>(newContext));
}

cContextSwitcher::~cContextSwitcher()
{
    // restore old context
    if (!callerContext)
        getSimulation()->setGlobalContext();
    else
        getSimulation()->setContext(callerContext);
}

//----

static va_list dummy_va;

int cMethodCallContextSwitcher::depth = 0;

cMethodCallContextSwitcher::cMethodCallContextSwitcher(const cComponent *newContext) :
    cContextSwitcher(newContext)
{
    depth++;
}

void cMethodCallContextSwitcher::methodCall(const char *methodFmt, ...)
{
    cComponent *newContext = getSimulation()->getContext();
    if (newContext != callerContext) {
        va_list va;
        va_start(va, methodFmt);
        EVCB.componentMethodBegin(callerContext, newContext, methodFmt, va, false);
        va_end(va);
    }
}

void cMethodCallContextSwitcher::methodCallSilent(const char *methodFmt, ...)
{
    cComponent *newContext = getSimulation()->getContext();
    if (newContext != callerContext) {
        va_list va;
        va_start(va, methodFmt);
        EVCB.componentMethodBegin(callerContext, newContext, methodFmt, va, true);
        va_end(va);
    }
}

void cMethodCallContextSwitcher::methodCallSilent()
{
    cComponent *newContext = getSimulation()->getContext();
    if (newContext != callerContext)
        EVCB.componentMethodBegin(callerContext, newContext, nullptr, dummy_va, true);
}

cMethodCallContextSwitcher::~cMethodCallContextSwitcher()
{
    depth--;
    cComponent *methodContext = getSimulation()->getContext();
    if (methodContext != callerContext)
        EVCB.componentMethodEnd();
}

//----

cContextTypeSwitcher::cContextTypeSwitcher(ContextType contextType)
{
    // save current context type and switch to new one
    cSimulation *simulation = getSimulation();
    savedContextType = simulation->getContextType();
    simulation->setContextType(contextType);
}

cContextTypeSwitcher::~cContextTypeSwitcher()
{
    getSimulation()->setContextType(savedContextType);
}

}  // namespace omnetpp

