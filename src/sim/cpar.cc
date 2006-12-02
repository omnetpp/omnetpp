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
printf("   cPar ctor, '%s' . %p  '%s'\n", component->fullPath().c_str(), p, p ? p->name() : "null");
}

cPar::~cPar()
{
printf("   cPar dtor DEFUNCT, '%s' . %p '%s'\n", ownercomponent->fullPath().c_str(), p, p ? p->name() : "null");
/*FIXME add back later
    if (!p->isShared())
        delete p;
*/
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
    return p->detailedInfo() + " " + properties()->info();
}

void cPar::copyIfShared()
{
    if (p->isShared())
    {
        p = p->dup();
        p->setIsShared(false);
    }
}

void cPar::reassign(cParValue *newp)
{
    printf("   cPar reassign, '%s' . %p '%s', new='%s'\n", ownercomponent->fullPath().c_str(), p, p ? p->name() : "null", newp ? newp->name() : "null"); //XXX
    ASSERT(newp);
    if (!p->isShared())
        delete p;
    p = newp;
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
    return cComponentType::getPropertiesFor(this);
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
        default:     return "???";
    }
}

// note: the following one-liners should really be inline functions, but
// they can't be put into cpar.h because of declaration order.

std::string cPar::toString() const
{
    return p->toString();
}

cPar::Type cPar::type() const
{
    return p->type();
}

bool cPar::isShared() const
{
    return p->isShared();
}

bool cPar::hasValue() const
{
    return p->hasValue();
}

bool cPar::isNumeric() const
{
    return p->isNumeric();
}

bool cPar::isVolatile() const
{
    return p->isVolatile();
}

bool cPar::isConstant() const
{
    return p->isConstant();
}

bool cPar::boolValue() const
{
    return p->boolValue(ownercomponent);
}

long cPar::longValue() const
{
    return p->longValue(ownercomponent);
}

double cPar::doubleValue() const
{
    return p->doubleValue(ownercomponent);
}

const char *cPar::stringValue() const
{
    return p->stringValue(ownercomponent);
}

std::string cPar::stdstringValue() const
{
    return p->stdstringValue(ownercomponent);
}

cXMLElement *cPar::xmlValue() const
{
    return p->xmlValue(ownercomponent);
}

cExpression *cPar::expression() const
{
    return p->expression();
}

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
        ev.readParameter(this);
        p->setIsInput(false); // clear flag to avoid accidental rereads
    }

    // convert non-volatile values to constant
    if (!isVolatile())
        convertToConst();

    // convert CONST subexpressions into constants
    if (p->containsConstSubexpressions())
    {
        copyIfShared();
        p->evaluateConstSubexpressions(ownercomponent);
    }

    //XXX printf("    %s read() --> %s\n", fullPath().c_str(), info().c_str());
}


void cPar::convertToConst()
{
    copyIfShared();
    p->convertToConst(ownercomponent);
}


bool cPar::parse(const char *text)
{
    // try to share cParValue objects for values coming from the configuration.
    // we use a map, indexed with "moduletypename:paramname:textualvalue".
    // this is possible because because expr representation does not depend on the context
    cComponentType *componentType = ownercomponent->componentType();
    std::string key = std::string(componentType->name()) + ":" + name() + ":" + text;
    cParValue *cachedValue = componentType->parValueCache()->get(key.c_str());
    if (cachedValue)
    {
        // an identical value found in the map -- use it
        //XXX printf("       *** reusing %s ==> %s, cached %s\n", key.c_str(), text, it->second->toString().c_str());
        reassign(cachedValue);
        return true;
    }
    else
    {
        // not found: parse text
        cParValue *tmp = p->dup(); // get object with same type, name etc as original
        if (tmp->parse(text))
        {
            // successfully parsed: install it
            componentType->parValueCache()->put(key.c_str(), tmp);
            reassign(tmp);
            return true;
        }
        else
        {
            // parse error
            delete tmp;
            return false;
        }
    }
}

/*XXX remove
//FIXME shouldn't this function go out into cEnvir??? it only calls public cPar methods!!!
void cPar::doReadValue()
{
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
*/

//---

cParValueCache::~cParValueCache()
{
    for (StringToParMap::iterator it = parMap.begin(); it!=parMap.end(); ++it)
        delete it->second;
    parMap.clear();
}

cParValue *cParValueCache::get(const char *key) const
{
    StringToParMap::const_iterator it = parMap.find(key);
    return it==parMap.end() ? NULL : it->second;
}

void cParValueCache::put(const char *key, cParValue *value)
{
    ASSERT(parMap.find(key)==parMap.end()); // not yet in there
    value->setIsShared(true);
    parMap[key] = value;
}


