//=========================================================================
//  COBJECTFACTORY.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "common/stringutil.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/cexception.h"

NAMESPACE_BEGIN


cObjectFactory::cObjectFactory(const char *name, cObject *(*creatorf)(), void *(*castf)(cObject *), const char *description)
  : cNoncopyableOwnedObject(name, false)
{
    this->creatorFunc = creatorf;
    this->castFunc = castf;
    this->description = description ? description : "";
}

std::string cObjectFactory::info() const
{
    if (description.empty())
        return "";
    return std::string("(") + description + ")";  //TODO isAbstract
}

cObjectFactory *cObjectFactory::doFind(const char *className)
{
    return dynamic_cast<cObjectFactory *>(classes.getInstance()->lookup(className));
}

cObjectFactory *cObjectFactory::find(const char *className, const char *contextNamespace, bool fallbackToOmnetpp)
{
    if (className[0] == ':' && className[1] == ':')
        return doFind(className+2);

    // try with contextNamespace
    if (!opp_isempty(contextNamespace)) {
        std::string ns = contextNamespace;
        while (!ns.empty()) {
            cObjectFactory *result = doFind((ns + "::" + className).c_str());
            if (result)
                return result;
            // remove last segment of namespace, and try again
            int pos = ns.rfind("::");
            if (pos == std::string::npos)
                break;
            ns = ns.substr(0, pos);
        }
    }

    // try in the global namespace, then in the omnetpp namespace if requested
    cObjectFactory *result = doFind(className);
//XXX note: NO #if USE_NAMESPACE here!
    if (!result && fallbackToOmnetpp)
        result = doFind((std::string("omnetpp::") + className).c_str());
//#endif
    return result;
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
    return p ? p->createOne() : NULL;
}

NAMESPACE_END

