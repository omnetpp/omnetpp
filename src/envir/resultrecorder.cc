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

CommonStringPool ResultRecorder::namesPool;


void ResultRecorder::init(cComponent *comp, const char *statsName, const char *recMode)
{
    component = comp;
    statisticName = namesPool.get(statsName);
    recordingMode = namesPool.get(recMode);
}

opp_string_map ResultRecorder::getStatisticAttributes()
{
    opp_string_map result;
    cProperty *property = getComponent()->getProperties()->get("statistic", getStatisticName());
    if (!property)
        return result;

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
    return result;
}

void ResultRecorder::tweakTitle(opp_string& title)
{
    //title = opp_string(getRecordingMode()) + " of " + title;
    title = title + ", " + getRecordingMode();
}

//---

void NumericResultRecorder::receiveSignal(ResultFilter *prev, simtime_t_cref t, long l)
{
    collect(t, l);
}

void NumericResultRecorder::receiveSignal(ResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    collect(t, l);
}

void NumericResultRecorder::receiveSignal(ResultFilter *prev, simtime_t_cref t, double d)
{
    collect(t, d);
}

void NumericResultRecorder::receiveSignal(ResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    collect(t, v.dbl());
}

void NumericResultRecorder::receiveSignal(ResultFilter *prev, simtime_t_cref t, const char *s)
{
    throw cRuntimeError("%s: Cannot convert const char * to double", getClassName());
}

void NumericResultRecorder::receiveSignal(ResultFilter *prev, simtime_t_cref t, cObject *obj)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    throw cRuntimeError("%s: Cannot convert cObject * to double", getClassName());
}

//----

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
                            "or the recorder was not registered with Register_ResultRecorder()", name);
    return p;
}


