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

cException::cException(const cException& other)
{
    copy(other);
}

cException::~cException()
{
    for (cRuntimeError *e : nestedExceptions)
        delete e;
}

cException& cException::operator=(const cException& other)
{
    if (this == &other)
        return *this;
    for (cRuntimeError *e : nestedExceptions)
        delete e;
    nestedExceptions.clear();
    copy(other);
    return *this;
}

void cException::storeContext()
{
    cSimulation *sim = cSimulation::getActiveSimulation();

    if (!sim) {
        simulationStage = cSimulation::STAGE_NONE;
        eventNumber = 0;
        simtime = SIMTIME_ZERO;
    }
    else {
        simulationStage = sim->getStage();
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
    simulation = cSimulation::getActiveSimulation();
    envir = simulation ? simulation->getEnvir() : cSimulation::getActiveEnvir();
    cComponent *context = simulation ? simulation->getContext() : nullptr;
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

void cException::addNestedException(std::exception& e)
{
    if (dynamic_cast<cRuntimeError*>(&e) != nullptr)
        addNestedException(static_cast<cRuntimeError&>(e));
    else {
        cRuntimeError re(e);
        addNestedException(re);
    }
}

void cException::addNestedException(cRuntimeError& e)
{
    nestedExceptions.push_back(e.dup());
}

void cException::copy(const cException& other)
{
    errorCode = other.errorCode;
    msg = other.msg;
    simulation = other.simulation;
    envir = other.envir;
    simulationStage = other.simulationStage;
    eventNumber = other.eventNumber;
    simtime = other.simtime;
    lifecycleListenerType = other.lifecycleListenerType;
    hasContext_ = other.hasContext_;
    contextClassName = other.contextClassName;
    contextFullPath = other.contextFullPath;
    contextComponentId = other.contextComponentId;
    contextComponentKind = other.contextComponentKind;

    for (cRuntimeError *e : other.nestedExceptions)
        nestedExceptions.push_back(e->dup());
}

static const char *getKindStr(cComponent::ComponentKind kind, bool capitalized)
{
    switch (kind) {
        case cComponent::KIND_MODULE: return capitalized ? "Module" : "module";
        case cComponent::KIND_CHANNEL: return capitalized ? "Channel" : "channel";
        default: return capitalized ? "Component" : "component";
    }
}

std::string cException::getFormattedMessage(bool addPrefix) const
{
    std::string when, where;
    switch (getSimulationStage()) {
        case cSimulation::STAGE_NONE: when = ""; break;
        case cSimulation::STAGE_BUILD: when = " during network setup"; break; // note leading spaces
        case cSimulation::STAGE_INITIALIZE: when = " during network initialization"; break;
        case cSimulation::STAGE_EVENT: when = opp_stringf(" at t=%ss, event #%" PRId64, SIMTIME_STR(getSimtime()), getEventNumber()); break;
        case cSimulation::STAGE_FINISH: when = " during finalization"; break;
        case cSimulation::STAGE_CLEANUP: when = " during network cleanup"; break;
    }

    if (lifecycleListenerType != LF_NONE)
        when = when + " in a " + cISimulationLifecycleListener::getSimulationLifecycleEventName(lifecycleListenerType) + " listener";

    //TODO include event name
    if (hasContext()) {
        where = opp_stringf(" in %s (%s) %s (id=%d)",
                getKindStr((cComponent::ComponentKind)getContextComponentKind(), false),
                getContextClassName(), getContextFullPath(), getContextComponentId());
    }

    std::string whereWhen = opp_join(",", where.c_str(), when.c_str());

    std::string result = opp_join(" --", what(), whereWhen.c_str());  // note: isError() is not used

    if (addPrefix && isError() && result.substr(0,5) != "Error")
        result = "Error: " + result;

    if (!nestedExceptions.empty())
        result += " (also: further exceptions occurred while handling this exception)";  //TODO add details if requested?

    return result;
}

bool cException::isError(std::exception& e)
{
    cException *ex = dynamic_cast<cException*>(&e);
    return ex ? ex->isError() : true;
}

std::string cException::getFormattedMessage(std::exception& e, bool addPrefix)
{
    cException *ex = dynamic_cast<cException*>(&e);
    return ex ? ex->getFormattedMessage(addPrefix) : std::string(e.what());
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
    //ASSERT(dynamic_cast<cException *>(&e) == nullptr);  // do not use this if this is a cException, as it loses information!
    std::string msg = e.what();
    std::string typeName = opp_typename(typeid(e));
    if (opp_stringbeginswith(typeName.c_str(), "std::"))
        msg = typeName + ": " + msg;
    if (!opp_isempty(location))
        msg += " -- at "s + location;
    init(nullptr, E_CUSTOM, msg);
    notifyEnvir();
}

void cRuntimeError::notifyEnvir()
{
    if (envir->shouldDebugNow(this))
        DEBUG_TRAP; // YOUR CODE IS A FEW FRAMES UP ON THE CALL STACK
}

}  // namespace omnetpp

