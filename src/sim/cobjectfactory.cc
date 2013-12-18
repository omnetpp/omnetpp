//=========================================================================
//  COBJECTFACTORY.CC - part of
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
#include "cobjectfactory.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#include "parsim/cplaceholdermod.h"
#endif

NAMESPACE_BEGIN


cObjectFactory::cObjectFactory(const char *name, cObject *(*creatorf)(), void *(*castf)(cObject *), const char *description)
  : cNoncopyableOwnedObject(name, false)
{
    creatorfunc = creatorf;
    castfunc = castf;
    descr = description ? description : "";
}

std::string cObjectFactory::info() const
{
    if (descr.empty())
        return "";
    return std::string("(") + descr + ")";  //TODO isAbstract
}

cObjectFactory *cObjectFactory::find(const char *classname)
{
    return dynamic_cast<cObjectFactory *>(classes.getInstance()->lookup(classname));
}

cObjectFactory *cObjectFactory::get(const char *classname)
{
    cObjectFactory *p = find(classname);
    if (!p)
        throw cRuntimeError("Class \"%s\" not found -- perhaps its code was not linked in, "
                            "or the class wasn't registered with Register_Class(), or in the case of "
                            "modules and channels, with Define_Module()/Define_Channel()", classname);
    return p;
}

cObject *cObjectFactory::createOne() const
{
    if (!creatorfunc)
        throw cRuntimeError("Class \"%s\" is registered as abstract, and cannot be instantiated", getFullName());
    return creatorfunc();
}

cObject *cObjectFactory::createOne(const char *classname)
{
    cObjectFactory *p = get(classname);
    return p->createOne();
}

cObject *cObjectFactory::createOneIfClassIsKnown(const char *classname)
{
    cObjectFactory *p = find(classname);
    return p ? p->createOne() : NULL;
}

NAMESPACE_END

