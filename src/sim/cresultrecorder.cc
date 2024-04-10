//==========================================================================
//  CRESULTRECORDER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include "common/stlutil.h"
#include "omnetpp/cresultrecorder.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cenum.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

void cResultRecorder::init(Context *ctx)
{
    component = ctx->component;
    statisticName = ctx->statisticName;
    recordingMode = ctx->recordingMode;
    attrsProperty = ctx->attrsProperty;
    manualAttrs = ctx->manualAttrs;
    if ((!attrsProperty) == (!manualAttrs))
        throw cRuntimeError("cResultRecorder::init(): Exactly one of attrsProperty and manualAttrs must be specified");

#ifdef __GNUC__ /* also affects clang */
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

    // call legacy init() method in case the user redefined it
    init(ctx->component, ctx->statisticName, ctx->recordingMode, ctx->attrsProperty, ctx->manualAttrs);

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif
}

cResultRecorder *cResultRecorder::clone() const
{
    ASSERT(!finishCalled);
    cResultRecorder *copy = (cResultRecorder *)createOne(getClassName());
    Context ctx { component, statisticName.c_str(), recordingMode.c_str(), attrsProperty, manualAttrs ? new opp_string_map(*manualAttrs) : nullptr };
    copy->init(&ctx);
    return copy;
}

void cResultRecorder::callFinish(cResultFilter *prev)
{
    if (!finishCalled) {
        finishCalled = true;
        finish(prev);
    }
}

std::string cResultRecorder::getResultName() const
{
    if (getDemuxLabel() == nullptr)
        return std::string(getStatisticName()) + ":" + getRecordingMode();
    else
        return std::string(getDemuxLabel()) + ":" + getStatisticName() + ":" + getRecordingMode();
}

opp_string_map cResultRecorder::getStatisticAttributes()
{
    if (manualAttrs)
        return *manualAttrs;
    return getStatisticAttributesFrom(attrsProperty);
}

static bool hasDisplayNameInPath(cComponent *component)
{
    cComponent *comp = component;
    for (; comp != nullptr; comp = comp->getParentModule())
        if (!opp_isempty(comp->getDisplayName()))
            return true;
    return false;
}

static const char *getDisplayNameOrName(cComponent *component)
{
    const char *displayName = component->getDisplayName();
    return opp_isempty(displayName) ? component->getFullName() : displayName;
}

static std::string getComponentDisplayPath(cComponent *component)
{
    std::string result = getDisplayNameOrName(component);
    for (cModule *m = component->getParentModule(); m != nullptr; m = m->getParentModule())
        result = std::string(getDisplayNameOrName(m)) + "." + result;
    return result;
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

    result["recordingmode"] = getRecordingMode();

    // add display name of owner module/channel
    cComponent *component = getComponent();
    if (hasDisplayNameInPath(component))
        result["moduledisplaypath"] = getComponentDisplayPath(component); // "module" is technically not correct, but probably more intuitive/useful than "component"

    // resolve enumname like cOutVector does
    if (containsKey(result, opp_string("enumname"))) {
        if (containsKey(result, opp_string("enum")))
            throw cRuntimeError(this, "The 'enum' and 'enumname' attributes are mutually exclusive, cannot specify both");
        const char *registeredEnumName = result["enumname"].c_str();
        cEnum *enumDecl = cEnum::find(registeredEnumName); // Note: we could utilize NED file's context namespace for the lookup, but it's not easily accessible from here
        if (!enumDecl)
            throw cRuntimeError(this, "Enum '%s' not found -- is it declared with Register_Enum()?", registeredEnumName);
        result["enum"] = enumDecl->str().c_str();
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
        // but suppress "vector", "histogram" and "stats", as they are obvious
        std::string suffix = getRecordingMode();
        if (suffix == "vector" || suffix == "histogram" || suffix == "stats")
            suffix = "";
        else if (opp_stringbeginswith(suffix.c_str(), "vector(") || opp_stringbeginswith(suffix.c_str(), "histogram(") || opp_stringbeginswith(suffix.c_str(), "stats("))
            suffix = opp_substringbeforelast(opp_substringafter(suffix, "("), ")");
        if (suffix != "")
            title = title + ", " + suffix;
    }
}

//---

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, bool b, cObject *details)
{
    collect(t, b, details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, intval_t l, cObject *details)
{
    collect(t, l, details);
}

void cNumericResultRecorder::receiveSignal(cResultFilter *prev, simtime_t_cref t, uintval_t l, cObject *details)
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

cResultRecorderType::cResultRecorderType(const char *name, const char *className, const char *description)
    : cNoncopyableOwnedObject(name, false), description(opp_nulltoempty(description)), className(className)
{
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

cResultRecorder *cResultRecorderType::create() const
{
    return check_and_cast<cResultRecorder *>(createOne(className.c_str()));
}

}  // namespace omnetpp

