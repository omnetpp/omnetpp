//=========================================================================
//  CCLASSFACTORY.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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


cClassFactory::cClassFactory(const char *name, cPolymorphic *(*f)(), const char *description)
  : cNoncopyableObject(name, false)
{
    creatorfunc = f;
    descr = description ? description : "";
}

std::string cClassFactory::info() const
{
    return std::string("(") + descr + ")";
}

cClassFactory *cClassFactory::find(const char *classname)
{
    return dynamic_cast<cClassFactory *>(classes.instance()->get(classname));
}

cPolymorphic *cClassFactory::createOne(const char *classname)
{
    cClassFactory *p = find(classname);
    if (!p)
        throw new cRuntimeError("Class \"%s\" not found -- perhaps its code was not linked in, "
                                "or the class wasn't registered with Register_Class(), or in the case of "
                                "modules and channels, with Define_Module()/Define_Channel()", classname);
    return p->createOne();
}

cPolymorphic *cClassFactory::createOneIfClassIsKnown(const char *classname)
{
    cClassFactory *p = find(classname);
    return p ? p->createOne() : NULL;
}


