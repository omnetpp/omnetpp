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
#include "cparvalue.h"
#include "cproperties.h"
#include "ccomponent.h"
#include "csimulation.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

cPar::cPar(cComponent *component, cParValue *p)
{
    this->ownercomponent = component;
    this->p = p;
}

cPar::~cPar()
{
/*FIXME
    if (p->owner()==this)
        dropAndDelete(p);
*/
}

const char *cPar::name() const
{
    return p->name();
}

void cPar::copyIfShared()
{
/*FIXME
    if (p->owner()!=this)
        take(p = p->dup());
*/
}

cComponent *cPar::ownerComponent()
{
    return ownercomponent;
}

void cPar::operator=(const cPar& other)
{
    printf("CPAR OP= UNIMPLEMENTED!!!\n");
    //FIXME todo
    // some models need to be able to copy parameters
}

cProperties *cPar::properties() const
{
    return NULL;  //FIXME return it from the type object
}

//FIXME make it inline...
cPar::Type cPar::type() const {return p->type();}

bool cPar::isNumeric() const {return p->isNumeric();}

bool cPar::isVolatile() const {return p->isVolatile();}

bool cPar::isConstant() const {return p->isConstant();}

bool cPar::boolValue() const  {return p->boolValue(ownercomponent);}

long cPar::longValue() const  {return p->longValue(ownercomponent);}

double cPar::doubleValue() const  {return p->doubleValue(ownercomponent);}

const char *cPar::stringValue() const  {return p->stringValue(ownercomponent);}

std::string cPar::stdstringValue() const  {return p->stdstringValue(ownercomponent);}

cXMLElement *cPar::xmlValue() const  {return p->xmlValue(ownercomponent);}

cExpression *cPar::expression() const  {return p->expression();}

std::string cPar::toString() const {return p->toString();}

cPar& cPar::setBoolValue(bool b)
{
    copyIfShared();
    p->setBoolValue(b);
    afterChange();
    return *this;
}

cPar& cPar::setLongValue(long l)
{
    copyIfShared();
    p->setLongValue(l);
    afterChange();
    return *this;
}

cPar& cPar::setDoubleValue(double d)
{
    copyIfShared();
    p->setDoubleValue(d);
    afterChange();
    return *this;
}

cPar& cPar::setStringValue(const char *s)
{
    copyIfShared();
    p->setStringValue(s);
    afterChange();
    return *this;
}

cPar& cPar::setXMLValue(cXMLElement *node)
{
    copyIfShared();
    p->setXMLValue(node);
    afterChange();
    return *this;
}

cPar& cPar::setExpression(cExpression *e)
{
    copyIfShared();
    p->setExpression(e);
    afterChange();
    return *this;
}

void cPar::afterChange()
{
    if (simulation.contextType()==CTX_EVENT) // don't call during build, initialize or finish
    {
        cComponent *owner = ownerComponent();
        if (owner)
        {
            cContextSwitcher tmp(owner);
            owner->handleParameterChange(fullName());
        }
    }

    //XXX set "changed" flag
}

void cPar::read()
{
    printf("    read() of %s\n", fullPath().c_str()); //XXX
    printf("       BEFORE: %s\n", info().c_str()); //XXX

    // obtain value if parameter is not set yet
    if (true /*!isSet()*/) //FIXME check module declaration
        doReadValue();

    // convert non-volatile values to constant
    if (!isVolatile())
        convertToConst();

    // convert "const" subexpressions to constant
    //FIXME TODO -- or better delegate to cExpression

    printf("       AFTER:  %s\n", info().c_str()); //XXX
}


void cPar::convertToConst()
{
    copyIfShared();
    p->convertToConst(ownercomponent);
}

bool cPar::parse(const char *text)
{
    copyIfShared();
    return p->parse(text);
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
        return;
    }

    // maybe we should use default value
    if (p->hasValue() && ev.getParameterUseDefault(simulation.runNumber(), fullPath().c_str()))
        return;

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
}


