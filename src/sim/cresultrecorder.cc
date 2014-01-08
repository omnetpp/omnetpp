//==========================================================================
//  CRESULTRECORDER.CC - part of
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

#include "cresultrecorder.h"
#include "cproperty.h"
#include "chistogram.h"
#include "cstringtokenizer.h"
#include "../common/opp_ctype.h"
#include "../common/stringutil.h"

NAMESPACE_BEGIN


void cResultRecorder::init(cComponent *comp, const char *statsName, const char *recMode, cProperty *property, opp_string_map *attrs)
{
    component = comp;
    statisticName = getPooled(statsName);
    recordingMode = getPooled(recMode);
    attrsProperty = property;
    manualAttrs = attrs;
    if ((!attrsProperty) == (!manualAttrs))
        throw cRuntimeError("cResultRecorder::init(): exactly one of attrsProperty and manualAttrs must be specified");
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

static std::string getPart(const char *s, int k)
{
    if (k >= 0) {
        cStringTokenizer tokenizer(s, ":");
        for (const char *token; (token = tokenizer.nextToken()) != NULL; k--)
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
    virtual bool isVariableNameChar(char c) {
        return opp_isalnum(c) || c=='_';
    }
    virtual std::string operator()(const std::string& name) {
        if (name == "name")
            return self->getStatisticName();
        else if (name == "mode")
            return self->getRecordingMode();
        else if (name == "component")
            return self->getComponent()->getFullPath();
        else if (opp_stringbeginswith(name.c_str(), "namePart") && opp_isdigit(name.c_str()[8]))
            return getPart(self->getStatisticName(), strtol(name.c_str()+8, NULL, 10) - 1);
        else
            throw cRuntimeError("unknown variable $%s", name.c_str());
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

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b)
{
    collect(t, b);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, long l)
{
    collect(t, l);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, unsigned long l)
{
    collect(t, l);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, double d)
{
    collect(t, d);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, const SimTime& v)
{
    collect(t, v.dbl());
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, const char *s)
{
    throw cRuntimeError("%s: Cannot convert const char * to double", getClassName());
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, cObject *obj)
{
    // note: cITimestampedValue stuff was already dispatched to (simtime_t,double) method in base class
    throw cRuntimeError("%s: Cannot convert cObject * to double", getClassName());
}

//----

cResultRecorderDescriptor::cResultRecorderDescriptor(const char *name, cResultRecorder *(*f)())
  : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
}

cResultRecorderDescriptor *cResultRecorderDescriptor::find(const char *name)
{
    return dynamic_cast<cResultRecorderDescriptor *>(resultRecorders.getInstance()->lookup(name));
}

cResultRecorderDescriptor *cResultRecorderDescriptor::get(const char *name)
{
    cResultRecorderDescriptor *p = find(name);
    if (!p)
        throw cRuntimeError("Result recorder \"%s\" not found -- perhaps the name is wrong, "
                            "or the recorder was not registered with Register_ResultRecorder()", name);
    return p;
}

NAMESPACE_END

