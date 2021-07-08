//==========================================================================
//  EXPRESSIONFILTER.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <climits>
#include "omnetpp/csimulation.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/any_ptr.h"
#include "expressionfilter.h"
#include "common/pooledstring.h"

using namespace omnetpp::common;

namespace omnetpp {

inline omnetpp::any_ptr convert(omnetpp::common::any_ptr p) { return omnetpp::any_ptr(p.raw(), p.pointerType()); }
inline omnetpp::common::any_ptr convert(omnetpp::any_ptr p) { return omnetpp::common::any_ptr(p.raw(), p.pointerType()); }


SignalSource::SignalSource(cComponent *component, const char *signalName) :
        component(component), signalID(cComponent::registerSignal(signalName)) {}

bool SignalSource::isSubscribed(cResultListener *listener) const
{
    return filter != nullptr ? filter->hasDelegate(listener) : component->isSubscribed(signalID, listener);
}

void SignalSource::subscribe(cResultListener *listener) const
{
    if (filter)
        filter->addDelegate(listener);
    else if (component && signalID != SIMSIGNAL_NULL)
        component->subscribe(signalID, listener);
    else
        throw cRuntimeError("subscribe() called on empty SignalSource");
}

std::string SignalSource::str() const
{
    std::stringstream out;

    if (filter)
        out << "(" << filter->getClassName() << ")" << filter->getName();
    else if (component && signalID != SIMSIGNAL_NULL)
        out << cComponent::getSignalName(signalID) << "@" << component->getFullPath();
    else
        out << "<empty>";
    return out.str();
}

//---

ExpressionFilter::FilterInput *ExpressionFilter::find(cResultFilter *prevFilter)
{
    if (numInputs == 1)
        return &soleInput;
    else
        for (int i = 0; i < numInputs; i++)
            if (inputs[i].source.filter == prevFilter)
                return inputs+i;
    throw cRuntimeError("Internal error: ExpressionFilter: cannot identify input");
}

ExpressionFilter::FilterInput *ExpressionFilter::find(cComponent *sourceComponent, simsignal_t signalID)
{
    if (numInputs == 1)
        return &soleInput;
    else
        for (int i = 0; i < numInputs; i++)
            if (inputs[i].source.component == sourceComponent && inputs[i].source.signalID == signalID)
                return inputs+i;
    throw cRuntimeError("Internal error: ExpressionFilter: cannot identify input");
}

simtime_t ExpressionFilter::now() const
{
    return getSimulation()->getSimTime();
}

void ExpressionFilter::process(simtime_t_cref t, cObject *details)
{
    lastTimestamp = t;
    lastOutput = expr.evaluate(nullptr);
    switch (lastOutput.getType()) {
        case ExprValue::UNDEF: break;
        case ExprValue::BOOL: fire(this, t, lastOutput.boolValue(), details); break;
        case ExprValue::INT: fire(this, t, lastOutput.intValue(), details); break;
        case ExprValue::DOUBLE: fire(this, t, lastOutput.doubleValue(), details); break;
        case ExprValue::STRING: fire(this, t, lastOutput.stringValue(), details); break;
        case ExprValue::POINTER: fire(this, t, fromAnyPtr<cObject>(convert(lastOutput.pointerValue())), details); break;
    }
}

const char *ExpressionFilter::getName() const
{
    return opp_staticpooledstring::get(expr.str().c_str());
}

std::string ExpressionFilter::str() const
{
    std::stringstream out;
    out << expr.str() << " = " << lastOutput.str() << " @ t=" << lastTimestamp;
    return out.str();
}

int ExpressionFilter::findInput(const SignalSource& source) const
{
    if (numInputs == 1)
        return soleInput.source == source ? 0 : -1;
    else {
        for (int i = 0; i < numInputs; i++)
            if (inputs[i].source == source)
                return i;
        return -1;
    }
}

int ExpressionFilter::addInput(const SignalSource& source, FilterInputNode *filterInputNode)
{
    // Use existing FilterInput if one has the same SignalSource.
    // This is important, as multiple FilterInputs with the same SignalSource
    // leads to incorrect behavior: FilterInputNode's process() will always
    // find and update the first input only, leading to incorrect computation results.
    int index = findInput(source);
    if (index != -1) {
        filterInputNode->setOwner(this, index);
        return index;
    }

    // else add new FilterInput
    index = numInputs++;
    FilterInput *input;
    if (numInputs == 1) {
        input = &soleInput;
    }
    else {
        FilterInput *newInputs = new FilterInput[numInputs];
        if (numInputs == 2) {
            newInputs[0] = soleInput;
            soleInput = FilterInput(); // make it blank
        }
        else {
            for (int i = 0; i < numInputs-1; i++)
                newInputs[i] = inputs[i];
            delete [] inputs;
        }
        inputs = newInputs;
        input = &inputs[index];
    }

    input->source = source;
    if (source.filter != nullptr) {
        double initialValue = source.filter->getInitialDoubleValue();
        if (!std::isnan(initialValue))
            input->lastValue = initialValue;
    }
    filterInputNode->setOwner(this, index);
    return index;
}

}  // namespace omnetpp
