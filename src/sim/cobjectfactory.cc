//=========================================================================
//  COBJECTFACTORY.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "common/stringutil.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/cexception.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

cObjectFactory::cObjectFactory(const char *name, cObject *(*creatorf)(), void *(*castf)(cObject *), const char *description)
    : cNoncopyableOwnedObject(name, false),
    creatorFunc(creatorf), castFunc(castf), description(description ? description : "")
{
}

std::string cObjectFactory::str() const
{
    return opp_join("; ", isAbstract() ? "abstract class" : "", description);
}

cObjectFactory *cObjectFactory::find(const char *className, const char *contextNamespace, bool fallbackToOmnetpp)
{
    return dynamic_cast<cObjectFactory *>(classes.getInstance()->lookup(className, contextNamespace, fallbackToOmnetpp));
}

cObjectFactory *cObjectFactory::get(const char *className, const char *contextNamespace, bool fallbackToOmnetpp)
{
    cObjectFactory *p = find(className, contextNamespace, fallbackToOmnetpp);
    if (!p)
        throw cRuntimeError("Class \"%s\" not found -- perhaps its code was not linked in, "
                            "or the class wasn't registered with Register_Class(), or in the case of "
                            "modules and channels, with Define_Module()/Define_Channel()", className);
    return p;
}

cObject *cObjectFactory::createOne() const
{
    if (!creatorFunc)
        throw cRuntimeError("Class \"%s\" is registered as abstract, and cannot be instantiated", getFullName());
    return creatorFunc();
}

cObject *cObjectFactory::createOne(const char *classname)
{
    cObjectFactory *p = get(classname);
    return p->createOne();
}

cObject *cObjectFactory::createOneIfClassIsKnown(const char *classname)
{
    cObjectFactory *p = find(classname);
    return p ? p->createOne() : nullptr;
}

}  // namespace omnetpp

