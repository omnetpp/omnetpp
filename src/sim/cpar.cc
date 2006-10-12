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
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif


//FIXME longValue() etc methods in subclasses should check isInput() flag!

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
    // Note: subclasses should call beforeChange()/afterChange()!
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

cPar *cPar::dup() const
{
    throw new cRuntimeError(this, eCANTDUP);  // cannot dup because this is an abstract class
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
    flags |= FL_HASCHANGED;

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


void cPar::read()
{
    printf("    read() of %s\n", fullPath().c_str()); //XXX

    // get the value
    doReadValue();
    ASSERT(isSet()); // part of doReadValue()'s job

    // convert non-volatile values to constant
    if (isVolatile())
        convertToConst();

    // convert "const" subexpressions to constant
    //FIXME TODO -- or better delegate to cExpression
}


void cPar::doReadValue()
{

//FIXME shouldn't most of this go out into cEnvir???

    // get it from ini file
    std::string str = ev.getParameter(simulation.runNumber(), fullPath().c_str());
    if (!str.empty())
    {
        bool success = parse(str.c_str());
        if (!success)
            throw new cRuntimeError("Wrong value `%s' for parameter `%s'", str.c_str(), fullPath().c_str());
    }
    if (isSet())
        return;

    // maybe we should use default value
    if (hasDefaultValue() && ev.getParameterUseDefault(simulation.runNumber(), fullPath().c_str()))
    {
        applyDefaultValue();
        return;
    }

    // otherwise, we have to ask the user
    bool success = false;
    while (!success)
    {
        std::string promptstr; //FIXME fill this in from @prompt property
        std::string reply;
        if (!promptstr.empty())
            reply = ev.gets(promptstr.c_str(), toString().c_str());
        else
            reply = ev.gets((std::string("Enter parameter `")+fullPath()+"':").c_str(), toString().c_str());

        try {
            success = false;
            success = parse(reply.c_str());
            if (!success)
                throw new cRuntimeError("Syntax error, please try again.");
        }
        catch (cException *e) {
            ev.printfmsg("%s", e->message());
            delete e;
        }
    }
    applyDefaultValue();
}

//----
#include "cboolpar.h"
#include "clongpar.h"
#include "cdoublepar.h"
#include "cstringpar.h"
#include "cxmlpar.h"

cPar *cPar::createWithType(Type type)
{
    switch (type)
    {
        case BOOL:    return new cBoolPar();
        case DOUBLE:  return new cDoublePar();
        case LONG:    return new cLongPar();
        case STRING:  return new cStringPar();
        case XML:     return new cXMLPar();
        default:      throw new cRuntimeError("cPar::createWithType(): no such type: %d", type);
    }
}

