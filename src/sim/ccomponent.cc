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
#include "crng.h"
#include "cmodule.h"


cComponent::cComponent(const char *name) : cDefaultList(name)
{
    componenttype = NULL;
    props = NULL;
    rngmapsize = 0;
    rngmap = 0;
    ev_enabled = true;
}

cComponent::~cComponent()
{
    delete props; //XXX really?
    delete [] rngmap;
    for (int i=0; i<params(); i++)
        delete paramv[i];
}

const char *cComponent::className() const
{
    // lie about the class name: return the NED type name instead of the real one,
    // that is, "MobileHost" instead of "cCompoundModule" for example.
    return componenttype ? componenttype->name() : cDefaultList::className();
}

void cComponent::setComponentType(cComponentType *componenttype)
{
    this->componenttype = componenttype;
}

cModule *cComponent::parentModule() const  
{
    return dynamic_cast<cModule *>(owner());
}

void cComponent::handleParameterChange(char const *)
{
}

cProperties *cComponent::properties()
{
    return props;
}

cProperties *cComponent::unlockProperties()
{
    if (!props)  //FIXME it's always non-NULL!!!
    {
        props = properties()->dup();
        props->setOwner(this);
    }
    return props;
}

void cComponent::addPar(const char *parname, cPar *par)
{
    if (findPar(parname)>=0)
       throw new cRuntimeError(this, "addPar(): Parameter %s.%s already present", fullPath().c_str(), parname);
    par->setName(parname);
    take(par);
    paramv.push_back(par);
}

cPar& cComponent::par(int k)
{
    if (k<0 || k>=paramv.size())
        throw new cRuntimeError(this, "parameter index %d out of range", k);
    return *paramv[k];
}

cPar& cComponent::par(const char *parname)
{
    int k = findPar(parname);
    if (k<0)
        throw new cRuntimeError(this, "has no parameter called `%s'", parname);
    return *paramv[k];
}

int cComponent::findPar(const char *parname) const
{
    int n = params();
    for (int i=0; i<n; i++)
        if (paramv[i]->isName(parname))
            return i;
    return -1;
}

void cComponent::readInputParams()
{
    printf("  readInputParams of %s:\n", fullPath().c_str());//XXX
    int n = params();
    for (int i=0; i<n; i++)
        if (!par(i).isSet())
            par(i).read();
}

