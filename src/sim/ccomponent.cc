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
#include "cdoublepar.h" //XXX
#include "cpar.h"
#include "crng.h"


cComponent::cComponent(const char *name) : cDefaultList(name)
{
    componenttype = NULL;
    props = NULL;
    paramv = NULL;
    rngmapsize = 0;
    rngmap = 0;
    ev_enabled = true;
}

cComponent::~cComponent()
{
    delete props;
    for (int i=0; i<params(); i++)
        delete paramv[i];
    delete [] paramv;
    delete [] rngmap;
}

void cComponent::netPack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,eCANTPACK);
}

void cComponent::netUnpack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,eCANTPACK);
}

void cComponent::setComponentType(cComponentType *componenttype)
{
    this->componenttype = componenttype;
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

int cComponent::params() const
{
//XXX    return componenttype->numPars(); //XXX
return 0;
}

const char *cComponent::parName(int k)
{
    if (k<0 || k>=params())
        throw new cRuntimeError(this, "has no parameter #%d", k);
return "";
//XXX    return componenttype->parName(k);
}

cPar& cComponent::par(int k)
{
/*XXX
    if (k<0 || k>=componenttype->numPars())
        throw new cRuntimeError(this, "has no parameter #%d", k);
    return *paramv[k];
*/
return *new cDoublePar();
}

cPar& cComponent::par(const char *parname)
{
/*XXX
    int k = componenttype->findPar(parname);
    if (k<0)
        throw new cRuntimeError(this, "has no parameter called `%s'", parname);
    return *paramv[k];
*/
return *new cDoublePar();
}

int cComponent::findPar(const char *parname) const
{
//XXX    return componenttype->findPar(parname);
return 0;
}

cProperties *cComponent::defaultParProperties(int k) const
{
return NULL;
//XXX    return componenttype->parValue(k)->properties();
}

