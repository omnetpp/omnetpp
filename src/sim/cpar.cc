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
#include "cproperty.h"
#include "ccomponent.h"
#include "ccomponenttype.h"
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
    if (!p->isShared())
        delete p;
}

const char *cPar::name() const
{
    return p->name();
}

std::string cPar::info() const
{
    return p->info();
}

std::string cPar::detailedInfo() const
{
    return p->detailedInfo() + properties()->info();
}

void cPar::copyIfShared()
{
    if (p->isShared())
    {
        p = p->dup();
        p->setIsShared(false);
    }
}

cObject *cPar::owner() const
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
    cComponentType *type = ownercomponent->componentType();
    return type->parProperties(name());
}

const char *cPar::typeName(Type t)
{
    switch (t)
    {
        case BOOL:   return "bool";
        case DOUBLE: return "double";
        case LONG:   return "long";
        case STRING: return "string";
        case XML:    return "xml";
        defult:      return "???";
    }
}


//FIXME make it inline...
cPar::Type cPar::type() const {return p->type();}

bool cPar::isShared() const {return p->isShared();}

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
        ASSERT(ownercomponent);
        cContextSwitcher tmp(ownercomponent);
        ownercomponent->handleParameterChange(fullName());
    }
}

void cPar::read()
{
    // obtain value if parameter is not set yet
    if (p->isInput())
    {
        doReadValue();
        p->setIsInput(false); // clear flag to avoid further confusion
    }

    // convert non-volatile values to constant
    if (!isVolatile())
        convertToConst();

    // convert "const" subexpressions to constant
    //FIXME TODO -- or better delegate to cExpression

    printf("    %s read() --> %s\n", fullPath().c_str(), info().c_str()); //XXX
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
        cProperties *props = properties();
        cProperty *prop = props->get("prompt");
		std::string prompt = prop ? prop->value(cProperty::DEFAULTKEY) : "";
        std::string reply;
        if (!prompt.empty())
            reply = ev.gets(prompt.c_str(), toString().c_str());
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


