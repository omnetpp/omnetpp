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
#include "cneddeclaration.h"
#include "cproperties.h"
#include "cpar.h"
#include "crng.h"


cComponent::cComponent(const char *name) : cDefaultList(name)
{
    decl = NULL;
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

void cComponent::setDeclaration(cNEDDeclaration *decl)
{
    this->decl = decl;
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
    return decl->numPars(); //XXX
}

const char *cComponent::parName(int k)
{
    if (k<0 || k>=params())
        throw new cRuntimeError(this, "has no parameter #%d", k);
return "";
//XXX    return decl->parName(k);
}

cPar& cComponent::par(int k)
{
    if (k<0 || k>=decl->numPars())
        throw new cRuntimeError(this, "has no parameter #%d", k);
    return *paramv[k];
}

cPar& cComponent::par(const char *parname)
{
    int k = decl->findPar(parname);
    if (k<0)
        throw new cRuntimeError(this, "has no parameter called `%s'", parname);
    return *paramv[k];
}

int cComponent::findPar(const char *parname) const
{
    return decl->findPar(parname);
}

cProperties *cComponent::defaultParProperties(int k) const
{
return NULL;
//XXX    return decl->parValue(k)->properties();
}

