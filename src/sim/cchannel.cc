//========================================================================
//  CCHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cChannel : channel class
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cchannel.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cIdealChannel);

void cChannel::MessageSentSignalValue::error() const
{
    throw cRuntimeError("cChannel::MessageSentSignalValue: Getter for unsupported value type called");
}

cChannel::cChannel(const char *name) : cComponent(name)
{
    srcGate = nullptr;
    nedConnectionElementId = -1;
}

cChannel::~cChannel()
{
    releaseLocalListeners();
}

std::string cChannel::str() const
{
    // print all parameters
    std::stringstream out;
    for (int i = 0; i < getNumParams(); i++) {
        cPar& p = const_cast<cChannel *>(this)->par(i);
        out << p.getFullName() << "=" << p.str() << " ";
    }
    return out.str();
}

void cChannel::parsimPack(cCommBuffer *buffer) const
{
    throw cRuntimeError(this, "parsimPack() not implemented");
}

void cChannel::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "parsimUnpack() not implemented");
}

void cChannel::finalizeParameters()
{
    if (!srcGate)
        throw cRuntimeError(this, "finalizeParameters() may only be called when the channel is already installed on a connection");
    cComponent::finalizeParameters();
}

void cChannel::callInitialize()
{
    cContextTypeSwitcher tmp(CTX_INITIALIZE);
    int stage = 0;
    while (initializeChannel(stage))
        ++stage;
}

bool cChannel::callInitialize(int stage)
{
    // note: numInitStages() will be checked inside initializeChannel()
    cContextTypeSwitcher tmp(CTX_INITIALIZE);
    return initializeChannel(stage);
}

bool cChannel::initializeChannel(int stage)
{
    // channels don't contain further subcomponents, so just we just invoke
    // initialize(stage) in the right context here.
    if (getSimulation()->getContextType() != CTX_INITIALIZE)
        throw cRuntimeError("Internal function initializeChannel() may only be called via callInitialize()");

    if (stage == 0) {
        if (initialized())
            throw cRuntimeError(this, "Channel already initialized");

        // call finalizeParameters() if user has forgotten to do it; this is needed
        // to make dynamic module/channel creation more robust
        if (!parametersFinalized())
            finalizeParameters();
    }

    int numStages = numInitStages();
    if (stage < numStages) {
        // switch context for the duration of the call
        cContextSwitcher tmp(this);
        getEnvir()->componentInitBegin(this, stage);
        try {
            initialize(stage);
        }
        catch (cException&) {
            throw;
        }
        catch (std::exception& e) {
            throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
        }
    }

    bool moreStages = stage < numStages - 1;

    // as a last step, call handleParameterChange() to notify the component about
    // parameter changes that occurred during initialization phase
    if (!moreStages) {
        setFlag(FL_INITIALIZED, true);
        handleParameterChange(nullptr);
    }
    return moreStages;
}

void cChannel::callFinish()
{
    // This is the interface for calling finish(). Channels don't contain further
    // subcomponents, so just we just invoke finish() in the right context here.
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_FINISH);
    try {
        recordParametersAsScalars();
        finish();
        fireFinish();
    }
    catch (cException&) {
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
    }
}

void cChannel::callRefreshDisplay()
{
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_REFRESHDISPLAY);
    try {
        refreshDisplay();
    }
    catch (cException&) {
        throw;
    }
    catch (std::exception& e) {
        throw cRuntimeError("%s: %s", opp_typename(typeid(e)), e.what());
    }
}

cModule *cChannel::getParentModule() const
{
    // find which (compound) module contains this connection
    if (!srcGate)
        return nullptr;
    cModule *ownerModule = srcGate->getOwnerModule();
    if (!ownerModule)
        return nullptr;
    return srcGate->getType() == cGate::INPUT ? ownerModule : ownerModule->getParentModule();
}

cProperties *cChannel::getProperties() const
{
    cModule *parent = getParentModule();
    cComponentType *type = getComponentType();
    cProperties *props;
    if (parent)
        props = parent->getComponentType()->getConnectionProperties(nedConnectionElementId, type->getFullName());
    else
        props = type->getProperties();
    return props;
}

bool cChannel::isBusy() const
{
    // a default implementation
    return simTime() < getTransmissionFinishTime();
}

//----

cIdealChannel *cIdealChannel::create(const char *name)
{
    return dynamic_cast<cIdealChannel *>(cChannelType::getIdealChannelType()->create(name));
}

}  // namespace omnetpp

