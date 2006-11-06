//========================================================================
//  CPOLYMORPHIC.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cpolymorphic.h"
#include "cexception.h"
#include "cclassdescriptor.h"
#include "errmsg.h"

const char *cPolymorphic::className() const
{
    return opp_typename(typeid(*this));
}

cClassDescriptor *cPolymorphic::getDescriptor()
{
    return cClassDescriptor::getDescriptorFor(this);
}

const char *cPolymorphic::fullName() const
{
    return "";
}

std::string cPolymorphic::fullPath() const
{
    return std::string(fullName());
}

std::string cPolymorphic::info() const
{
    return std::string();
}

void cPolymorphic::info(char *buf)
{
    opp_strprettytrunc(buf, info().c_str(), MAX_OBJECTINFO);
}

std::string cPolymorphic::detailedInfo() const
{
    return std::string();
}

cPolymorphic *cPolymorphic::dup() const
{
    throw new cRuntimeError("The dup() method, declared in cPolymorphic, is not "
                            "redefined in class %s", className());
}

