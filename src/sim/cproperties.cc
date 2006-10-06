//==========================================================================
//  CPROPERTIES.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cproperties.h"
#include "cproperty.h"


cProperties::cProperties()
{
    islocked = false;
}

cProperties::~cProperties()
{
    for (int i=0; i<propv.size(); i++)
        delete propv[i];
}

cProperties& cProperties::operator=(const cProperties& other)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);

    // note: do NOT copy islocked flag

    // delete existing contents
    for (int i=0; i<propv.size(); i++)
        delete propv[i];
    propv.clear();

    // copy stuff from other
    for (int i=0; i<other.propv.size(); i++)
        propv.push_back(other.propv[i]->dup());
    return *this;
}

const char *cProperties::fullName() const
{
    return "properties";
}

std::string cProperties::fullPath() const
{
    return ""; // TBD!!!!
}

std::string cProperties::info() const
{
    if (propv.empty())
        return std::string("empty");
    std::stringstream out;
    out << "size=" << propv.size();
    return out.str();
}

void cProperties::netPack(cCommBuffer *buffer)
{
    // TBD
}

void cProperties::netUnpack(cCommBuffer *buffer)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);
    // TBD
}

cProperty *cProperties::get(const char *name) const
{
    for (int i=0; i<propv.size(); i++)
        if (!strcmp(propv[i]->fullName(),name))
            return propv[i];
    return NULL;
}

void cProperties::add(cProperty *p)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);
    propv.push_back(p);
}

void cProperties::remove(const char *name)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);

    int i;
    for (i=0; i<propv.size(); i++)
        if (!strcmp(propv[i]->fullName(),name))
            break;
    if (i!=propv.size())
    {
        delete propv[i];
        propv.erase(propv.begin()+i);
    }
}

std::vector<const char *> cProperties::getNames() const
{
    std::vector<const char *> v;
    for (int i=0; i<propv.size(); i++)
        v.push_back(propv[i]->fullName());
    return v;
}

void cProperties::lock()
{
    islocked = true;
    for (int i=0; i<propv.size(); i++)
        propv[i]->lock();
}


