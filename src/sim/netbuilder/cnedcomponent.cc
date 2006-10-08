//==========================================================================
// CNEDCOMPONENT.CC -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cnedcomponent.h"
#include "cexception.h"
#include "cpar.h"
#include "cproperties.h"


cNEDComponent::ParamDescription::ParamDescription()
{
    type = cPar::DOUBLE;
    isVolatile = false;
    value = NULL;
    properties = NULL;
}

cNEDComponent::ParamDescription cNEDComponent::ParamDescription::deepCopy() const
{
    ParamDescription tmp = *this;
    if (tmp.value)
        tmp.value = (cPar *) tmp.value->dup();
    if (tmp.properties)
        tmp.properties = tmp.properties->dup();
    return tmp;
}

cNEDComponent::GateDescription::GateDescription()
{
    type = cGate::INPUT;
    isVector = false;
    gatesize = NULL;
    properties = NULL;
}

cNEDComponent::GateDescription cNEDComponent::GateDescription::deepCopy() const
{
    GateDescription tmp = *this;
    if (tmp.gatesize)
        tmp.gatesize = (cPar *) tmp.gatesize->dup();
    if (tmp.properties)
        tmp.properties = tmp.properties->dup();
    return tmp;
}

//-----

cNEDComponent::cNEDComponent(const char *name, NEDElement *tree) :
cNoncopyableObject(name), NEDComponent(tree)
{
    props = NULL;
    locked = false;
}

cNEDComponent::~cNEDComponent()
{
    for (int i=0; i<params.size(); i++)
        delete params[i].value;
    for (int i=0; i<gates.size(); i++)
        delete gates[i].gatesize;
}

std::string cNEDComponent::info() const
{
    return ""; //FIXME todo
}

std::string cNEDComponent::detailedInfo() const
{
    return ""; //FIXME todo
}

void cNEDComponent::addExtendsName(const char *name)
{
    if (locked)
        throw new cRuntimeError(this, "addExtendsName(): too late, object already locked");
    extendsnames.push_back(name);
}

void cNEDComponent::addInterfaceName(const char *name)
{
    if (locked)
        throw new cRuntimeError(this, "addInterfaceName(): too late, object already locked");
    interfacenames.push_back(name);
}

void cNEDComponent::addPar(const ParamDescription& paramDesc)
{
    if (locked)
        throw new cRuntimeError(this, "addPar(): too late, object already locked");
    paramNameMap[paramDesc.name] = params.size();
    params.push_back(paramDesc);
    if (paramDesc.value)
        take(paramDesc.value);
}

void cNEDComponent::addGate(const GateDescription& gateDesc)
{
    if (locked)
        throw new cRuntimeError(this, "addGate(): too late, object already locked");
    gateNameMap[gateDesc.name] = gates.size();
    gates.push_back(gateDesc);
    if (gateDesc.gatesize)
        take(gateDesc.gatesize);
}

void cNEDComponent::setProperties(cProperties *p)
{
    if (locked)
        throw new cRuntimeError(this, "setProperties(): too late, object already locked");
    if (props)
        throw new cRuntimeError(this, "setProperties(): properties already set");
    p = props;
    props->setOwner(this);
    props->lock();
}

const char *cNEDComponent::interfaceName(int k) const
{
    if (k<0 || k>=interfacenames.size())
        throw new cRuntimeError(this, "interface index %d out of range 0..%d", k, interfacenames.size()-1);
    return interfacenames[k].c_str();
}

const char *cNEDComponent::extendsName(int k) const
{
    if (k<0 || k>=extendsnames.size())
        throw new cRuntimeError(this, "extendsName(): index %d out of range 0..%d", k, extendsnames.size()-1);
    return extendsnames[k].c_str();
}

int cNEDComponent::numPars() const
{
    return params.size();
}

cProperties *cNEDComponent::properties()
{
    return props;
}

const cNEDComponent::ParamDescription& cNEDComponent::paramDescription(int k) const
{
    if (k<0 || k>=params.size())
        throw new cRuntimeError(this, "parameter index %d out of range 0..%d", k, params.size()-1);
    return params[k];
}

