//==========================================================================
//   CCOMPONENT.CC  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "ccomponent.h"
#include "ccomponenttype.h"
#include "cmodule.h"
#include "cproperties.h"
#include "cproperty.h"
#include "cpar.h"
#include "cparimpl.h"
#include "crng.h"
#include "cconfiguration.h"
#include "cconfigkey.h"
#include "cdisplaystring.h"
#include "stringutil.h"

USING_NAMESPACE


Register_PerObjectConfigEntry(CFGID_SAVE_AS_SCALAR, "save-as-scalar", CFG_BOOL, "false", "Applicable to module parameters: specifies whether the module parameter should be recorded into the output scalar file. Set it for parameters whose value you'll need for result analysis.");


cComponent::cComponent(const char *name) : cDefaultList(name)
{
    componenttype = NULL;
    rngmapsize = 0;
    rngmap = 0;

    paramvsize = numparams = 0;
    paramv = NULL;

    dispstr = NULL;
    setEvEnabled(true);
}

cComponent::~cComponent()
{
    delete [] rngmap;
    delete [] paramv;
    delete dispstr;
}

const char *cComponent::className() const
{
    // lie about the class name: return the NED type name instead of the real one,
    // that is, return "MobileHost" instead of "cCompoundModule" for example.
    return componenttype ? componenttype->name() : cDefaultList::className();
}

void cComponent::forEachChild(cVisitor *v)
{
    for (int i=0; i<numparams; i++)
        v->visit(&paramv[i]);

    cDefaultList::forEachChild(v);
}

void cComponent::setComponentType(cComponentType *componenttype)
{
    this->componenttype = componenttype;
}

void cComponent::initialize()
{
    // Called before simulation starts (or usually after dynamic module was created).
    // Should be redefined by user.
}

void cComponent::finish()
{
    // Called at the end of a successful simulation (and usually before destroying a dynamic module).
    // Should be redefined by user.
}

void cComponent::handleParameterChange(const char *)
{
    // Called when a module parameter changes.
    // Can be redefined by the user.
}

cProperties *cComponent::properties()
{
    cModule *parent = parentModule();
    cComponentType *type = componentType();
    cProperties *props;
    if (parent)
        props = parent->componentType()->subcomponentProperties(name(), type->fullName());
    else
        props = type->properties();
    return props;
}

void cComponent::reallocParamv(int size)
{
    ASSERT(size>=numparams);
    if (size!=(short)size)
        throw cRuntimeError(this, "reallocParamv(%d): at most %d parameters allowed", size, 0x7fff);
    cPar *newparamv = new cPar[size];
    for (int i=0; i<numparams; i++)
        paramv[i].moveto(newparamv[i]);
    delete [] paramv;
    paramv = newparamv;
    paramvsize = (short)size;
}

void cComponent::addPar(cParImpl *value)
{
    if (findPar(value->name())>=0)
        throw cRuntimeError(this, "addPar(): Parameter %s.%s already present", fullPath().c_str(), value->name());
    if (numparams==paramvsize)
        reallocParamv(paramvsize+1);
    paramv[numparams++].assign(this, value);
}

cPar& cComponent::par(int k)
{
    if (k<0 || k>=numparams)
        throw cRuntimeError(this, "parameter id %d out of range", k);
    return paramv[k];
}

cPar& cComponent::par(const char *parname)
{
    int k = findPar(parname);
    if (k<0)
        throw cRuntimeError(this, "has no parameter named `%s'", parname);
    return paramv[k];
}

int cComponent::findPar(const char *parname) const
{
    int n = params();
    for (int i=0; i<n; i++)
        if (paramv[i].isName(parname))
            return i;
    return -1;
}

void cComponent::finalizeParameters()
{
    // temporarily switch context
    cContextSwitcher tmp(this);
    cContextTypeSwitcher tmp2(CTX_BUILD);

    // read input parameters
    int n = params();
    for (int i=0; i<n; i++)
        par(i).read();

    flags |= FL_PARAMSFINALIZED;
}

bool cComponent::hasDisplayString()
{
    if (dispstr)
        return true;
    if (flags & FL_DISPSTR_CHECKED)
        return flags & FL_DISPSTR_NOTEMPTY;

    // not yet checked: do it now
    cProperties *props = properties();
    cProperty *prop = props->get("display");
    const char *propValue = prop ? prop->value(cProperty::DEFAULTKEY) : NULL;
    bool result = !opp_isempty(propValue);

    flags |= FL_DISPSTR_CHECKED;
    setFlag(FL_DISPSTR_NOTEMPTY, result);
    return result;
}

cDisplayString& cComponent::displayString()
{
    if (!dispstr)
    {
        dispstr = new cDisplayString();
        dispstr->setHostObject(this);

        // set display string (it may depend on parameter values via "$param" references)
        if (!areParamsFinalized())
            throw cRuntimeError(this, "Cannot access display string yet: parameters not yet set up");
        cProperties *props = properties();
        cProperty *prop = props->get("display");
        const char *propValue = prop ? prop->value(cProperty::DEFAULTKEY) : NULL;
        if (propValue)
            dispstr->parse(propValue);
    }
    return *dispstr;
}

// DEPRECATED
void cComponent::setDisplayString(const char *s, bool)
{
    displayString().parse(s);
}

void cComponent::bubble(const char *text)
{
    ev.bubble(this, text);
}

void cComponent::recordParametersAsScalars()
{
    int n = params();
    for (int i=0; i<n; i++)
    {
        if (ev.config()->getAsBool(par(i).fullPath().c_str(), CFGID_SAVE_AS_SCALAR, false))
        {
            //XXX the following checks should probably produce a WARNING not an error
            if (!par(i).isNumeric())
                throw cRuntimeError(this, "cannot record non-numeric parameter `%s' as an output scalar", par(i).name());
            if (par(i).isVolatile() && (par(i).doubleValue()!=par(i).doubleValue() || par(i).doubleValue()!=par(i).doubleValue()))
                throw cRuntimeError(this, "recording volatile parameter `%s' that contains a non-constant value (probably a random variate) as an output scalar -- recorded value is probably just a meaningless random number", par(i).name());
            recordScalar(par(i).name(), par(i).doubleValue());  //XXX mark value specially (as being a "param") in the file?
        }
    }
}

void cComponent::recordScalar(const char *name, double value, const char *unit)
{
    if (!unit)
        ev.recordScalar(this, name, value);
    else {
        opp_string_map attributes;
        attributes["unit"] = unit;
        ev.recordScalar(this, name, value, &attributes);
    }
}



