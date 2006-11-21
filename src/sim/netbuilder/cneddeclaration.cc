//==========================================================================
// CNEDDECLARATION.CC -
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

#include "cneddeclaration.h"
#include "cexception.h"
#include "cproperties.h"
#include "cdynamicexpression.h"
#include "ned2generator.h"
#include "nederror.h"


cNEDDeclaration::ParamDescription::ParamDescription()
{
    value = NULL;
    properties = new cProperties();  //FIXME add destructor!!!
}

cNEDDeclaration::ParamDescription cNEDDeclaration::ParamDescription::deepCopy() const
{
    ParamDescription tmp = *this;
    if (tmp.value)
        tmp.value = tmp.value->dup();
    if (tmp.properties)
        tmp.properties = tmp.properties->dup();
    return tmp;
}

cNEDDeclaration::GateDescription::GateDescription()
{
    type = cGate::INPUT;
    isVector = false;
    gatesize = NULL;
    properties = new cProperties();  //FIXME add destructor!!!
}

cNEDDeclaration::GateDescription cNEDDeclaration::GateDescription::deepCopy() const
{
    GateDescription tmp = *this;
    if (tmp.gatesize)
        tmp.gatesize = tmp.gatesize->dup();
    if (tmp.properties)
        tmp.properties = tmp.properties->dup();
    return tmp;
}

//-----

cNEDDeclaration::cNEDDeclaration(const char *name, NEDElement *tree) :
cNoncopyableOwnedObject(name), NEDComponent(tree)
{
    locked = false;   //FIXME check if locking makes sense
    props = new cProperties();
}

cNEDDeclaration::~cNEDDeclaration()
{
    for (int i=0; i<params.size(); i++)
        delete params[i].value;
    for (int i=0; i<gates.size(); i++)
        delete gates[i].gatesize;
    delete props;
}

std::string cNEDDeclaration::info() const
{
    std::stringstream out;
    if (numExtendsNames()>0)
    {
        out << "extends ";
        for (int i=0; i<numExtendsNames(); i++)
            out << (i?", ":"") << extendsName(i);
        out << "  ";
    }

    if (numInterfaceNames()>0)
    {
        out << "like ";
        for (int i=0; i<numInterfaceNames(); i++)
            out << (i?", ":"") << interfaceName(i);
        out << "  ";
    }

    if (!implClassName.empty())
        out << "C++ class: " << implClassName;

    return out.str();
}

std::string cNEDDeclaration::detailedInfo() const
{
    std::stringstream out;
    out << "Name: " << name() << "\n";
    if (numExtendsNames()>0)
    {
        out << "Extends: ";
        for (int i=0; i<numExtendsNames(); i++)
            out << (i?", ":"") << extendsName(i);
        out << "\n";
    }

    if (numInterfaceNames()>0)
    {
        out << "Like: ";
        for (int i=0; i<numInterfaceNames(); i++)
            out << (i?", ":"") << interfaceName(i);
        out << "\n";
    }

    if (!implClassName.empty())
        out << "C++ class: " << implClassName << "\n";

    out << "Properties (incl. inherited ones):\n";
    out << "  " << properties()->info() << "\n";

    out << "Parameters (incl. inherited ones):\n";
    if (params.size()==0) out << "  none\n";
    for (int i=0; i<params.size(); i++)
    {
        const ParamDescription& desc = paramDescription(i);
        out << "  param " << desc.value->name() << " INPUT:" << (desc.isInput?"true":"false");
        out << " value=" << desc.value->info() << ", ISEXPR=" << (desc.value->isConstant()?0:1) << ", " << desc.value->toString()
            << " " << desc.properties->info() << "\n";
    }

    out << "Gates (incl. inherited ones):\n";
    if (gates.size()==0) out << "  none\n";
    for (int i=0; i<gates.size(); i++)
    {
        const GateDescription& desc = gateDescription(i);
        out << "  gate " << desc.name << ": ..."
            << " " << desc.properties->info() << "\n";
    }
    out << "________________________________\n\n";

    // NED
    NEDErrorStore errors;
    generateNED2(out, getTree(), &errors);

    return out.str();
}

std::string cNEDDeclaration::declaration() const
{
    std::stringstream out;
    NEDErrorStore errors;
    generateNED2(out, getTree(), &errors);
    return out.str();
}

void cNEDDeclaration::addExtendsName(const char *name)
{
    if (locked)
        throw new cRuntimeError(this, "addExtendsName(): too late, object already locked");
    extendsnames.push_back(name);
}

void cNEDDeclaration::addInterfaceName(const char *name)
{
    if (locked)
        throw new cRuntimeError(this, "addInterfaceName(): too late, object already locked");
    interfacenames.push_back(name);
}

