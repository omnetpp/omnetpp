//==========================================================================
//  CRESULTRECORDER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cstringtokenizer.h"

using namespace omnetpp::common;

namespace omnetpp {

void cResultRecorder::init(cComponent *comp, const char *statsName, const char *recMode, cProperty *property, opp_string_map *attrs)
{
    component = comp;
    statisticName = getPooled(statsName);
    recordingMode = getPooled(recMode);
    attrsProperty = property;
    manualAttrs = attrs;
    finishCalled = false;
    if ((!attrsProperty) == (!manualAttrs))
        throw cRuntimeError("cResultRecorder::init(): Exactly one of attrsProperty and manualAttrs must be specified");
}

void cResultRecorder::callFinish(cResultFilter *prev)
{
    if (!finishCalled) {
        finishCalled = true;
        finish(prev);
    }
}

opp_string_map cResultRecorder::getStatisticAttributes()
{
    if (manualAttrs)
        return *manualAttrs;
    return getStatisticAttributesFrom(attrsProperty);
}

opp_string_map cResultRecorder::getStatisticAttributesFrom(cProperty *property)
{
    opp_string_map result;

    // fill result[] from the properties
    const std::vector<const char *>& keys = property->getKeys();
    for (auto key : keys) {
        if (!strcmp(key, "record"))
            continue;  // no need to save record= key
        int numValues = property->getNumValues(key);
        if (numValues == 0)
            result[key] = "";
        else if (numValues == 1)
            result[key] = property->getValue(key, 0);
        else {
            std::string buf;
            for (int j = 0; j < numValues; j++) {
                if (j > 0)
                    buf += ",";
                buf += property->getValue(key, j);
            }
            result[key] = buf;
        }

        if (strcmp(key, "title") == 0)
            tweakTitle(result[key]);
    }
    return result;
}

static std::string getPart(const char *s, int k)
{
    if (k >= 0) {
        cStringTokenizer tokenizer(s, ":");
        for (const char *token; (token = tokenizer.nextToken()) != nullptr; k--)
            if (k == 0)
                return token;
    }
    return "";
}

namespace {
struct Resolver : public opp_substitutevariables_resolver
{
    cResultRecorder *self;
    Resolver(cResultRecorder *self) : self(self) {}
    virtual bool isVariableNameChar(char c) override
    {
        return opp_isalnum(c) || c == '_';
    }

    virtual std::string operator()(const std::string& name) override
    {
        if (name == "name")
            return self->getStatisticName();
        else if (name == "mode")
            return self->getRecordingMode();
        else if (name == "component")
            return self->getComponent()->getFullPath();
        else if (opp_stringbeginswith(name.c_str(), "namePart") && opp_isdigit(name.c_str()[8]))
            return getPart(self->getStatisticName(), strtol(name.c_str() + 8, nullptr, 10) - 1);
        else
            throw cRuntimeError("Unknown variable $%s", name.c_str());
    }
};
}

void cResultRecorder::tweakTitle(opp_string& title)
{
    if (strchr(title.c_str(), '$')) {
        Resolver resolver(this);
        title = opp_substitutevariables(title.c_str(), resolver).c_str();
    }
    else {
        // if title didn't make use of any $ macro, just add recording mode after a comma as default
        title = title + ", " + getRecordingMode();
    }
}

//---

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    collect(t, b, details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l, cObject *details)
{
    collect(t, l, details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l, cObject *details)
{
    collect(t, l, details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d, cObject *details)
{
    collect(t, d, details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v, cObject *details)
{
    collect(t, v.dbl(), details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s, cObject *details)
{
    throw cRuntimeError("%s: Cannot convert const char * to double", getClassName());
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj, cObject *details)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    throw cRuntimeError("%s: Cannot convert cObject * to double", getClassName());
}

//----

cResultRecorderType::cResultRecorderType(const char *name, cResultRecorder *(*f)())
    : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
}

cResultRecorderType *cResultRecorderType::find(const char *name)
{
    return dynamic_cast<cResultRecorderType *>(resultRecorders.getInstance()->lookup(name));
}

cResultRecorderType *cResultRecorderType::get(const char *name)
{
    cResultRecorderType *p = find(name);
    if (!p)
        throw cRuntimeError("Result recorder \"%s\" not found -- perhaps the name is wrong, "
                            "or the recorder was not registered with Register_ResultRecorder()", name);
    return p;
}

}  // namespace omnetpp

