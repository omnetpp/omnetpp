//==========================================================================
//  RESULTRECORDER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "resultrecorder.h"
#include "cproperty.h"
#include "chistogram.h"


cGlobalRegistrationList resultRecorders("resultRecorders");

CommonStringPool ResultRecorder::statisticNamesPool;


ResultRecorderDescriptor::ResultRecorderDescriptor(const char *name, ResultRecorder *(*f)())
  : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
}

ResultRecorderDescriptor *ResultRecorderDescriptor::find(const char *name)
{
    return dynamic_cast<ResultRecorderDescriptor *>(resultRecorders.getInstance()->lookup(name));
}

ResultRecorderDescriptor *ResultRecorderDescriptor::get(const char *name)
{
    ResultRecorderDescriptor *p = find(name);
    if (!p)
        throw cRuntimeError("Result recorder \"%s\" not found -- perhaps the name is wrong, "
                            "or the recorder wasn't registered with Register_ResultRecorder()", name);
    return p;
}

//----

void ResultRecorder::init(cComponent *comp, const char *statsName)
{
    component = comp;
    statisticName = statisticNamesPool.get(statsName);
}

void ResultRecorder::listenerAdded(cComponent *component, simsignal_t signalID)
{
    ASSERT(getSubscribeCount() == 1);  // may only be subscribed once (otherwise results get mixed)
}

void ResultRecorder::listenerRemoved(cComponent *component, simsignal_t signalID)
{
    if (getSubscribeCount() == 0)
        delete this;
}

std::string ResultRecorder::makeName(const char *suffix)
{
    return std::string(statisticName) + ":" + suffix;
}

void ResultRecorder::extractStatisticAttributes(cComponent *component, opp_string_map& result)
{
    cProperty *property = component->getProperties()->get("statistic", getStatisticName());
    if (!property)
        return;

    // fill result[] from the properties
    const std::vector<const char *>& keys = property->getKeys();
    for (int i = 0; i < (int)keys.size(); i++)
    {
        const char *key = keys[i];
        if (!strcmp(key, "record"))
            continue; // no need to save record= key
        int numValues = property->getNumValues(key);
        if (numValues == 0)
            result[key] = "";
        else if (numValues == 1)
            result[key] = property->getValue(key, 0);
        else {
            std::string buf;
            for (int j = 0; j < numValues; j++) {
                if (j > 0) buf += ",";
                buf += property->getValue(key, j);
            }
            result[key] = buf;
        }

        if (strcmp(key,"title")==0)
            tweakTitle(result[key]);
    }
}

//---

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    collect(l);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    collect(d);
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t d)
{
    collect(SIMTIME_DBL(d));
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    throw cRuntimeError("cannot convert const char * to double"); //FIXME better message
}

void NumericResultRecorder::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    cISignalValue *v = dynamic_cast<cISignalValue *>(obj);
    if (!v)
        throw cRuntimeError("cannot convert cObject * to double"); //FIXME better message

    collect(v->getSignalTime(signalID), v->getSignalValue(signalID));
}