void cNEDDeclaration::setImplementationClassName(const char *name)
{
    implClassName = name ? name : "";
}

void cNEDDeclaration::addPar(const ParamDescription& paramDesc)
{
    if (locked)
        throw new cRuntimeError(this, "addPar(): too late, object already locked");
    paramNameMap[paramDesc.value->name()] = params.size();
    params.push_back(paramDesc);
}

void cNEDDeclaration::addGate(const GateDescription& gateDesc)
{
    if (locked)
        throw new cRuntimeError(this, "addGate(): too late, object already locked");
    gateNameMap[gateDesc.name] = gates.size();
    gates.push_back(gateDesc);
}

void cNEDDeclaration::setProperties(cProperties *p)
{
    if (p == props)
        return;
    if (locked)
        throw new cRuntimeError(this, "setProperties(): too late, object already locked");
    delete props;
    props = p;
    props->setOwner(this);
    props->lock();
}

const char *cNEDDeclaration::interfaceName(int k) const
{
    if (k<0 || k>=interfacenames.size())
        throw new cRuntimeError(this, "interface index %d out of range 0..%d", k, interfacenames.size()-1);
    return interfacenames[k].c_str();
}

const char *cNEDDeclaration::extendsName(int k) const
{
    if (k<0 || k>=extendsnames.size())
        throw new cRuntimeError(this, "extendsName(): index %d out of range 0..%d", k, extendsnames.size()-1);
    return extendsnames[k].c_str();
}

const char *cNEDDeclaration::implementationClassName() const
{
    return implClassName.empty() ? NULL : implClassName.c_str();
}

int cNEDDeclaration::numPars() const
{
    return params.size();
}

cProperties *cNEDDeclaration::properties() const
{
    return props;
}

const cNEDDeclaration::ParamDescription& cNEDDeclaration::paramDescription(int k) const
{
    if (k<0 || k>=params.size())
        throw new cRuntimeError(this, "parameter index %d out of range 0..%d", k, params.size()-1);
    return params[k];
}

const cNEDDeclaration::ParamDescription& cNEDDeclaration::paramDescription(const char *name) const
{
    int k = findPar(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such parameter: %s", name);
    return params[k];
}

const cNEDDeclaration::GateDescription& cNEDDeclaration::gateDescription(int k) const
{
    if (k<0 || k>=gates.size())
        throw new cRuntimeError(this, "gate index %d out of range 0..%d", k, gates.size()-1);
    return gates[k];
}

const cNEDDeclaration::GateDescription& cNEDDeclaration::gateDescription(const char *name) const
{
    int k = findGate(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such gate: %s", name);
    return gates[k];
}

int cNEDDeclaration::findPar(const char *parname) const
{
    StringToIntMap::const_iterator i = paramNameMap.find(parname);
    if (i==paramNameMap.end())
        return -1;
    return i->second;
}

int cNEDDeclaration::numGates() const
{
    return gates.size();
}

int cNEDDeclaration::findGate(const char *name) const
{
    StringToIntMap::const_iterator i = gateNameMap.find(name);
    if (i==gateNameMap.end())
        return -1;
    return i->second;
}

void cNEDDeclaration::setGateSize(const char *name, cDynamicExpression *gatesize)
{
    if (locked)
        throw new cRuntimeError(this, "setGateSize(): too late, object already locked");
    int k = findGate(name);
    if (k==-1)
        throw new cRuntimeError(this, "no such gate: %s", name);

    GateDescription& desc = gates[k];
    delete desc.gatesize;
    desc.gatesize = gatesize;
}

SubmodulesNode *cNEDDeclaration::getSubmodules()
{
    return (SubmodulesNode *)getTree()->getFirstChildWithTag(NED_SUBMODULES);
}

ConnectionsNode *cNEDDeclaration::getConnections()
{
    return (ConnectionsNode *)getTree()->getFirstChildWithTag(NED_CONNECTIONS);
}


//-------------------

/*
void cNEDDeclaration::assertParMatchesBase(cPar *par, const cPar *basepar,
                                                const char *parname, const char *basename)
{
    if (basepar->isSet() && par->isSet())
        throw new cRuntimeError(this, "parameter `%s' already set in base `%s', it cannot be overridden", parname, basename);

    if (par->type()!=basepar->type() || par->isVolatile()!=ifpar->isVolatile())
        throw new cRuntimeError(this, "type of parameter `%s' differs from that in base `%s'", parname, basename);
}

bool cNEDDeclaration::parMatchesInterface(cPar *par, const cPar *ifpar)
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

