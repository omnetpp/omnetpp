//=========================================================================
//  CPAR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cpar.h"
#include "cproperties.h"
#include "ccomponent.h"
#include "cneddeclaration.h"
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif


cPar::cPar()
{
    props = NULL;
}

cPar::~cPar()
{
    // subclasses must call beforeChange() in their destructor!
    // we cannot call *their* virtual function any more from here.
    delete props;
}

cPar& cPar::operator=(const cPar& other)
{
    // Note: subclasses should call beforeChange()/afterChange()
    flags = other.flags;
    return *this;
}

void cPar::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cPar::netUnpack(cCommBuffer *buffer)
{
    //TBD
}

cComponent *cPar::ownerComponent()
{
    return dynamic_cast<cComponent *>(owner());
}

cProperties *cPar::properties() const
{
    // props may actually point into the declaration object
    return props;
}

cProperties *cPar::unlockProperties()
{
    if (!props) //FIXME
    {
        props = properties()->dup();
        props->setOwner(this);
    }
    return props;
}

void cPar::beforeChange()
{
}

void cPar::afterChange()
{
    flags |= FL_ISSET | FL_HASCHANGED;

    if (simulation.contextType()==CTX_EVENT) // don't call during build, initialize or finish
    {
        cComponent *owner = ownerComponent();
        if (owner)
        {
            cContextSwitcher tmp(owner);
            owner->handleParameterChange(fullName());
        }
    }
}

bool cPar::changed()
{
    bool ret = flags & FL_HASCHANGED;
    flags &= ~FL_HASCHANGED;
    return ret;
}

void cPar::markAsUnset()
{
    // if the value got set prior to this call, that will be the default value
    if (flags & FL_ISSET)
        flags |= FL_HASDEFAULT;
    flags &= ~FL_ISSET;
}

void cPar::applyDefaultValue()
{
    // if it had a default value set, use that, otherwise the param will remain unset
    if (flags & FL_HASDEFAULT)
        flags |= FL_ISSET;
}


bool cPar::equals(cPar& other)
{
    if (!isSet() || !other.isSet() || type()!=other.type())
        return false;

    switch (type())
    {
        case 'B': return boolValue()==other.boolValue();
        case 'L': return longValue()==other.longValue();
        case 'D': return doubleValue()==other.doubleValue();
        case 'S': return stringValue()==other.stringValue();
        case 'X': return xmlValue()==other.xmlValue();
        default: return false;
    }
}

int cPar::cmpbyvalue(cPar *one, cPar *other)
{
    double x = one->doubleValue() - other->doubleValue();
    return sgn(x);
}

