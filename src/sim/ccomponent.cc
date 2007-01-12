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
#include "cproperties.h"
#include "cpar.h"
#include "cparvalue.h"
#include "crng.h"


cComponent::cComponent(const char *name) : cDefaultList(name)
{
    componenttype = NULL;
    rngmapsize = 0;
    rngmap = 0;
    ev_enabled = true;

    paramvsize = numparams = 0;
    paramv = NULL;
}

cComponent::~cComponent()
{
    delete [] rngmap;
    delete [] paramv;
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

//FIXME TODO redefine forEachChild and traverse parameters too!!!!

void cComponent::handleParameterChange(const char *)
{
    // Called when a module parameter changes.
    // Can be redefined by the user.
}

cProperties *cComponent::properties()
{
    return cComponentType::getPropertiesFor(this);
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

void cComponent::addPar(cParValue *value)
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
        throw cRuntimeError(this, "parameter index %d out of range", k);
    return paramv[k];
}

cPar& cComponent::par(const char *parname)
{
    int k = findPar(parname);
    if (k<0)
        throw cRuntimeError(this, "has no parameter called `%s'", parname);
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
}

