//=========================================================================
//  CEXCEPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cstdlib>  // for exit()
#include <cstdarg>  // for va_list
#include <csignal>  // raise()
#include "common/stringutil.h"
#include "common/commonutil.h"
#include "omnetpp/cexception.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/ccontextswitcher.h"
#include "omnetpp/platdep/platmisc.h"  // PRId64 and DEBUG_TRAP

using namespace omnetpp::common;
using namespace std::string_literals;

namespace omnetpp {

#define VA_PRINTF_INTO(variable, lastArg, format) \
    va_list va; \
    va_start(va, lastArg); \
    std::string variable = opp_vstringf(format,va); \
    va_end(va);

cException::cException() : std::exception(), errorCode(E_CUSTOM), msg("n/a")
{
    storeContext();
}

cException::cException(ErrorCodeInt errorCode...) : std::exception()
{
    VA_PRINTF_INTO(msg, errorCode, cErrorMessages::get((ErrorCode)errorCode));
    init(nullptr, (ErrorCode)errorCode, msg);
}

cException::cException(const char *msgFormat...) : std::exception()
{
    VA_PRINTF_INTO(msg, msgFormat, msgFormat);
    init(nullptr, E_CUSTOM, msg);
}

cException::cException(const cObject *where, ErrorCodeInt errorCode...) : std::exception()
{
    VA_PRINTF_INTO(msg, errorCode, cErrorMessages::get((ErrorCode)errorCode));
    init(where, (ErrorCode)errorCode, msg);
}

cException::cException(const cObject *where, const char *msgFormat...) : std::exception()
{
    VA_PRINTF_INTO(msg, msgFormat, msgFormat);
    init(where, E_CUSTOM, msg);
}

void cException::storeContext()
{
    cSimulation *sim = cSimulation::getActiveSimulation();

    if (!sim) {
        simulationStage = CTX_NONE;
        eventNumber = 0;
        simtime = SIMTIME_ZERO;
    }
    else {
        simulationStage = sim->getSimulationStage();
        eventNumber = sim->getEventNumber();
        simtime = sim->getSimTime();
    }

    if (!sim || !sim->getContext()) {
        hasContext_ = false;
        contextComponentId = -1;
    }
    else {
        hasContext_ = true;
        contextClassName = sim->getContext()->getClassName();
        contextFullPath = sim->getContext()->getFullPath().c_str();
        contextComponentId = sim->getContext()->getId();
        contextComponentKind = sim->getContext()->getComponentKind();
    }
}

void cException::exitIfStartupError()
{
    if (!cStaticFlag::insideMain()) {
        // note: ev may not be available at this time
        fprintf(stderr, "<!> Error during startup/shutdown: %s. Aborting.", what());
        abort();
    }
}

void cException::init(const cObject *where, ErrorCode errorcode, const std::string& text)
{
    // store error code
    this->errorCode = errorcode;

    // prefix message with "where" object conditionally, as "(%s)%s:"
    //  - if object is the module itself: skip
    //  - if object is local in module: use getFullName()
    //  - if object is somewhere else: use getFullPath()
    //
    cSimulation *sim = cSimulation::getActiveSimulation();
    cComponent *context = sim ? sim->getContext() : nullptr;
    std::string prefix;
    if (where && where != context) {
        // if object is in the child of the context component, no need to print path
        std::string contextComponentFullPath = context ? context->getFullPath().c_str() : "";
        bool needPath = where->getFullPath() != (contextComponentFullPath + "." + where->getFullName());
        prefix = needPath ? where->getClassAndFullPath() : where->getClassAndFullName();
        prefix += ": ";
    }

    // the full exception message
    msg = prefix + text;

    // store context
    storeContext();

    // if a global object's ctor/dtor throws an exception, there won't be
    // anyone around to catch it, so print it and abort here.
    exitIfStartupError();
}

static const char *getKindStr(cComponent::ComponentKind kind, bool capitalized)
{
    switch (kind) {
        case cComponent::KIND_MODULE: return capitalized ? "Module" : "module";
        case cComponent::KIND_CHANNEL: return capitalized ? "Channel" : "channel";
        default: return capitalized ? "Component" : "component";
    }
}

std::string cException::getFormattedMessage() const
{
    std::string when, where;
    switch (getSimulationStage()) {
        case CTX_NONE: when = ""; break;
        case CTX_BUILD: when = " during network setup"; break; // note leading spaces
        case CTX_INITIALIZE: when = " during network initialization"; break;
        case CTX_EVENT: when = opp_stringf(" at t=%ss, event #%" PRId64, SIMTIME_STR(getSimtime()), getEventNumber()); break;
        case CTX_FINISH: when = " during finalization"; break;
        case CTX_CLEANUP: when = " during network cleanup"; break;
    }

    //TODO include event name
    if (hasContext()) {
        where = opp_stringf(" in %s (%s) %s (id=%d)",
                getKindStr((cComponent::ComponentKind)getContextComponentKind(), false),
                getContextClassName(), getContextFullPath(), getContextComponentId());
    }

    std::string whereWhen = opp_join(",", where.c_str(), when.c_str());
    std::string result = opp_join(" --", what(), whereWhen.c_str());  // note: isError() is not used
    return result;
}

//---

cTerminationException::cTerminationException(ErrorCodeInt errorCode...)
{
    VA_PRINTF_INTO(msg, errorCode, cErrorMessages::get((ErrorCode)errorCode));
    init(nullptr, (ErrorCode)errorCode, msg);
}

cTerminationException::cTerminationException(const char *msgFormat...)
{
    VA_PRINTF_INTO(msg, msgFormat, msgFormat);
    init(nullptr, E_CUSTOM, msg);
}

const std::type_info *getTerminationExceptionTypeInfoPointer() { return &typeid(cTerminationException); }

//---

cRuntimeError::cRuntimeError(ErrorCodeInt errorCode...)
{
    VA_PRINTF_INTO(msg, errorCode, cErrorMessages::get((ErrorCode)errorCode));
    init(nullptr, (ErrorCode)errorCode, msg);
    notifyEnvir();
}

cRuntimeError::cRuntimeError(const char *msgFormat...)
{
    VA_PRINTF_INTO(msg, msgFormat, msgFormat);
    init(nullptr, E_CUSTOM, msg);
    notifyEnvir();
}

cRuntimeError::cRuntimeError(const cObject *where, ErrorCodeInt errorCode...)
{
    VA_PRINTF_INTO(msg, errorCode, cErrorMessages::get((ErrorCode)errorCode));
    init(where, (ErrorCode)errorCode, msg);
    notifyEnvir();
}

cRuntimeError::cRuntimeError(const cObject *where, const char *msgFormat...)
{
    VA_PRINTF_INTO(msg, msgFormat, msgFormat);
    init(where, E_CUSTOM, msg);
    notifyEnvir();
}

cRuntimeError::cRuntimeError(const std::exception& e, const char *location)
{
    std::string msg = e.what();
    if (!opp_isempty(location))
        msg += " -- at "s + location;
    init(nullptr, E_CUSTOM, msg);
    notifyEnvir();
}


void cRuntimeError::notifyEnvir()
{
    if (getEnvir()->debugOnErrors && getEnvir()->ensureDebugger(this))
        DEBUG_TRAP; // YOUR CODE IS A FEW FRAMES UP ON THE CALL STACK
}

}  // namespace omnetpp