const cNEDComponent::ParamDescription& cNEDComponent::paramDescription(const char *name) const
{
    int k = findPar(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such parameter: %s", name);
    return params[k];
}

const cNEDComponent::GateDescription& cNEDComponent::gateDescription(int k) const
{
    if (k<0 || k>=gates.size())
        throw new cRuntimeError(this, "gate index %d out of range 0..%d", k, gates.size()-1);
    return gates[k];
}

const cNEDComponent::GateDescription& cNEDComponent::gateDescription(const char *name) const
{
    int k = findGate(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such gate: %s", name);
    return gates[k];
}

int cNEDComponent::findPar(const char *parname) const
{
    StringToIntMap::const_iterator i = paramNameMap.find(parname);
    if (i==paramNameMap.end())
        return -1;
    return i->second;
}

void cNEDComponent::setParamValue(const char *name, cPar *value)
{
    if (locked)
        throw new cRuntimeError(this, "setParamValue(): too late, object already locked");
    int k = findPar(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such parameter: %s", name);

    ParamDescription& desc = params[k];
    if (desc.value)
        delete desc.value;
    desc.value = value;
}

int cNEDComponent::numGates() const
{
    return gates.size();
}

int cNEDComponent::findGate(const char *name) const
{
    StringToIntMap::const_iterator i = gateNameMap.find(name);
    if (i==gateNameMap.end())
        return -1;
    return i->second;
}

void cNEDComponent::setGateSize(const char *name, cPar *gateSize)
{
    if (locked)
        throw new cRuntimeError(this, "setGateSize(): too late, object already locked");
    int k = findGate(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such gate: %s", name);

    GateDescription& desc = gates[k];
    if (desc.gatesize)
        delete desc.gatesize;
    desc.gatesize = gateSize;
}


//-------------------

/*
void cNEDComponent::assertParMatchesBase(cPar *par, const cPar *basepar,
                                                const char *parname, const char *basename)
{
    if (basepar->isSet() && par->isSet())
        throw new cRuntimeError(this, "parameter `%s' already set in base `%s', it cannot be overridden", parname, basename);

    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw new cRuntimeError(this, "type of parameter `%s' differs from that in base `%s'", parname, basename);
}

bool cNEDComponent::parMatchesInterface(cPar *par, const cPar *ifpar)
{
    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw new cRuntimeError(this, "type of parameter `%s' differs from that in interface `%s'", parname, interfacename);

    if (ifpar->isSet()) // values must match as well
    {
        if (!par->isSet())
            throw new cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);

        if (ifpar->isConstant())
        {
            if (!par->isConstant())
                throw new cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);
            if (!const_cast<cPar&>(ifpar).equals(*par))   //FIXME const_cast -- eliminate
                throw new cRuntimeError(this, "parameter `%s' must have the same value as in interface `%s'", parname, interfacename);
        }
        else
        {
            // we accept expressions without any check -- comparison cannot be performed 100% accurately
        }
    }
}
*/

/*
void cInterfaceNEDDeclaration::resolve()
{
...
    // verify compliance to interfaces
    for (int i=0; i<interfacenames.size(); i++)
    {
        const char *interfacename = interfacenames[i].c_str();

        // find and resolve interface declaration
        cInterfaceNEDDeclaration *ifdecl = findInterfaceDeclaration(interfacename);
        if (!ifdecl)
            throw new cRuntimeError(this, "declaration for interface `%s' not found", interfacename);
        if (!ifdecl->isResolved())
            ifdecl->resolve();

        // check parameters
        for (int i=0; i<ifdecl->numPars(); i++)
        {
            int k = findPar(ifdecl->parName(i));
            if (k<0)
                throw new cRuntimeError(this, "has no parameter `%s' required by interface `%s'", ifdecl->parName(i), interfacename);
            verifyParameterMatch(params[k].value, ifdecl->par(i), ifdecl->parName(i), interfacename);
        }

        // check gates
        for (int i=0; i<ifdecl->numGates(); i++)
        {
            int k = findGate(ifdecl->gateName(i));
            if (k<0)
                throw new cRuntimeError(this, "has no gate `%s' required by interface `%s'", ifdecl->gateName(i), interfacename);
            GateDescription& g = gates[k];
            if (g.type != ifdecl->gateType(i))
                throw new cRuntimeError(this, "type of gate `%s' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
            if (g.isvector != ifdecl->gateIsVector(i))
                throw new cRuntimeError(this, "vectorness of gate `%s' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
            if (ifdecl->gateSize(i)!=-1 && g.gatesize!=ifdecl->gateSize(i))
                throw new cRuntimeError(this, "size of gate vector `%s[]' differs from that in interface `%s'", ifdecl->gateName(i), interfacename);
        }
    }
}
*/

