//=========================================================================
//  CCLASSFACTORY.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include "cclassfactory.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#include "parsim/cplaceholdermod.h"
#endif

USING_NAMESPACE


cClassFactory::cClassFactory(const char *name, cObject *(*f)(), const char *description)
  : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = f;
    descr = description ? description : "";
}

std::string cClassFactory::info() const
{
    if (descr.empty())
        return "";
    return std::string("(") + descr + ")";
}

cClassFactory *cClassFactory::find(const char *classname)
{
    return dynamic_cast<cClassFactory *>(classes.getInstance()->lookup(classname));
}

cObject *cClassFactory::createOne(const char *classname)
{
    cClassFactory *p = find(classname);
    if (!p)
        throw cRuntimeError("Class \"%s\" not found -- perhaps its code was not linked in, "
                            "or the class wasn't registered with Register_Class(), or in the case of "
                            "modules and channels, with Define_Module()/Define_Channel()", classname);
    return p->createOne();
}

cObject *cClassFactory::createOneIfClassIsKnown(const char *classname)
{
    cClassFactory *p = find(classname);
    return p ? p->createOne() : NULL;
}


